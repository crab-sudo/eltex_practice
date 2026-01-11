#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/version.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Kernel module for chardev communication");
MODULE_VERSION("1.0");

#define MODULE_NAME     "datrachar_module"
#define CLASS_NAME      "datrachar_class"
#define DEVICE_NAME     "datrachar"
#define MAX_BUFFER_SIZE 1024
#define MINOR_COUNT     1
#define FIRST_MINOR     0



struct module_data {
    char    *buffer;
    size_t  buffer_size;
    size_t  data_length;
    struct  cdev cdev;
    dev_t   dev_num;
    struct  class *dev_class;
    struct  device *dev_device;
};



static struct module_data *mod_data;
static int major_num = 0;





static int dev_open(struct inode *inodep, struct file *filep)
{
    pr_info("%s: Device opened\n", MODULE_NAME);
    return 0;
}





static int dev_release(struct inode *inodep, struct file *filep)
{
    pr_info("%s: Device closed\n", MODULE_NAME);
    return 0;
}





static ssize_t dev_read(struct file *filep, 
                        char __user *buffer,
                        size_t      len, 
                        loff_t      *offset)
{
    size_t bytes_to_read;
    
    if (!mod_data || !mod_data->buffer)
        return -ENODEV;
    
    if (*offset >= mod_data->data_length)
        return 0;
    
    bytes_to_read = min(len, mod_data->data_length - *offset);
    
    if (copy_to_user(buffer, mod_data->buffer + *offset, bytes_to_read)) {

        pr_err("%s: Failed to copy data to user\n", MODULE_NAME);
        return -EFAULT;
    }
    
    *offset += bytes_to_read;
    pr_info("%s: Sent %zu bytes to user\n", MODULE_NAME, bytes_to_read);
    
    return bytes_to_read;
}





static ssize_t dev_write(   struct file *filep, 
                            const char  __user *buffer,
                            size_t      len, 
                            loff_t      *offset)
{
    size_t bytes_to_copy;
    
    if (!mod_data)      return -ENODEV;
    
    bytes_to_copy = min_t(size_t, len, MAX_BUFFER_SIZE - 1);
    
    if (mod_data->buffer_size < bytes_to_copy + 1) {

        kfree(mod_data->buffer);
        mod_data->buffer = kmalloc(bytes_to_copy + 1, GFP_KERNEL);
        if (!mod_data->buffer) {

            mod_data->buffer_size = 0;
            mod_data->data_length = 0;
            return -ENOMEM;
        }
        mod_data->buffer_size = bytes_to_copy + 1;
    }
    
    if (copy_from_user(mod_data->buffer, buffer, bytes_to_copy)) {

        pr_err("%s: Failed to copy data from user\n", MODULE_NAME);
        return -EFAULT;
    }
    
    mod_data->buffer[bytes_to_copy] = '\0';
    mod_data->data_length           = bytes_to_copy;
    
    pr_info("%s: Received %zu bytes: %s\n", MODULE_NAME, bytes_to_copy, mod_data->buffer);
    
    return bytes_to_copy;
}




static struct file_operations fops = {
    .owner      = THIS_MODULE,
    .open       = dev_open,
    .read       = dev_read,
    .write      = dev_write,
    .release    = dev_release,
};






static int __init chardev_module_init(void)
{
    int status = 0;
    pr_info("%s: Initializing chardev module\n", MODULE_NAME);
    

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
    

    status = alloc_chrdev_region(   &mod_data->dev_num, 
                                    FIRST_MINOR, 
                                    MINOR_COUNT, 
                                    DEVICE_NAME);
    if (status < 0) {
        pr_err("%s: Failed to allocate char device region\n", MODULE_NAME);
        
        kfree(mod_data->buffer);
        kfree(mod_data);
        mod_data = NULL;
        return status;
    }
    
    major_num = MAJOR(mod_data->dev_num);
    pr_info("%s: Allocated major number %d\n", MODULE_NAME, major_num);
    

    cdev_init(&mod_data->cdev, &fops);
    mod_data->cdev.owner = THIS_MODULE;
    

    status = cdev_add(&mod_data->cdev, mod_data->dev_num, MINOR_COUNT);
    if (status < 0) {
        pr_err("%s: Failed to add char device\n", MODULE_NAME);
        unregister_chrdev_region(mod_data->dev_num, MINOR_COUNT);
    }
    

    mod_data->dev_class = class_create(CLASS_NAME);
    if (IS_ERR(mod_data->dev_class)) {

        pr_err("%s: Failed to create device class\n", MODULE_NAME);
        status = PTR_ERR(mod_data->dev_class);
        cdev_del(&mod_data->cdev);
    }
    

    mod_data->dev_device = device_create(   mod_data->dev_class, 
                                            NULL,
                                            mod_data->dev_num, 
                                            NULL, 
                                            DEVICE_NAME);
    if (IS_ERR(mod_data->dev_device)) {

        pr_err("%s: Failed to create device\n", MODULE_NAME);
        status = PTR_ERR(mod_data->dev_device);
        class_destroy(mod_data->dev_class);
    }
    
    pr_info("%s: Module initialized successfully\n", MODULE_NAME);
    pr_info("%s: Device created: /dev/%s (major: %d, minor: %d)\n",
            MODULE_NAME, DEVICE_NAME, major_num, FIRST_MINOR);
    
    return 0;
}






static void __exit chardev_module_exit(void)
{
    pr_info("%s: Removing chardev module\n", MODULE_NAME);
    
    if (mod_data) {
        if (mod_data->dev_device) {
            device_destroy(mod_data->dev_class, mod_data->dev_num);
        }
        
        if (mod_data->dev_class) {
            class_destroy(mod_data->dev_class);
        }
        
        cdev_del(&mod_data->cdev);
        
        unregister_chrdev_region(mod_data->dev_num, MINOR_COUNT);
        
        if (mod_data->buffer) {
            kfree(mod_data->buffer);
        }
        
        kfree(mod_data);
        mod_data = NULL;
    }
    
    pr_info("%s: Module removed\n", MODULE_NAME);
}




module_init(chardev_module_init);
module_exit(chardev_module_exit);

// tomato tomato tomato tomato 