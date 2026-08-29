#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/proc_fs.h>
#include <linux/inet.h>
#include <linux/netlink.h>
#include <net/sock.h>

#define MAX_IPS 10
static __be32 blacklist[MAX_IPS];
static int ip_count = 0;
static DEFINE_SPINLOCK(lock);
static char FILE_NAME = "ip_filter"

static struct proc_dir_entry *proc_file;
static struct nf_hook_ops nfho;
static struct sock *nl_sk;

// --- ОТПРАВКА СООБЩЕНИЯ ЧЕРЕЗ NETLINK ---
static void send_to_user(char *msg)
{
    struct sk_buff *skb;
    struct nlmsghdr *nlh;
    int len = strlen(msg) + 1;

    if (!nl_sk) return;

    skb = nlmsg_new(len, 0);
    if (!skb) return;

    nlh = nlmsg_put(skb, 0, 0, NLMSG_DONE, len, 0);
    strcpy(nlmsg_data(nlh), msg);

    // Отправляем всем подписчикам 1-й группы
    nlmsg_multicast(nl_sk, skb, 0, 1, GFP_ATOMIC);
}

// --- ФИЛЬТРАЦИЯ ПАКЕТОВ (NETFILTER) ---
static unsigned int hook_func(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
    struct iphdr *iph = ip_hdr(skb);
    int i;
    char msg[64];

    if (!iph) return NF_ACCEPT;

    spin_lock(&lock);
    for (i = 0; i < ip_count; i++) {
        if (iph->daddr == blacklist[i]) {
            snprintf(msg, sizeof(msg), "Block: %pI4", &iph->daddr);
            send_to_user(msg);
            spin_unlock(&lock);
            return NF_DROP;
        }
    }
    spin_unlock(&lock);
    return NF_ACCEPT;
}

// --- ЧТЕНИЕ ИЗ /proc/ip_filter ---
static ssize_t proc_read(struct file *file, char __user *ubuf, size_t count, loff_t *ppos)
{
    char buf[256] = {0};
    int len = 0, i;

    if (*ppos > 0) return 0;

    spin_lock(&lock);
    for (i = 0; i < ip_count; i++) {
        len += snprintf(buf + len, sizeof(buf) - len, "%pI4\n", &blacklist[i]);
    }
    spin_unlock(&lock);

    if (copy_to_user(ubuf, buf, len)) return -EFAULT;
    *ppos = len;
    return len;
}

// --- ЗАПИСЬ В /proc/ip_filter ---
static ssize_t proc_write(struct file *file, const char __user *ubuf, size_t count, loff_t *ppos)
{
    char buf[32];
    char action;
    __be32 ip;
    int i;

    if (count >= sizeof(buf) || copy_from_user(buf, ubuf, count)) return -EFAULT;
    buf[count] = '\0';
    action = buf[0];

    // Конвертируем строку в IP, начиная со 2-го символа
    if (in4_pton(buf + 1, -1, (u8 *)&ip, -1, NULL) == 0) return -EINVAL;

    spin_lock(&lock);
    if (action == '+') {
        // Добавление в массив
        if (ip_count < MAX_IPS) blacklist[ip_count++] = ip;
    } else if (action == '-') {
        // Удаление из массива (заменяем удаляемый элемент последним)
        for (i = 0; i < ip_count; i++) {
            if (blacklist[i] == ip) {
                blacklist[i] = blacklist[--ip_count];
                break;
            }
        }
    }
    spin_unlock(&lock);

    return count;
}

static const struct proc_ops proc_fops = {
    .proc_read = proc_read,
    .proc_write = proc_write,
};

static int __init filter_init(void)
{
    struct netlink_kernel_cfg cfg = { .groups = 1 };
    
    proc_file = proc_create(FILE_NAME, 0666, NULL, &proc_fops);
    nl_sk = netlink_kernel_create(&init_net, 31, &cfg);

    nfho.hook = hook_func;
    nfho.hooknum = NF_INET_LOCAL_OUT;
    nfho.pf = PF_INET;
    nfho.priority = NF_IP_PRI_FIRST;
    nf_register_net_hook(&init_net, &nfho);

    return 0;
}

static void __exit filter_exit(void)
{
    nf_unregister_net_hook(&init_net, &nfho);
    netlink_kernel_release(nl_sk);
    proc_remove(proc_file);
}

module_init(filter_init);
module_exit(filter_exit);
MODULE_LICENSE("GPL");