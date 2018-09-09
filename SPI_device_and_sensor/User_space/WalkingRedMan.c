
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>

#include <linux/input.h>
//#include <SPI.h>
#include <linux/spi/spidev.h>
#include "pattern.h"

#define SS "/sys/class/gpio/gpio15/value"
#define Time_Duration 200*1000 //200ms
#define FastTime_Duration 50*1000 //50ms


char decode_mode_setting[2]  = {0x09,0x00};
char intensity_setting[2]    = {0x0a,0x01};
char scan_limit_setting[2]   = {0x0b,0x07};
char shutdown_setting[2]     = {0x0c,0x01};
char non_display_test[2]     = {0x0f,0x00};
char display_test[2]         = {0x0f,0x01};
char x_pattern[2];

struct spi_ioc_transfer xfer[8] = {{0}};

void SPI_transmit(int fd, int Fd15, char * value)
{
	xfer[0].tx_buf = (unsigned long)value;
	xfer[0].len = 2;
	xfer[0].delay_usecs = 1;
	xfer[0].speed_hz = 10000000;
	xfer[0].cs_change = 1;
	xfer[0].bits_per_word = 8;
	xfer[0].pad = 0;

	//printf("Debug: xfer.tx_buf = %d\n", (int)(xfer[0].tx_buf));
	//printf("Debug: SPI config decode mode\n");
	write(Fd15,"0",1);
	ioctl(fd,SPI_IOC_MESSAGE(1),xfer);
	write(Fd15,"1",1);
}

