#include <linux/init.h>                                                                                                                                                                                                                            
#include <linux/module.h>                                                                                                                                                                                                                          
#include <linux/kernel.h>                                                                                                                                                                                                                          
#include <linux/random.h>                                                                                                                                                                                                                          
#include <linux/slab.h>                                                                                                                                                                                                                            
#include "checker.h"                                                                                                                                                                                                                               
                                                                                                                                                                                                                                                   
#define MIN_SIZE 5                                                                                                                                                                                                                                 
#define MAX_SIZE 10                                                                                                                                                                                                                                
                                                                                                                                                                                                                                                   
static int my_arr_sum(short* arr, size_t n)                                                                                                                                                                                                        
{                                                                                                                                                                                                                                                  
        if(arr)                                                                                                                                                                                                                                    
        {                                                                                                                                                                                                                                          
        int res = 0;                                                                                                                                                                                                                               
        size_t i = 0;                                                                                                                                                                                                                              
                for( i = 0; i < n; i++)                                                                                                                                                                                                            
                {                                                                                                                                                                                                                                  
                        res = res + arr[i];                                                                             
                }                                                                                                       
                return res;                                                                                             
        }                                                                                                               
        return 0;                                                                                                       

}

static int __init my_init(void)
{
        unsigned char times;
        unsigned char size_arr;
        short *arr;
        //get_random_bytes(&times, sizeof(unsigned char));
        times = 10;
        if(times < 10)
        {
                times += 10;
        }

        unsigned char i;
        for( i = 0; i < times; i++)
        {
         //       printk(KERN_INFO "time=%d\n",i);
                //get_random_bytes(&size_arr, sizeof(unsigned char));
                size_arr = 5;
                arr = kmalloc(size_arr, GFP_KERNEL);
                get_random_bytes(arr, size_arr * sizeof(short));
                int sum = array_sum(arr, size_arr);

                int my_sum = my_arr_sum(arr, size_arr);
                printk(KERN_INFO "my_sum=%d",my_sum);
                //-----size=-----<len int>-<len short> * size_arr
                size_t size_buf =2+5 +  (2 + 5) * size_arr;
                char* buf = kmalloc(size_buf, GFP_KERNEL);
                ssize_t res = generate_output(sum,arr,size_arr,buf);
                if(sum == my_sum)
                {
                        printk(KERN_INFO "%s\n",buf);
                }
                else
                {
                        printk(KERN_ERR "%s\n",buf);
                }
                kfree(arr);
                kfree(buf);

        }


        return 0;
}

static void __exit my_exit(void)
{
        printk(KERN_INFO "Bye from Export Symbol 1 module.");
}

module_init(my_init);
module_exit(my_exit);

