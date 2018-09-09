#include<linux/module.h>
#include<linux/jiffies.h>
#include<linux/kernel.h>
#include<linux/types.h>
#include<linux/cdev.h>
#include<linux/fs.h>
#include<linux/device.h>
#include<linux/slab.h>
#include<linux/init.h>
#include<linux/moduleparam.h>
#include <linux/gpio.h>
//ocasi
//#include <linux/ioctl.h>
//#include<asm/uaccess.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
//#include <asm/spinlock.h>

#include "RGBLed_ioctl.h"
//ocasi, set parameter
//#define cycle_duration 20
//#define period 500

#define DEVICE_NAME "RGBLed"
#define DEVICE_NUM 1
#define DEVICES 1

/* per device structure */
struct RGBLed_dev {
	struct cdev cdev;               /* The cdev structure */
	//struct hrtimer hr_timer; 
	//int  polarity;
	char name[24];                  /* Name of device*/
	int  buff_R,buff_G,buff_B;	        /* buffer for the input pin */
	int  pin_R, pin_G, pin_B;       /* Pin Number for RGB, ocasi*/
	int  buff_setup_R, buff_setup_G, buff_setup_B;	/*turn on or not*/
  	int  buff_duty_cycle;
	
	//char kn_buffer_[4];          /*store 3 data from user*/

} *RGBLed_devp;

//struct RGB_info{
//	int duty_cycle;
//	int R_pin;
//	int G_pin;
//	int B_pin;
//};


static dev_t RGBLed_dev_num;      	/* Allocate device number */
struct class *RGBLed_dev_class;		/* Tie with the device model? */
static struct device *RGBLed_dev_device;
//ocasi
struct RGB_info *obj_led;
static struct hrtimer hr_timer; 

static char *user_name = "TEAM_16";

module_param(user_name,charp,0000);	//to get parameter from load.sh script to greet the user

static int  __init RGBLed_kn_create(void);
static void __exit RGBLed_kn_delete(void);
/*include all gpio sets*/

ssize_t RGBLed_kn_write(struct file *, const char *, size_t, loff_t *);
//long RGBLed_kn_ioctl(struct file*, unsigned int, struct RGB_info *);
long RGBLed_kn_ioctl(struct file*, unsigned int, unsigned long);
//int (*RGBLed_kn_ioctl) (struct inode*, struct file*, unsigned int, unsigned long);
//static int pin_num = 0;
int RGBLed_kn_open(struct inode *, struct file *);
static int RGBLed_kn_release(struct inode *, struct file *);
int kn_R, kn_G, kn_B;
int kn_setup_R, kn_setup_G, kn_setup_B;
int kn_duty_cycle;
//char input[3];
//static spinlock_t gpio_lock = SPIN_LOCK_UNLOCKED;
spinlock_t gpio_lock;
unsigned long flags;
int polarity = 0;
ktime_t ktime;//ocasi, test of hrtimer
unsigned long delay_in_ns = 1;
unsigned long pos_in_ns = 1;
unsigned long neg_in_ns = 1;
int ioctl_init = 0;

struct timer_list myTimer;//ocasi
/* File operations structure. Defined in linux/fs.h */
static struct file_operations RGBLed_fops = {
    .owner		= THIS_MODULE,        /* Owner */
    .open		= RGBLed_kn_open,        /* Open method */
    .write		= RGBLed_kn_write,     /* Write method */
    .unlocked_ioctl	= RGBLed_kn_ioctl,       /* Construct method */
    .release		= RGBLed_kn_release,         /* release method */
};




