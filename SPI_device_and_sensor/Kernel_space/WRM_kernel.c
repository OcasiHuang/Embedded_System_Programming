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
#include <linux/spi/spi.h>
#include <linux/delay.h>
#include <linux/errno.h>
//ocasi
//#include <linux/ioctl.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/kthread.h>

#include "pattern.h"
#include "WRMLED_ioctl.h"
//ocasi, set parameter
//#define cycle_duration 20
//#define period 500

//#define DEVICE_NAME_LED "WRMLED"
#define DEVICE_NAME_SPI "spidev_WRM"
//#define DEVICE_NUM_LED 1
#define DEVICE_NUM_SPI 1
#define DEVICES 1
#define SPIDEV_MAJOR 153
//#define SPIDEV_MAJOR 243
//#define SPIDEV_MAJOR 'r'
#define DEBUG
#define KTHREAD

//#define SPIDEV /dev/spidev1.0

/* per device structure */
struct WRMLED_dev {
	struct cdev cdev;               /* The cdev structure */

	char name[24];                  /* Name of device*/
	struct PATTERN_list pattern;	        /* Pattern queue*/
	int  duration_index[32];       /* Delay queue, in ms*/
	int  pattern_index[32];
	int  index;
	char pattern_sequence[256];
	//SPI
	struct spi_transfer xfer;
	struct spi_message led_m;
} *WRMLED_devp;

//static dev_t WRMLED_dev_num, spidev_num;    /* Allocate device number */
static dev_t WRMLED_dev_num;    /* Allocate device number */
struct class *spidev_dev_class;/* Tie with the device model? */
static struct device *WRMLED_dev_device;
//ocasi
struct PATTERN_list *pat_lis;
struct spi_device *WRM_spi;
static int spidev_probe(struct spi_device *spi); // prototype
static int spidev_remove(struct spi_device *spi); // prototype

//spinlock
//static DEFINE_SPINLOCK(spi_lock);
//unsigned long spi_flags;
spinlock_t spi_lock;
unsigned long spi_flags;

//kthread
#ifdef KTHREAD 
static struct task_struct *led_tsk;
#endif
int LED_KTHREAD;

//struct PATTERN_list *obj_led;
//timer
//static struct hrtimer hr_timer; 
//ktime_t ktime;//ocasi, test of hrtimer

//SPI initial settings
unsigned char decode_mode_setting[2]  = {0x09,0x00};
#ifdef BRIGHT
unsigned char intensity_setting[2]    = {0x0a,0x0f};
#else
unsigned char intensity_setting[2]    = {0x0a,0x01};
#endif
unsigned char scan_limit_setting[2]   = {0x0b,0x07};
unsigned char shutdown_setting[2]     = {0x0c,0x01};
unsigned char non_display_test[2]     = {0x0f,0x00};
unsigned char display_test[2]         = {0x0f,0x01};
unsigned char x_pattern[2];

static char *user_name = "TEAM_16";

module_param(user_name,charp,0000);	//to get parameter from load.sh script to greet the user

static int  __init WRMLED_kn_create(void);
static void __exit WRMLED_kn_delete(void);

int led_thread_function(void * data);
/*include all gpio sets*/

ssize_t 	WRMLED_kn_write(struct file *, const char *, size_t, loff_t *);
long 		WRMLED_kn_ioctl(struct file*, unsigned int, unsigned long);
int 		WRMLED_kn_open(struct inode *, struct file *);
static int 	WRMLED_kn_release(struct inode *, struct file *);

static struct file_operations WRMLED_fops = {
//static struct file_operations spidev_fops = {
    .owner		= THIS_MODULE,        /* Owner */
    .open		= WRMLED_kn_open,        /* Open method */
    .write		= WRMLED_kn_write,     /* Write method */
    .unlocked_ioctl	= WRMLED_kn_ioctl,       /* Construct method */
    .release		= WRMLED_kn_release,         /* release method */
};

static const struct spi_device_id spi_ids[] = {
{"spidev",0},
//{"spidev_WRM",0},
{},
};
 