void IOSetup(int fd)
{
	int FdExport; 
	//int Fd13, Fd34, Fd77;//pin2
	//int Fd14, Fd16, Fd76;//pin3

	//int Fd5 ,Fd24,Fd44,Fd72; //pin11: SPI_MOSI
	int Fd24,Fd44,Fd72; //pin11: SPI_MOSI
	int Fd15,Fd42;	         //pin12: GPIO for chip_select
	//int Fd7 ,Fd30,Fd46;      //pin13: SPI_SCK
	int Fd30,Fd46;      //pin13: SPI_SCK
	    

	FdExport = open("/sys/class/gpio/export", O_RDONLY | O_WRONLY| O_NONBLOCK);
	//modify to avoid undesired error msg while rerun
	write(FdExport,"13",2);
	write(FdExport,"34",2);
	write(FdExport,"77",2);

	write(FdExport,"14",2);
	write(FdExport,"16",2);
	write(FdExport,"76",2);

	//write(FdExport,"5",2);
	write(FdExport,"24",2);
	write(FdExport,"44",2);
	write(FdExport,"72",2);

	write(FdExport,"15",2);
	write(FdExport,"42",2);

	//write(FdExport,"7",2);
	write(FdExport,"30",2);
	write(FdExport,"46",2);
		
//*********************set I11
		 /* Initialize all GPIOs */
//		Fd5 = open("/sys/class/gpio/gpio5/direction", O_WRONLY);
//		if (Fd5 < 0)
//			printf("\n gpio5 direction open failed");
		
	Fd24 = open("/sys/class/gpio/gpio24/direction", O_WRONLY);
	if (Fd24 < 0)
		printf("\n gpio24 direction open failed");

	Fd44 = open("/sys/class/gpio/gpio44/direction", O_WRONLY);
	if (Fd44 < 0)
		printf("\n gpio44 direction open failed");

		/*enter direction type*/
//		if(0> write(Fd5,"out",3))
//			printf("error Fd5");

	if(0> write(Fd24,"out",3))
		printf("error Fd24");

	if(0> write(Fd44,"out",3))
		printf("error Fd44");

//************************value	
	Fd24 = open("/sys/class/gpio/gpio24/value", O_WRONLY);
	if (Fd24 < 0)
		printf("\n gpio24 value open failed");
	
	Fd44 = open("/sys/class/gpio/gpio44/value", O_WRONLY);
	if (Fd44 < 0)
		printf("\n gpio44 value open failed");
	
	Fd72 = open("/sys/class/gpio/gpio72/value", O_WRONLY);
	if (Fd72 < 0)
		printf("\n gpio44 value open failed");

	if(0> write(Fd24,"0",1))
		printf("error Fd24 value");

	if(0> write(Fd44,"1",1))
		printf("error Fd44 value");

	if(0> write(Fd72,"0",1))
		printf("error Fd72 value");


//*********************set I12

//	if(0> write(FdExport,"15",2))
//		printf("error FdExport 15");
//	if(0> write(FdExport,"42",2))
//		printf("error FdExport 42");	

	/* Initialize all GPIOs */
	Fd15 = open("/sys/class/gpio/gpio15/direction", O_WRONLY);
	if (Fd15 < 0)
		printf("\n gpio15 direction open failed");
	
	Fd42 = open("/sys/class/gpio/gpio42/direction", O_WRONLY);
	if (Fd42 < 0)
		printf("\n gpio42 direction open failed");
	
	/*enter direction type*/
	if(0> write(Fd15,"out",3))
		printf("error Fd15");

	if(0> write(Fd42,"out",3))
		printf("error Fd42");

//**************************set value
	Fd15 = open("/sys/class/gpio/gpio15/value",O_WRONLY);
	if (Fd15 < 0)
		printf("\n gpio15 value open failed");

	Fd42 = open("/sys/class/gpio/gpio42/value",O_WRONLY);
	if (Fd42 < 0)
		printf("\n gpio42 value open failed");
			
	if(0> write(Fd15,"0",1))
		printf("error Fd15 value");

	if(0> write(Fd42,"0",1))
		printf("error Fd42 value");


//*********************set I13
//	if(0> write(FdExport,"7",2))
//		printf("error FdExport 7");
//	if(0> write(FdExport,"30",2))
//		printf("error FdExport 30");
//	if(0> write(FdExport,"46",2))
//		printf("error FdExport 46");

		 /* Initialize all GPIOs */
//		Fd7 = open("/sys/class/gpio/gpio7/direction", O_WRONLY);
//		if (Fd7 < 0)
//			printf("\n gpio7 direction open failed");
		
	Fd30 = open("/sys/class/gpio/gpio30/direction", O_WRONLY);
	if (Fd30 < 0)
		printf("\n gpio30 direction open failed");

	Fd46 = open("/sys/class/gpio/gpio46/direction", O_WRONLY);
	if (Fd46 < 0)
			printf("\n gpio46 direction open failed");

		/*enter direction type*/
//		if(0> write(Fd7,"out",3))
//			printf("error Fd7");

	if(0> write(Fd30,"out",3))
		printf("error Fd30");

	if(0> write(Fd46,"out",3))
		printf("error Fd46");

//************************value	
	Fd30 = open("/sys/class/gpio/gpio30/value", O_WRONLY);
	if (Fd30 < 0)
		printf("\n gpio30 value open failed");
	
	Fd46 = open("/sys/class/gpio/gpio46/value", O_WRONLY);
	if (Fd46 < 0)
		printf("\n gpio46 value open failed");
	
	if(0> write(Fd30,"0",1))
		printf("error Fd30 value");

	if(0> write(Fd46,"1",1))
		printf("error Fd46 value");

	//ioctl(fd, SPI_IOC_WR_MODE, SPI_MODE0);	
	write(Fd15,"1",1);

	SPI_transmit(fd, Fd15,shutdown_setting);
	sleep(1);	
	SPI_transmit(fd, Fd15,scan_limit_setting);
	sleep(1);	
	printf("Debug: SPI write display-test mode\n");
	SPI_transmit(fd, Fd15,display_test);
	sleep(1);	
	SPI_transmit(fd, Fd15,decode_mode_setting);
	sleep(1);	
	SPI_transmit(fd, Fd15,intensity_setting);
	sleep(1);	

	printf("Debug: SPI write normal display mode\n");
	SPI_transmit(fd, Fd15,non_display_test);
	sleep(1);	
	printf("SPI initialization finished!!!\n");
	

	close(Fd15);
	//Entering normal operation
	//xfer[0].tx_buf = (unsigned long)(0x0f01);
	//xfer[0].len = 2;
	//xfer[0].cs_change = 0;
	//
	//printf("Debug: SPI write display-test mode\n");
	//write(Fd15,"0",1);
	//ioctl(fd,SPI_IOC_MESSAGE(1),xfer);
	//write(Fd15,"1",1);

	
	//xfer[0].tx_buf = (unsigned long)(0x0f00);
	//xfer[0].len = 2;
	//xfer[0].cs_change = 0;
	//	
	//printf("Debug: SPI write normal display mode\n");
	//write(Fd15,"0",1);
	//ioctl(fd,SPI_IOC_MESSAGE(1),xfer);
	//write(Fd15,"1",1);

	//return Fd15;
////*********************set I02
///*		if(0> write(FdExport,"13",2))
//			printf("error FdExport 13");
//		if(0> write(FdExport,"34",2))
//			printf("error FdExport 34");
//		if(0> write(FdExport,"77",2))
//			printf("error FdExport 77");	
//*/
//		/* Initialize all GPIOs */
//		Fd13 = open("/sys/class/gpio/gpio13/direction", O_WRONLY);
//		if (Fd13 < 0)
//			printf("\n gpio13 direction open failed");
//		
//		Fd34 = open("/sys/class/gpio/gpio34/direction", O_WRONLY);
//		if (Fd34 < 0)
//			printf("\n gpio34 direction open failed");
//		
//		
//		Fd77 = open("/sys/class/gpio/gpio77/direction", O_WRONLY);
////		if (Fd77 < 0)
////			printf("\n gpio77 direction open failed");
//		
//		
//		/*enter direction type*/
//		if(0> write(Fd13,"out",3))
//			printf("error Fd13");
//
//		if(0> write(Fd34,"out",3))
//			printf("error Fd34");
//		
//		if(0> write(Fd77,"out",3))
//			printf("error Fd77");
////********************set value
//		Fd34 = open("/sys/class/gpio/gpio34/value",O_WRONLY);
//		if (Fd34 < 0)	
//			printf("\n gpio34 value open failed");
//				
//		Fd77 = open("/sys/class/gpio/gpio77/value", O_WRONLY);
//		if (Fd77 < 0)
//			printf("\n gpio77 value open failed");
//				
//
//		if(0> write(Fd34,"0",1))
//			printf("error Fd34 value");
//
//		if(0> write(Fd77,"0",1))
//			printf("error Fd77 value");
//
//
////*********************set Green I03
//
///*		if(0> write(FdExport,"14",2))
//			printf("error FdExport 14");
//		if(0> write(FdExport,"16",2))
//			printf("error FdExport 16");
//		if(0> write(FdExport,"76",2))
//			printf("error FdExport 76");	
//*/
//		/* Initialize all GPIOs */
//		Fd14 = open("/sys/class/gpio/gpio14/direction", O_WRONLY);
//		if (Fd14 < 0)
//			printf("\n gpio14 direction open failed");
//		
//		Fd16 = open("/sys/class/gpio/gpio16/direction", O_WRONLY);
//		if (Fd16 < 0)
//			printf("\n gpio16 direction open failed");
//		
//		Fd76 = open("/sys/class/gpio/gpio76/direction", O_WRONLY);
////		if (Fd76 < 0)
////			printf("\n gpio76 direction open failed");
//		
//		
//		/*enter direction type*/
//		if(0> write(Fd14,"out",3))
//			printf("error Fd14");
//
//		if(0> write(Fd16,"out",3))
//			printf("error Fd16");
//		
//		if(0> write(Fd76,"out",3))
//			printf("error Fd76");
////**************************set value
//		Fd16 = open("/sys/class/gpio/gpio16/value",O_WRONLY);
//		if (Fd16 < 0)	
//			printf("\n gpio16 value open failed");
//				
//		Fd76 = open("/sys/class/gpio/gpio76/value", O_WRONLY);
//		if (Fd76 < 0)	
//			printf("\n gpio76 value open failed");
//				
//		if(0> write(Fd16,"0",1))
//			printf("error Fd16 value");
//
//		if(0> write(Fd76,"0",1))
//			printf("error Fd76 value");

}

