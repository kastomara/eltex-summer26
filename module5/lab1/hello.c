#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("Увы");
MODULE_AUTHOR("Астанин Артём");
MODULE_DESCRIPTION("Сделал модуль ядра для первого задания по новому модулю. Модуль модуль модуль");

static int __init hello_init(void){
	printk(KERN_INFO "Hello world!\n");
	return 0;
}

static void __exit hello_terminate(void){
	printk(KERN_INFO "Cleaning up module\n"); 
}

module_init(hello_init);
module_exit(hello_terminate);