static struct spi_driver WRM_spidriver = {
	.id_table = spi_ids,
	.driver = {
		.name = "spidev", 
		//.name = "spidev_WRM", 
		.owner = THIS_MODULE
	},
	.probe = spidev_probe,
    	.remove = spidev_remove,
};



static int __init WRMLED_kn_create()
{
	int status;
	int time_since_boot;
	
//	//Dynamic allocate device number  already editted!!!!!!!!!
//	if (alloc_chrdev_region(&WRMLED_dev_num, 0, DEVICE_NUM_LED, DEVICE_NAME_LED) < 0) {
//		printk(KERN_DEBUG "Can't register device\n"); 
//		return -1;
//	}
//
//	//Create Class
//	WRMLED_dev_class = class_create(THIS_MODULE, DEVICE_NAME_LED);
//
//	if(IS_ERR(WRMLED_dev_class)){
//		printk("Can't create class\n");
////		return -1;
//	}
//
//	//Allocate memoryWRMLED_devp
//	WRMLED_devp = kmalloc(sizeof(struct WRMLED_dev), GFP_KERNEL);//on behave of KERNEL
//		
//	if (!WRMLED_devp) {
//		printk("Bad Kmalloc\n"); 
//		return -1;
//	}
//
//	//Create Device
//	WRMLED_dev_device = device_create(WRMLED_dev_class, NULL, MKDEV(MAJOR(WRMLED_dev_num), 0), NULL, DEVICE_NAME_LED);		
//
//	/* Request I/O region */
//	sprintf(WRMLED_devp->name, DEVICE_NAME_LED);//Device Name 
//
//	/* Connect the file operations with the cdev */
//	cdev_init(&WRMLED_devp->cdev, &WRMLED_fops);
//	WRMLED_devp->cdev.owner = THIS_MODULE;
//	
//	//memset(WRMLED_devp->in_string, 0, DEFAULT_SIZE);//Memory initialize
//
//	/* Connect the major/minor number to the cdev */
//	//status = cdev_add(&WRMLED_devp->cdev, (WRMLED_dev_num), 1);
//	status = cdev_add(&WRMLED_devp->cdev, MKDEV(MAJOR(WRMLED_dev_num),0), 1);
//
//	if (status) {
//		printk("Error! Bad cdev\n");
//		return status;
//	}
//
//	printk("WRMLED driver initialized.\n");
	//TBD, is it neccessary to define spidev1.0?

        //status = alloc_chrdev_region(&spidev_num, 0, DEVICE_NUM_SPI, DEVICE_NAME_SPI);
	status = register_chrdev(SPIDEV_MAJOR,"spi",&WRMLED_fops);
	if(status<0){
		printk("Error! SPI allocate fail\n");
		return status;
	}
	printk("SPI space allocated.\n");

	spidev_dev_class = class_create(THIS_MODULE, DEVICE_NAME_SPI);
	if(IS_ERR(spidev_dev_class)){
		printk("SPI class not created.\n");
		unregister_chrdev(SPIDEV_MAJOR, WRM_spidriver.driver.name);	
		return -1;
	}
	printk("SPI class created.\n");
	
	//Allocate memory
	WRMLED_devp = kmalloc(sizeof(struct WRMLED_dev), GFP_KERNEL);//on behave of KERNEL
		
	if (!WRMLED_devp) {
		printk("Bad Kmalloc\n"); 
		return -1;
	}


	/* Request I/O region */
	sprintf(WRMLED_devp->name, DEVICE_NAME_SPI);//Device Name 

	/* Connect the file operations with the cdev */
	cdev_init(&WRMLED_devp->cdev, &WRMLED_fops);
	WRMLED_devp->cdev.owner = THIS_MODULE;
	
	//memset(WRMLED_devp->in_string, 0, DEFAULT_SIZE);//Memory initialize

	/* Connect the major/minor number to the cdev */
	//status = cdev_add(&WRMLED_devp->cdev, (WRMLED_dev_num), 1);
	status = cdev_add(&WRMLED_devp->cdev, MKDEV(MAJOR(WRMLED_dev_num),0), 1);

	if (status) {
		printk("Error! Bad cdev\n");
		return status;
	}

	status = spi_register_driver(&WRM_spidriver);
	if(status<0){
		printk("Error! SPI registeration fail\n");
		class_destroy(spidev_dev_class);
		//class_destroy(WRMLED_dev_class);
		unregister_chrdev(SPIDEV_MAJOR, WRM_spidriver.driver.name);	
		//unregister_chrdev_region(WRMLED_dev_num,DEVICES);
		return status;
	}

	//spi_setup(WRM_spi);

	printk("SPI driver initialized.\n");
	time_since_boot=(jiffies-INITIAL_JIFFIES)/HZ;//since on some systems jiffies is a very huge uninitialized value at boot and saved.
	printk("Hi %s, this machine has been on for %d seconds\n", user_name, time_since_boot);

	
	return 0;
}



