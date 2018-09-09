#include <errno.h>
#include <inttypes.h> 
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <poll.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#define _GNU_SOURCE  
#include <sys/types.h>
#include <linux/input.h>
#include <linux/spi/spidev.h>

#include "pattern.h"
#include "WRMLED_ioctl.h"

//#define SS "/sys/class/gpio/gpio15/value"

#define BRIGHT
#define SENSOR_ADDED
//#define KTHREAD
//#define DEBUG

//#define DRIVER_NAME "/dev/WRMLED"
//#define DRIVER_NAME "/dev/spidev1.0"
#define DRIVER_NAME "/dev/spidev_WRM"
#ifdef SENSOR_ADDED
#define PULSE_DRIVER_NAME "/dev/pulse"
#endif
#ifndef KTHREAD
#define TOTAL_RUN_TIME 20
#endif

//**********User define area**********
//#define Slow_Duration 400*1000 //400ms
//#define Medium_Duration 200*1000 //200ms
//#define Fast_Duration 100*1000 //100ms
//#define Ultra_Duration 50*1000 //50ms
//
//#define Slow_Medium_Boundary 2400
//#define Medium_Fast_Boundary 1800
//#define Fast_Ultra_Boundary  1200

//#define test_pattern "(0,100,3,200,0,0)"
#define test_pattern_L0 "(0,200,1,200,2,200,3,200,4,200,0,0)" //char string of test_pattern
#define test_pattern_L1 "(0,100,1,100,2,100,3,250,4,100,0,0)" //char string of test_pattern
#define test_pattern_L2 "(0, 50,1, 50,2, 50,3, 50,4, 50,0,0)" //char string of test_pattern
#define test_pattern_length 35 //total char # of test_pattern
#define test_pattern_R0 "(5,200,6,200,7,200,8,200,9,200,0,0)" //char string of test_pattern
#define test_pattern_R1 "(5,100,6,100,7,100,8,100,9,100,0,0)" //char string of test_pattern
#define test_pattern_R2 "(5, 50,6, 50,7, 50,8, 50,9, 50,0,0)" //char string of test_pattern
#define test_pattern_time_0 1200000
#define test_pattern_time_1  600000
#define test_pattern_time_2  300000
//**********User define area**********
#ifndef KTHREAD
#ifdef SENSOR_ADDED
int END;
int pulseWidth=15;
int last_pulseWidth=20;
pthread_mutex_t Width_mutex;
#endif
#endif

struct PATTERN_list TEN_PATTERN;

#ifdef SENSOR_ADDED
int write_pulse(int fd)
{
	int retValue=0;
   	char* writeBuffer;
	
	writeBuffer = (char *)malloc(10);
	while(1)
	{
		retValue = write(fd, writeBuffer, 10);
		if(retValue < 0)
		{
			printf("Trigger Failure\n");
		}
		else
		{
			printf("Trigger Successful\n");
			break;
		}
		usleep(100000);
	}
	free(writeBuffer);
	return retValue;
}


int read_pulse(int fd)
{
	int retValue=0;
  	unsigned int writeBuffer =0;
	while(1)
	{
		retValue = read(fd, (char *) &writeBuffer, sizeof(writeBuffer));
		if(retValue < 0)
		{
			printf("Read Failure\n");
		}
		else
		{
			//printf("Read Successful\n");
			break;
		}
		usleep(100000);
	}
	return writeBuffer;
}
#endif



#ifndef KTHREAD
#ifdef SENSOR_ADDED
void *sensor_detect(void *data)
{
	int fd = *(int *)data;
	while(END^1)
	{
		write_pulse(fd);
		pthread_mutex_lock(&Width_mutex);
		pulseWidth = read_pulse(fd);
		printf("The distance is : %d\n",pulseWidth);
		pthread_mutex_unlock(&Width_mutex);
		usleep(100000);
	}
	#ifdef DEBUG
	printf("Debug: Sensor thread ending...\n");
	#endif
	pthread_exit(NULL);
}
#endif

