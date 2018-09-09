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
//#include <sched.h>
//#include <bits/sched.h>
//#include <iostream>

#define MOUSEFILE "/dev/input/event4"
#define BASE_PERIOD 1000
#define SIZE 10
#pragma intrinsic(__rdtsc)  

//Sorry, we are unable to use dynamic allocation this time...:( .by YuChun Huang
double items1[SIZE+2];//queue 1
double items2[SIZE+2];//queue 2
double * items[2];

int num_i1, num_i2;//number of data received at each queue
int64_t total_i1, total_i2;//total of data value received at each queue(E[X])
int64_t e2_i1, e2_i2;//E[X^2] of each queue, to calculate std deviation
int64_t i1_write_t[SIZE];//store for the write time for queue1
int64_t i2_write_t[SIZE];//store for the write time for queue2

sem_t s_act; 
int ret;
int END = 0;

struct sched_param param;


//  Linux/GCC
uint64_t rdtsc(){
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}


//	   int j=0;
//        int sum=0;
//        int mean=0;
//        int standardDeviation=0;
//        int64_t b;
//        int64_t ti[200];
		



//int main()
//{
//    for (int i=1; i<=5;i++)
//	 { 
// 	     b=rdtsc();
//	//	b=b+1;
//		sleep(1);//deal with the process
//	   	ti[i]=b;
//	  }
//          
//
//
//		for(int k=1; k<=5; k++)
//                    {
//                         sum += ti[k];
//                         printf("%"PRIu64" \n",ti[k]);
//                    }
//
//    		mean = sum/5;
//
//   		 for(int l = 1; l <= 5; l++)
//       		{
//				 standardDeviation += (ti[l] - mean)*(ti[l] - mean);
//			}
//		 standardDeviation=standardDeviation/5;
//
//       printf("%"PRIu64" \n",ti[1]);
//       printf("%"PRIu64" \n",ti[2]);
//       printf("%"PRIu64" \n",standardDeviation);
//       printf("%"PRIu64" \n",mean);
//
//
//}

//-----------------------------------------------------------------------------------//
int isFull(double* it)
{
    if( (it[10] == it[11] + 1) || (it[10] == 0 && it[11] == SIZE-1)) return 1;
    return 0;
}

int isEmpty(double* it)
{
    if(it[10] == -1) return 1;
    return 0;
}

void sq_write(double* it, double element)//write queue
{
    if(isFull(it)) { 
	printf("\n Queue is full!! \n");
	//return(-1);
    }else{
        if(it[10] == -1) it[10] = 0;
        it[11] = (int)(it[11] + 1) % SIZE;
        it[(int)it[11]] = element;
	if(it==&items1[0]){
		i1_write_t[(int)it[11]] = rdtsc();
		printf("TIME Q1 = %"PRIu64"\n",i1_write_t[(int)it[11]]);
	}else{
		i2_write_t[(int)it[11]] = rdtsc();
		printf("TIME Q2 = %"PRIu64"\n",i2_write_t[(int)it[11]]);
	}
    }
}


double sq_read(double* it)//read queue
{
    double element;
    int64_t temp;
    
    if(isEmpty(it)) {
        printf("\n Queue is empty !! \n");
        return(-1);
    } else {
        element = it[(int)it[10]];
	if(it==&items1[0]){// count for TSC	
		temp = rdtsc();
		temp -= i1_write_t[(int)it[10]];
		printf("TIME interval Q1 = %"PRIu64"\n",temp);
 		num_i1 += 1;
 		total_i1 += temp;
 		e2_i1 += pow(temp,2);
	}else{	
		temp = rdtsc();
		temp -= i2_write_t[(int)it[10]];
		printf("TIME interval Q2 = %"PRIu64"\n",temp);
 		num_i2 += 1;
 		total_i2 += temp;
 		e2_i2 += pow(temp,2);
	}
        if (it[10] == it[11]){
            it[10] = -1;
            it[11] = -1;
        } 
        else {
            it[10] = (int)(it[10] + 1) % SIZE;
            
        }
        printf("\n Deleted element -> %lf \n", element);
        return(element);
    }
}

