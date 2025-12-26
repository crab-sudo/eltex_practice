#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/slab.h>




MODULE_LICENSE(     "GPL");
MODULE_AUTHOR(      "Your Name");
MODULE_DESCRIPTION( "Kernel module for procfs communication");
MODULE_VERSION(     "1.0");



#define MODULE_NAME     "datatrader_module"
#define PROC_DIR_NAME   "datatrader"
#define PROC_FILE_NAME  "data"
#define MAX_BUFFER_SIZE 1024




struct module_data {
    char    *buffer;
    size_t  buffer_size;
    struct  proc_dir_entry *proc_dir;
    struct  proc_dir_entry *proc_file;
};



static struct module_data *mod_data;





static int proc_show(   struct seq_file *m, 
                        void            *v){

    if (!mod_data || !mod_data->buffer)
        return 0;
    
    seq_printf(m, "Current data: %s\n", mod_data->buffer);
    seq_printf(m, "Buffer size: %zu\n", mod_data->buffer_size);
    return 0;
}





static int proc_open(   struct inode    *inode, 
                        struct file     *file){

    return single_open(file, proc_show, NULL);
}





static ssize_t proc_write(  struct file         *file, 
                            const char __user   *user_buffer,
                            size_t              count, 
                            loff_t              *ppos){

    size_t bytes_to_copy;
    
    if (!mod_data)
        return -ENODEV;
    
    bytes_to_copy = min_t(size_t, count, MAX_BUFFER_SIZE - 1);
    

    if (mod_data->buffer_size < bytes_to_copy + 1) {

        kfree(mod_data->buffer);
        mod_data->buffer        = kmalloc(bytes_to_copy + 1, GFP_KERNEL);
        if (!mod_data->buffer) {
            mod_data->buffer_size = 0;
            return -ENOMEM;
        }
        mod_data->buffer_size = bytes_to_copy + 1;
    }
    
 
    if (copy_from_user(mod_data->buffer, user_buffer, bytes_to_copy)) {
        return -EFAULT;
    }
    
    mod_data->buffer[bytes_to_copy] = '\0';
    
    pr_info("%s: Received %zu bytes: %s\n", MODULE_NAME, bytes_to_copy, mod_data->buffer);
    
    return bytes_to_copy;
}






#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
static const struct proc_ops proc_fops = {
    .proc_open = proc_open,
    .proc_read = seq_read,
    .proc_write = proc_write,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
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






static int __init proc_comm_module_init(void)
{
    int ret = 0;
    
    pr_info("%s: Initializing module\n", MODULE_NAME);
    

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
    
    strcpy(mod_data->buffer, "Hello from kernel module!");
    
 
    mod_data->proc_dir = proc_mkdir(PROC_DIR_NAME, NULL);
    if (!mod_data->proc_dir) {

        pr_err("%s: Failed to create proc directory\n", MODULE_NAME);
        ret = -ENOMEM;
        goto cleanup_buffer;
    }
    

    mod_data->proc_file = proc_create(  PROC_FILE_NAME, 
                                        0666, 
                                        mod_data->proc_dir, 
                                        &proc_fops);
    if (!mod_data->proc_file) {
        
        pr_err("%s: Failed to create proc file\n", MODULE_NAME);
        ret = -ENOMEM;
        goto cleanup_dir;
    }
    
    pr_info("%s: Module initialized successfully\n", MODULE_NAME);
    pr_info("%s: Proc interface: /proc/%s/%s\n", 
                MODULE_NAME, 
                PROC_DIR_NAME, 
                PROC_FILE_NAME);
    
    return 0;

cleanup_dir:
    proc_remove(mod_data->proc_dir);
    
cleanup_buffer:
    kfree(mod_data->buffer);
    kfree(mod_data);
    mod_data = NULL;
    return ret;
}






static void __exit proc_comm_module_exit(void)
{
    pr_info("%s: Removing module\n", MODULE_NAME);
    
    if (mod_data) {
        if (mod_data->proc_file)    proc_remove(mod_data->proc_file);
        
        if (mod_data->proc_dir)     proc_remove(mod_data->proc_dir);
        
        if (mod_data->buffer)       kfree(mod_data->buffer);
        
        kfree(mod_data);
        mod_data = NULL;
    }
    
    pr_info("%s: Module removed\n", MODULE_NAME);
}



module_init(proc_comm_module_init);
module_exit(proc_comm_module_exit);