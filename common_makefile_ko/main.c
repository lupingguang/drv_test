/*
参考：深入浅出linux设备驱动开发
*/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/wait.h>
#include <linux/semaphore.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/device.h>

#include <sub.h>

//内核模块的初始化
static int globalvar_init(void)
{
    printk(KERN_ERR"this is main init");
    sub_init();
    return 0;
}
static void globalvar_exit(void)
{
    sub_exit();
    printk("this is main exit");
}
module_init(globalvar_init);
module_exit(globalvar_exit);
MODULE_LICENSE("GPL");
