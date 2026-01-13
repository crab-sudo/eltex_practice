#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/slab.h>
#include <linux/inet.h>

MODULE_LICENSE      ("GPL");
MODULE_AUTHOR       ("You");
MODULE_DESCRIPTION  ("Outgoing IP filter with /proc control");
MODULE_VERSION      ("1.0");

#define FILE_NAME "ip_blacklist"

struct ip_node {
    __be32 ip;
    struct list_head list;
};

static LIST_HEAD    (ip_blacklist);
static DEFINE_MUTEX (list_lock);



static unsigned int hook_func(void                          *priv,
                              struct sk_buff                *skb,
                              const struct nf_hook_state    *state)
{
    struct iphdr *ip;
    struct ip_node *node;

    if (!skb)
        return NF_ACCEPT;

    ip = ip_hdr(skb);
    if (!ip)
        return NF_ACCEPT;

    mutex_lock(&list_lock);
    list_for_each_entry(node, &ip_blacklist, list) {
        if (node->ip == ip->daddr) {

            mutex_unlock(&list_lock);
            return NF_DROP;
        }
    }
    mutex_unlock(&list_lock);

    return NF_ACCEPT;
}




static struct nf_hook_ops nfho = {
    .hook     = hook_func,
    .pf       = NFPROTO_IPV4,
    .hooknum  = NF_INET_POST_ROUTING,
    .priority = NF_IP_PRI_FIRST,
};




static ssize_t proc_read(   struct file *file, 
                            char __user *buf,
                            size_t      count, 
                            loff_t      *ppos)
{
    char *kbuf;
    int len = 0;
    struct ip_node *node;

    if (*ppos > 0)  return 0;

    kbuf = kmalloc(PAGE_SIZE, GFP_KERNEL);
    if (!kbuf)      return -ENOMEM;

    mutex_lock(&list_lock);
    list_for_each_entry(node, &ip_blacklist, list) {
        len += scnprintf(kbuf + len, PAGE_SIZE - len,
                         "%pI4\n", &node->ip);
    }
    mutex_unlock(&list_lock);

    if (copy_to_user(buf, kbuf, len)) {
        kfree(kbuf);
        return -EFAULT;
    }

    kfree(kbuf);
    *ppos = len;
    return len;
}




static ssize_t proc_write(struct file *file, const char __user *buf,
                          size_t count, loff_t *ppos)
{
    char kbuf[64];
    char cmd[8];
    char ip_str[32];
    __be32 ip;
    struct ip_node *node, *tmp;

    if (count >= sizeof(kbuf))
        return -EINVAL;

    if (copy_from_user(kbuf, buf, count))
        return -EFAULT;

    kbuf[count] = '\0';

    if (sscanf(kbuf, "%7s %31s", cmd, ip_str) != 2)
        return -EINVAL;

    ip = in_aton(ip_str);

    mutex_lock(&list_lock);

    if (!strcmp(cmd, "add")) {
        node = kmalloc(sizeof(*node), GFP_KERNEL);
        if (node) {
            node->ip = ip;
            list_add(&node->list, &ip_blacklist);
        }
    } else if (!strcmp(cmd, "del")) {
        list_for_each_entry_safe(node, tmp, &ip_blacklist, list) {
            if (node->ip == ip) {
                list_del(&node->list);
                kfree(node);
                break;
            }
        }
    }

    mutex_unlock(&list_lock);
    return count;
}




#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
static const struct proc_ops proc_fops = {
    .proc_read  = proc_read,
    .proc_write = proc_write,
};
#else
static const struct file_operations proc_fops = {
    .owner = THIS_MODULE,
    .open = proc_open,
    .read = seq_read,
    .write = proc_write,
    .llseek = seq_lseek,
    .release = single_release,
};
#endif

static struct proc_dir_entry *proc_file;




static void clear_list(void)
{
    struct ip_node *node, *tmp;

    mutex_lock(&list_lock);
    list_for_each_entry_safe(node, tmp, &ip_blacklist, list) {
        list_del(&node->list);
        kfree(node);
    }
    mutex_unlock(&list_lock);
}




static int __init ip_filter_init(void)
{
    int ret;

    proc_file = proc_create(FILE_NAME, 0666, NULL, &proc_fops);
    if (!proc_file)
        return -ENOMEM;

    ret = nf_register_net_hook(&init_net, &nfho);
    if (ret) {
        proc_remove(proc_file);
        return ret;
    }

    pr_info("ip_filter loaded\n");
    return 0;
}




static void __exit ip_filter_exit(void)
{
    nf_unregister_net_hook(&init_net, &nfho);
    proc_remove(proc_file);
    clear_list();
    pr_info("ip_filter unloaded\n");
}




module_init(ip_filter_init);
module_exit(ip_filter_exit);

// tomato tomato