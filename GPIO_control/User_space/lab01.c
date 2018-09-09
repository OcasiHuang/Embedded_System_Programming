#include <stdio.h>
#include <unistd.h>

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
// #include <pthread.h>
#include <poll.h>

/*
 * On board LED blink C example
 *
 * Demonstrate how to blink the on board LED, writing a digital value to an
 * output pin
 *
 * - digital out: a LED on shield pin 10
 *
 * Additional linker flags: none
 */

void IOSetup(void)
{
	int FdExport, Fd38, Fd26, Fd74, Fd10;

	FdExport = open("/sys/class/gpio/export", O_RDONLY | O_WRONLY| O_NONBLOCK);
		if (FdExport < 0)
		{
			printf("\n gpio export open failed");
		}

		if(0> write(FdExport,"10",2))
			printf("error FdExport 10");
		if(0> write(FdExport,"38",2))
			printf("error FdExport 38");
		if(0> write(FdExport,"26",2))
			printf("error FdExport 26");
		if(0> write(FdExport,"74",2))
			printf("error FdExport 74");

		close(FdExport);

	    /* Initialize all GPIOs */
		Fd10 = open("/sys/class/gpio/gpio10/direction", O_WRONLY);
		if (Fd10 < 0)
		{
			printf("\n gpio10 direction open failed");
		}
		Fd38 = open("/sys/class/gpio/gpio38/direction", O_WRONLY);
		if (Fd38 < 0)
		{
			printf("\n gpio38 direction open failed");
		}
		//IO00
		Fd26 = open("/sys/class/gpio/gpio26/direction", O_WRONLY);
		if (Fd26 < 0)
		{
			printf("\n gpio26 direction open failed");
		}

		Fd74 = open("/sys/class/gpio/gpio74/direction", O_WRONLY);
		if (Fd74 < 0)
		{
			printf("\n gpio74 direction open failed");
		}


		if(0> write(Fd10,"out",3))
			printf("error Fd10");

		if(0> write(Fd38,"out",3))
			printf("error Fd38");

		if(0> write(Fd26,"out",3))
			printf("error Fd26");

		if(0> write(Fd74,"out",3))
			printf("error Fd74");

		Fd26 = open("/sys/class/gpio/gpio26/value", O_WRONLY);
		if (Fd26 < 0)
				{
					printf("\n gpio26 value open failed");
				}
		Fd74 = open("/sys/class/gpio/gpio74/value", O_WRONLY);
		if (Fd74 < 0)
				{
					printf("\n gpio74 value open failed");
				}

		if(0> write(Fd26,"0",1))
					printf("error Fd26 value");

		if(0> write(Fd74,"0",1))
					printf("error Fd74 value");
}

int main()
{
	// select onboard LED pin based on the platform type
	// create a GPIO object from MRAA using it
	int Led;

	IOSetup();

	Led = open("/sys/class/gpio/gpio10/value", O_WRONLY);

	// loop forever toggling the LED every second
	for (;;) {
		write(Led,"1",1);
		sleep(1);
		write(Led, "0", 1);
		sleep(1);
	}

	return 0;
}
