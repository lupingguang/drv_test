#include<sys/types.h>
#include<unistd.h>
#include<sys/stat.h>
#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<errno.h>
int main()
{
    int fd;
    char msg[100];
    fd= open("/dev/charmem",O_RDWR,S_IRUSR|S_IWUSR);
    if(fd!=-1)
    {
	printf("[app_write]device open success\n");
	printf("[app_write]dev massage:%s\n",strerror(errno));
        while(1)
        {
            printf("[app_write]Please input the globar:\n");

            scanf("%s",msg);
            write(fd,msg,strlen(msg));
            if(strcmp(msg,"quit")==0)
            {
                close(fd);
                break;
            }
        }
    }
    else
    {
        printf("[app_write]device open failure\n");
        printf("[app_write]dev massage:%s\n",strerror(errno));
	
    }
    return 0;
}