//-----------------------------------------------------------------------------------//

double time_diff(struct timeval x , struct timeval y)//to calculate time difference for timeval data type
{
    double x_ms , y_ms , diff;
     
    x_ms = (double)x.tv_sec*1000000 + (double)x.tv_usec;
    y_ms = (double)y.tv_sec*1000000 + (double)y.tv_usec;
     
    diff = (double)y_ms - (double)x_ms;
     
    return diff;
}

struct timespec time_add(struct timespec start, int a) {//to deal with timespec data type, for periodic thread
	struct timespec temp;
	long int b = (a * BASE_PERIOD) % 1000000000;
	long int c = (a * BASE_PERIOD) / 1000000000;


	if(b + start.tv_nsec >= 1000000000){
		temp.tv_nsec = start.tv_nsec + b - 1000000000;
		temp.tv_sec = start.tv_sec + c + 1;
	}else{
		temp.tv_nsec = start.tv_nsec + b;
		temp.tv_sec = start.tv_sec + c;
	}
	return temp;
}

//C program citing from 
//https://www.codeproject.com/Articles/813185/Calculating-the-Number-PI-Through-Infinite-Sequenc
// Approximation of the number PI through the Viete's series
// Language: C
// Author: Jose Cintra (jose.cintra@html-apps.info)
double pi_iteration(double n){//n : iteration number
   double i, j;     // Number of iterations and control variables
   double f;           // factor that repeats
   double pi = 1;
      
   for(i = n; i > 1; i--) {
      f = 2;
      for(j = 1; j < i; j++){
         f = 2 + sqrt(f);
      }
      f = sqrt(f);
      pi = pi * f / 2;
   }
   pi *= sqrt(2) / 2;
   pi = 2 / pi;
   
   return pi;
}

//-----------------------------------------------------------------------------------//
void *thread_receiver(void *data)//receiver thread with period 40*PERIOD
{
	double value;
	struct timespec period;
	//double cqueue[2] = *(double *)data;
	int queue_sel = 0;
	printf("New RX thread created...!?\n");

	clock_gettime(CLOCK_MONOTONIC, &period);
	printf("R-Thread number %ld\n", syscall(SYS_gettid));
	while (!END){

		sem_wait(&s_act);
		switch(queue_sel){
		case 0:
			while(items1[10] != items1[11]){
				value = sq_read(&items1[0]);
				printf("Dequeue value = %lf\n", value);
			}
			break;
		case 1:
			while(items2[10] != items2[11]){
				value = sq_read(&items2[0]);
				printf("Dequeue value = %lf\n", value);
			}
			break;
		}
		queue_sel = !queue_sel;
  		period = time_add(period, 40);
		sem_post(&s_act);
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &period, 0);
	}
	while(items1[10] != items1[11]){
		value = sq_read(&items1[0]);
		printf("Dequeue value = %lf\n", value);
	}
	while(items2[10] != items2[11]){
		value = sq_read(&items2[0]);
		printf("Dequeue value = %lf\n", value);
	}//pop out all remaining data for safety
	pthread_exit(NULL);
}
//-----------------------------------------------------------------------------------//

void *thread_period(void *data)//periodic thread, with 4 different multiplier for PERIOD 
{
	double pi, iteration;
	struct timespec period;
	int period_multiplier = *(int *)data;
	printf("New thread created...!?\n");
	//function

	clock_gettime(CLOCK_MONOTONIC, &period);
	printf("Thread number %ld, %d\n", syscall(SYS_gettid),period_multiplier);
	while (!END){

		sem_wait(&s_act);
		iteration = (double)(rand()%41)+10;//range: 10~50
		pi = pi_iteration(iteration);	
	
		if(period_multiplier == 12 || period_multiplier == 32){	
			sq_write(&items1[0],pi);
		}else if(period_multiplier == 18 || period_multiplier == 28){	
			sq_write(&items2[0],pi);
		}

  		period = time_add(period, period_multiplier);
		sem_post(&s_act);
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &period, 0);
	}
	pthread_exit(NULL);
}