//void Pattern_CTRL(int num, int fd, int SS_fd)
void Pattern_CTRL(int num, int fd)
{
//	PATTERN pattern_write;
	int i;
	int ret = 0;
	int SS_fd = open(SS, O_WRONLY);
	
	if (SS_fd < 0)
		printf("gpio15 value open failed\n");

	//printf("Debug: %c,%c,%c,%c,%c,%c,%c,%c\n",walk_left_A.led[0],walk_left_A.led[1],walk_left_A.led[2],walk_left_A.led[3],walk_left_A.led[4],walk_left_A.led[5],walk_left_A.led[6],walk_left_A.led[7]);
//	switch(num)
//	{
//		case  0: 
//			pattern_write = walk_left_0;		
//			break;
//		case  1: 
//			pattern_write = walk_left_1;		
//			break;
//		case  2: 
//			pattern_write = walk_left_2;		
//			break;
//		case  3: 
//			pattern_write = walk_left_3;		
//			break;
//		case  4: 
//			pattern_write = walk_left_4;		
//			break;
//		case  5: 
//			pattern_write = walk_left_5;		
//			break;
//		case  6: 
//			pattern_write = walk_left_6;		
//			break;
//		case  7: 
//			pattern_write = walk_left_7;		
//			break;
//		case  8: 
//			pattern_write = walk_left_8;		
//			break;
//		case  9: 
//			pattern_write = walk_left_9;		
//			break;
//		case 10: 
//			pattern_write = walk_left_A;		
//			break;
//		case 11: 
//			pattern_write = walk_left_B;		
//			break;
//		default:
//			pattern_write = walk_left_B;		
//	}
	//printf("Debug: 3rd 16 bit: %d\n", pattern_write.led[2]);
	for(i=0;i<8;i++){
		x_pattern[0] = 0xaa;
		x_pattern[1] = i + '1';
		//x_pattern[1] = pattern_write.led[i];
		
		SPI_transmit(fd, SS_fd,x_pattern);
		//sleep(1);	
		//xfer[i].tx_buf = (unsigned long)x_pattern[i];
		//xfer[i].len = 2;
		//xfer[i].delay_usecs = 1;
		//xfer[i].speed_hz = 10000000;
		//xfer[i].cs_change = 1;
		//xfer[i].bits_per_word = 8;
		//xfer[i].pad = 0;
		//
		//write(SS_fd,"0",1);
		////printf("Debug: 3rd 16 bit: %ld\n", (unsigned long)(xfer[2].tx_buf));
		////ret = ioctl(fd,SPI_IOC_MESSAGE(8),xfer);
		//ret = ioctl(fd,SPI_IOC_MESSAGE(1),xfer);
		//write(SS_fd,"1",1);
	}
	if(ret<0){
		printf("SPI write ERR!!!\n");
	}

}

