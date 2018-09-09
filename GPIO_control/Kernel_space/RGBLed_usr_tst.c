#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <linux/input.h>
#include <syscall.h>
#include <inttypes.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "RGBLed_ioctl.h"

#define MOUSEFILE "/dev/input/event3"
#define DRIVER_NAME "/dev/RGBLed"

#define Color_R   "100"
#define Color_G   "010"
#define Color_B   "001"
#define Color_RG  "110"
#define Color_RB  "101"
#define Color_GB  "011"
#define Color_RGB "111"

//#define DEBUG
//struct RGB_info{
//	int duty_cycle;
//	int R_pin;
//	int G_pin;
//	int B_pin;
//} RGB_dev;
struct RGB_info RGB_dev;

int END;

//ocasi, mouse detect
void *mouse_detect(void *data)
{
	int fd;
	struct input_event ie;

	if((fd = open(MOUSEFILE, O_RDONLY)) == -1) {
		perror("opening device");
		exit(EXIT_FAILURE);		
		pthread_exit(NULL);
	}

	while(read(fd, &ie, sizeof(struct input_event))) {
		if((ie.code == 272 && ie.value == 1) || (ie.code == 273 && ie.value == 1)){ 
			printf("Mouse click detected. TERMINATING!!!\n");
			END = 1;
			break;
		}	
	}
	pthread_exit(NULL);
}

int main()
{
	int fd;
	long io_err=0;
	//int duty_cycle, R_pin, G_pin, B_pin;
	
	END = 0;
	fd = open(DRIVER_NAME, O_RDWR);
	if (fd < 0){
		printf("Can not open device file.\n");		
		return -1;
	}else{
		printf("Please input duty_cycle, R_pin, G_pin and B_pin\n");
		scanf("%d %d %d %d", &RGB_dev.duty_cycle, &RGB_dev.R_pin, &RGB_dev.G_pin, &RGB_dev.B_pin);
		io_err = ioctl(fd,IOCTL_VALSET,&RGB_dev);//parse info to kernel for input check and pin setting
		if(io_err == -1){
			errno = EINVAL;
			return -1;
		}
	#ifdef DEBUG
		int i;
		for(i=0;i<1;i++)
	#else
		int error;
		pthread_t pt_mouse;
		
		error = pthread_create(&pt_mouse, NULL, mouse_detect, NULL);
		if(error != 0) printf("Thread mouse detect not create...!?\n");

		while(1)
	#endif
		{
			write(fd, Color_R  , 3);
			usleep(period*1000);
			if(END) break;
			write(fd, Color_G  , 3);
			usleep(period*1000);
			if(END) break;
			write(fd, Color_B  , 3);
			usleep(period*1000);
			if(END) break;
			write(fd, Color_RG , 3);
			usleep(period*1000);
			if(END) break;
			write(fd, Color_RB , 3);
			usleep(period*1000);
			if(END) break;
			write(fd, Color_GB , 3);
			usleep(period*1000);
			if(END) break;
			write(fd, Color_RGB, 3);
			usleep(period*1000);
			if(END) break;
		}
	}
	//release(DRIVER_NAME);
	close(fd);
	return 0;
}
