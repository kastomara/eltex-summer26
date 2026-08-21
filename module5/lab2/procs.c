#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/slab.h>

#define COUNT_SIZE 10;
#define FILE_NAME "proc";

static int len,temp;
static char *msg;
 
static ssize_t read_proc(struct file *filp, char *buf, size_t count, loff_t *offp ) {
    if(count > temp) {
        count = temp;
    }
    temp = temp - count;
    copy_to_user(buf, msg, count);
    if(count == 0)
        temp = len;
    return count;
}
 
static ssize_t write_proc(struct file *filp, const char *buf, size_t count, loff_t *offp) {
    copy_from_user(msg, buf, count);
    len = count;
    temp = len;
    return count;
}
 
static const struct proc_ops proc_fops = {
    proc_read: read_proc,
    proc_write: write_proc,
};
 
static void create_new_proc_entry(void) {
    proc_create(FILE_NAME, 0, NULL, &proc_fops);
    msg = kmalloc(COUNT_SIZE * sizeof(char), GFP_KERNEL);
}
 
static int __init proc_init (void) {
    create_new_proc_entry();
    return 0;
}
 
static void __exit proc_cleanup(void) {
    remove_proc_entry("hello", NULL);
    kfree(msg);
}

module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("Увы");
MODULE_AUTHOR("Астанин Артём");
MODULE_DESCRIPTION("Сделал модуль ядра для первого задания по новому модулю. Модуль модуль модуль");