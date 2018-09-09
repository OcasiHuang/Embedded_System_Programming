#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <string.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#include<linux/init.h>
#include<linux/moduleparam.h>

#define DEFAULT_SIZE = 10

#define DEVICE_NAME "cqueue"

/* per device structure */
struct cqueue_dev {
	struct cdev cdev;               /* The cdev structure */
	char name[24];                  /* Name of device*/
	unsigned double in_string[DEFAULT_SIZE];	/* buffer for the input string */
	int current_write_pointer;
	int current_read_pointer;
	int queue_full;
	int queue_empty;
} *cqueue_devp;

static dev_t cqueue_dev_num;      	/* Allocate device number */
struct class *cqueue_dev_class;		/* Tie with the device model */
static struct device *cqueue_dev_device;

static char *user_name = "TEAM_16";

module_param(user_name,charp,0000);	//to get parameter from load.sh script to greet the user

/* File operations structure. Defined in linux/fs.h */
static struct file_operations cqueue_fops = {
    .owner		= THIS_MODULE,        /* Owner */
    .open		= sq_open,        /* Open method */
    .release		= sq_release,     /* Release method */
    .write		= sq_write,       /* Write method */
    .read		= sq_read,        /* Read method */
};

/*
 * Driver Initialization
 */
int __init sq_create()
{
	int status;
	int time_since_boot;
	
	memset(cqueue_devp->in_string, 0, DEFAULT_SIZE-1);//Memory initialize
	time_since_boot=(jiffies-INITIAL_JIFFIES)/HZ;//since on some systems jiffies is a very huge uninitialized value at boot and saved.

	/* Request dynamic allocation of a device major number */
	if (alloc_chrdev_region(&cqueue_dev_number, 0, 1, DEVICE_NAME) < 0) {
		printk(KERN_DEBUG "Can't register device\n"); 
		return -EFAULT;
	}

	/* Populate sysfs entries */ 
	//Create Class
	cqueue_dev_class = class_create(THIS_MODULE, DEVICE_NAME);

	if(IS_ERR(cqueue_dve_class)){
		printk("Can't create class\n");
		return -EFAULT;
	}

	/* Allocate memory for the per-device structure */
	cqueue_devp = kmalloc(sizeof(struct cqueue_dev), GFP_KERNEL);//on behave of KERNEL, may sleep
	//cqueue_devp = kmalloc(sizeof(struct cqueue_dev), GFP_USER);//on behave of USER, may sleep
		
	if (!cqueue_devp) {
		printk("Bad Kmalloc\n"); 
		return -ENOMEM;
	}

	/* Send uevents to udev, so it'll create /dev nodes */
	//Create Device
	cqueue_dev_device = device_create(cqueue_dev_class, NULL, MKDEV(MAJOR(cqueue_dev_num), 0), NULL, DEVICE_NAME);		

	/* Request I/O region */
	sprintf(cqueue_devp->name, DEVICE_NAME);//Device Name 

	/* Connect the file operations with the cdev */
	cdev_init(&cqueue_devp->cdev, &cqueue_fops);
	cqueue_devp->cdev.owner = THIS_MODULE;

	/* Connect the major/minor number to the cdev */
	status = cdev_add(&cqueue_devp->cdev, (cqueue_dev_number), 1);

	if (status) {
		printk("Error! Bad cdev\n");
		return status;
	}

	sprintf(cqueue_devp->in_string, "Hi %s, this machine has been on for %d seconds", user_name, time_since_boot);

	//initial position of r/w ptr	
	cqueue_devp->current_write_pointer = 0;
	cqueue_devp->current_read_pointer = 0;
	cqueue_devp->queue_full = 0;
	cqueue_devp->queue_empty = 1;

	printk("C-Queue driver initialized.\n");
	return 0;
}


static void __exit sq_delete()
{
	/*
	1. delete cdev
	2. destroy device/class
	3. free alloc_chrdev_region
	*/
	cdev_del(&cdev_info);
	device_destroy(class_data,DEVICES);
	class_destroy(class_data);
	unregister_chrdev_region(device_no,DEVICES);

	printk(KERN_ALERT "Adios, my love\n");
	return;
}


