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
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/mutex.h>

#define DEBUG 1

#define LEDKEY_DEV_NAME     "kerneltimer"
#define LEDKEY_DEV_MAJOR    230
#define OFF 0
#define ON 1
#define GPIOCNT 8

static int gpioLedInit(void);
static void gpioLedSet(long val);
static void gpioLedFree(void);
static int gpioKeyInit(void);
static int gpioKeyGet(void);
static void gpioKeyFree(void);

static int timerVal = 100;
module_param(timerVal, int, 0);
static int ledVal = 0;
module_param(ledVal, int, 0);
struct timer_list timerLed;

static int gpioLed[GPIOCNT] = { 518,519,520,521,522,523,524,525 };
static int gpioKey[GPIOCNT] = { 528,529,530,531,532,533,534,535 };
static int irqKey[GPIOCNT];

static int openFlag = 0;
static int keyNum = 0;
static DEFINE_MUTEX(keyMutex);

static void kerneltimer_func(struct timer_list* t);
static void kerneltimer_registertimer(unsigned long timeover)
{
	timerLed.expires = get_jiffies_64() + timeover;
	timer_setup(&timerLed, kerneltimer_func, 0);
	add_timer(&timerLed);
}

static void kerneltimer_func(struct timer_list* t)
{
#if DEBUG
	printk("ledVal : %#04x\n", (unsigned int)(ledVal));
#endif
	int i;
	for (i = 0; i < GPIOCNT; i++)
	{
		gpio_set_value(gpioLed[i], (ledVal & (0x01 << i)));
	}

	ledVal = ~ledVal & 0xff;
	mod_timer(t, get_jiffies_64() + timerVal);
}

static int gpioLedInit(void)
{
	int i;
	int ret = 0;
	char gpioName[10];
	for (i = 0; i < GPIOCNT; i++)
	{
		sprintf(gpioName, "led%d", i);
		ret = gpio_request(gpioLed[i], gpioName);
		if (ret < 0)
		{
			printk("Failed request gpio%d error\n", gpioLed[i]);
			return ret;
		}
		ret = gpio_direction_output(gpioLed[i], OFF);
		if (ret < 0)
		{
			printk("Failed directon_output gpio%d error\n", gpioLed[i]);
			return ret;
		}
	}
	return ret;
}

static void gpioLedSet(long val)
{
	int i;
	for (i = 0; i < GPIOCNT; i++)
	{
		gpio_set_value(gpioLed[i], (val & (0x01 << i)));
	}
}

static void gpioLedFree(void)
{
	int i;
	for (i = 0; i < GPIOCNT; i++)
	{
		gpio_free(gpioLed[i]);
	}
}

static int gpioKeyInit(void)
{
	int i;
	int ret = 0;
	char gpioName[10];
	for (i = 0; i < GPIOCNT; i++)
	{
		sprintf(gpioName, "key%d", i);
		ret = gpio_request(gpioKey[i], gpioName);
		if (ret < 0) {
			printk("Failed Request gpio%d error\n", gpioKey[i]);
			return ret;
		}
		ret = gpio_direction_input(gpioKey[i]);
		if (ret < 0) {
			printk("Failed direction_output gpio%d error\n", gpioKey[i]);
			return ret;
		}
	}
	return ret;
}

static int	gpioKeyGet(void)
{
	int i;
	int ret;
	int keyData = 0;
	for (i = 0; i < GPIOCNT; i++)
	{
		//		ret=gpio_get_value(gpioKey[i]) << i;
		//		keyData |= ret;
		ret = gpio_get_value(gpioKey[i]);
		keyData = keyData | (ret << i);
	}
	return keyData;
}

static void gpioKeyFree(void)
{
	int i;
	for (i = 0; i < GPIOCNT; i++)
	{
		gpio_free(gpioKey[i]);
	}
}

static int irqKeyInit(void)
{
	int i;
	int ret = 0;
	for (i = 0; i < GPIOCNT; i++)
	{
		irqKey[i] = gpio_to_irq(gpioKey[i]);
		if (irqKey[i] < 0)
		{
			printk("Failed gpio_to_irq() gpio%d error\n", gpioKey[i]);
			return irqKey[i];
		}
#if DEBUG
		else
			printk("gpio_to_irq() gpio%d (irq%d) \n", gpioKey[i], irqKey[i]);
#endif
	}
	return ret;
}

