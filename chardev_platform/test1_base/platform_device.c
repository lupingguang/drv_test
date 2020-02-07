
#include <linux/platform_device.h>
#include <linux/ioport.h>
#include "platform_common.h"
//#include <linux/init.h>
//#include <linux/fs.h>
//#include <asm/uaccess.h>
//#include <linux/wait.h>
//#include <linux/semaphore.h>
//#include <linux/sched.h>
//#include <linux/cdev.h>
//#include <linux/types.h>
//#include <linux/kdev_t.h>
//#include <linux/device.h>

static int platform_device_init(void);
static int platform_device_exit(void);
static int platform_device_dummy_release(void);

//实例化led1、led2的自定义设备附加数据结构体
static struct s5pv210_led_platdata s5pv210_led1_pdata = {
	.name		= "led1",
	.gpio		= 4,  //led1 gpio num
};
static struct s5pv210_led_platdata s5pv210_led2_pdata = {
	.name		= "led2",
	.gpio		= 5,  //led1 gpio num
};

//实例化led1、led2的板级数据结构体
static struct resource s5pv210_led1_resource[] = {
	[0] = {
		.start = 0x1000,
		.end   = 0x2000,
		.flags  = IORESOURCE_MEM
	      },
};
static struct resource s5pv210_led2_resource[] = {
	[0] = {
		.start = 0x8000,
		.end   = 0x9000,
		.flags  = IORESOURCE_MEM
	      },
};
//实例化led1、led2的设备结构体
static struct platform_device s5pv210_led1 = {
	.name		= "lpg_led",
	.id		= 0,
	.num_resources   = ARRAY_SIZE(s5pv210_led1_resource),
	.resource       = s5pv210_led1_resource,
	.dev		= {
				.platform_data	= &s5pv210_led1_pdata,
				.release        = platform_device_dummy_release,
	},
};
static struct platform_device s5pv210_led2 = {
	.name		= "lpg_led",
	.id		= 1,
	.num_resources   = ARRAY_SIZE(s5pv210_led2_resource),
	.resource       = s5pv210_led2_resource,
	.dev		= {
				.platform_data	= &s5pv210_led2_pdata,
				.release        = platform_device_dummy_release,
	},
};

//此处将设备注册借助insmod加载执行，实际上，platform_device的注册需要修改为内核启动时自动注册或者热插拔自动注册
static int platform_device_init(void)
{
	platform_device_register(&s5pv210_led1);
	platform_device_register(&s5pv210_led2);
	printk("platform_device_init success\n");
	return 0;
}

static int platform_device_exit(void)
{
	platform_device_unregister(&s5pv210_led1);
	platform_device_unregister(&s5pv210_led2);
	printk("platform_device_exit success\n");
	return 0;
}

static int platform_device_dummy_release(void)
{
	printk("platform_device_relase success\n");
	return 0;
}

module_init(platform_device_init);

module_exit(platform_device_exit);

MODULE_LICENSE("GPL");


