#include <stdio.h>
#include <time.h>

#define DETECT_TIME 500//in ms

int main()
{
        struct timespec time;
        time.tv_sec = 0;
        time.tv_nsec = DETECT_TIME*1000000L;//500ms
        if(nanosleep(&time,NULL) < 0)
        {
                printf("Nano sleep failed\n");
                return -1;
        }
        //printf("Nano sleep ZzzZzz\n");
        return 0;
}

