//#include <stdio.h>
//#include <stdlib.h>
//#include <iostream.h>
//#include <string.h>
#include<linux/jiffies.h>
#include<linux/module.h>
#include<linux/jiffies.h>
#include<linux/kernel.h>
#include<linux/types.h>
#include<linux/cdev.h>
#include<linux/fs.h>
#include<linux/device.h>
#include<linux/slab.h>
#include<asm/uaccess.h>

#include<linux/init.h>
#include<linux/moduleparam.h>

//#define DEFAULT_SIZE 10
#define DEFAULT_SIZE 80

#define DEVICE_NAME1 "dataqueue1"
#define DEVICE_NAME2 "dataqueue2"
#define DEVICE_CLASS1 "virtual_char1" 	//the device class as shown in /sys/class
#define DEVICE_CLASS2 "virtual_char2" 	//the device class as shown in /sys/class
#define DEVICE "dataqueue"

#define DEVICE_MAJOR_NUM1 0
#define DEVICE_MAJOR_NUM2 1

#define DEVICES 2

/* per device structure */
struct cqueue_dev {
	struct cdev cdev;               /* The cdev structure */
	char name[24];                  /* Name of device*/
	char in_string[DEFAULT_SIZE];	/* buffer for the input string */
	int current_write_pointer;
	int current_read_pointer;
	int queue_full;
	int queue_empty;
} *cqueue_devp1, *cqueue_devp2;

static dev_t cqueue_dev_num;      	/* Allocate device number */
struct class *cqueue_dev_class;		/* Tie with the device model */
static struct device *cqueue_dev_device1, *cqueue_dev_device2;

static char *user_name = "TEAM_16";

module_param(user_name,charp,0000);	//to get parameter from load.sh script to greet the user

//static int  sq_kn_create_sub(int, const char *, const char *);
static int  __init sq_kn_create(void);
static void __exit sq_kn_delete(void);

ssize_t sq_kn_read(struct file *, char *, size_t, loff_t *);
ssize_t sq_kn_write(struct file *, const char *, size_t, loff_t *);

int sq_kn_open(struct inode *, struct file *);
int sq_kn_release(struct inode *, struct file *);

/* File operations structure. Defined in linux/fs.h */
static struct file_operations cqueue_fops = {
    .owner		= THIS_MODULE,        /* Owner */
    .open		= sq_kn_open,        /* Open method */
    .release		= sq_kn_release,     /* Release method */
    .write		= sq_kn_write,       /* Write method */
    .read		= sq_kn_read,         /* Read method */
};

/*
 * Driver Initialization
 */

//static int __init sq_kn_create()
//{
//	sq_kn_create_sub(DEVICE_MAJOR_NUM1,DEVICE_NAME1,DEVICE_CLASS1);
//	sq_kn_create_sub(DEVICE_MAJOR_NUM2,DEVICE_NAME2,DEVICE_CLASS2);
//	return 0;
//}

static void __exit sq_kn_delete()
{
	/*
	1. delete cdev
	2. destroy device/class
	3. free alloc_chrdev_region
	*/
	cdev_del(&cqueue_devp1->cdev);
	cdev_del(&cqueue_devp2->cdev);
	device_destroy(cqueue_dev_class,MKDEV(MAJOR(cqueue_dev_num), 0));
	device_destroy(cqueue_dev_class,MKDEV(MAJOR(cqueue_dev_num), 1));
	//class_destroy(cqueue_dev_class);
	class_destroy(cqueue_dev_class);
	unregister_chrdev_region(cqueue_dev_num,DEVICES);

	printk(KERN_ALERT "Adios, my love\n");
	//return;
}

