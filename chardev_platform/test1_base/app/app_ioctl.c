#include<sys/types.h>
#include<unistd.h>
#include<sys/stat.h>
#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<errno.h>
#include <linux/ioctl.h>
/*-----------------------------------------ioctl相关开始--------------------------------------------*/
#define LED_MAGIC 'm' //定义一个幻数，而长度正好和ASC码长度一样为8位，所以这里定义个字符
#define LED_OFF      _IOR(LED_MAGIC, 1, int)      //设置为读状态
#define LED_ON       _IOW(LED_MAGIC, 2, int)      //设置为写命令
/*-----------------------------------------ioctl相关结束--------------------------------------------*/



int main()
{
    int fd;
    unsigned int cmd =0;
    char msg[100];
    int  arg = 0;
    fd= open("/dev/led_cdev",O_RDWR,S_IRUSR|S_IWUSR);
    if(fd!=-1)
    {
	printf("[app_write]device open success\n");
	printf("[app_write]dev massage:%s\n",strerror(errno));
        while(1)
        {
            printf("[app_write]::\n");
            scanf("%s",msg);

	    switch (msg[0])
            {
               case 'R':
	       {
                  cmd = LED_OFF; arg = 4;  break;
               }

               case 'r':
               {
 		  cmd = LED_OFF; arg = 5;  break;	  
               }
               case 'W':
               {
                  cmd = LED_ON; arg = 4;  break;  
               }

               case 'w':
               {
                  cmd = LED_ON; arg = 5; break;
               }
	       default :
	       {
		  continue;
               }
              //write(fd,msg,strlen(msg));
           }
	   // printf("[app_write] ready to send cmd : %d-%c\n",cmd, arg);
           int ret = ioctl(fd, cmd, &arg);
           if(ret != 0)
	       printf("[app_write]ioctl ret:%d:%s\n",ret,strerror(errno));
        }
    }
    else
    {
        printf("[app_write]device open failure\n");
        printf("[app_write]dev massage:%s\n",strerror(errno));
	
    }
    return 0;
}
