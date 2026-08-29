#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/init.h>
#include <linux/tty.h>
#include <linux/kd.h>
#include <linux/vt.h>
#include <linux/console_struct.h>
#include <linux/vt_kern.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

static struct kobject *example_kobject;
static struct timer_list my_timer;
static int test = 0;
static int _kbledstatus = 0;

#define BLINK_DELAY (HZ / 5)
#define RESTORE_LEDS 0xFF

static void set_leds(int state)
{
    struct tty_driver *my_driver;
    
    if (vc_cons[fg_console].d && vc_cons[fg_console].d->port.tty) {
        my_driver = vc_cons[fg_console].d->port.tty->driver;
        if (my_driver && my_driver->ops->ioctl) {
            (my_driver->ops->ioctl)(vc_cons[fg_console].d->port.tty, KDSETLED, state);
        }
    }
}

static void my_timer_fun(struct timer_list *t) {
    int *pstatus = &_kbledstatus;
    if (*pstatus == test) {
        *pstatus = RESTORE_LEDS;
    } else {
        *pstatus = test;
    }
    set_leds(*pstatus);

    if (test >= 1 && test <= 7) {
        mod_timer(&my_timer, jiffies + BLINK_DELAY);
    }
}

static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", test);
}

static ssize_t foo_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    if (sscanf(buf, "%du", &test) == 1) {
        if (test >= 1 && test <= 7) {
            set_leds(test);
            /* mod_timer(&my_timer, jiffies + BLINK_DELAY);*/
        } else {
            timer_delete_sync(&my_timer);
            set_leds(RESTORE_LEDS);
        }
    }
    return count;
}

static struct kobj_attribute foo_attribute = __ATTR(test, 0660, foo_show, foo_store);

static int __init sys_init(void)
{
    timer_setup(&my_timer, my_timer_fun, 0);
    
    example_kobject = kobject_create_and_add("systest", kernel_kobj);
    if (!example_kobject)
        return -ENOMEM;

    if (sysfs_create_file(example_kobject, &foo_attribute.attr)) {
        pr_err("Failed to create the test file in /sys/kernel/systest\n");
        kobject_put(example_kobject);
        return -EINVAL;
    }

    pr_info("LED module initialized successfully\n");
    return 0;
}

static void __exit sys_exit(void)
{
    timer_delete_sync(&my_timer);
    set_leds(RESTORE_LEDS);
    kobject_put(example_kobject);
    pr_info("LED module unloaded successfully\n");
}

module_init(sys_init);
module_exit(sys_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Астанин Артём");
MODULE_DESCRIPTION("Минималистичный модуль ядра для управления LED");