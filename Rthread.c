#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <semaphore.h>
#include <time.h>  
int receive_t=0;



/*
struct Q_message
{
	float number;
	int message_id, source_id;
	int64_t TSC_t;
	int number=2; 
}*/


int clock_nanosleep(clockid_t clock_id, int flags,
                    const struct timespec *request,
                    struct timespec *remain);


struct timespec timespec_add(struct timespec a,struct timespec b) {
   a.tv_sec += b.tv_sec;
   a.tv_nsec += b.tv_nsec;
   return a;
 }


struct timespec timespec_set(struct timespec a)
{
   a.tv_sec=10;
   a.tv_nsec=0;
   return a;
}




void *receivertest(void *arg)
{
     double[2]= *(double*arg);
     
    
	 struct timespec next, period;
	 period=timespec_set(period);//set value
	 clock_gettime(CLOCK_MONOTONIC, &next);

		while(receive_t<10)
			{ //waits for 10 and increments receieved 
   			      next= timespec_add (next,period);
				 clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next,0);
				 printf("received\n");
				 receive_t++;					
			}

}



void main()
{
	int queue1=10;
	pthread_t receiver;
	

	pthread_create(&receiver,NULL,receivertest,NULL);
	pthread_join(receiver, NULL);

	printf("Receieved: %d", receive_t);
}


