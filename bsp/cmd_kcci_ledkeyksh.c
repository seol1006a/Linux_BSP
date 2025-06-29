#include <vsprintf.h>
#include <command.h>
#include <asm/io.h>

#define BCM2711_GPIO_GPFSEL0 	0xFE200000 	
#define BCM2711_GPIO_GPFSEL1 	0xFE200004 
#define BCM2711_GPIO_GPFSEL2 	0xFE200008 
#define BCM2711_GPIO_GPSET0 	0xFE20001C
#define BCM2711_GPIO_GPCLR0 	0xFE200028
#define BCM2711_GPIO_GPLEV0 	0xFE200034

#define GPIO6_9_SIG_OUTPUT 		0x09240000	//gpio6~9 output
//#define GPIO10_13_SIG_OUTPUT 	0x00012249  //txd,rxd
#define GPIO10_13_SIG_OUTPUT 	0x00000249 	//gpio10~13 output 
#define GPIO6_13_SET_CLR_BIT    0x00003fc0

void led_init(void)
{
	unsigned long temp;
	temp = readl(BCM2711_GPIO_GPFSEL0);		//read
//	temp = temp & 0xc003ffff;				//gpio6~9 bit clear
	temp = temp & ~0x3ffc0000;				//gpio6~9 bit clear	
	temp = temp | GPIO6_9_SIG_OUTPUT;		//modify gpio output
	writel(temp,BCM2711_GPIO_GPFSEL0);		//write 	

	temp = readl(BCM2711_GPIO_GPFSEL1);		//read
//	temp = temp & 0xfffff000;				//gpio10~13 bit clear
	temp = temp & ~0x00000fff;				//gpio10~13 bit clear
	temp = temp | GPIO10_13_SIG_OUTPUT;		//modify gpio output
	writel(temp,BCM2711_GPIO_GPFSEL1);  	//write

}
void led_write(unsigned long led_data)  //led_data : 0x00~0xff
{
	writel(GPIO6_13_SET_CLR_BIT, BCM2711_GPIO_GPCLR0); 	//led0~7 all off
	led_data = led_data << 6;
	writel(led_data, BCM2711_GPIO_GPSET0);  //ledX on
}
void key_init(void)
{
	unsigned long temp;
	temp = readl(BCM2711_GPIO_GPFSEL1);		//read
	temp &= ~0x3ffc0000;					//gpio16~19 bit clear & input	
//	temp &= ~((7 << 18) | (7 << 21) | (7 << 24) | (7 << 27)); 
	writel(temp,BCM2711_GPIO_GPFSEL1);		//write 	

	temp = readl(BCM2711_GPIO_GPFSEL2);		//read
	temp &= ~0x00000fff;					//gpio20~23 bit clear & input	
//	temp &= ~((7 << 0) | (7 << 3) | (7 << 6) | (7 << 9));
	writel(temp,BCM2711_GPIO_GPFSEL2);		//write 	
}
void key_read(unsigned long * key_data)
{
	*key_data = (readl(BCM2711_GPIO_GPLEV0) >> 16) & 0x000000ff;
}
static int do_KCCI_LED(struct cmd_tbl *cmdtp,int flag, int argc, char * const argv[])
{
	unsigned long led_data;
	unsigned long key_data;
	unsigned long key_data_old=0;
	int i;
  	if(argc != 2)
	{
		cmd_usage(cmdtp);
		return 1;
	}
	printf("*LED TEST START\n");
  	led_init();
	key_init();
	led_data = simple_strtoul(argv[1],NULL,16);
  	led_write(led_data);

	do {
		key_read(&key_data)	;
		if(key_data != key_data_old)
		{
			if(key_data)
			{
				led_write(key_data);
	
				puts("0:1:2:3:4:5:6:7\n");
				for(i=0;i<8;i++)
				{
					if(key_data & (0x01 << i))
						putc('O');
					else
						putc('X');
					if(i != 7 )
						putc(':');
					else
						putc('\n');
				}
				putc('\n');

				if(key_data == 0x80)
					break;
			}
			key_data_old = key_data;
		}
	}while(1);

	printf("*LED TEST END(key : %#04x)\n\n ",(unsigned int)key_data);
	return 0;
}
U_BOOT_CMD(
	led,2,0,do_KCCI_LED,
	"kcci LED Test.",
	"number - Input argument is only one.(led [0x00~0xff])\n");
