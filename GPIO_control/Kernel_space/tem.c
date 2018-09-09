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
#include <linux/gpio.h>
#include <linux/ioctl.h>

#define DEFAULT_SIZE 8
#define DEVICE_NAME "RGBLed"
#define DEVICE_MAJOR_NUM 1
#define DEVICES 1
//------------------------
#define DEV_IOCTLID 'k'
#define	CONFIG	_IOW(DEV_IOCTLID, 10, int)
//------------------------

/* per device structure */
struct RGBLed_dev {
	struct cdev cdev;               /* The cdev structure */
	char name[24];                  /* Name of device*/
	int  buff_R,buff_G,buff_B;	        /* buffer for the input pin */
	int  buff_setup_R, buff_setup_G, buff_setup_B;	/*turn on or not*/
  	int  buff_duty_cycle;
	
char kn_buffer_[4];          /*store 3 data from user*/

} *RGBLed_devp;



static dev_t RGBLed_dev_num;      	/* Allocate device number */
struct class *RGBLed_dev_class;		/* Tie with the device model? */
static struct device *RBGLed_dev_device;

static char *user_name = "TEAM_16";

module_param(user_name,charp,0000);	//to get parameter from load.sh script to greet the user

static int  __init RGBLed_kn_create(void);
static void __exit RGBLed_kn_delete(void);
/*include all gpio sets*/
struct kn_gpio {
		unsigned	gpio;
		unsigned long	flags;
		const char	*label;
	        };

ssize_t RGBLed_kn_write(struct file *, const char *, size_t, loff_t *);
static int RGBLed_kn_ioctl(struct inode*, struct file*, unsigned int, unsigned long);
static int pin_num = 0;
int RGBLed_kn_open(struct inode *, struct file *);
int RGBLed_kn_release(struct inode *, struct file *);
int kn_R, kn_G, kn_B;
int kn_setup_R, kn_setup_G, kn_set_B;
int kn_duty_cycle;
/* File operations structure. Defined in linux/fs.h */
static struct file_operations RGBLed_fops = {
    .owner		= THIS_MODULE,        /* Owner */
    .open		= RGBLed_kn_open,        /* Open method */
    .write		= RGBLed_kn_write,     /* Write method */
    .ioctl		= RGBLed_kn_ioctl,       /* Construct method */
    .release		= RGBLed_kn_release,         /* release method */
};




static void __init RGBLed_kn_create()
{
	int status;
	int time_since_boot;
	
	//Dynamic allocate device number  already editted!!!!!!!!!
	if (alloc_chrdev_region(&RGBLed_dev_num, 0, device_major_num, DEVICE_NAME) < 0) {
		printk(KERN_DEBUG "Can't register device\n"); 
		return -EFAULT;
	}

	//Create Class
	RGBLed_dev_class = class_create(THIS_MODULE, DEVICE_NAME);

	if(IS_ERR(RGBLed_dev_class)){
		printk("Can't create class\n");
		return -EFAULT;
	}

	//Allocate memoryRGBLed_devp
	RGBLed_devp = kmalloc(sizeof(struct RGBLed_dev), GFP_KERNEL);//on behave of KERNEL
		
	if (!RGBLed_devp) {
		printk("Bad Kmalloc\n"); 
		return -ENOMEM;
	}

	//Create Device
	RGBLed_dev_device = device_create(RGBLed_dev_class, NULL, MKDEV(MAJOR(RGBLed_dev_num), 0), NULL, DEVICE_NAME);		

	/* Request I/O region */
	sprintf(RGBLed->name, DEVICE_NAME);//Device Name 

	/* Connect the file operations with the cdev */
	cdev_init(&RGBLed_devp->cdev, &RGBLed_fops);
	RGBLed_devp->cdev.owner = THIS_MODULE;
	
	memset(RGBLed_devp->in_string, 0, DEFAULT_SIZE);//Memory initialize

	/* Connect the major/minor number to the cdev */
	status = cdev_add(&RGBLed_devp->cdev, (RGBLed_dev_num), 1);

	if (status) {
		printk("Error! Bad cdev\n");
		return status;
	}

	time_since_boot=(jiffies-INITIAL_JIFFIES)/HZ;//since on some systems jiffies is a very huge uninitialized value at boot and saved.
	printk("Hi %s, this machine has been on for %d seconds", user_name, time_since_boot);

	//initial position of r/w ptr	
	RGBLed_devp->current_write_pointer = 0;
	RGBLed_devp->current_read_pointer = 0;
	

	printk("RGBLed driver initialized.\n");
	
	return 0;
}