static void __exit WRMLED_kn_delete()
{
	/*
	1. delete cdev
	2. destroy device/class
	3. free alloc_chrdev_region
	*/
	//printk(KERN_DEBUG "SPI Removing.\n");
	//spi_unregister_driver(&WRM_spidriver);
	//class_destroy(spidev_dev_class);
	//unregister_chrdev(SPIDEV_MAJOR, WRM_spidriver.driver.name);	

	//hrtimer_try_to_cancel(&hr_timer);
	cdev_del(&WRMLED_devp->cdev);
	spi_unregister_driver(&WRM_spidriver);
	class_destroy(spidev_dev_class);
	unregister_chrdev(SPIDEV_MAJOR, WRM_spidriver.driver.name);	

//	spi_unregister_driver(&WRM_spidriver);
//	class_destroy(spidev_dev_class);
//	unregister_chrdev(SPIDEV_MAJOR, WRM_spidriver.driver.name);	
//	cdev_del(&WRMLED_devp->cdev);
	//kthread_stop(led_tsk);
	//class_destroy(WRMLED_dev_class);
	//unregister_chrdev_region(WRMLED_dev_num,DEVICES);

	printk(KERN_ALERT "Adios, my love\n");
	//return;
}

//static uint8_t WRMLED_kn_spi_write(uint8_t value)
//void WRM_spi_write(unsigned char [] value)
void WRM_spi_write(char * value)
{
	//printk("Debug mode: Entering SPI function, time = %ld\n", jiffies);
	spi_message_init(&(WRMLED_devp->led_m));
	//memset(xfer,0,sizeof(xfer));
	//WRMLED_devp->xfer.tx_buf = &value;
	WRMLED_devp->xfer.tx_buf = &value[0];
	WRMLED_devp->xfer.rx_buf = NULL;
	WRMLED_devp->xfer.len = 2;
	WRMLED_devp->xfer.delay_usecs = 1;
	WRMLED_devp->xfer.speed_hz = 1000000;
	WRMLED_devp->xfer.cs_change = 1;
	WRMLED_devp->xfer.bits_per_word = 8;
	//xfer[0].pad = 0;
	spi_message_add_tail(&(WRMLED_devp->xfer),&(WRMLED_devp->led_m));
	//spin_lock_irqsave(&spi_lock,spi_flags);
	gpio_set_value(15, 0);
	//spi_async(WRM_spi,&led_m);	
	spi_sync(WRM_spi,&(WRMLED_devp->led_m));	
	gpio_set_value(15, 1);
	//spin_unlock_irqrestore(&spi_lock,spi_flags);

}

//void timerFun (unsigned long arg) {
//enum hrtimer_restart TimerFun (struct hrtimer *timer) {
//	//ktime_t currtime;
//	//unsigned long time_ms = 1000000UL;//nanosec
//	////unsigned long delay_s, delay_ms;
//	//int i,j;
//	//int tmp;
//
//	printk("Debug mode: Entering HRTimer, time = %ld\n", jiffies);
//	//for(i = 0;i < WRMLED_devp -> index;i++){
//	//delay_s = (WRMLED_devp->duration_index[i]) / 1000;
//	//delay_ms = (WRMLED_devp->duration_index[i]) % 1000 * time_ms;
//	//ktime = ktime_set(delay_s,delay_ms);
//	//currtime = ktime_get();
//	//hrtimer_forward(&hr_timer,currtime,ktime);
//		//return HRTIMER_RESTART;
//	//}
//	return HRTIMER_NORESTART;
//}