void *thread_mouse_l(void *data)//aperiodic left-click mouse thread 
{
	double pi, iteration;
	//struct para_a para = (struct para_a *)data;
	//int fdm = *(int *)data;
	int fdm;
	if((fdm = open(MOUSEFILE, O_RDONLY)) == -1) {
		perror("opening device");
		exit(EXIT_FAILURE);		
	}
	struct input_event ie;
	printf("New async thread created !!! Type left\n");
	printf("AL-Thread number %ld\n", syscall(SYS_gettid));
	//function
	while (read(fdm, &ie, sizeof(struct input_event))){
		if(ie.value == 1 && ie.code == 272){//Left click
   			printf("LEFT CLICK\n");
			sem_wait(&s_act);
			iteration = (double)(rand()%41)+10;//range: 10~50
			pi = pi_iteration(iteration);	
		
			sq_write(&items1[0],pi);
   			//printf("PI = %1.16lf, iteration = %lf\n", pi, iteration);
			sem_post(&s_act);
		}
		if(END) break;
	}
	pthread_exit(NULL);
}

void *thread_mouse_r(void *data)//aperiodic right-click mouse thread 
{
	double pi, iteration;
	//struct para_a para = (struct para_a *)data;
	//int fdm = *(int *)data;
	int fdm;
	if((fdm = open(MOUSEFILE, O_RDONLY)) == -1) {
		perror("opening device");
		exit(EXIT_FAILURE);
	}	
	struct input_event ie;
	printf("New async thread created !!! Type right\n");
	printf("AR-Thread number %ld\n", syscall(SYS_gettid));
	//function
	while (read(fdm, &ie, sizeof(struct input_event))){
		if(ie.value == 1 && ie.code == 273){//Right click
   			printf("RIGHT CLICK\n");
			sem_wait(&s_act);
			iteration = (double)(rand()%41)+10;//range: 10~50
			pi = pi_iteration(iteration);	
		
			sq_write(&items2[0],pi);
   			//printf("PI = %1.16lf, iteration = %lf\n", pi, iteration);
			sem_post(&s_act);
		}
		if(END) break;
	}
	pthread_exit(NULL);
}


