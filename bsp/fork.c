#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int fatal(char *);
int main(void)
{
	int ret;
#if 1
	pid_t pid;
	char *argv[] = {"ls","-l",NULL};
	switch(pid=fork()) {
		case -1:
			fatal("fork failed");
			break;
		case 0:
//			sleep(10);
//			execl("/bin/ls","ls", "-l",(char *)0);
//			execlp("ls","ls", "-l",(char *)0);
			execvp("ls",argv);
			
//			execl("/bin/ls","ls", "-l","../.",(char *)0);
//			execl("/home/ubuntu/linuxC/copy","copy", "copy.c","copy.txt",(char *)0);
			fatal("exec failed");
			break;
		default:
			wait((int *)0);
			printf("ls completed\n");
			exit(0);
	}
#endif
#if 0
	ret = system("/home/ubuntu/linuxC/copy copy.c copy.txt1");
	printf("ls completed %d\n",ret);
#endif
	return 0;
}
int fatal(char *s)
{
	perror(s);
	exit(1);
}