// Open driver~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int WRMLED_kn_open(struct inode *inode, struct file *file)
{
	struct WRMLED_dev *WRMLED_devp;
	/* Get the per-device structure that contains this cdev */
	WRMLED_devp = container_of(inode->i_cdev, struct WRMLED_dev, cdev);

	/* Easy access to cmos_devp from rest of the entry points */
	file->private_data = WRMLED_devp;
	//ioctl_init = 0;
	printk("\n%s is openning \n", WRMLED_devp->name);

	gpio_request(13,"GPIO02"); 
	gpio_request(34,"GPIO02_shifter"); 
	gpio_request(77,"GPIO02_mux"); 
	gpio_request(14,"GPIO03"); 
	gpio_request(16,"GPIO03_shifter"); 
	gpio_request(76,"GPIO03_mux"); 
	gpio_request(64,"GPIO03_mux2"); 

	gpio_request(24,"GPIO11_shifter"); 
	gpio_request(44,"GPIO11_mux"); 
	gpio_request(72,"GPIO11_mux2"); 
	gpio_request(15,"GPIO12"); 
	gpio_request(42,"GPIO12_shifter"); 
	gpio_request(30,"GPIO13_shifter"); 
	gpio_request(46,"GPIO13_mux"); 
		
	gpio_direction_output(13, 0);
	gpio_direction_output(34, 0);
	//gpio_direction_output(77, 0);

	gpio_direction_input(14);

	gpio_direction_output(16, 0);
	//gpio_direction_output(76, 0);
	//gpio_direction_output(64, 0);

	gpio_direction_output(24, 0);
	gpio_direction_output(44, 0);
	//gpio_direction_output(72, 0);
	gpio_direction_output(15, 0);
	gpio_direction_output(42, 0);
	gpio_direction_output(30, 0);
	gpio_direction_output(46, 0);

	//gpio_set_value(34, 0);
	//gpio_set_value(77, 0);
	gpio_set_value(16, 1);
	//gpio_set_value(76, 0);
	//gpio_set_value(64, 0);

	//gpio_set_value(24, 0);
	gpio_set_value(44, 1);
	//gpio_set_value(72, 0);
	gpio_set_value(15, 1);
	//gpio_set_value(42, 0);
	//gpio_set_value(30, 0);
	gpio_set_value(46, 1);

	printk("GPIO initialized.\n");
	WRM_spi_write(shutdown_setting);
	WRM_spi_write(scan_limit_setting);
//#ifdef DEBUG
//	printk("Debug: SPI write display-test mode\n");
//	WRM_spi_write(display_test);
//	msleep(2000);	
//#endif
	WRM_spi_write(decode_mode_setting);
	WRM_spi_write(intensity_setting);
#ifdef DEBUG
	printk("Debug: SPI write normal display mode\n");
#endif
	WRM_spi_write(non_display_test);
	printk("SPI initialized.\n");

	return 0;
}

//  Release driver
 
static int WRMLED_kn_release(struct inode *inode, struct file *filp)
{
	//struct WRMLED_dev *WRMLED_devp = filp->private_data;
	
	printk("\n%s is closing\n", WRMLED_devp->name);
	//hrtimer_cancel(&hr_timer);
	kfree(pat_lis);

	gpio_free(13);
	gpio_free(34);
	gpio_free(77);
	gpio_free(14);
	gpio_free(16);
	gpio_free(76);
	gpio_free(64);

	gpio_free(24);
	gpio_free(44);
	gpio_free(72);
	gpio_free(15);
	gpio_free(42);
	gpio_free(30);
	gpio_free(46);
	
	//kfree(filp->private_data);
  	//filp->private_data = NULL;
	return 0;
}

#ifdef KTHREAD
//int led_thread_function(void * data)
//{
//	printk("Debug: Entering K thread function. May the force be with you!!!\n");
//	kthread_stop(led_tsk);
//	return 0;
//}
 
