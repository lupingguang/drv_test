#include<sys/types.h>
#include<unistd.h>
#include<sys/stat.h>
#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include <linux/ioctl.h>
/*-----------------------------------------ioctl相关开始--------------------------------------------*/
#define MEM_MAGIC 'm' //定义一个幻数，而长度正好和ASC码长度一样为8位，所以这里定义个字符
#define MEM_READ      _IOR(MEM_MAGIC, 1, int)      //设置为读状态
#define MEM_WRITE     _IOW(MEM_MAGIC, 2, int)      //设置为写命令
/*-----------------------------------------ioctl相关结束--------------------------------------------*/
typedef struct params
{
    unsigned int num;
    unsigned char id;
    unsigned int useraddr;
    unsigned int *puseraddr;
}param;


int main()
{
    param userparam = {1,1,0x30000000,0x4000000};
    unsigned char userarry[5] = {0};
    unsigned char *p = malloc(sizeof(unsigned char)*5);
    userparam.useraddr = (unsigned int)userarry;
    userparam.puseraddr = (unsigned int *)p;
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
            errno = 0;
	    switch (msg[0])
            {
               case 'R':
               case 'r':
               {
 		  cmd = MEM_READ;  break;	  
               }
               case 'W':
               case 'w':
               {
                  cmd = MEM_WRITE; break;
               }
	       default :
	       {
		  continue;
               }
              //write(fd,msg,strlen(msg));
           }
	   // printf("[app_write] ready to send cmd : %d-%c\n",cmd, arg);
           int ret = ioctl(fd, cmd, &userparam);
           
           if(msg[0] == 'R' || msg[0] == 'r')
           {
	       printf("[app_read]recive kernel ret:---------------\n");
               printf("[app_read]userparam.num=%d\n",userparam.num);
               printf("[app_read]userparam.id=%d\n",userparam.id);
               printf("[app_read]userparam.useraddr=%s\n",(unsigned char *)(userparam.useraddr));
               printf("[app_read]userparam.puseraddr=%s\n",(unsigned char *)(userparam.puseraddr));           
	   }
	  // if(ret != 0)
	   printf("[app_write]ioctl ret:%d:errno:%d:str:%s\n",ret,errno,strerror(errno));
        }
    }
    else
    {
        printf("[app_write]device open failure\n");
        printf("[app_write]dev massage:%s\n",strerror(errno));
	
    }
    return 0;
}