//int Dist_mearsure()

int main()
{
	
	//int function;
	int i;
	int fd=0;
	//int fd15;
        
        printf("Select function:\n");
	printf("0): <3\n");
	printf("1): Walking RED man\n");
	//scanf("%d",&function);

//	if(duty_cycle_percent >= 100 || duty_cycle_percent <= 0){
//		printf("pin_red error\n"); 
//		return 0;
//	}
//	printf("Choose the R pins\n");
//	scanf("%d",&pin_a);
	fd = open("/dev/spidev1.0",O_RDWR);
	if(fd<0){
		printf("Can't access SPI!!!\n");
		return -1;
	}
	//fd15 = IOSetup(fd);
	IOSetup(fd);
	
//	int error;
//	pthread_t pt_mouse;
//	
//	
//	error = pthread_create(&pt_mouse, NULL, mouse_detect, NULL);
//	if(error != 0) printf("Thread mouse detect not create...!?\n");
	//mouse_detect();

		for (i=0;i<10;i++) 
		{
			Pattern_CTRL(10,fd);
			usleep(Time_Duration);	
			Pattern_CTRL(11,fd);
			usleep(Time_Duration);	
		}
        printf("SPI function finished. ^____________^\n");
	//pthread_join(pt_mouse,NULL);
	close(fd);
	//close(fd15);

	return 0;

}










