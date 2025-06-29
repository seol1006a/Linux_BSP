#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVICE_FILENAME "/dev/ledkey"

int main(void)
{
    int dev;
    char buff;
    int ret;

    if(access(DEVICE_FILENAME,F_OK) != 0) //파일 없을시
    {
        int ret = mknod(DEVICE_FILENAME, S_IRWXU | S_IRWXG | S_IFCHR, (230<<8) | 0);
        if(ret <0)
            perror("mknod()");
    }

    dev = open(DEVICE_FILENAME, O_RDWR | O_NDELAY);
	if(dev < 0)
	{
		perror("open");
		return 1;
	}
	//while
	ret = read(dev, &buff, sizeof(buff));
	if(ret < 0)
	{
		perror("read()");
		return 2;
	}
	printf("ret = %08X, key = %#04x\n",ret, buff);

	buff = 0x55;
	ret = write(dev, &buff, sizeof(buff));
	printf("ret = %08X\n", ret);

	ret = close(dev);
    return 0;
}