static void irqKeyFree(void)
{
	int i;
	for (i = 0; i < GPIOCNT; i++)
	{
		free_irq(irqKey[i], NULL);
	}
}

static irqreturn_t keyIsr(int irq, void* data)
{
	int i;
	for (i = 0; i < GPIOCNT; i++)
	{
		if (irq == irqKey[i])
		{
			if (mutex_trylock(&keyMutex) != 0)
			{
				keyNum = i + 1;
				if (keyNum != 8)
					ledVal = 0x01 << i;
				mutex_unlock(&keyMutex);
				break;
			}
		}
	}
#if DEBUG
	printk("keyIsr() irq : %d, keyNum : %d\n", irq, keyNum);
#endif
	return IRQ_HANDLED;
}

static int ledkey_open(struct inode* inode, struct file* filp)
{
#if DEBUG
	int num0 = MAJOR(inode->i_rdev);
	int num1 = MINOR(inode->i_rdev);
	printk("call open -> major : %d\n", num0);
	printk("call open -> minor : %d\n", num1);
#endif

	if (openFlag)
		return -EBUSY;
	else
		openFlag = 1;

	if (!try_module_get(THIS_MODULE))
		return -ENODEV;

	return 0;
}

static loff_t ledkey_llseek(struct file* filp, loff_t off, int whence)
{
#if DEBUG
	printk("call llseek -> off : %08X, whenec : %08X\n", (unsigned int)off, whence);
#endif
	return 0x23;
}

static ssize_t ledkey_read(struct file* filp, char* buf, size_t count, loff_t* f_pos)
{
	char kbuf = 0;

	if (mutex_trylock(&keyMutex) != 0)
	{
		if (keyNum != 0)
		{
			kbuf = (char)keyNum;
			keyNum = 0;
		}
		mutex_unlock(&keyMutex);
	}

	put_user(kbuf, buf);

#if DEBUG
	printk("call read -> key : %#04x\n", kbuf);
#endif

	return sizeof(kbuf);
}

static ssize_t ledkey_write(struct file* filp, const char* buf, size_t count, loff_t* f_pos)
{
	char kbuf;

	get_user(kbuf, buf);

#if DEBUG
	printk("call write -> led : %#04x\n", kbuf);
#endif

	ledVal = kbuf;

	return count;
}

static long ledkey_ioctl(struct file* filp, unsigned int cmd, unsigned long arg)
{
	int err = 0;
	return err;
}

static int ledkey_release(struct inode* inode, struct file* filp)
{
	printk("call release \n");

	module_put(THIS_MODULE);
	openFlag = 0;
	return 0;
}

struct file_operations ledkey_fops =
{
	//    .owner    = THIS_MODULE,
		.open = ledkey_open,
		.read = ledkey_read,
		.write = ledkey_write,
		.unlocked_ioctl = ledkey_ioctl,
		.llseek = ledkey_llseek,
		.release = ledkey_release,
};

static int kerneltimer_init(void)
{
	int result;
	char* irqName[GPIOCNT] = { "irqKey0","irqKey1","irqKey2","irqKey3","irqKey4","irqKey5","irqKey6","irqKey7", };

#if DEBUG
	printk("call ledkey_init \n");
#endif

	mutex_init(&keyMutex);

	result = gpioLedInit();
	if (result < 0)
		return result;

	result = gpioKeyInit();
	if (result < 0)
		return result;

	result = irqKeyInit();
	if (result < 0)
		return result;

	for (int i = 0; i < GPIOCNT; i++)
	{
		result = request_irq(irqKey[i], keyIsr, IRQF_TRIGGER_RISING, irqName[i], NULL);
		if (result < 0)
			return result;
	}

	result = register_chrdev(LEDKEY_DEV_MAJOR, LEDKEY_DEV_NAME, &ledkey_fops);
	if (result < 0) return result;

	kerneltimer_registertimer(timerVal);
	return 0;
}

static void kerneltimer_exit(void)
{
	if (timer_pending(&timerLed))
		del_timer(&timerLed);

	unregister_chrdev(LEDKEY_DEV_MAJOR, LEDKEY_DEV_NAME);

	irqKeyFree();
	gpioLedFree();
	gpioKeyFree();
	mutex_destroy(&keyMutex);
}
module_init(kerneltimer_init);
module_exit(kerneltimer_exit);
MODULE_AUTHOR("KCCI-AIOT KSH");
MODULE_DESCRIPTION("test module");
MODULE_LICENSE("Dual BSD/GPL");
