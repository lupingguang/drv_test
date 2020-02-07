#include "platform_common.h"
#include <linux/module.h>		// module_init  module_exit
#include <linux/init.h>			// __init   __exit
#include <linux/io.h>
#include <linux/ioport.h>
#include <mach/gpio.h>
#include <linux/platform_device.h>
#include <linux/list.h>
#include <linux/slab.h>

#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/wait.h>
#include <linux/semaphore.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/device.h>

#include <linux/io.h>
#include <linux/mm.h>
#include <linux/ioport.h>
//真正字符设备相关
static ssize_t led_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
struct file_operations led_fops =
{   
    .unlocked_ioctl= led_ioctl
};

struct cdev devm; //字符设备
static struct class *my_class;
int major=0;


#define LED_MAGIC 'm' //定义一个幻数，而长度正好和ASC码长度一样为8位，所以这里定义个字符
#define LED_OFF      _IOR(LED_MAGIC, 1, int)      //读命令
#define LED_ON       _IOW(LED_MAGIC, 2, int)      //写命令

//硬件相关
#define GPJ0CON_PA    0xe0200240
#define GPJ0DATA_PA   0xe0200244
unsigned int *vir_gpj0con = 0;
unsigned int *vir_gpj0data = 0;

#define X210_LED_OFF	1			
#define X210_LED_ON	0			
unsigned int drv_used_cnt = 0;

//platform driver 相关
LIST_HEAD(device_addr_table_header);

struct device_addr_table 
{
	unsigned int  gpio_num;
	char  *name;
	struct list_head list;
};


static void s5pv210_led_set(unsigned int func_id, unsigned int gpio_num);

static int s5pv210_led_probe(struct platform_device *dev);

static int s5pv210_led_remove(struct platform_device *dev);

static struct platform_driver s5pv210_led_driver = {
	.probe		= s5pv210_led_probe,
	.remove		= s5pv210_led_remove,
	.driver		= {
		.name		= "lpg_led",
		.owner		= THIS_MODULE,
	},
};

static int __init s5pv210_led_init(void)
{
	printk(KERN_INFO "----platform driver init success\n"); 
	return platform_driver_register(&s5pv210_led_driver);
}

static void __exit s5pv210_led_exit(void)
{
	printk(KERN_INFO "----platform driver exit success\n"); 
        platform_driver_unregister(&s5pv210_led_driver);
	
}

static void s5pv210_led_set(unsigned int func_id,unsigned int gpio_num)
{
	struct device_addr_table *ptable = (struct device_addr_table *)0;
        unsigned int pre_value =*(vir_gpj0data);
        if (func_id == LED_OFF)
        {   
		
                // 用户给了个0，希望LED灭
                *(vir_gpj0data) = pre_value | (1<<gpio_num); 
        }
        else
        {
                // 用户给的是非0，希望LED亮
                *(vir_gpj0data) = pre_value & (~(1<<gpio_num)); 
        }
	
        list_for_each_entry (ptable, &device_addr_table_header, list ) 
	{
         printk ("whole device info:gpio_num=%d,name=%s\n" , ptable->gpio_num,ptable->name);
	}
}