int led_thread_function(void * data)
{
	//unsigned long time_ms = 1000000UL;//nanosec
	//unsigned long delay_s, delay_ms;
	//struct WRMLED_dev *WRMLED_devp =(struct WRMLED_dev *) data;
	struct WRMLED_dev *WRMLED_devp =(void *) data;
	int j, i;
	int tmp;
	
	
//	printk("Debug: Entering K thread function. May the force be with you!!!\n");
//	ktime = ktime_set(0,100*time_ms);
//	hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
//	hr_timer.function = &TimerFun;

//	while(!kthread_should_stop()){
//		for(j=0;j<WRMLED_devp->index;j++){
//			printk("Debug mode: Pattern index = %d, Pattern duration = %d\n",WRMLED_devp->pattern_index[j],WRMLED_devp->duration_index[j]);
//		}
		for(i=0;i<WRMLED_devp->index;i++){
			for(j = 0;j < 8;j++){
				tmp = WRMLED_devp->pattern_index[i];
				x_pattern[0] = 0x01 + j; 
				x_pattern[1] = (WRMLED_devp->pattern).pattern_num[tmp].led[j]; 
				WRM_spi_write(x_pattern);
			}                                
			tmp = WRMLED_devp->duration_index[i];
//			printk("Debug: Loop = %d, druation = %d\n",i,tmp);
			msleep(tmp);
			//hrtimer_start(&hr_timer,ktime,HRTIMER_MODE_REL);
			//delay_s = (WRMLED_devp->duration_index[i]) / 1000;
			//delay_ms = (WRMLED_devp->duration_index[i]) % 1000 * time_ms;
			//ktime = ktime_set(delay_s,delay_ms);
			//currtime = ktime_get();
		}
//		if(i == WRMLED_devp->index){ 
//			kthread_stop(led_tsk);
//			printk("Debug: Stopping kthread!!!, time = %ld\n", jiffies);
//			msleep(10);
//		}else{
//			printk("Debug: Kthread not stopped!!!, time = %ld\n", jiffies);
//			msleep(10);
//		}
//	}
//	printk("Debug: Kthread stopped!!!, time = %ld\n", jiffies);

	//for(i = 0;i < WRMLED_devp -> index;i++){
	//	for(j = 0;j < 8;j++){
	//		x_pattern[0] = 0x01 + j; 
	//		x_pattern[1] = (WRMLED_devp->pattern).pattern_num[WRMLED_devp->pattern_index[i]].led[j]; 
	//		WRM_spi_write(x_pattern);
	//	}
	//	ktime = ktime_set(0,neg_in_ns);
	//	hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	//	hr_timer.function = &TimerFun;
	//	hrtimer_start(&hr_timer,ktime,HRTIMER_MODE_REL);
	// 	//add delay here, using WRMLED_devp-> duration_index[i]
	//}
	
	LED_KTHREAD = 0;//note that kthread is finished
	kthread_stop(led_tsk);
	//printk("Debug: Entering Timer function. May the force be with you!!!\n");
	return 0;
}

#endif

ssize_t WRMLED_kn_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
	struct WRMLED_dev *WRMLED_devp = file->private_data;
	int i,j=0;
#ifndef KTHREAD
	int tmp = 0;
