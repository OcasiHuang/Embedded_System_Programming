#include <stdio.h>
#include <inttypes.h>
#pragma intrinsic(__rdtsc)  
#include <math.h>
#include <stdint.h>

//  Linux/GCC
uint64_t rdtsc(){
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}


	   int j=0;
        int sum=0;
        int mean=0;
        int standardDeviation=0;
        int64_t b;
        int64_t ti[200];
		



int main()
{
    for (int i=1; i<=5;i++)
	 { 
 	     b=rdtsc();
	//	b=b+1;
		sleep(1);//deal with the process
	   	ti[i]=b;
	  }
          


		for(int k=1; k<=5; k++)
                    {
                         sum += ti[k];
                         printf("%"PRIu64" \n",ti[k]);
                    }

    		mean = sum/5;

   		 for(int l = 1; l <= 5; l++)
       		{
				 standardDeviation += (ti[l] - mean)*(ti[l] - mean);
			}
		 standardDeviation=standardDeviation/5;

       printf("%"PRIu64" \n",ti[1]);
       printf("%"PRIu64" \n",ti[2]);
       printf("%"PRIu64" \n",standardDeviation);
       printf("%"PRIu64" \n",mean);


}