static int __init RGBLed_kn_create()
{
	int status;
	int time_since_boot;
	
	//Dynamic allocate device number  already editted!!!!!!!!!
	if (alloc_chrdev_region(&RGBLed_dev_num, 0, DEVICE_NUM, DEVICE_NAME) < 0) {
		printk(KERN_DEBUG "Can't register device\n"); 
		return -1;
	}

	//Create Class
	RGBLed_dev_class = class_create(THIS_MODULE, DEVICE_NAME);

	if(IS_ERR(RGBLed_dev_class)){
		printk("Can't create class\n");
		return -1;
	}

	//Allocate memoryRGBLed_devp
	RGBLed_devp = kmalloc(sizeof(struct RGBLed_dev), GFP_KERNEL);//on behave of KERNEL
		
	if (!RGBLed_devp) {
		printk("Bad Kmalloc\n"); 
		return -1;
	}

	//Create Device
	RGBLed_dev_device = device_create(RGBLed_dev_class, NULL, MKDEV(MAJOR(RGBLed_dev_num), 0), NULL, DEVICE_NAME);		

	/* Request I/O region */
	sprintf(RGBLed_devp->name, DEVICE_NAME);//Device Name 

	/* Connect the file operations with the cdev */
	cdev_init(&RGBLed_devp->cdev, &RGBLed_fops);
	RGBLed_devp->cdev.owner = THIS_MODULE;
	
	//TBD
	//memset(RGBLed_devp->in_string, 0, DEFAULT_SIZE);//Memory initialize

	/* Connect the major/minor number to the cdev */
	//status = cdev_add(&RGBLed_devp->cdev, (RGBLed_dev_num), 1);
	status = cdev_add(&RGBLed_devp->cdev, MKDEV(MAJOR(RGBLed_dev_num),0), 1);

	if (status) {
		printk("Error! Bad cdev\n");
		return status;
	}

	time_since_boot=(jiffies-INITIAL_JIFFIES)/HZ;//since on some systems jiffies is a very huge uninitialized value at boot and saved.
	printk("Hi %s, this machine has been on for %d seconds\n", user_name, time_since_boot);

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
	//hrtimer_try_to_cancel(&hr_timer);
	//kfree(obj_led);
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
	ioctl_init = 0;
	printk("\n%s is openning \n", RGBLed_devp->name);

	gpio_request(11,"GPIO00"); 
	gpio_request(32,"GPIO00_shifter"); 
	gpio_request(12,"GPIO01"); 
	gpio_request(28,"GPIO01_shifter"); 
	gpio_request(45,"GPIO01_mux"); 
	gpio_request(13,"GPIO02"); 
	gpio_request(34,"GPIO02_shifter"); 
	gpio_request(77,"GPIO02_mux"); 
	gpio_request(14,"GPIO03"); 
	gpio_request(16,"GPIO03_shifter"); 
	gpio_request(76,"GPIO03_mux"); 
	gpio_request(6,"GPIO04"); 
	gpio_request(36,"GPIO04_shifter"); 
	gpio_request(0,"GPIO05"); 
	gpio_request(18,"GPIO05_shifter"); 
	gpio_request(66,"GPIO05_mux"); 
	gpio_request(1,"GPIO06"); 
	gpio_request(20,"GPIO06_shifter"); 
	gpio_request(68,"GPIO06_mux"); 
	gpio_request(38,"GPIO07"); 
	gpio_request(40,"GPIO08"); 
	gpio_request(4,"GPIO09"); 
	gpio_request(22,"GPIO09_shifter"); 
	gpio_request(70,"GPIO09_mux"); 
	gpio_request(10,"GPIO10"); 
	gpio_request(26,"GPIO10_shifter"); 
	gpio_request(74,"GPIO10_mux"); 
	gpio_request(5,"GPIO11"); 
	gpio_request(24,"GPIO11_shifter"); 
	gpio_request(44,"GPIO11_mux"); 
	gpio_request(15,"GPIO12"); 
	gpio_request(42,"GPIO12_shifter"); 
	gpio_request(7,"GPIO13"); 
	gpio_request(30,"GPIO13_shifter"); 
	gpio_request(46,"GPIO13_mux"); 
		
	gpio_direction_output(11, 0);
	gpio_direction_output(32, 0);
	gpio_direction_output(12, 0);
	gpio_direction_output(28, 0);
	gpio_direction_output(45, 0);
	gpio_direction_output(13, 0);
	gpio_direction_output(34, 0);
	gpio_direction_output(14, 0);
	gpio_direction_output(16, 0);
	gpio_direction_output(6,  0);
	gpio_direction_output(36, 0);
	gpio_direction_output(0,  0);
	gpio_direction_output(18, 0);
	gpio_direction_output(1,  0);
	gpio_direction_output(20, 0);
	gpio_direction_output(34, 0);
	gpio_direction_output(40, 0);
	gpio_direction_output(38, 0);
	gpio_direction_output(4,  0);
	gpio_direction_output(22, 0);
	gpio_direction_output(10, 0);
	gpio_direction_output(26, 0);
	gpio_direction_output(5,  0);
	gpio_direction_output(24, 0);
	gpio_direction_output(44, 0);
	gpio_direction_output(15, 0);
	gpio_direction_output(42, 0);
	gpio_direction_output(7,  0);
	gpio_direction_output(30, 0);
	gpio_direction_output(46, 0);

	printk("GPIO initialized.\n");

	return 0;
}


