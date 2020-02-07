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
int sub_init(void)
{
    printk("sub init");
    return 0;
}
void sub_exit(void)
{
    printk("sub exit");
}
//module_init(globalvar_init);
//module_exit(globalvar_exit);
MODULE_LICENSE("GPL");
