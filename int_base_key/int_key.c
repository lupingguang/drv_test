#include <linux/input.h> 
#include <linux/module.h> 
#include <linux/init.h>
#include <asm/irq.h> 
#include <asm/io.h>

#include <mach/irqs.h>			// arch/arm/mach-s5pv210/include/mach/irqs.h
#include <linux/interrupt.h>
#include <linux/gpio.h>

/* 实验时一定去掉内核对于button-x210的编译，否则按键中断就被别的模块劫走了
 * X210:
 *
 * POWER  -> EINT1   -> GPH0_1
 * LEFT   -> EINT2   -> GPH0_2 use this
 * DOWN   -> EINT3   -> GPH0_3
 * UP     -> KP_COL0 -> GPH2_0
 * RIGHT  -> KP_COL1 -> GPH2_1
 * MENU   -> KP_COL3 -> GPH2_3 (KEY_A)
 * BACK   -> KP_COL2 -> GPH2_2 (KEY_B)
 */
//中断相关
#define BUTTON_IRQ	IRQ_EINT2
//定义input设备
static struct input_dev *button_dev;
//决定使用tasklet上报还是wq上报
static  unsigned int report_swith = 0;
//tasklet和底半部函数，并关联
void key_int_do_tasklet(unsigned long);
DECLARE_TASKLET(key_tasklet,key_int_do_tasklet,11);


void key_int_do_tasklet(unsigned long what)
{
    	int flag;

	s3c_gpio_cfgpin(S5PV210_GPH0(2), S3C_GPIO_SFN(0x0));		// input模式
	flag = gpio_get_value(S5PV210_GPH0(2));
	s3c_gpio_cfgpin(S5PV210_GPH0(2), S3C_GPIO_SFN(0x0f));		// eint2模式
	printk("key-tasklet scheduled value =%d, what=%d\n",flag,what);
	
	input_report_key(button_dev, KEY_LEFT, !flag);
	input_sync(button_dev);
}

//工作队列相关

void key_int_do_work(unsigned long);

DECLARE_WORK(key_wq,key_int_do_work);

void key_int_do_work(unsigned long what)

{

    	int flag;

	s3c_gpio_cfgpin(S5PV210_GPH0(2), S3C_GPIO_SFN(0x0));		// input模式
	flag = gpio_get_value(S5PV210_GPH0(2));
	s3c_gpio_cfgpin(S5PV210_GPH0(2), S3C_GPIO_SFN(0x0f));		// eint2模式
	printk("key-workqueue scheduled value =%d, what=%d\n",flag,what);
	
	input_report_key(button_dev, KEY_LEFT, !flag);
	input_sync(button_dev);
}

static irqreturn_t button_interrupt(int irq, void *dummy) 
{ 


	printk("irq =%d\n",irq);
        
	//tasklet_schedule(&key_tasklet);    //调度tasklet       
	schedule_work(&key_wq);    //调度工作队列
	return IRQ_HANDLED; 
}

static int __init button_init(void) 
{ 
	int error;
        
        INIT_WORK(&key_wq,key_int_do_work);	

	gpio_free(S5PV210_GPH0(2));
	
	error = gpio_request(S5PV210_GPH0(2), "GPH0_2");

	if(error)
		printk("key-s5pv210: request gpio GPH0(2) fail");
	s3c_gpio_cfgpin(S5PV210_GPH0(2), S3C_GPIO_SFN(0x0f));		// eint2模式
	
	if (request_irq(BUTTON_IRQ, button_interrupt, IRQF_TRIGGER_FALLING, "button-x210", NULL)) 
	{ 
		printk(KERN_ERR "key-s5pv210.c: Can't allocate irq %d\n", BUTTON_IRQ);
		return -EBUSY; 
	}
	button_dev = input_allocate_device();
	if (!button_dev) 
	{ 
		printk(KERN_ERR "key-s5pv210.c: Not enough memory\n");
		error = -ENOMEM;
		goto err_free_irq; 
	}
	
	button_dev->evbit[0] = BIT_MASK(EV_KEY);
	button_dev->keybit[BIT_WORD(KEY_LEFT)] = BIT_MASK(KEY_LEFT);
	
	error = input_register_device(button_dev);
	if (error) 
	{ 
		printk(KERN_ERR "key-s5pv210.c: Failed to register device\n");
		goto err_free_dev; 
	}
	return 0;
	
err_free_dev:
	input_free_device(button_dev);
err_free_irq:
	free_irq(BUTTON_IRQ, button_interrupt);
	return error; 
}

static void __exit button_exit(void) 
{ 
	input_unregister_device(button_dev); 
	free_irq(BUTTON_IRQ, button_interrupt); 
}

module_init(button_init); 
module_exit(button_exit); 

MODULE_LICENSE("GPL");
MODULE_AUTHOR("aston <1264671872@qq.com>");
MODULE_DESCRIPTION("key driver for x210 button.");