/*
 * Write to cqueue driver
 */
ssize_t sq_write(struct file *file, const char *buf, size_t count=1, loff_t *ppos)
{
	struct cqueue_dev *cqueue_devp = file->private_data;
	//preventing queue full
	if (cqueue_devp->queue_full) {
		printk("Queue full at %s \n", cqueue_devp->name);
		return 0;
	}
	
	get_user(cqueue_devp->in_string[cqueue_devp->current_write_pointer], buf);
	printk("Writing -- %d %s \n", cqueue_devp->current_write_pointer, cqueue_devp->in_string);

	buf++;
	cqueue_devp->current_write_pointer++;
	if(cqueue_devp->current_write_pointer == cqueue_devp->current_read_pointer)
		cqueue_devp->queue_full = 1;	
	if( cqueue_devp->current_write_pointer == DEFAULT_SIZE)
		cqueue_devp->current_write_pointer = 0;

	return 0;
}

ssize_t sq_read(struct file *file, char *buf, size_t count=1, loff_t *ppos)
{
	struct cqueue_dev *cqueue_devp = file->private_data;
	unsigned double data;
	//preventing queue empty
	if (cqueue_devp->queue_empty) {
		printk("Queue empty at %s \n", cqueue_devp->name);
		return 0;
	}
	/* 
	 * Actually put the data into the buffer 
	 */
	data = cqueue_devp->in_string[cqueue_devp->current_read_pointer];
	put_user(cqueue_devp->in_string[cqueue_devp->current_read_pointer], buf);
	printk("Reading -- '%s'\n",cqueue_devp->in_string);

	buf++;
	cqueue_devp->queue_full = 0;	
	cqueue_devp->current_read_pointer++;
	if( cqueue_devp->current_read_pointer == DEFAULT_SIZE)
		cqueue_devp->current_read_pointer = 0;
	if(cqueue_devp->current_write_pointer == cqueue_devp->current_read_pointer)
		cqueue_devp->queue_empty = 1;	

	//return read data
	return data;
}

/*
* Open driver
*/
int sq_open(struct inode *inode, struct file *file)
{
	struct cqueue_dev *cqueue_devp;
//	printk("\nopening\n");

	/* Get the per-device structure that contains this cdev */
	cqueue_devp = container_of(inode->i_cdev, struct cqueue_dev, cdev);

	/* Easy access to cmos_devp from rest of the entry points */
	file->private_data = cqueue_devp;
	printk("\n%s is openning \n", cqueue_devp->name);
	return 0;
}

/*
 * Release driver
 */
int sq_release(struct inode *inode, struct file *file)
{
	struct cqueue_dev *cqueue_devp = file->private_data;
	
	printk("\n%s is closing\n", cqueue_devp->name);
	return 0;
}

//module_init(sq_create);
//module_exit(sq_delete);

/*
template <class T>
class Circular_Queue
{
  static int read = 0; //Read cursor, always access slot "read_cursor"
  static int write = 0; //Write cursor, always access slot "write_cursor"
  static int empty = true;
//  static char * FIFO[SIZE];

  T sq_create(int size = DEFAULT_SIZE){
    double *pi = NULL
    pi = (double*)malloc(sizeof(double)*10);
    printf("Queue Created!!! GL HF!!!\n");
    return *pi;
  }

  T IsFull(){
    if(read == write) return !empty;
    else return 0;
  }
	
  T IsEmpty(){
    if(read == write) return empty;
    else return 0;
  }	

  T sq_write(const char * data, int size = DEFAULT_SIZE){//Should lock queue at implementing pthread
    if(IsFull()){
      printf("Queue FULL!!! Bad Writing Behavior\n");
      return -1;
    }else{
      m_queue.push(*data);
      write = (write == size-1) ? 0:write+1;
      empty = 0;
      return 1;
    }
  }

  T sq_read(int size = DEFAULT_SIZE){
    if(IsEmpty()){
      printf("Queue EMPTY!!! Bad Reading Behavior\n");
      return -1;
    }else{
      read = (read == size-1) ? 0:read+1;
      return m_queue.pop(*data);
    }
  }

  T sq_create(const char * data){//Should lock queue at implementing pthread
}
*/