void *led_matrix_display(void *data)
{
	int spi_fd = *(int *)data;
	int direction = 0;
	int speed = 0;
	int pat = 0;
	int temp;
	while(END^1)
	{
		pthread_mutex_lock(&Width_mutex);
		temp = pulseWidth;
		pthread_mutex_unlock(&Width_mutex);
		direction = (temp != last_pulseWidth) ? ((temp > last_pulseWidth) ? 1 : 0) : direction;//1: right, 0: left
		if(temp <= 12) speed = 2;
		else if(temp <= 20) speed = 1;
		else speed = 0;
		pat = direction*4 + speed;
		printf("Direction = %d, Speed = %d, pat = %d, last_pulseWidth = %d, pulseWidth = %d\n",direction,speed,pat,last_pulseWidth,temp);
		switch(pat)
		{
			case 0:
				write(spi_fd,test_pattern_L0,test_pattern_length);
				usleep(test_pattern_time_0);
				last_pulseWidth = temp; 
				break;
			case 1:
				write(spi_fd,test_pattern_L1,test_pattern_length);
				usleep(test_pattern_time_1);
				last_pulseWidth = temp; 
				break;
			case 2:
				write(spi_fd,test_pattern_L2,test_pattern_length);
				usleep(test_pattern_time_2);
				last_pulseWidth = temp; 
				break;
			case 4:
				write(spi_fd,test_pattern_R0,test_pattern_length);
				usleep(test_pattern_time_0);
				last_pulseWidth = temp; 
				break;
			case 5:
				write(spi_fd,test_pattern_R1,test_pattern_length);
				usleep(test_pattern_time_1);
				last_pulseWidth = temp; 
				break;
			case 6:
				write(spi_fd,test_pattern_R2,test_pattern_length);
				usleep(test_pattern_time_2);
				last_pulseWidth = temp; 
				break;
			default:
				printf("Invalid speed/direction of WRM...\n");	
				usleep(100000);
		}
	}
	#ifdef DEBUG
	printf("Debug: Display thread ending...\n");
	#endif
	pthread_exit(NULL);
}
#endif


int main()
{
	
	int spi_fd=0;
#ifdef KTHREAD
	int i;
#endif
	#ifdef SENSOR_ADDED
	int sensor_fd=0;
	#endif
#ifndef KTHREAD
	#ifdef SENSOR_ADDED
	pthread_t pt_sensor,pt_led_matrix;
	#else
	pthread_t pt_led_matrix;
	#endif
#endif
	//int errno;
	long error;
       
	printf("RED man, RUN FOR YOUR LIFE!!!\n");
	spi_fd = open(DRIVER_NAME, O_RDWR);
	if(spi_fd<0){
		printf("Can't access SPI DRIVER!!!\n");
		return -1;
	}
	#ifdef SENSOR_ADDED
	sensor_fd = open(PULSE_DRIVER_NAME, O_RDWR);
	if(sensor_fd<0){
		printf("Can't access SENSOR DRIVER!!!\n");
		return -1;
	}
	#endif
	TEN_PATTERN.pattern_num[0] = walk_left_0;
	TEN_PATTERN.pattern_num[1] = walk_left_1;
	TEN_PATTERN.pattern_num[2] = walk_left_2;
	TEN_PATTERN.pattern_num[3] = walk_left_3;
	TEN_PATTERN.pattern_num[4] = walk_left_4;
	TEN_PATTERN.pattern_num[5] = walk_right_0;
	TEN_PATTERN.pattern_num[6] = walk_right_1;
	TEN_PATTERN.pattern_num[7] = walk_right_2;
	TEN_PATTERN.pattern_num[8] = walk_right_3;
	TEN_PATTERN.pattern_num[9] = walk_right_4;

	error = ioctl(spi_fd,IOCTL_VALSET,&TEN_PATTERN);//parse info to kernel for input check and pin setting
	if(error == -1){
		//errno = EINVAL;
		printf("IOCTL error\n");
		printf("Return code: %ld, meaning: %s\n",error, strerror(errno));
		return -1;
	}
#ifndef KTHREAD
	#ifdef SENSOR_ADDED
	error = pthread_create(&pt_sensor, NULL, sensor_detect, &sensor_fd);
	if(error != 0) printf("Sensor thread not created...!?\n");
	#endif
	error = pthread_create(&pt_led_matrix, NULL, led_matrix_display, &spi_fd);
	if(error != 0) printf("LED matrix thread not create...!?\n");
	
	//sleep(test_pattern_time_0);

	sleep(TOTAL_RUN_TIME);
	END = 1;
	#ifdef SENSOR_ADDED
	pthread_join(pt_sensor,NULL);
	#endif
	pthread_join(pt_led_matrix,NULL);
#else	
	for(i=0;i<10;i++){
	write(spi_fd,test_pattern_L0,test_pattern_length);
				usleep(test_pattern_time_0);
	printf("uuuu\n");
	write(spi_fd,test_pattern_L1,test_pattern_length);
				usleep(test_pattern_time_1);
	printf("vvvv\n");
	write(spi_fd,test_pattern_L2,test_pattern_length);
				usleep(test_pattern_time_2);
	}
#endif
	printf("GOAL!!!\n");
	close(spi_fd);
	#ifdef SENSOR_ADDED
	close(sensor_fd);
	#endif
	return 0;

}