//  Release driver
 
static int RGBLed_kn_release(struct inode *inode, struct file *filp)
{
	//struct RGBLed_dev *RGBLed_devp = filp->private_data;
	
	printk("\n%s is closing\n", RGBLed_devp->name);
	hrtimer_cancel(&hr_timer);
	kfree(obj_led);

	gpio_free(11);
	gpio_free(32);
	gpio_free(12);
	gpio_free(28);
	gpio_free(45);
	gpio_free(13);
	gpio_free(34);
	gpio_free(77);
	gpio_free(14);
	gpio_free(16);
	gpio_free(76);
	gpio_free(6 );
	gpio_free(36);
	gpio_free(0 );
	gpio_free(18);
	gpio_free(66);
	gpio_free(1 );
	gpio_free(20);
	gpio_free(68);
	gpio_free(38);
	gpio_free(40);
	gpio_free(4 );
	gpio_free(22);
	gpio_free(70);
	gpio_free(10);
	gpio_free(26);
	gpio_free(74);
	gpio_free(5 );
	gpio_free(24);
	gpio_free(44);
	gpio_free(15);
	gpio_free(42);
	gpio_free(7 );
	gpio_free(30);
	gpio_free(46);
	
	//kfree(filp->private_data);
  	//filp->private_data = NULL;
	return 0;
}

//ocasi
//void timerFun (unsigned long arg) {
enum hrtimer_restart TimerFun (struct hrtimer *timer) {
	//spin_lock(&gpio_lock);
	ktime_t currtime;
	spin_lock_irqsave(&gpio_lock, flags);
	//printk("hrtimer called (%ld)\n",jiffies);
	//struct RGBLed_dev *RGBLed_devp;
	//printk("Debug mode: Entering HRTimer, polarity = %d, time = %ld\n",polarity, jiffies);
	if(polarity == 0){
		gpio_set_value(kn_R, kn_setup_R);
		//printk("Debug mode: jiffies = %ld\n",jiffies);
		gpio_set_value(kn_G, kn_setup_G);
		//printk("Debug mode: jiffies = %ld\n",jiffies);
		gpio_set_value(kn_B, kn_setup_B);
		//printk("Debug mode: jiffies = %ld\n",jiffies);
		ktime = ktime_set(0,pos_in_ns);
		polarity=1;
	}else{
		gpio_set_value(kn_R, 0);
		//printk("Debug mode: jiffies = %ld\n",jiffies);
		gpio_set_value(kn_G, 0);
		//printk("Debug mode: jiffies = %ld\n",jiffies);
		gpio_set_value(kn_B, 0);
		//printk("Debug mode: jiffies = %ld\n",jiffies);
		ktime = ktime_set(0,neg_in_ns);
		polarity=0;
	}
	currtime = ktime_get();
	hrtimer_forward(&hr_timer,currtime,ktime);
	spin_unlock_irqrestore(&gpio_lock, flags);
	return HRTIMER_RESTART;
}

