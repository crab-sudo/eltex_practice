#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>

MODULE_LICENSE      ("GPL");
MODULE_AUTHOR       ("Evgeniy");
MODULE_DESCRIPTION  ("Kernel module for netlink communication");
MODULE_VERSION      ("1.0");

#define MODULE_NAME     "datatrader_netlink"
#define NETLINK_USER    31
#define MAX_BUFFER_SIZE 1024



struct module_data {
    char        *buffer;
    size_t      buffer_size;
    size_t      data_length;
    struct sock *nl_sock;
    u32         pid;
};



static struct module_data *mod_data;
static int netlink_id = NETLINK_USER;



static void netlink_recv_msg(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    struct sk_buff  *skb_out;
    int             msg_size;
    char            *msg;
    int             status;
    
    if (!skb) {
        pr_err("%s: Received NULL skb\n", MODULE_NAME);
        return;
    }
    
    nlh = (struct nlmsghdr *)skb->data;
    
    if (!nlh) {
        pr_err("%s: Received NULL nlmsghdr\n", MODULE_NAME);
        return;
    }
    
    mod_data->pid   = nlh->nlmsg_pid;
    msg             = (char *)NLMSG_DATA(nlh);
    msg_size        = NLMSG_PAYLOAD(nlh, 0);
    
    pr_info("%s: Received %d bytes from pid %u: %s\n", 
            MODULE_NAME, msg_size, mod_data->pid, msg);
    

    if (!mod_data) {
        pr_err("%s: Module data not initialized\n", MODULE_NAME);
        return;
    }
    

    if (mod_data->buffer_size < msg_size + 1) {
        kfree(mod_data->buffer);
        mod_data->buffer = kmalloc(msg_size + 1, GFP_KERNEL);
        if (!mod_data->buffer) {
            
            mod_data->buffer_size = 0;
            mod_data->data_length = 0;
            pr_err("%s: Failed to allocate buffer\n", MODULE_NAME);
            return;
        }
        mod_data->buffer_size = msg_size + 1;
    }
    

    memcpy(mod_data->buffer, msg, msg_size);
    mod_data->buffer[msg_size]  = '\0';
    mod_data->data_length       = msg_size;
    

    if (mod_data->data_length > 0) {
        skb_out = nlmsg_new(mod_data->data_length, 0);
        if (!skb_out) {
            pr_err("%s: Failed to allocate skb\n", MODULE_NAME);
            return;
        }
        
        nlh = nlmsg_put(skb_out, 0, 0, 
                        NLMSG_DONE, 
                        mod_data->data_length, 
                        0);
        NETLINK_CB(skb_out).dst_group   = 0;
        
        strncpy(nlmsg_data(nlh), mod_data->buffer, mod_data->data_length);
        

        status = nlmsg_unicast(mod_data->nl_sock, skb_out, mod_data->pid);
        if (status < 0) {
            pr_err("%s: Failed to send message\n", MODULE_NAME);
        } else {
            pr_info("%s: Sent %zu bytes to user\n", 
                    MODULE_NAME, mod_data->data_length);
        }
    }
}





static struct netlink_kernel_cfg nl_cfg = {
    .input = netlink_recv_msg,
};






static int __init netlink_module_init(void)
{
    pr_info("%s: Initializing netlink module\n", MODULE_NAME);
    

    mod_data = kmalloc(sizeof(*mod_data), GFP_KERNEL);
    if (!mod_data) {
        pr_err("%s: Failed to allocate module data\n", MODULE_NAME);
        return -ENOMEM;
    }
    memset(mod_data, 0, sizeof(*mod_data));
    

    mod_data->buffer_size   = 32;
    mod_data->buffer        = kmalloc(mod_data->buffer_size, GFP_KERNEL);
    if (!mod_data->buffer) {

        kfree(mod_data);
        pr_err("%s: Failed to allocate buffer\n", MODULE_NAME);
        return -ENOMEM;
    }
    strcpy(mod_data->buffer, "Init msg!");
    mod_data->data_length = strlen(mod_data->buffer);
    

    mod_data->nl_sock = netlink_kernel_create(&init_net, 
                                              netlink_id, 
                                              &nl_cfg);
    if (!mod_data->nl_sock) {
        pr_err("%s: Failed to create netlink socket\n", MODULE_NAME);
        
        kfree(mod_data->buffer);
        kfree(mod_data);
        mod_data = NULL;
        return -ENOMEM;
    }
    
    pr_info("%s: Netlink socket created with id %d\n", 
            MODULE_NAME, netlink_id);
    
    pr_info("%s: Module initialized successfully\n", MODULE_NAME);
    pr_info("%s: Use netlink socket %d for communication\n",
            MODULE_NAME, netlink_id);
    
    return 0;
}






static void __exit netlink_module_exit(void)
{
    pr_info("%s: Removing netlink module\n", MODULE_NAME);
    
    if (mod_data) {
        if (mod_data->nl_sock) {
            netlink_kernel_release(mod_data->nl_sock);
        }
        
        if (mod_data->buffer) {
            kfree(mod_data->buffer);
        }
        
        kfree(mod_data);
        mod_data = NULL;
    }
    
    pr_info("%s: Module removed\n", MODULE_NAME);
}




module_init(netlink_module_init);
module_exit(netlink_module_exit);

// tomato