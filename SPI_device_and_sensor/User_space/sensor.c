#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <poll.h>
#include <signal.h>
#include <linux/input.h>
#define _GNU_SOURCE   
#include <math.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <inttypes.h>


//  Linux/GCC
uint64_t rdtsc(){
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

struct pollfd fds;
int timeout;
int poll_fd=0;

void IOSetup(void)
{
	int FdExport;
	int Fd13, Fd34, Fd77;//IO2 Triger
	int Fd14, Fd16, Fd76;//IO3 Echo
	int Fd64;    
	
	FdExport = open("/sys/class/gpio/export", O_RDONLY | O_WRONLY| O_NONBLOCK);
		
		write(FdExport,"13",2);
		write(FdExport,"34",2);
		write(FdExport,"77",2);
		write(FdExport,"14",2);
		write(FdExport,"16",2);
		write(FdExport,"76",2);
		write(FdExport,"64",2);
		//IO2_trigger
		Fd13 = open("/sys/class/gpio/gpio13/direction", O_WRONLY);
		if (Fd13 < 0)
			printf("\n gpio13 direction open failed");
		
		Fd34 = open("/sys/class/gpio/gpio34/direction", O_WRONLY);
		if (Fd34 < 0)
			printf("\n gpio34 direction open failed");

		
		/*enter direction type*/
		if(0> write(Fd13,"out",3))
			printf("error Fd13");

		if(0> write(Fd34,"out",3))
			printf("error Fd34");
		

		//IO2 set value
		Fd34 = open("/sys/class/gpio/gpio34/value",O_WRONLY);
		if (Fd34 < 0)	
			printf("\n gpio34 value open failed");
				
		Fd77 = open("/sys/class/gpio/gpio77/value", O_WRONLY);
		if (Fd77 < 0)
			printf("\n gpio77 value open failed");
				

		if(0> write(Fd34,"0",1))
			printf("error Fd34 value");

		if(0> write(Fd77,"0",1))
			printf("error Fd77 value");


		//I03_echo
		Fd14 = open("/sys/class/gpio/gpio14/direction", O_WRONLY);
		if (Fd14 < 0)
			printf("\n gpio14 direction open failed");
		
		Fd16 = open("/sys/class/gpio/gpio16/direction", O_WRONLY);
		if (Fd16 < 0)
			printf("\n gpio16 direction open failed");
		
		
		
		/*enter direction type*/
		if(0> write(Fd14,"in",2))
			printf("error Fd14");

		//if(0> write(Fd16,"out",3))//ocasi,in?
		if(0> write(Fd16,"in",2))//ocasi,in?
			printf("error Fd16");

		//set value
		Fd16 = open("/sys/class/gpio/gpio16/value",O_WRONLY);
		if (Fd16 < 0)	
			printf("\n gpio16 value open failed");
				
		Fd76 = open("/sys/class/gpio/gpio76/value", O_WRONLY);
		if (Fd76 < 0)	
			printf("\n gpio76 value open failed");
		Fd64 = open("/sys/class/gpio/gpio64/value", O_WRONLY);
		if (Fd76 < 0)	
			printf("\n gpio64 value open failed");
		
		if(0> write(Fd16,"1",1))
			printf("error Fd16 value");
		if(0> write(Fd76,"0",1))
			printf("error Fd76 value");
		if(0> write(Fd64,"0",1))
			printf("error Fd64 value");
}




int main()
{
	
	IOSetup();
	int poll_fall_ret;
	int trigger=0;
	unsigned long long echo_value,echo_start_time,echo_end_time;
	//int echo_value=0;
	//int echo_start_time=0;
        //int echo_end_time=0;
	int poll_ret;
	//int poll_fd_falling=0;
	int poll_wait_value=0;
	//char buff[1];
	trigger= open("/sys/class/gpio/gpio13/value", O_WRONLY);
	poll_wait_value =  open("/sys/class/gpio/gpio14/value", O_RDONLY);//poll target
	poll_fd = open("/sys/class/gpio/gpio14/edge", O_WRONLY);
	write(trigger,"0",1);
	while(1)
	{
		write(poll_fd,"rising",6);
		

		fds.fd=poll_wait_value;
		//fds.events=POLLPRI|POLLIN;
		fds.events=POLLIN;
		//fds.revents=0;
		timeout = 500;
		
		write(trigger,"1",1);
		//usleep(15);
		usleep(10);
		write(trigger,"0",1);
		//usleep(60000);//60ms, ocasi
		
		poll_ret=poll(&fds,1,timeout);
		
		//defualt
		printf("Debug: fds.revents = %d\n",fds.revents);
		if (poll_ret<0)
			printf("no value\n");
		else if (poll_ret==0)
			printf("timeout\n");
		//if ((fds.revents & POLLERR) == POLLERR)
		//	printf("Polling Error\n");
		
		//if((fds.revents & POLLPRI) == POLLPRI || (fds.revents & POLLIN) == POLLIN)
		if((fds.revents & POLLIN) == POLLIN)
		{
			printf("sucessful\n");
			echo_start_time=rdtsc();
			//read(fds.fd,buff,1);
			//printf("%d\n", echo_start_time);
			printf("Start = %llu\n", echo_start_time);
			
			//poll_fd_falling=open("/sys/class/gpio/gpio14/edge", O_WRONLY);
			//write(poll_fd_falling,"falling",7);        
			write(poll_fd,"falling",7);        
			poll_fall_ret=poll(&fds,1,timeout);
			
			if(poll_fall_ret==1)
			{
				//if(fds.revents & POLLPRI)
				if((fds.revents & POLLIN) == POLLIN)
				{
					printf("sucessful_falling\n");
					echo_end_time=rdtsc();
					//read(fds.fd,buff,1);
					//printf("%d\n", echo_end_time);
					printf("End = %llu\n", echo_end_time);
					echo_value=echo_end_time-echo_start_time;
					//printf("%d\n", echo_value);
					printf("Diff = %llu\n", echo_value);
				}
			}
		}
					
		sleep(2);
		//usleep(500000);
	}
	close(poll_fd);
	return 0;
}