ssize_t RGBLed_kn_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
	struct RGBLed_dev *RGBLed_devp = file->private_data;
	//int HZ_per_cycle; 
	//int HZ_per_pos;
	//int HZ_per_neg;
	char input[3];
	if(ioctl_init == 0){
		delay_in_ns = cycle_duration*10000UL;//precision in nanosec 
		pos_in_ns = (RGBLed_devp->buff_duty_cycle)*delay_in_ns;//precision in nanosec 
		neg_in_ns = (100-RGBLed_devp->buff_duty_cycle)*delay_in_ns;//precision in nanosec 	
		ktime = ktime_set(0,neg_in_ns);
		hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);//(struct hrtimer *time, clockid_t which_clock, enum hrtimer_mode mode)
		hr_timer.function = &TimerFun;
	}
	//printk("Debug mode: Entering GPIO display\n");

	get_user(input[0],buf++);
	//printk("Debug mode: input[0] = %c\n",input[0]);
	RGBLed_devp->buff_setup_R = (input[0] == '1') ? 1:0; 
	kn_setup_R=RGBLed_devp->buff_setup_R;

	get_user(input[1],buf++);
	//printk("Debug mode: input[1] = %c\n",input[1]);
	RGBLed_devp->buff_setup_G = (input[1] == '1') ? 1:0; 
	kn_setup_G=RGBLed_devp->buff_setup_G;

	get_user(input[2],buf++);
	//printk("Debug mode: input[2] = %c\n",input[2]);
	RGBLed_devp->buff_setup_B = (input[2] == '1') ? 1:0; 
	kn_setup_B=RGBLed_devp->buff_setup_B;

	if(ioctl_init == 0){
		hrtimer_start(&hr_timer,ktime,HRTIMER_MODE_REL);
		ioctl_init++;
	}

	//printk("Debug mode: pin_R = %d\n",RGBLed_devp->pin_R);
	//printk("Debug mode: pin_G = %d\n",RGBLed_devp->pin_G);
	//printk("Debug mode: pin_B = %d\n",RGBLed_devp->pin_B);
	//printk("Debug mode: setup_R = %d\n",RGBLed_devp->buff_setup_R);
	//printk("Debug mode: setup_G = %d\n",RGBLed_devp->buff_setup_G);
	//printk("Debug mode: setup_B = %d\n",RGBLed_devp->buff_setup_B);
	//printk("Debug mode: kn_pin_R = %d\n",kn_R);
	//printk("Debug mode: kn_pin_G = %d\n",kn_G);
	//printk("Debug mode: kn_pin_B = %d\n",kn_B);
	//printk("Debug mode: kn_setup_R = %d\n",kn_setup_R);
	//printk("Debug mode: kn_setup_G = %d\n",kn_setup_G);
	//printk("Debug mode: kn_setup_B = %d\n",kn_setup_B);

	printk("Debug mode: delay_in_ns = %ld\n",delay_in_ns);
	printk("Debug mode: pos_in_ns = %ld\n",pos_in_ns);
	printk("Debug mode: neg_in_ns = %ld\n",neg_in_ns);
	
	return 0;
}

