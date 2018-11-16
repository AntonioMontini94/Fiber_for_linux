#include <linux/init.h>           // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <linux/uaccess.h>          // Required for the copy to user function
#include <linux/list.h>
#include <linux/slab.h>
#define  DEVICE_NAME "fiber"    ///< The device will appear at /dev/fiber using this value
#define  CLASS_NAME  "fibers"        ///< The device class -- this is a character device driver
 
MODULE_LICENSE("GPL");            ///< The license type -- this affects available functionality
MODULE_AUTHOR("Anto & Marco");    ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("AOSV Project");  ///< The description -- see modinfo
MODULE_VERSION("0.1");            ///< A version number to inform users
 
static int    majorNumber;                  ///< Stores the device number -- determined automatically
static char   message[256] = {0};           ///< Memory for the string that is passed from userspace
static short  size_of_message;              ///< Used to remember the size of the string stored
static int    numberOpens = 0;              ///< Counts the number of times the device is opened
static struct class*  fiberClass  = NULL; ///< The device-driver class struct pointer
static struct device* fiberDevice = NULL; ///< The device-driver device struct pointer
 
// The prototype functions for the character driver -- must come before the struct definition
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);
static long    dev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

//command
#define CONVERT_THREAD 1


struct fiber_t {
   //contesto
   //stack
   //total_execution_time
   int fiber_id;
   short active;
   int num_activation;
   int num_activation_fail;
   void* (function);
   void* arg_function;
   long thread_id;
   long process_id;
   struct list_head list;
};

struct fiber_t fiber_list;

static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
   .unlocked_ioctl = dev_ioctl,
};

struct arg_device{
   long thread_id;
   long process_id;
   void* arg_function;
};
 
/** @brief The LKM initialization function
 *  The static keyword restricts the visibility of the function to within this C file. The __init
 *  macro means that for a built-in driver (not a LKM) the function is only used at initialization
 *  time and that it can be discarded and its memory freed up after that point.
 *  @return returns 0 if successful
 */
static int __init fiber_init(void){
   printk(KERN_INFO "Fiber: Initializing Fiber module\n");

   INIT_LIST_HEAD(&fiber_list.list);
 
   // Try to dynamically allocate a major number for the device -- more difficult but worth it
   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   if (majorNumber<0){
      printk(KERN_ALERT "Fiber failed to register a major number\n");
      return majorNumber;
   }
   printk(KERN_INFO "Fiber: registered correctly with major number %d\n", majorNumber);
 
   // Register the device class
   fiberClass = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(fiberClass)){                // Check for error and clean up if there is
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Fiber: Failed to register device class\n");
      return PTR_ERR(fiberClass);          // Correct way to return an error on a pointer
   }
   printk(KERN_INFO "Fiber: device class registered correctly\n");
 
   // Register the device driver
   fiberDevice = device_create(fiberClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(fiberDevice)){               // Clean up if there is an error
      class_destroy(fiberClass);           // Repeated code but the alternative is goto statements
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to create the device\n");
      return PTR_ERR(fiberDevice);
   }
   printk(KERN_INFO "Fiber: device class created correctly\n");
   return 0;
}
 
/** @brief The LKM cleanup function
 *  Similar to the initialization function, it is static. The __exit macro notifies that if this
 *  code is used for a built-in driver (not a LKM) that this function is not required.
 */
static void __exit fiber_exit(void){

   struct fiber_t* node;
   struct fiber_t* tmp;

   list_for_each_entry_safe(node,tmp,&fiber_list.list,list){
      printk(KERN_INFO "node free %d", node->fiber_id);
      list_del(&node->list);
      kfree(node);
   }

   device_destroy(fiberClass, MKDEV(majorNumber, 0));     // remove the device
   class_unregister(fiberClass);                          // unregister the device class
   class_destroy(fiberClass);                             // remove the device class
   unregister_chrdev(majorNumber, DEVICE_NAME);           // unregister the major number
   printk(KERN_INFO "Fiber: Goodbye from fiber!\n");
}
 
/** @brief The device open function that is called each time the device is opened
 *  This will only increment the numberOpens counter in this case.
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_open(struct inode *inodep, struct file *filep){
   numberOpens++;
   printk(KERN_INFO "Fiber: Device has been opened %d time(s)\n", numberOpens);
   return 0;
}
 
/** @brief This function is called whenever device is being read from user space i.e. data is
 *  being sent from the device to the user. In this case is uses the copy_to_user() function to
 *  send the buffer string to the user and captures any errors.
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 *  @param buffer The pointer to the buffer to which this function writes the data
 *  @param len The length of the b
 *  @param offset The offset if required
 */
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
   int error_count = 0;
   // copy_to_user has the format ( * to, *from, size) and returns 0 on success
   error_count = copy_to_user(buffer, message, size_of_message);
 
   if (error_count==0){            // if true then have success
      printk(KERN_INFO "Fiber: Sent %d characters to the user\n", size_of_message);
      return (size_of_message=0);  // clear the position to the start and return 0
   }
   else {
      printk(KERN_INFO "Fiber: Failed to send %d characters to the user\n", error_count);
      return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
   }
}
 
/** @brief This function is called whenever the device is being written to from user space i.e.
 *  data is sent to the device from the user. The data is copied to the message[] array in this
 *  LKM using the sprintf() function along with the length of the string.
 *  @param filep A pointer to a file object
 *  @param buffer The buffer to that contains the string to write to the device
 *  @param len The length of the array of data that is being passed in the const char buffer
 *  @param offset The offset if required
 */
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
   sprintf(message, "%s(%zu letters)", buffer, len);   // appending received string with its length
   size_of_message = strlen(message);                 // store the length of the stored message
   printk(KERN_INFO "Fiber: Received %zu characters from the user\n", len);
   return len;
}
 
/** @brief The device release function that is called whenever the device is closed/released by
 *  the userspace program
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "Fiber: Device successfully closed\n");
   return 0;
}

static long dev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg){
   struct fiber_t* new_node;
   struct arg_device* arg_read;
   struct fiber_t* node;
   switch(cmd){
      case CONVERT_THREAD:
         arg_read = (struct arg_device*) arg;

         new_node = kmalloc(sizeof(*new_node),GFP_KERNEL);
         new_node -> fiber_id = 0;
         new_node -> active   = 1;
         new_node -> num_activation = 1;
         new_node -> num_activation_fail  = 0;
         new_node -> function = NULL;
         new_node -> arg_function = NULL;
         new_node -> thread_id =  arg_read->thread_id;
         new_node -> process_id = arg_read->process_id;

         INIT_LIST_HEAD(&new_node -> list);
         list_add_tail(&(new_node->list),&(fiber_list.list));

         list_for_each_entry(node,&fiber_list.list,list){
            printk(KERN_INFO "fiber_id =  %d, process_id =  %lu, thread_id = %lu", node->fiber_id, node->process_id, node->thread_id );
         }

         printk(KERN_INFO "Fiber: IOCTL command {%d}\n",cmd);
         break;
      default:
         printk(KERN_INFO "Fiber: IOCTL command {%d}\n",cmd);
         break;
   }
   return 0;
}
module_init(fiber_init);
module_exit(fiber_exit);