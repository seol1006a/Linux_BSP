#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/gpio.h>
#include <linux/moduleparam.h>

#define DEBUG 1
#define GPIOCNT 8
#define LED_OFF 0
#define LED_ON 1

static unsigned int gpioLed[GPIOCNT] = {518,519,520,521,522,523,524,525};
static int gpioLedInit(void);
static void gpioLedSet(long val);
static void gpioLedFree(void);
static unsigned int gpioKey[GPIOCNT] = {528,529,530,531,532,533,534,535};
static int gpioKeyInit(void);
static long gpioKeyGet(void);
static void gpioKeyFree(void);

static int timerVal = 100;  //f=100HZ, T=1/100 = 10ms, 100*10ms = 1sec
module_param(timerVal, int, 0);
static int ledVal = 0;
module_param(ledVal, int , 0);
struct timer_list timerLed;

static void kerneltimer_func(struct timer_list *t);
static void kerneltimer_registertimer(unsigned long timeover)
{
	timerLed.expires = get_jiffies_64() + timeover;
	timer_setup(&timerLed,kerneltimer_func, 0);
	add_timer(&timerLed);
}

static void kerneltimer_func(struct timer_list *t)
{
#if DEBUG
	printk("ledVal : %#04x\n", (unsigned int)(ledVal));
#endif
	gpioLedSet(ledVal);

	ledVal = ~ledVal & 0xff;
	mod_timer(t, get_jiffies_64() + timerVal);
}

static int kerneltimer_init(void)
{
#if DEBUG
	printk("timerVal : %d, sec : %d \n", timerVal, timerVal/HZ);
#endif

	int result;
	result = gpioLedInit();    //gpio_request(), gpiot_direction_output();
    if(result < 0)
        return result;

    result = gpioKeyInit();
    if(result < 0)
        return result;

	kerneltimer_registertimer(timerVal);
	gpioLedInit();
	return 0;
}

static void kerneltimer_exit(void)
{
	if(timer_pending(&timerLed))
		del_timer(&timerLed);
		gpioLedFree();
		gpioKeyFree();
}



static int gpioLedInit(void)
{
    int i;
    int ret=0 ;
    char gpioName[10];
    for(i=0;i<GPIOCNT;i++)
    {
        sprintf(gpioName,"led%d",i);
        ret=gpio_request(gpioLed[i],gpioName);
        if(ret < 0)
        {
            printk("Failed request gpio%d error\n",gpioLed[i]);
            return ret;
        }
    }
    for(i=0;i<GPIOCNT;i++)
    {
        ret = gpio_direction_output(gpioLed[i],LED_OFF);
        if(ret < 0)
        {
            printk("Failed direction_output gpio%d error\n",gpioLed[i]);
            return ret;
        }
    }
    return ret;
}
static void gpioLedSet(long val)
{
    int i;
    for(i=0;i<GPIOCNT;i++)
    {
        gpio_set_value(gpioLed[i],((val >> i) & 0x01));
    }
}
static void gpioLedFree(void)
{
    int i;
    for(i=0;i<GPIOCNT;i++)
    {
        gpio_free(gpioLed[i]);
    }
}

static int gpioKeyInit(void)
{
    int i;
    int ret=0 ;
    char gpioName[10];
    for(i=0;i<GPIOCNT;i++)
    {
        sprintf(gpioName,"key%d",i);
        ret=gpio_request(gpioKey[i],gpioName);
        if(ret < 0)
        {
            printk("Failed request gpio%d error\n",gpioKey[i]);
            return ret;
        }
    }
    for(i=0;i<GPIOCNT;i++)
    {
        ret = gpio_direction_input(gpioKey[i]);
        if(ret < 0)
        {
            printk("Failed direction_output gpio%d error\n",gpioKey[i]);
            return ret;
        }
    }
    return ret;
}
static long gpioKeyGet(void)
{
    int i;
    long key=0;
    long ret;
    for(i=0;i<GPIOCNT;i++)
    {
        ret = gpio_get_value(gpioKey[i]);
        if(ret < 0)
            return ret;
        key |= ret << i;

//      key |= gpio_get_value(gpioKey[i]) << i;
    }
    return key;
}
static void gpioKeyFree(void)
{
    int i;
    for(i=0;i<GPIOCNT;i++)
    {
        gpio_free(gpioKey[i]);
    }
}


module_init(kerneltimer_init);
module_exit(kerneltimer_exit);
MODULE_AUTHOR("KCCI-AIOT KSH");
MODULE_DESCRIPTION("test module");
MODULE_LICENSE("Dual BSD/GPL");
