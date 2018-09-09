#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/irq.h>
#include <asm/errno.h>
#include <linux/math64.h>

#define DRIVER_NAME "pulse"
#define PULSE_MINOR_NUMBER 0 		
#define GPIO_DIRECTION_IN 1
#define GPIO_DIRECTION_OUT 0
#define GPIO_VALUE_LOW 0
#define GPIO_VALUE_HIGH 1
#define RISE_DETECTION 0
#define FALL_DETECTION 1

static dev_t pulse_dev_number;      /* Allotted Device Number */
static struct class *pulse_class;   /* Device class */
static unsigned char Edge = RISE_DETECTION;


/*per device structure*/

typedef struct Pulse_Device_Tag
{
	struct cdev cdev;               /* The cdev structure */
	char name[20];                  /* Name of device */
	unsigned int BUSY_FLAG;		  	/* Busy Flag Status */
	unsigned long long timeRising;		/* TimeStamp to record Start Time */
	unsigned long long timeFalling;		/* TimeStamp to record End Time */
	int irq;
} Pulse_Device;

Pulse_Device *pulse_dev;

/*TSC*/
static __inline__ unsigned long long rdtsc(void)
{
	unsigned hi, lo;
	__asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
	return ((unsigned long long) lo) | ((unsigned long long) hi)<<32;
}

/*change_state_interrupt*/
static irqreturn_t change_state_interrupt(int irq, void *dev_id)
{
	if(Edge==RISE_DETECTION)
	{
		pulse_dev->timeRising = rdtsc();	
	        irq_set_irq_type(irq, IRQF_TRIGGER_FALLING);
	        Edge=FALL_DETECTION;
	}
	else
	{
		pulse_dev->timeFalling = rdtsc();
	        irq_set_irq_type(irq, IRQF_TRIGGER_RISING);
	        Edge=RISE_DETECTION;
		pulse_dev->BUSY_FLAG = 0;
	}
	return IRQ_HANDLED;
}

/*pulse_open*/
int pulse_open(struct inode *inode, struct file *filp)
{
	int irq_line;
	int irq_req_res_rising;
	int retValue;
	
	pulse_dev->BUSY_FLAG = 0;
	pulse_dev = container_of(inode->i_cdev, Pulse_Device, cdev);
	filp->private_data = pulse_dev;
	
	gpio_request(13,"GPIO02"); 
	gpio_request(34,"GPIO02_shifter"); 
	gpio_request(77,"GPIO02_mux"); 
	gpio_request(14,"GPIO03"); 
	gpio_request(16,"GPIO03_shifter"); 
	gpio_request(76,"GPIO03_mux01"); 
	gpio_request(64,"GPIO03_mux02"); 

	gpio_direction_output(13, 0);
	gpio_direction_output(34, 0);
	gpio_direction_output(77, 0);
	gpio_direction_input(14);
	gpio_direction_output(16, 1);
	gpio_direction_output(76, 0);
	gpio_direction_output(64, 0);
	
	printk("GPIO initialized.\n");
	
	

	
	/*install interrupt handler*/
	irq_line = gpio_to_irq(14);
	
	if(irq_line < 0)
	{
		printk("Gpio %d cannot be used as interrupt\n",14);
		retValue=-EINVAL;
	}
	pulse_dev->irq = irq_line;
	
	pulse_dev->timeRising=0;
	pulse_dev->timeFalling=0;
	
	irq_req_res_rising = request_irq(irq_line, change_state_interrupt, IRQF_TRIGGER_RISING, "gpio_change_state", pulse_dev);
	if(irq_req_res_rising)
	{
		printk("Unable to claim irq %d; error %d\n ", irq_line, irq_req_res_rising);
		return 0;
	}
	
	printk("HCSR04 Open End \n");
	return 0;
}

