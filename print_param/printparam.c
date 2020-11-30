#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/sched.h>
static char *param_mess = "qwerty";
static int param_int_num = 4;
module_param(param_mess, charp, S_IRUGO);
module_param(param_int_num, int, S_IRUGO);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("El Puablo");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.04");

static int __init print_param_init(void)
{
	
	printk(KERN_INFO "Hellow, I'm going print some.. %d times\n", param_int_num);
	int i;
	for(i=0; i < param_int_num;i++)
	{
		printk(KERN_INFO "%s", param_mess);
	}
	return 0;
}
static void __exit print_param_exit(void)
{
 printk(KERN_INFO "Goodbye, World! My pid was %d\n", current->pid);
}
module_init(print_param_init);
module_exit(print_param_exit);
