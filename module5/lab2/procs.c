#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/slab.h>

#define COUNT_SIZE 50
#define FILE_NAME "proc"

static int len,temp;
static char *msg;
 
static ssize_t read_proc(struct file *filp, char *buf, size_t count, loff_t *offp) {
    if(count > temp) {
        count = temp;
    }
    temp = temp - count;
    if(copy_to_user(buf, msg, count)) {
		return -EFAULT;
	}
    if(count == 0)
        temp = len;
    return count;
}
 
static ssize_t write_proc(struct file *filp, const char *buf, size_t count, loff_t *offp) {
	memset(msg, 0, COUNT_SIZE);
    if (count > COUNT_SIZE) {
		count = COUNT_SIZE;
	}
    if(copy_from_user(msg, buf, count)) {
		return -EFAULT;
	}
    msg[count] = '\0';
    len = count;
    temp = len;
    *offp = 0;
    return count;
}
 
static const struct proc_fops = {
    .proc_read = read_proc,
    .proc_write = write_proc,
};
 
static void create_new_proc_entry(void) {
    proc_create(FILE_NAME, 0660, NULL, &proc_fops);
    msg = kmalloc(COUNT_SIZE * sizeof(char), GFP_KERNEL);
//    if (msg = NULL) {
//        printk("Не удалось выделить запрашиваемый объем памяти");
//        break;
//    }
}
 
static int __init proc_init (void) {
    create_new_proc_entry();
    return 0;
}
 
static void __exit proc_cleanup(void) {
    remove_proc_entry(FILE_NAME, NULL);
    kfree(msg);
}

module_init(proc_init);
module_exit(proc_cleanup);

MODULE_LICENSE("Увы");
MODULE_AUTHOR("Астанин Артём");
MODULE_DESCRIPTION("Сделал модуль ядра для первого задания по новому модулю. Модуль модуль модуль");