#endif
	int temp = 0;
	int sel = 0;

	copy_from_user(WRMLED_devp->pattern_sequence, buf, count);
	//get_user(WRMLED_devp->pattern_squence,buf++);
	for(i=0;i<count;i++){
		switch(WRMLED_devp->pattern_sequence[i]){
			case '0':
				temp = temp*10;
				break;
			case '1':
				temp = temp*10 + 1;
				break;
			case '2':
				temp = temp*10 + 2;
				break;
			case '3':
				temp = temp*10 + 3;
				break;
			case '4':
				temp = temp*10 + 4;
				break;
			case '5':
				temp = temp*10 + 5;
				break;
			case '6':
				temp = temp*10 + 6;
				break;
			case '7':
				temp = temp*10 + 7;
				break;
			case '8':
				temp = temp*10 + 8;
				break;
			case '9':
				temp = temp*10 + 9;
				break;
			case ',':
				if(sel == 0)	WRMLED_devp -> pattern_index[j] = temp;
				else	WRMLED_devp -> duration_index[j] = temp; 
				//printk("Debug: Sel = %d, Value = %d\n",sel,temp);
				temp = 0;
				j = (sel) ? j+1:j;
				sel = sel^1;	
				break;
			case '(':
				j = 0;
				temp = 0;
				sel = 0;
				break;
			case ')':
				if(sel == 1 && temp == 0 && (WRMLED_devp -> pattern_index[j]) == 0){ 
					WRMLED_devp -> duration_index[j] = temp;
				//	printk("Debug: Sel = %d, Value = %d\n",sel,temp);
				//	printk("Display Sequence Transmitted Successfully!!!\n");
				}else{
				//	printk("Display Sequence Transmitted Failed...Terminating...\n");
					return -1;
				}
				break;
			case ' ':
				break;
			default:
				printk("Unknown Character in Display Sequence!!! Terminating...\n");
				return -1;
				break;
		}
	}
	WRMLED_devp -> index = j;
	//End of pattern transmitting, use pattern_index[j] and duration_index[j] for following function

	//init kthread for SPI write
	LED_KTHREAD = 1;
//	for(j=0;j<WRMLED_devp->index;j++){
//		printk("Debug mode: Pattern index = %d, Pattern duration = %d\n",WRMLED_devp->pattern_index[j],WRMLED_devp->duration_index[j]);
//	}
#ifdef KTHREAD 
	//printk("Debug: Entering K thread. May the force be with you!!!\n");
	led_tsk = kthread_run(&led_thread_function,(void *)WRMLED_devp,"led_thread_function");//data = NULL since not used
	if(IS_ERR(led_tsk)){
		printk("Error Creating KThread for Led Matrix!!! Terminating...\n");
		return -1;
	}
	//while(LED_KTHREAD == 1)
	//{}
#else
//	for(j=0;j<8;j++){
//		x_pattern[0] = 0x01 + j; 
//		x_pattern[1] = (WRMLED_devp->pattern).pattern_num[2].led[j]; 
//		printk("Debug mode: Pattern[2] = %c, %c,\n",x_pattern[1],(WRMLED_devp->pattern).pattern_num[2].led[j]);
//		
//		printk("Debug mode: Table for Pattern[2], %c, %c, %c, %c, %c, %c, %c, %c,\n",0x00,0x13,0x8f,0x7c,0x34,0x64,0xc0,0x00);
//		WRM_spi_write(x_pattern);
//	} 
	for(i=0;i<WRMLED_devp->index;i++){
		for(j = 0;j < 8;j++){
			tmp = WRMLED_devp->pattern_index[i];
			x_pattern[0] = 0x01 + j; 
			x_pattern[1] = (WRMLED_devp->pattern).pattern_num[tmp].led[j]; 
			WRM_spi_write(x_pattern);
		}                                
		tmp = WRMLED_devp->duration_index[i];
//		printk("Debug: Loop = %d, duration = %d\n",i,tmp);
		msleep(tmp);
	}


	
//	printk("Debug mode: Check table again\n");
//	for(j=0;j<10;j++)
//	{
//		printk("Debug mode: Pattern[%d][1] = %c,\n",j,(WRMLED_devp->pattern).pattern_num[j].led[1]);
//		printk("Debug mode: Pattern[%d][2] = %c,\n",j,(WRMLED_devp->pattern).pattern_num[j].led[2]);
//		printk("Debug mode: Pattern[%d][3] = %c,\n",j,(WRMLED_devp->pattern).pattern_num[j].led[3]);
//	}		
//	printk("Debug mode: Table 1, %c, %c, %c, %c, %c, %c, %c, %c, %c, %c,\n",0x00,0x00,0x13,0x93,0x66,0x00,0x00,0x00,0xc0,0x66);
//	printk("Debug mode: Table 2, %c, %c, %c, %c, %c, %c, %c, %c, %c, %c,\n",0x83,0x93,0x8f,0xcf,0xff,0xc0,0x60,0xe8,0x64,0xef);
//	printk("Debug mode: Table 3, %c, %c, %c, %c, %c, %c, %c, %c, %c, %c,\n",0xff,0xff,0xfc,0x7c,0xff,0x7c,0x3c,0x34,0x34,0xef);
#endif
	//kthread_stop(led_tsk); //kthread in the program should stop by itself when detecting (0,0)
	return 0;
}

