#include <linux/kernel.h>
#include <linux/gpio.h>
#define GPIOCNT 8
#define LED_OFF 0
#define LED_ON 1
//int gpioLed[GPIOCNT] = {6,7,8,9,10,11,12,13};
int gpioLed[GPIOCNT] = {518,519,520,521,522,523,524,525};
long sys_mysyscall(long val)
{
//	printk(KERN_INFO "Welcome to KCCI's embedded system!! app value=%ld\n",val);
	int i;
	int ret;
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
	for(i=0;i<GPIOCNT;i++)
	{
		gpio_set_value(gpioLed[i],((val >> i) & 0x01));
	}			

	for(i=0;i<GPIOCNT;i++)
	{
		gpio_free(gpioLed[i]);
	}
	return val;
}
