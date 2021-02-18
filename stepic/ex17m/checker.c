#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/slab.h> // for kmallock
#include <linux/random.h>

#include "checker.h"
void call_me(const char* message)
{
	printk(KERN_INFO "call_me:%s",message);
}
EXPORT_SYMBOL(call_me);

int array_sum(short *arr, size_t n)
{
	int res = 0;
	int i;
	for(i = 0; i < n; i++)
	{
		res = res + arr[i];
	}
	return res;
}
EXPORT_SYMBOL(array_sum);

ssize_t generate_output(int sum, short *arr, size_t size, char *buf)
{
	ssize_t real_len = 0;
	real_len += sprintf(buf,"<%d>",sum);
	size_t i;
	for(i = 0; i < size; i++)
	{
		real_len += sprintf(buf + real_len,"<%d>",arr[i]);
	}
	return real_len;
}
EXPORT_SYMBOL(generate_output);

static int __init my_init(void)
{
	printk(KERN_INFO "INIT CHECKER");
	short arr[5] = {1,2,3,4,5};
	//-----size=-----<len int>-<len short> * size_arr
	size_t size_buf =2+5 +  (2 + 5) * 5; 
	char* buf = kmalloc(size_buf, GFP_KERNEL);
	generate_output(15,arr,5,buf);
	printk(KERN_INFO "%s\n",buf);
	kfree(buf);
	return 0;
}

static void __exit my_exit(void)
{
	printk(KERN_INFO "Bye from CHECKER");
}

module_init(my_init);
module_exit(my_exit);
