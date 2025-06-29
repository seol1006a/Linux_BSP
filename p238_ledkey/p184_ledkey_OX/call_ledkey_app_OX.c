#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define DEVICE_FILENAME  "/dev/ledkey"

int main(int argc, char * argv[])
{
    int dev;
    char buff;
    int ret;
	char key;
	int key_data, key_data_old = 0;
	

	if(argc != 2)
	{
		printf("USAGE : %s ledval[0x00~0xff]\n",argv[0]);
		return 1;
	}
	key = strtoul(argv[1],NULL,16);

	if (key < 0 || 0xff < key)
	{
		printf("Usage : %s ledValue[0x00~0xff]\n", argv[0]);
		return 2;
	}

    if(access(DEVICE_FILENAME,F_OK) != 0)   //파일 없을시
    {
        int ret = mknod(DEVICE_FILENAME, S_IRWXU | S_IRWXG | S_IFCHR, (230 << 8) | 0);
        if(ret < 0)
            perror("mknod()");
    }

    dev = open( DEVICE_FILENAME, O_RDWR|O_NDELAY);
    if( dev < 0 )
    {
		perror("open");
		return 2;
	}
	buff = key;

	do {

		ret = read(dev, &buff, sizeof(buff));
		if (ret < 0)
		{
			perror("read()");
			return 3;
		}
		printf("ret = %08X, key = %#04x\n", ret, buff);

		key_data = buff;
		if (key_data != key_data_old)
		{
			key_data_old = key_data;
			if (key_data)
			{
				ret = write(dev, &buff, sizeof(buff));
				printf("ret = %08X\n", ret);

				puts("0:1:2:3:4:5:6:7");
				for (int i = 0; i < 8; i++)
				{
					if (key_data & (0x01 << i))
						putchar('O');
					else
						putchar('X');
					if (i != 7)
						putchar(':');
					else
						putchar('\n');
				}
				putchar('\n');

			}
			if (key_data == 0x80)
				break;
		}

	} while(1);
	ret = close(dev);
	return 0;
}