/*pulse_release*/
int pulse_release(struct inode *inode, struct file *filp)
{
	Pulse_Device *local_pulse_dev;
	
	pulse_dev->BUSY_FLAG = 0;
	local_pulse_dev = filp->private_data;
	free_irq(pulse_dev->irq,pulse_dev);
	
	gpio_free(13);
	gpio_free(14);
	gpio_free(34); 
	gpio_free(77); 
	gpio_free(16); 
	gpio_free(76); 
	gpio_free(64); 
	printk("Good Night!!\n");
	return 0;
}

/*pulse_write */
static ssize_t pulse_write(struct file *filp, const char *buf, size_t count, loff_t *ppos)
{
	int retValue = 0;
	
	if(pulse_dev->BUSY_FLAG == 1)
	{
		retValue = -EBUSY;
		return -EBUSY;
	}
	
	gpio_set_value_cansleep(13, 1);
	udelay(15);
	gpio_set_value_cansleep(13, 0);
	pulse_dev->BUSY_FLAG = 1;
	return retValue;
}

/*pulse_read*/
static ssize_t pulse_read(struct file *file, char *buf, size_t count, loff_t *ptr)
{	int retValue=0;
	unsigned int c;
	unsigned long long tempBuffer;
	if(pulse_dev->BUSY_FLAG == 1)
	{
		return -1;
	}
	else
	{

		tempBuffer = pulse_dev->timeFalling - pulse_dev->timeRising;
		c = div_u64(tempBuffer,23323);
		retValue = copy_to_user((void *)buf, (const void *)&c, sizeof(c));
	}
	return retValue;
}


/*File operations*/
static struct file_operations pulse_fops =
{
		.owner = THIS_MODULE,		
		.open = pulse_open,            
		.release = pulse_release,     
		.write = pulse_write,           
		.read = pulse_read				
};

/* pulse_init */
static int __init pulse_init(void)
{
	int retValue;
	
	/* Request dynamic allocation of a device major number */
	if(alloc_chrdev_region(&pulse_dev_number, 0, PULSE_MINOR_NUMBER, DRIVER_NAME) < 0)
	{
		printk("Can't register device\n");
		return -1;
	}
	
	/* Populate sysfs entries */
	pulse_class = class_create(THIS_MODULE, DRIVER_NAME);
	
	if(IS_ERR(pulse_class)){
		printk("Can't create class\n");
		return -1;
	}
	/* Allocate memory pulse_dev */
	pulse_dev = kmalloc(sizeof(Pulse_Device), GFP_KERNEL);
	if(!pulse_dev)
	{
		printk("Bad Kmalloc pulse_dev\n");
		return -1;
	}

	/* Request I/O Region */
	sprintf(pulse_dev->name, DRIVER_NAME);

	/* Connect the file operations with the cdev */
	cdev_init(&pulse_dev->cdev, &pulse_fops);
	pulse_dev->cdev.owner = THIS_MODULE;

	/* Connect the major/minor number to the cdev */
	retValue = cdev_add(&pulse_dev->cdev, MKDEV(MAJOR(pulse_dev_number), PULSE_MINOR_NUMBER), 1);
	if(retValue)
	{
		printk("Bad cdev for pulse_dev\n");
		return retValue;
	}
	
	/* A struct device will be created in sysfs, registered to the specified class.*/
	device_create(pulse_class, NULL, MKDEV(MAJOR(pulse_dev_number), PULSE_MINOR_NUMBER), NULL, DRIVER_NAME);
	
	printk("Team16 ready to Go!\n");
	return 0;
}

/*pulse_exit*/
static void __exit pulse_exit(void)
{

	/* Destroy device with Minor Number 0*/
	
	cdev_del(&pulse_dev->cdev);
	device_destroy(pulse_class, MKDEV(MAJOR(pulse_dev_number), PULSE_MINOR_NUMBER));
	kfree(pulse_dev);
	class_destroy(pulse_class);
	unregister_chrdev_region(pulse_dev_number, 1);
	printk(KERN_ALERT "Good Night!!\n");

}

module_init(pulse_init);
module_exit(pulse_exit);

MODULE_AUTHOR("MIN CHE WENG");
MODULE_DESCRIPTION("PULSE Driver");
MODULE_LICENSE("GPL");