//Transmit 10 patterns to kernel 
long WRMLED_kn_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct WRMLED_dev *WRMLED_devp = filp->private_data;
	struct PATTERN_list *pat_lis = kmalloc(sizeof(*pat_lis),GFP_KERNEL);
	int i;

//	printk("Debug: Entering IOCTL\n");
	switch (cmd){
		case IOCTL_VALSET:

			copy_from_user(pat_lis, (void *)arg, sizeof(*pat_lis));
			for(i=0;i<10;i++)
			{
				(WRMLED_devp->pattern).pattern_num[i] = pat_lis->pattern_num[i]; 
//				printk("Debug mode: Pattern[%d][1] = %c,\n",i,(WRMLED_devp->pattern).pattern_num[i].led[1]);
//				printk("Debug mode: Pattern[%d][2] = %c,\n",i,(WRMLED_devp->pattern).pattern_num[i].led[2]);
//				printk("Debug mode: Pattern[%d][3] = %c,\n",i,(WRMLED_devp->pattern).pattern_num[i].led[3]);
			}		
//			printk("Debug mode: Table 1, %c, %c, %c, %c, %c, %c, %c, %c, %c, %c,\n",0x00,0x00,0x13,0x93,0x66,0x00,0x00,0x00,0xc0,0x66);
//			printk("Debug mode: Table 2, %c, %c, %c, %c, %c, %c, %c, %c, %c, %c,\n",0x83,0x93,0x8f,0xcf,0xff,0xc0,0x60,0xe8,0x64,0xef);
//			printk("Debug mode: Table 3, %c, %c, %c, %c, %c, %c, %c, %c, %c, %c,\n",0xff,0xff,0xfc,0x7c,0xff,0x7c,0x3c,0x34,0x34,0xef);
			break;
		default:
			printk("Debug: Bug in IOCTL\n");
			return -1;
	}
    	return 0;
}

static int spidev_probe(struct spi_device *spi)
{
	//struct spidev_data *spidev_local;
	//int status;
	//unsigned long minor;

	//if(spi->dev.of_node && !of_match_device(spidev_dt_ids, &spi->dev)){
	//	WARN_ON(spi->dev.of_node && !of_match_device(spidev_dt_ids, &spi->dev));	
	//}
	//spidev_probe_acpi(spi);
	//allocate driver data
	//status = kzalloc(sizeof(*spidev_local),GFP_KERNEL);
	//if(!status) return -ENOMEM;
	
	//Create Device
	WRMLED_dev_device = device_create(spidev_dev_class, &spi->dev, MKDEV(SPIDEV_MAJOR, 0), NULL, DEVICE_NAME_SPI);	
	
	//initial the driver data
	//WRM_spi->spi = spi;
	WRM_spi = spi;
	//WRM_spi->speed_hz = spi->max_speed_hz;
	//spi_setup(spi);
	//spi_set_drvdata(spi,spidev_local);
	return 0;
	//spin_lock_init(&spidev_local->spi_lock);
	//mutex_init(&spidev_local->buf_lock);

	//INIT_LIST_HEAD(&spidev_local->device_entry);
	//mutex_lock(&device_list_lock);
	//minor = find_first_zero_bit(monors, N_SPI_MINORS);
	
	
		
}
static int spidev_remove(struct spi_device *spi)
{
	printk(KERN_DEBUG "SPI Removing.\n");
	//WRM_spi = NULL;
	//unregister_chrdev_region(MAJOR(spidev_num),1);	
	//device_destroy(spidev_dev_class,DEVICES);
	//kfree(WRMLED_devp);
	device_destroy(spidev_dev_class,MKDEV(SPIDEV_MAJOR, 0));
	kfree(WRMLED_devp);
	return 0;
}


module_init(WRMLED_kn_create);
module_exit(WRMLED_kn_delete);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A Walking Red Man LED driver in kernel");
MODULE_AUTHOR("YU-CHUN HUANG");
