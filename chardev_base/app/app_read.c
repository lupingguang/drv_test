#include<sys/types.h>
#include<unistd.h>
#include<sys/stat.h>
#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<errno.h>
int main()
{
    int fd,i;
    char msg[101];
    fd= open("/dev/charmem",O_RDWR,S_IRUSR|S_IWUSR);
    if(fd!=-1)
    {
	printf("[app_read]device open success,%d\n",fd);
	printf("[app_read]dev massage:%s\n",strerror(errno));
        while(1)
        {
            for(i=0;i<101;i++)
                msg[i]='\0';
            read(fd,msg,100);
            printf("%s\n",msg);
            if(strcmp(msg,"quit")==0)
            {
                close(fd);
                break;
            }
        }
    }
    else
    {
        printf("[app_read]device open failure,%d\n",fd);
        printf("[app_read]dev massage:%s\n",strerror(errno));
    }
    return 0;
}