//static int sq_kn_create_sub(int device_major_num, const char * device_x, const char * class_x)
static int __init sq_kn_create()
{
	int status1, status2;
	int time_since_boot;
	
	//Dynamic allocate device number
	if (alloc_chrdev_region(&cqueue_dev_num, 0, 2, DEVICE) < 0) {
	//if (alloc_chrdev_region(&cqueue_dev_num, 0, device_major_num, device_x) < 0) {
		printk(KERN_DEBUG "Can't register device\n"); 
		return -EFAULT;
	}

	//Create Class
	cqueue_dev_class = class_create(THIS_MODULE, DEVICE);

	if(IS_ERR(cqueue_dev_class)){
		printk("Can't create class\n");
		//return -EFAULT;
	}

	//Allocate memory
	cqueue_devp1 = kmalloc(sizeof(struct cqueue_dev), GFP_KERNEL);//on behave of KERNEL
	cqueue_devp2 = kmalloc(sizeof(struct cqueue_dev), GFP_KERNEL);//on behave of KERNEL
		
	if (!cqueue_devp1 || !cqueue_devp2) {
		printk("Bad Kmalloc\n"); 
		return -ENOMEM;
	}

	//Create Device
	cqueue_dev_device1 = device_create(cqueue_dev_class, NULL, MKDEV(MAJOR(cqueue_dev_num), 0), NULL, DEVICE_NAME1);		
	cqueue_dev_device2 = device_create(cqueue_dev_class, NULL, MKDEV(MAJOR(cqueue_dev_num), 1), NULL, DEVICE_NAME2);		

	if(IS_ERR(cqueue_dev_device1)){
		printk("Can't create device 1\n");
		return -EFAULT;
	}
	if(IS_ERR(cqueue_dev_device2)){
		printk("Can't create device 2\n");
		return -EFAULT;
	}

	/* Request I/O region */
	sprintf(cqueue_devp1->name, DEVICE_NAME1);//Device Name 
	sprintf(cqueue_devp2->name, DEVICE_NAME2);//Device Name 

	cqueue_devp1->cdev.owner = THIS_MODULE;
	cqueue_devp2->cdev.owner = THIS_MODULE;

	memset(cqueue_devp1->in_string, 0, DEFAULT_SIZE);//Memory initialize
	memset(cqueue_devp2->in_string, 0, DEFAULT_SIZE);//Memory initialize
	
	/* Connect the file operations with the cdev */
	cdev_init(&cqueue_devp1->cdev, &cqueue_fops);
	cdev_init(&cqueue_devp2->cdev, &cqueue_fops);


	/* Connect the major/minor number to the cdev */
	status1 = cdev_add(&cqueue_devp1->cdev, MKDEV(MAJOR(cqueue_dev_num), 0), 1);
	status2 = cdev_add(&cqueue_devp2->cdev, MKDEV(MAJOR(cqueue_dev_num), 1), 1);

	if (status1) {
		printk("Error! Bad cdev\n");
		return status1;
	}
	if (status2) {
		printk("Error! Bad cdev\n");
		return status2;
	}

	time_since_boot=(jiffies-INITIAL_JIFFIES)/HZ;//since on some systems jiffies is a very huge uninitialized value at boot and saved.
	printk("Hi %s, this machine has been on for %d seconds\n", user_name, time_since_boot);

	//initial position of r/w ptr	
	cqueue_devp1->current_write_pointer = 0;
	cqueue_devp1->current_read_pointer = 0;
	cqueue_devp1->queue_full = 0;
	cqueue_devp1->queue_empty = 1;

	cqueue_devp2->current_write_pointer = 0;
	cqueue_devp2->current_read_pointer = 0;
	cqueue_devp2->queue_full = 0;
	cqueue_devp2->queue_empty = 1;

	printk("C-Queue driver initialized.\n");
	
	return 0;
}
/*
 * Write to cqueue d
*/
ssize_t sq_kn_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
	struct cqueue_dev *cqueue_devp = file->private_data;
	char data;
	int i=0;
	printk(KERN_ALERT "Reading...\n");
	//preventing queue empty
	if (cqueue_devp->queue_empty) {
		printk("Queue empty at %s \n", cqueue_devp->name);
		return 0;
	}
	/* 
	 * Actually put the data into the buffer 
	 */
	for (i = 0;i < count;i++) {	
		data = cqueue_devp->in_string[cqueue_devp->current_read_pointer];
		put_user(cqueue_devp->in_string[cqueue_devp->current_read_pointer], buf);//__fixdfsi
		printk(KERN_ALERT "Reading -- '%c', at '%d'\n",cqueue_devp->in_string[cqueue_devp->current_read_pointer], cqueue_devp->current_read_pointer);

		buf++;
		cqueue_devp->queue_full = 0;	
		cqueue_devp->current_read_pointer++;
		if( cqueue_devp->current_read_pointer == DEFAULT_SIZE)
			cqueue_devp->current_read_pointer = 0;
		if(cqueue_devp->current_write_pointer == cqueue_devp->current_read_pointer)
			cqueue_devp->queue_empty = 1;	
	}
	//return data;//__fixdfsi
	return i;
}

ssize_t sq_kn_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
	struct cqueue_dev *cqueue_devp = file->private_data;
	int i;
	//preventing queue full
	printk(KERN_ALERT "Writing...\n");
	if (cqueue_devp->queue_full) {
		printk("Queue full at %s \n", cqueue_devp->name);
		return 0;
	}

	for (i = 0;i < count;i++) {	
		get_user(cqueue_devp->in_string[cqueue_devp->current_write_pointer], buf);//__floatsidf
		printk(KERN_ALERT "Writing --  '%c', at '%d' \n", cqueue_devp->in_string[cqueue_devp->current_write_pointer], cqueue_devp->current_write_pointer);

		buf++;
		cqueue_devp->queue_empty = 0;	
		cqueue_devp->current_write_pointer++;
		if(cqueue_devp->current_write_pointer == DEFAULT_SIZE)
			cqueue_devp->current_write_pointer = 0;
		if(cqueue_devp->current_write_pointer == cqueue_devp->current_read_pointer)
			cqueue_devp->queue_full = 1;	
	}
	return 0;
}

/*
* Open driver
*/
int sq_kn_open(struct inode *inode, struct file *file)
{
	struct cqueue_dev *cqueue_devp;
	printk("\nopening\n");

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
int sq_kn_release(struct inode *inode, struct file *file)
{
	struct cqueue_dev *cqueue_devp = file->private_data;
	
	printk("\n%s is closing\n", cqueue_devp->name);
	return 0;
}

module_init(sq_kn_create);
module_exit(sq_kn_delete);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A circular queue with size of 10 double, for kernel use");
MODULE_AUTHOR("YU-CHUN HUANG");