static void __exit RGBLed_kn_delete()
{
	/*
	1. delete cdev
	2. destroy device/class
	3. free alloc_chrdev_region
	*/
	cdev_del(&RGBLed_devp->cdev);
	device_destroy(RGBLed_dev_class,DEVICES);
	class_destroy(RGBLed_dev_class);
	unregister_chrdev_region(RGBLed_dev_num,DEVICES);

	printk(KERN_ALERT "Adios, my love\n");
	//return;
}



// Open driver~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int RGBLed_kn_open(struct inode *inode, struct file *file)
{
	struct RGBLed_dev *RGBLed_devp;

	/* Get the per-device structure that contains this cdev */
	RGBLed_devp = container_of(inode->i_cdev, struct RGBLed_dev, cdev);

	/* Easy access to cmos_devp from rest of the entry points */
	file->private_data = RGBLed_devp;
	printk("\n%s is openning \n", RGBLed_devp->name);
	return 0;
}


//  Release driver
 
int RGBLed_kn_release(struct inode *inode, struct file *file)
{
	struct RGBLed_dev *RGBLed_devp = file->private_data;
	
	printk("\n%s is closing\n", RGBLed_devp->name);
	return 0;
}




ssize_t RGBLed_kn_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
	struct RGBLed_dev *RGBLed_devp = file->private_data;
	
		get_user(RGBLed_devp->buff_setup_R, buf);
			kn_setup_R=RGBLed_devp->buff_setup_R;
		get_user(RGBLed_devp->buff_setup_G, buf);
			kn_setup_G=RGBLed_devp->buff_setup_G;
		get_user(RGBLed_devp->buff_setup_B, buf);
			kn_setup_B=RGBLed_devp->buff_setup_B;

/*		int ret=gpio_request(buff_R, "led_R");
		if(ret<0)
  			 pr_err("%s: gpio_request failed for gpio %d\n");	
		else		
			gpio_set_value(kn_setup_R, 0);
		
		gpio_request_one(kn_setup_R, GPIOF_OUT_INIT_LOW, "led1");
		gpio_set_value(kn_setup_R, 0);
*/
}


int RGBLed_kn_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
		case CONFIG:
			
			copy_from_user(RGBLed_devp->buff_duty_cycle, (int user *)arg, sizeof(data));
				kn_duty_cycle=RGBLed_devp->buff_duty_cycle;
			copy_from_user(RGBLed_devp->buff_R, (int user *)arg, sizeof(data));
				kn_R=RGBLed_devp->buff_R;
			copy_from_user(RGBLed_devp->buff_G, (int user *)arg, sizeof(data));
				kn_G=RGBLed_devp->buff_G;
			copy_from_user(RGBLed_devp->buff_B, (int user *)arg, sizeof(data));
				kn_B=RGBLed_devp->buff_B;


			/* detect the value from user*/
			if (kn_duty_cycle>100||kn_duty_cycle<0)
				return -1;
			else if (kn_R>13||kn_R<0)
				return -1;
			else if (kn_G>13||kn_G<0||kn_G=kn_R)	
				return -1;
			else if (kn_B>13||kn_B<0||kn_B=kn_G||kn_B=kn_R)	
				return -1;
			/*intensity and pin set*/
			



			
			break;
		default:
			return -1;
	}
    	return 0;





}





