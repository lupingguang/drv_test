#include<sys/types.h>
#include<unistd.h>
#include<sys/stat.h>
#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<errno.h>
#include <linux/ioctl.h>
/*-----------------------------------------ioctl相关开始--------------------------------------------*/
#define MEM_MAGIC 'm' //定义一个幻数，而长度正好和ASC码长度一样为8位，所以这里定义个字符
#define MEM_READ      _IOR(MEM_MAGIC, 1, int)      //设置为读状态
#define MEM_WRITE     _IOW(MEM_MAGIC, 2, int)      //设置为写命令
/*-----------------------------------------ioctl相关结束--------------------------------------------*/



int main()
{
    int fd;
    unsigned int cmd =0;
    char msg[100];
    int  arg = 0;
    fd= open("/dev/charmem",O_RDWR,S_IRUSR|S_IWUSR);
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
               case 'r':
               {
 		  cmd = MEM_READ; arg = 11;  break;	  
               }
               case 'W':
               case 'w':
               {
                  cmd = MEM_WRITE; arg = 22; break;
               }
	       default :
	       {
		  continue;
               }
              //write(fd,msg,strlen(msg));
           }
	   // printf("[app_write] ready to send cmd : %d-%c\n",cmd, arg);
           int ret = ioctl(fd, cmd, &arg);
          // if(msg[0] == 'R' || msg[0] == 'r')
	       printf("[app_write]recive kernel arg:%d\n", arg);
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
