#include<sys/types.h>
#include<unistd.h>
#include<sys/stat.h>
#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<errno.h>
#include <linux/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
/*-----------------------------------------ioctl相关开始--------------------------------------------*/
#define MEM_MAGIC 'm' //定义一个幻数，而长度正好和ASC码长度一样为8位，所以这里定义个字符
#define MEM_READ      _IOR(MEM_MAGIC, 1, int)      //设置为读状态
#define MEM_WRITE     _IOW(MEM_MAGIC, 2, int)      //设置为写命令
/*-----------------------------------------ioctl相关结束--------------------------------------------*/

/* mmap GPIO ,注意，mmap总是按页帧映射的，因此无论用户传递的offset设置为寄存器基地址还是该组寄存器中的某一个寄存器地址偏移，
实际映射后得到虚拟地址都是这组寄存器所在页帧的映射虚拟地址，因此最好直接将offset指定为寄存器基地址，
并且清楚知道需控制的寄存器地址相对于基地址的偏移量。
 */
#define GPIOBASE          0xe0200000
#define GPJ0DATA_OFFSET   0x0244
#define BLOCK_SIZE        (4*1024)


volatile unsigned int *gpio;
void *gpio_map;

int main()
{
    int fd;
    unsigned int cmd =0;
    char msg[100];
    int  arg = 0;
    fd= open("/dev/charmem",O_RDWR,S_IRUSR|S_IWUSR);
    if(fd!=-1)
    {
	printf("[app]dev open  massage:%s\n",strerror(errno));
        /* mmap GPIO */
        gpio_map = mmap(
        	NULL, 
                BLOCK_SIZE,   
        	PROT_READ|PROT_WRITE,
        	MAP_SHARED,     
        	fd,          
        	GPIOBASE);
        //若驱动自己记忆硬件寄存器地址，那么可以注释掉上一句，开启下面这句
	//	0);
  	close(fd);
  	if (gpio_map == MAP_FAILED) {
   	   	printf("[app]mmap error: %s\n", strerror(errno));
      		exit(-1);
  	}
        else
	{
		printf("[app]get gpio vir addr=%p",gpio_map);
	}

        while(1)
        {
            printf("[app]::\n");
            scanf("%s",msg);

	    switch (msg[0])
            {
               case 'R':
               case 'r':
               {
 		  *((volatile unsigned int *)(gpio_map + GPJ0DATA_OFFSET)) = (0<<3) | (0<<4) | (0<<5);  break;	  
               }
               case 'W':
               case 'w':
               {
		   *((volatile unsigned int *)(gpio_map + GPJ0DATA_OFFSET)) = (1<<3) | (1<<4) | (1<<5);  break;
               }
	       default :
	       {
		  continue;
               }
              //write(fd,msg,strlen(msg));
           }
	   //printf("[app_write] ready to send cmd : %d-%c\n",cmd, arg);
           //int ret = ioctl(fd, cmd, &arg);
           //if(msg[0] == 'R' || msg[0] == 'r')
	   //printf("[app_write]recive kernel arg:%d\n", arg);
        }
    }
    else
    {
        printf("[app]device open failure\n");
        printf("[app]dev massage:%s\n",strerror(errno));
	
    }
    return 0;
}