static int s5pv210_led_probe(struct platform_device *dev)
{
        //获取platform device信息
        int result = 0;
        int err = 0;
        struct resource	 *regs = 0;

        //获取设备板级信息，此处仅打印之
	regs = platform_get_resource(dev, IORESOURCE_MEM, 0);
        printk("get device resource start:%x,end:%x\n",regs->start,regs->end);
        struct s5pv210_led_platdata *pdata = dev->dev.platform_data;
        
        //将每个devcie附加数据信息保存为链表结构体 
	struct device_addr_table *ptable = (struct device_addr_table *)kmalloc(sizeof(struct device_addr_table), GFP_KERNEL);
	ptable->name = pdata->name;
        ptable->gpio_num = pdata->gpio;
	INIT_LIST_HEAD(&(ptable->list));
	list_add(&(ptable->list),  &device_addr_table_header);
         
	//硬件初始化,只有第一次执行probe函数时
        if(drv_used_cnt == 0)
	{
		if(!request_mem_region(GPJ0CON_PA,4,"LEDGPIO"))
		{
			printk("GPJ0CON_PA requst_mem_region error");
			return -1;
		}
		vir_gpj0con = ioremap(GPJ0CON_PA,4);
	    
		if(!request_mem_region(GPJ0DATA_PA,4,"LEDGPIO"))
		{
	       		printk("GPJ0DATA_PA requst_mem_region error");
	       	 	return -1;
		}
		//gpio设置为通用输出
		vir_gpj0data = ioremap(GPJ0DATA_PA,4);
		*vir_gpj0con = 0x11111111;
		//初始状态为所有LED关闭
		*(vir_gpj0data) = (1<<3)|(1<<4)|(1<<5); 
	}
        //真正字符设备驱动注册,只有第一次执行probe函数时
        if(drv_used_cnt == 0)
	{
		dev_t cdev = MKDEV(major, 0);
		if(major)
		{
		        //静态申请设备编号
		        result = register_chrdev_region(cdev, 1, "led_cdev");
		}
		else
		{
		       //动态分配设备号
		       result = alloc_chrdev_region(&cdev, 0, 1, "led_cdev");
		       major = MAJOR(cdev);
		}
		if(result < 0)
	       	       return result;
	   
		cdev_init(&devm, &led_fops);
		devm.owner = THIS_MODULE;
		err = cdev_add(&devm, cdev, 1);
		if(err)
		       printk(KERN_INFO "Error %d adding char_mem device", err);
		else
		{
		       printk("globalvar register success\n");
		
                }

		my_class = class_create(THIS_MODULE, "led_cdev_class");
		device_create(my_class, NULL, cdev, NULL, "led_cdev");
	}

	drv_used_cnt ++;
        printk(KERN_INFO "----platform driver probe,device info:%x,drv_used_cnt:%d\n",pdata->gpio,drv_used_cnt);  
        return 0;
}

static int s5pv210_led_remove(struct platform_device *dev)
{
        if(drv_used_cnt > 0) drv_used_cnt--;
	//没有设备使用驱动时，将真正驱动以及相关资源释放
        if(drv_used_cnt == 0)  
	{
		struct device_addr_table *ptable = (struct device_addr_table *)0;
		list_for_each_entry (ptable, &device_addr_table_header, list ) 
		{
		 printk ("kfree device,info:gpio_num=%d,name=%s\n" , ptable->gpio_num,ptable->name);
		 kfree(ptable);
		}
		

		device_destroy(my_class, MKDEV(major, 0));
		class_destroy(my_class);

		cdev_del(&devm);
		unregister_chrdev_region(MKDEV(major, 0), 1);

		iounmap(vir_gpj0con);
		iounmap(vir_gpj0data);
		release_mem_region(GPJ0CON_PA,4);
		release_mem_region(GPJ0DATA_PA,4);
	}
        printk(KERN_INFO "----platform driver remove success,drv_used_cnt=%d\n",drv_used_cnt);
        return 0;
}


static int led_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	//printk(KERN_WARNING"IOCTL %d&%d from app recieved!\n",arg, cmd);
        int karg1 =0;
	switch(cmd)
	{
		
		case LED_ON: //假如这里传入的是重启的命令
			copy_from_user(&karg1,(int *)arg,sizeof(int));
			printk(KERN_WARNING"LED_ON to device! LED_NUM= %d\n", karg1);
			s5pv210_led_set(LED_ON, karg1);
				
			return 0;
		case LED_OFF: //如果传入的是设置参数命令
		        copy_from_user(&karg1,(int *)arg,sizeof(int));
			printk(KERN_WARNING"LED_OFF to device! LED_NUM= %d\n", karg1);//打印出传入的参数
			s5pv210_led_set(LED_OFF, karg1);

			return 0;
		default:
			return -EINVAL;//其他的打印错误
			
	}
	
	return 0;

}
module_init(s5pv210_led_init);
module_exit(s5pv210_led_exit);

// MODULE_xxx这种宏作用是用来添加模块描述信息
MODULE_LICENSE("GPL");							// 描述模块的许可证
MODULE_AUTHOR("lupingguang");		// 描述模块的作者
MODULE_DESCRIPTION("s5pv210 platform_led driver");		// 描述模块的介绍信息
MODULE_ALIAS("s5pv210_led");					// 描述模块的别名信息