int main(){
	items1[10] = -1.0;
 	items2[10] = -1.0;
 	items1[11] = -1.0;
 	items2[11] = -1.0;
	items[0] = &items1[0];
	items[1] = &items2[0];
	num_i1 = 0.0; 
	num_i2 = 0.0;
	total_i1 = 0.0; 
	total_i2 = 0.0;
	e2_i1 = 0.0; 
	e2_i2 = 0.0;
	pthread_t pt_A1, pt_A2, pt_A3, pt_B1, pt_B2, pt_B3, pt_R1;
	pthread_attr_t tattr, tattr_a, tattr_r;
	//struct para_a para;
	void *thread_result;
	int error, fd, fdm;
	int p_period_multiplier[4] = {12,32,18,28};
	int r_period_multiplier[1] = {40};
	//int a_type[2] = {0,1};

	struct input_event ie;
	struct timeval last;
	int click_count = 0;
	double result;

	//initial define thread attribute
	ret = pthread_attr_init(&tattr);//periodic
	ret = pthread_attr_setschedpolicy(&tattr, SCHED_FIFO);//policy
	param.sched_priority = 50;
	ret = pthread_attr_setschedparam(&tattr, &param);//priority

	ret = pthread_attr_init(&tattr_a);//aperiodic
	ret = pthread_attr_setschedpolicy(&tattr_a, SCHED_FIFO);//policy
	param.sched_priority = 30;//higher than receiver thread
	ret = pthread_attr_setschedparam(&tattr_a, &param);//priority

	ret = pthread_attr_init(&tattr_r);//receiver
	param.sched_priority = 80;
	ret = pthread_attr_setschedparam(&tattr_r, &param);//priority

	if((fdm = open(MOUSEFILE, O_RDONLY)) == -1) {
		perror("opening device");
		exit(EXIT_FAILURE);		
	}

	//para.file = fdm;
	//fd = open("/dev/cqueue", O_RDWR);
	ret = sem_init(&s_act, 1, 1);//sharing option and initial value of semaphore
	//create 4 pthread, policy: RT-FIFO, Athread > Pthread in priority	
	error = pthread_create(&pt_A1, &tattr  , thread_period , &p_period_multiplier[0]);
	if(error != 0) printf("Thread A1 not create...!?\n");    
                                                                 
	error = pthread_create(&pt_A2, &tattr  , thread_period , &p_period_multiplier[1]);
	if(error != 0) printf("Thread A2 not create...!?\n");    
                                                               
        //para.type = 0;                                         
        error = pthread_create(&pt_A3, &tattr_a, thread_mouse_l, &fdm);
        if(error != 0) printf("Async Thread A3 not create...!?\n");

	error = pthread_create(&pt_B1, &tattr  , thread_period , &p_period_multiplier[2]);
	if(error != 0) printf("Thread B1 not create...!?\n");

	error = pthread_create(&pt_B2, &tattr  , thread_period , &p_period_multiplier[3]);
	if(error != 0) printf("Thread B2 not create...!?\n");

	//para.type = 1;
	error = pthread_create(&pt_B3, &tattr_a, thread_mouse_r, &fdm);
	if(error != 0) printf("Async Thread B3 not create...!?\n");

	error = pthread_create(&pt_R1, &tattr_r, thread_receiver, &items[0]);
	if(error != 0) printf("Async Thread R1 not create...!?\n");

	printf("Inside main thread \n");

	while(read(fdm, &ie, sizeof(struct input_event))){//terminating criteria detecting	
		printf("time %ld.%06ld\ttype %d\tcode %d\tvalue %d\n", ie.time.tv_sec, ie.time.tv_usec, ie.type, ie.code, ie.value);  	
		if(ie.code == 272 && ie.value == 1){	
			if(click_count != 0)
			{
				result = time_diff(last,ie.time);
				if(result <= 500000){
					printf("Double click of left-mouse is detected. Terminating...\n");
					goto END;
				}
			}else{
				click_count++;
			}	
			last = ie.time;
		}
	}
	END:
		printf("ENDING!!! \n");
		END = 1;
		pthread_join(pt_A1, NULL);//wait for the thread to end
		printf("A1 END\n");
		pthread_join(pt_A2, NULL);//wait for the thread to end
		printf("A2 END\n");
      		pthread_join(pt_A3, NULL);//wait for the thread to end
      		printf("A3 END\n");
		pthread_join(pt_B1, NULL);//wait for the thread to end
		printf("B1 END\n");
		pthread_join(pt_B2, NULL);//wait for the thread to end
		printf("B2 END\n");
		pthread_join(pt_B3, NULL);//wait for the thread to end
		printf("B3 END\n");
		pthread_join(pt_R1, NULL);//wait for the thread to end
		printf("R1 END\n");
		sem_destroy(&s_act);
		//close(fd); 
		printf("CQ1 Rear = %lf, front = %lf\n",items1[10], items1[11]);
		printf("CQ2 Rear = %lf, front = %lf\n",items2[10], items2[11]);//for safety monitoring purpose
		
		int64_t avg1 = total_i1/num_i1;
		int64_t avg2 = total_i2/num_i2;
		int64_t std1 = sqrt((e2_i1/num_i1) - pow(avg1,2));
		int64_t std2 = sqrt((e2_i2/num_i2) - pow(avg2,2));
		
		printf("Number of data queued in Q1 = %d\n",num_i1);
		printf("AVERAGE TIME of Q1 = %"PRIu64", STD = %"PRIu64"\n",avg1,std1);
		printf("Number of data queued in Q2 = %d\n",num_i2);
		printf("AVERAGE TIME of Q2 = %"PRIu64", STD = %"PRIu64"\n",avg2,std2);
		return 0;
}

//gcc ____.c -lpthread -o __(name)__
//gcc -wl