//ocasi
//int RGBLed_kn_ioctl (struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
//long RGBLed_kn_ioctl (struct file *filp, unsigned int cmd, struct RGB_info* arg)
long RGBLed_kn_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct RGBLed_dev *RGBLed_devp = filp->private_data;
	struct RGB_info *obj_led = kmalloc(sizeof(*obj_led),GFP_KERNEL);

	switch (cmd){
		case IOCTL_VALSET:
		//	copy_from_user(RGBLed_devp->buff_duty_cycle, (int user *)arg, sizeof(data));
		//		kn_duty_cycle=RGBLed_devp->buff_duty_cycle;
		//	copy_from_user(RGBLed_devp->buff_R, (int user *)arg, sizeof(data));
		//		kn_R=RGBLed_devp->buff_R;
		//	copy_from_user(RGBLed_devp->buff_G, (int user *)arg, sizeof(data));
		//		kn_G=RGBLed_devp->buff_G;
		//	copy_from_user(RGBLed_devp->buff_B, (int user *)arg, sizeof(data));
		//		kn_B=RGBLed_devp->buff_B;
			//copy_from_user(obj, (void *)arg, sizeof(struct RGB_info));
			copy_from_user(obj_led, (void *)arg, sizeof(*obj_led));
			printk("Debug mode: duty_cycle = %d\n",obj_led->duty_cycle);
			printk("Debug mode: buff_R = %d\n",obj_led->R_pin);
			printk("Debug mode: buff_G = %d\n",obj_led->G_pin);
			printk("Debug mode: buff_B = %d\n",obj_led->B_pin);
			//copy_from_user(&data, (int __user *)arg, sizeof(data))
			RGBLed_devp->buff_duty_cycle = obj_led->duty_cycle;
			RGBLed_devp->buff_R = obj_led->R_pin;
			RGBLed_devp->buff_G = obj_led->G_pin;
			RGBLed_devp->buff_B = obj_led->B_pin;
			printk("Debug mode: duty_cycle = %d\n",RGBLed_devp->buff_duty_cycle);
			printk("Debug mode: buff_R = %d\n",RGBLed_devp->buff_R);
			printk("Debug mode: buff_G = %d\n",RGBLed_devp->buff_G);
			printk("Debug mode: buff_B = %d\n",RGBLed_devp->buff_B);
			//copy_from_user(RGBLed_devp->buff_duty_cycle, (int *)arg->duty_cycle, sizeof(arg->duty_cycle));
			kn_duty_cycle=RGBLed_devp->buff_duty_cycle;
			//copy_from_user(RGBLed_devp->buff_R, *(int *)arg.R_pin, sizeof(*(int *)arg.R_pin));
			kn_R=RGBLed_devp->buff_R;
			//copy_from_user(RGBLed_devp->buff_G, *(int *)arg.G_pin, sizeof(*(int *)arg.G_pin));
			kn_G=RGBLed_devp->buff_G;
			//copy_from_user(RGBLed_devp->buff_B, *(int *)arg.B_pin, sizeof(*(int *)arg.B_pin));
			kn_B=RGBLed_devp->buff_B;

			/* detect the value from user*/
			if (kn_duty_cycle>=100||kn_duty_cycle<=0)
				return -1;
			else if (kn_R>13||kn_R<0||kn_R==7||kn_R==8)
				return -1;
			else if (kn_G>13||kn_G<0||kn_G==kn_R||kn_R==7||kn_R==8)	
				return -1;
			else if (kn_B>13||kn_B<0||kn_B==kn_G||kn_B==kn_R||kn_R==7||kn_R==8)	
				return -1;
			/*intensity and pin set*/
			switch(kn_R)
			{
				case 0:  RGBLed_devp->pin_R = 11; break; 
				case 1:  RGBLed_devp->pin_R = 12; break; 
				case 2:  RGBLed_devp->pin_R = 13; break; 
				case 3:  RGBLed_devp->pin_R = 14; break; 
				case 4:  RGBLed_devp->pin_R = 6;  break; 
				case 5:  RGBLed_devp->pin_R = 0;  break; 
				case 6:  RGBLed_devp->pin_R = 1;  break; 
				case 7:  RGBLed_devp->pin_R = 38; break; 
				case 8:  RGBLed_devp->pin_R = 40; break;  
				case 9:  RGBLed_devp->pin_R = 4;  break; 
				case 10: RGBLed_devp->pin_R = 10; break; 
				case 11: RGBLed_devp->pin_R = 5;  break; 
				case 12: RGBLed_devp->pin_R = 15; break; 
				case 13: RGBLed_devp->pin_R = 7;  break; 
				default: printk("pin_R error\n"); return -1;
			}	
			kn_R = RGBLed_devp->pin_R;
			switch(kn_G)
			{
				case 0:  RGBLed_devp->pin_G = 11; break;
				case 1:  RGBLed_devp->pin_G = 12; break;
				case 2:  RGBLed_devp->pin_G = 13; break;
				case 3:  RGBLed_devp->pin_G = 14; break;
				case 4:  RGBLed_devp->pin_G = 6;  break;
				case 5:  RGBLed_devp->pin_G = 0;  break;
				case 6:  RGBLed_devp->pin_G = 1;  break;
				case 7:  RGBLed_devp->pin_G = 38; break;
				case 8:  RGBLed_devp->pin_G = 40; break;
				case 9:  RGBLed_devp->pin_G = 4;  break;
				case 10: RGBLed_devp->pin_G = 10; break;
				case 11: RGBLed_devp->pin_G = 5;  break;
				case 12: RGBLed_devp->pin_G = 15; break;
				case 13: RGBLed_devp->pin_G = 7;  break;
				default: printk("pin_G error\n"); return -1;
			}	
			kn_G = RGBLed_devp->pin_G;
			switch(kn_B)
			{
				case 0:  RGBLed_devp->pin_B = 11; break;
				case 1:  RGBLed_devp->pin_B = 12; break;
				case 2:  RGBLed_devp->pin_B = 13; break;
				case 3:  RGBLed_devp->pin_B = 14; break;
				case 4:  RGBLed_devp->pin_B = 6;  break;
				case 5:  RGBLed_devp->pin_B = 0;  break;
				case 6:  RGBLed_devp->pin_B = 1;  break;
				case 7:  RGBLed_devp->pin_B = 38; break;
				case 8:  RGBLed_devp->pin_B = 40; break;
				case 9:  RGBLed_devp->pin_B = 4;  break;
				case 10: RGBLed_devp->pin_B = 10; break;
				case 11: RGBLed_devp->pin_B = 5;  break;
				case 12: RGBLed_devp->pin_B = 15; break;
				case 13: RGBLed_devp->pin_B = 7;  break;
				default: printk("pin_B error\n"); return -1;
			}	
			kn_B = RGBLed_devp->pin_B;
			break;
		default:
			return -1;
	}
    	return 0;
}

module_init(RGBLed_kn_create);
module_exit(RGBLed_kn_delete);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A LED driver in kernel");
MODULE_AUTHOR("YU-CHUN HUANG");
