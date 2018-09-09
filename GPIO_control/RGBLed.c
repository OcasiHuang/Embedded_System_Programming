#include <stdio.h>
#include <unistd.h>

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <poll.h>
#define cycle_duration 20

#include <linux/input.h>
#define MOUSEFILE "/dev/input/event3"

int END;

void IOSetup(void)
{
	int FdExport, Fd11, Fd32; 
	int Fd12, Fd28,Fd45;
	int Fd13, Fd34, Fd77;
	int Fd14, Fd16, Fd76;
	int Fd6, Fd36;		  
       	int Fd0,Fd18,Fd66;
        int Fd1,Fd20,Fd68;
	int Fd38, Fd40;//7,8
	int Fd4, Fd22, Fd70;
	int Fd10, Fd26, Fd74;
	int Fd5, Fd24,Fd44;
	int Fd15, Fd42;
	int Fd7, Fd30, Fd46;
	    

	FdExport = open("/sys/class/gpio/export", O_RDONLY | O_WRONLY| O_NONBLOCK);
	//open("/sys/class/gpio/export", O_RDONLY | O_WRONLY| O_NONBLOCK);

	//modify to avoid undesired error msg while rerun
	write(FdExport,"11",2);
	write(FdExport,"32",2);
	write(FdExport,"12",2);
	write(FdExport,"28",2);
	write(FdExport,"45",2);
	write(FdExport,"13",2);
	write(FdExport,"34",2);
	write(FdExport,"77",2);
	write(FdExport,"14",2);
	write(FdExport,"16",2);
	write(FdExport,"76",2);
	write(FdExport,"6",2);
	write(FdExport,"36",2);
	write(FdExport,"0",2);
	write(FdExport,"18",2);
	write(FdExport,"66",2);
	write(FdExport,"1",2);
	write(FdExport,"20",2);
	write(FdExport,"68",2);
	write(FdExport,"38",2);
	write(FdExport,"40",2);
	write(FdExport,"4",2);
	write(FdExport,"22",2);
	write(FdExport,"70",2);
	write(FdExport,"10",2);
	write(FdExport,"26",2);
	write(FdExport,"74",2);
	write(FdExport,"5",2);
	write(FdExport,"24",2);
	write(FdExport,"44",2);
	write(FdExport,"15",2);
	write(FdExport,"42",2);
	write(FdExport,"7",2);
	write(FdExport,"30",2);
	write(FdExport,"46",2);
/*		if (FdExport < 0)
		{
			printf("\n gpio export open failed");
		}
*/		
		
//*********************set I00

//		if(0> write(FdExport,"11",2))
//			printf("error FdExport 11");
//		if(0> write(FdExport,"32",2))
//			printf("error FdExport 32");
		
		/* Initialize all GPIOs */
		Fd11 = open("/sys/class/gpio/gpio11/direction", O_WRONLY);
		if (Fd11 < 0)
		{
			printf("\n gpio11 direction open failed");
		}
		
		Fd32 = open("/sys/class/gpio/gpio32/direction", O_WRONLY);
		if (Fd32 < 0)
		{
			printf("\n gpio32 direction open failed");
		}
		
		/*enter direction type*/
		if(0> write(Fd11,"out",3))
			printf("error Fd11");

		if(0> write(Fd32,"out",3))
			printf("error Fd32");

//*********************set value
		Fd32 = open("/sys/class/gpio/gpio32/value",O_WRONLY);
		if (Fd32 < 0)
			printf("\n gpio32 value open failed");
				
		if(0> write(Fd32,"0",1))
			printf("error Fd32 value");


//*********************set I01
/*		if(0> write(FdExport,"12",2))
			printf("error FdExport 12");
		if(0> write(FdExport,"28",2))
			printf("error FdExport 28");
		if(0> write(FdExport,"45",2))
			printf("error FdExport 45");	
*/
		/* Initialize all GPIOs */
		Fd12 = open("/sys/class/gpio/gpio12/direction", O_WRONLY);
		if (Fd12 < 0)
			printf("\n gpio12 direction open failed");

		Fd28 = open("/sys/class/gpio/gpio28/direction", O_WRONLY);
		if (Fd28 < 0)
			printf("\n gpio28 direction open failed");
		
		Fd45 = open("/sys/class/gpio/gpio45/direction", O_WRONLY);
		if (Fd45 < 0)
			printf("\n gpio45 direction open failed");

		
		/*enter direction type*/
		if(0> write(Fd12,"out",3))
			printf("error Fd12");

		if(0> write(Fd28,"out",3))
			printf("error Fd28");
		
		if(0> write(Fd45,"out",3))
			printf("error Fd45");
//**************************************************
		Fd28 = open("/sys/class/gpio/gpio28/value",O_WRONLY);
		if (Fd28 < 0)	
			printf("\n gpio28 value open failed");
				
		Fd45 = open("/sys/class/gpio/gpio45/value", O_WRONLY);
		if (Fd45 < 0)
			printf("\n gpio45 value open failed");
				

		if(0> write(Fd28,"0",1))
			printf("error Fd28 value");

		if(0> write(Fd45,"0",1))
			printf("error Fd45 value");

//*********************set I02
/*		if(0> write(FdExport,"13",2))
			printf("error FdExport 13");
		if(0> write(FdExport,"34",2))
			printf("error FdExport 34");
		if(0> write(FdExport,"77",2))
			printf("error FdExport 77");	
*/
		/* Initialize all GPIOs */
		Fd13 = open("/sys/class/gpio/gpio13/direction", O_WRONLY);
		if (Fd13 < 0)
			printf("\n gpio13 direction open failed");
		
		Fd34 = open("/sys/class/gpio/gpio34/direction", O_WRONLY);
		if (Fd34 < 0)
			printf("\n gpio34 direction open failed");
		
		
		Fd77 = open("/sys/class/gpio/gpio77/direction", O_WRONLY);
//		if (Fd77 < 0)
//			printf("\n gpio77 direction open failed");
		
		
		/*enter direction type*/
		if(0> write(Fd13,"out",3))
			printf("error Fd13");

		if(0> write(Fd34,"out",3))
			printf("error Fd34");
		
		if(0> write(Fd77,"out",3))
			printf("error Fd77");
//********************set value
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


//*********************set Green I03

/*		if(0> write(FdExport,"14",2))
			printf("error FdExport 14");
		if(0> write(FdExport,"16",2))
			printf("error FdExport 16");
		if(0> write(FdExport,"76",2))
			printf("error FdExport 76");	
*/
		/* Initialize all GPIOs */
		Fd14 = open("/sys/class/gpio/gpio14/direction", O_WRONLY);
		if (Fd14 < 0)
			printf("\n gpio14 direction open failed");
		
		Fd16 = open("/sys/class/gpio/gpio16/direction", O_WRONLY);
		if (Fd16 < 0)
			printf("\n gpio16 direction open failed");
		
		Fd76 = open("/sys/class/gpio/gpio76/direction", O_WRONLY);
//		if (Fd76 < 0)
//			printf("\n gpio76 direction open failed");
		
		
		/*enter direction type*/
		if(0> write(Fd14,"out",3))
			printf("error Fd14");

		if(0> write(Fd16,"out",3))
			printf("error Fd16");
		
		if(0> write(Fd76,"out",3))
			printf("error Fd76");
//**************************set value
		Fd16 = open("/sys/class/gpio/gpio16/value",O_WRONLY);
		if (Fd16 < 0)	
			printf("\n gpio16 value open failed");
				
		Fd76 = open("/sys/class/gpio/gpio76/value", O_WRONLY);
		if (Fd76 < 0)	
			printf("\n gpio76 value open failed");
				
		if(0> write(Fd16,"0",1))
			printf("error Fd16 value");

		if(0> write(Fd76,"0",1))
			printf("error Fd76 value");

//*********************set I04

/*		if(0> write(FdExport,"6",2))
			printf("error FdExport 6");
		if(0> write(FdExport,"36",2))
			printf("error FdExport 36");	
*/
		/* Initialize all GPIOs */
		Fd6 = open("/sys/class/gpio/gpio6/direction", O_WRONLY);
		if (Fd6 < 0)
			printf("\n gpio6 direction open failed");
		
		Fd36 = open("/sys/class/gpio/gpio36/direction", O_WRONLY);
		if (Fd36 < 0)
			printf("\n gpio36 direction open failed");
		
		/*enter direction type*/
		if(0> write(Fd6,"out",3))
			printf("error Fd6");

		if(0> write(Fd36,"out",3))
			printf("error Fd36");

//**************************set value
		Fd36 = open("/sys/class/gpio/gpio36/value",O_WRONLY);
		if (Fd36 < 0)
			printf("\n gpio36 value open failed");
				
		if(0> write(Fd36,"0",1))
			printf("error Fd36 value");

//*********************set I05
/*		if(0> write(FdExport,"0",2))
			printf("error FdExport 0");
		if(0> write(FdExport,"18",2))
			printf("error FdExport 18");
		if(0> write(FdExport,"66",2))
			printf("error FdExport 66");	
*/
		/* Initialize all GPIOs */
		Fd0 = open("/sys/class/gpio/gpio0/direction", O_WRONLY);
		if (Fd0 < 0)
			printf("\n gpio0 direction open failed");

		Fd18 = open("/sys/class/gpio/gpio18/direction", O_WRONLY);
		if (Fd18 < 0)
			printf("\n gpio18 direction open failed");
		
		Fd66 = open("/sys/class/gpio/gpio66/direction", O_WRONLY);
		if (Fd66 < 0)
			printf("\n gpio66 direction open failed");
		
		/*enter direction type*/
		if(0> write(Fd0,"out",3))
			printf("error Fd0");

		if(0> write(Fd18,"out",3))
			printf("error Fd18");
		
		if(0> write(Fd66,"out",3))
			printf("error Fd66");
//**************************set value
		Fd18 = open("/sys/class/gpio/gpio18/value",O_WRONLY);
		if (Fd18 < 0)	
			printf("\n gpio18 value open failed");
				
		Fd66 = open("/sys/class/gpio/gpio66/value", O_WRONLY);
		if (Fd66 < 0)	
			printf("\n gpio66 value open failed");
				
		if(0> write(Fd18,"0",1))
			printf("error Fd18 value");

		if(0> write(Fd66,"0",1))
			printf("error Fd66 value");

//**********************set Blue IO6
/*		if(0> write(FdExport,"1",2))
			printf("error FdExport 1");
		if(0> write(FdExport,"20",2))
			printf("error FdExport 20");
		if(0> write(FdExport,"68",2))
			printf("error FdExport 68");	
*/
		/* Initialize all GPIOs */
		Fd1 = open("/sys/class/gpio/gpio1/direction", O_WRONLY);
		if (Fd1 < 0)		
			printf("\n gpio1 direction open failed");

		Fd20 = open("/sys/class/gpio/gpio20/direction", O_WRONLY);
		if (Fd20 < 0)
			printf("\n gpio20 direction open failed");
		
		Fd68 = open("/sys/class/gpio/gpio68/direction", O_WRONLY);
//		if (Fd68 < 0)
//			printf("\n gpio68 direction open failed");
		
		/*enter direction type*/
		if(0> write(Fd1,"out",3))
			printf("error Fd1");

		if(0> write(Fd20,"out",3))
			printf("error Fd20");
		
		if(0> write(Fd68,"out",3))
			printf("error Fd68");
//**************************set value
		Fd20 = open("/sys/class/gpio/gpio20/value",O_WRONLY);
		if (Fd20 < 0)
			printf("\n gpio20 value open failed");
		
		Fd68 = open("/sys/class/gpio/gpio68/value", O_WRONLY);
		if (Fd68 < 0)
			printf("\n gpio68 value open failed");

		if(0> write(Fd20,"0",1))
			printf("error Fd20 value");

		if(0> write(Fd68,"0",1))
			printf("error Fd68 value");

//*********************set I07
//		if(0> write(FdExport,"38",2))
//			printf("error FdExport 38");
		
		/* Initialize all GPIOs */
		Fd38 = open("/sys/class/gpio/gpio38/direction", O_WRONLY);
		if (Fd38 < 0)
			printf("\n gpio38 direction open failed");
			
		/*enter direction type*/
		if(0> write(Fd38,"out",3))
			printf("error Fd38");

//*********************set value
		


//*********************set I08
//		if(0> write(FdExport,"40",2))
//			printf("error FdExport 40");
		
		
		/* Initialize all GPIOs */
		Fd40 = open("/sys/class/gpio/gpio40/direction", O_WRONLY);
		if (Fd40 < 0)
			printf("\n gpio40 direction open failed");
			
		/*enter direction type*/
		if(0> write(Fd40,"out",3))
			printf("error Fd40");

//**********************set IO9
/*		if(0> write(FdExport,"4",2))
			printf("error FdExport 4");
		if(0> write(FdExport,"22",2))
			printf("error FdExport 22");
		if(0> write(FdExport,"70",2))
			printf("error FdExport 70");
*/
		 /* Initialize all GPIOs */

		Fd4 = open("/sys/class/gpio/gpio4/direction", O_WRONLY);
		if (Fd4 < 0)
			printf("\n gpio4 direction open failed");
		
		Fd22 = open("/sys/class/gpio/gpio22/direction", O_WRONLY);
		if (Fd22 < 0)
			printf("\n gpio22 direction open failed");
		
		Fd70 = open("/sys/class/gpio/gpio70/direction", O_WRONLY);
		if (Fd70 < 0)
			printf("\n gpio70 direction open failed");

		/*enter direction type*/
		if(0> write(Fd4,"out",3))
			printf("error Fd4");

		if(0> write(Fd22,"out",3))
			printf("error Fd22");

		if(0> write(Fd70,"out",3))
			printf("error Fd70");
//*************************value
		Fd22 = open("/sys/class/gpio/gpio22/value",O_WRONLY);
		if (Fd22 < 0)
			printf("\n gpio22 value open failed");
		
		Fd70 = open("/sys/class/gpio/gpio70/value", O_WRONLY);
		if (Fd70 < 0)
			printf("\n gpio70 value open failed");
				
		if(0> write(Fd22,"0",1))
			printf("error Fd22 value");

		if(0> write(Fd70,"0",1))
			printf("error Fd70 value");


//*********************set I10

/*		if(0> write(FdExport,"10",2))
			printf("error FdExport 10");
		if(0> write(FdExport,"26",2))
			printf("error FdExport 26");
		if(0> write(FdExport,"74",2))
			printf("error FdExport 74");
*/
		 /* Initialize all GPIOs */
		Fd10 = open("/sys/class/gpio/gpio10/direction", O_WRONLY);
		if (Fd10 < 0)
			printf("\n gpio10 direction open failed");
		
		Fd26 = open("/sys/class/gpio/gpio26/direction", O_WRONLY);
		if (Fd26 < 0)
			printf("\n gpio26 direction open failed");

		Fd74 = open("/sys/class/gpio/gpio74/direction", O_WRONLY);
		if (Fd74 < 0)
     			printf("\n gpio74 direction open failed");

		/*enter direction type*/
		if(0> write(Fd10,"out",3))
			printf("error Fd10");

		if(0> write(Fd26,"out",3))
			printf("error Fd26");

		if(0> write(Fd74,"out",3))
			printf("error Fd74");

//**************************************************		
		Fd26 = open("/sys/class/gpio/gpio26/value", O_WRONLY);
		if (Fd26 < 0)
			printf("\n gpio26 value open failed");
		
		Fd74 = open("/sys/class/gpio/gpio74/value", O_WRONLY);
		if (Fd74 < 0)
			printf("\n gpio74 value open failed");
		
		if(0> write(Fd26,"0",1))
			printf("error Fd26 value");

		if(0> write(Fd74,"0",1))
			printf("error Fd74 value");


//*********************set I11

/*		if(0> write(FdExport,"5",2))
			printf("error FdExport 5");
		if(0> write(FdExport,"24",2))
			printf("error FdExport 24");
		if(0> write(FdExport,"44",2))
			printf("error FdExport 44");
*/
		 /* Initialize all GPIOs */
		Fd5 = open("/sys/class/gpio/gpio5/direction", O_WRONLY);
		if (Fd5 < 0)
			printf("\n gpio5 direction open failed");
		
		Fd24 = open("/sys/class/gpio/gpio24/direction", O_WRONLY);
		if (Fd24 < 0)
			printf("\n gpio24 direction open failed");

		Fd44 = open("/sys/class/gpio/gpio44/direction", O_WRONLY);
		if (Fd44 < 0)
			printf("\n gpio44 direction open failed");

		/*enter direction type*/
		if(0> write(Fd5,"out",3))
			printf("error Fd5");

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
		
		if(0> write(Fd24,"0",1))
			printf("error Fd24 value");

		if(0> write(Fd44,"0",1))
			printf("error Fd44 value");



//*********************set I12

/*		if(0> write(FdExport,"15",2))
			printf("error FdExport 15");
		if(0> write(FdExport,"42",2))
			printf("error FdExport 42");	
*/
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
		Fd42 = open("/sys/class/gpio/gpio42/value",O_WRONLY);
		if (Fd42 < 0)
			printf("\n gpio42 value open failed");
				
		if(0> write(Fd42,"0",1))
			printf("error Fd42 value");


//*********************set I13
/*		if(0> write(FdExport,"7",2))
			printf("error FdExport 7");
		if(0> write(FdExport,"30",2))
			printf("error FdExport 30");
		if(0> write(FdExport,"46",2))
			printf("error FdExport 46");
*/
		 /* Initialize all GPIOs */
		Fd7 = open("/sys/class/gpio/gpio7/direction", O_WRONLY);
		if (Fd7 < 0)
			printf("\n gpio7 direction open failed");
		
		Fd30 = open("/sys/class/gpio/gpio30/direction", O_WRONLY);
		if (Fd30 < 0)
			printf("\n gpio30 direction open failed");

		Fd46 = open("/sys/class/gpio/gpio46/direction", O_WRONLY);
		if (Fd46 < 0)
			printf("\n gpio46 direction open failed");

		/*enter direction type*/
		if(0> write(Fd7,"out",3))
			printf("error Fd7");

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

		if(0> write(Fd46,"0",1))
			printf("error Fd46 value");
}



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
		//printf("time %ld.%06ld\ttype %d\tcode %d\tvalue %d\n", ie.time.tv_sec, ie.time.tv_usec, ie.type, ie.code, ie.value);  	
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
	int Led_Red,Led_Green,Led_Blue;
	int duty_cycle_percent=0;
	int pin_a=0;
	int pin_b=0;
	int pin_c=0;
        
        printf("Choose the duty cycle\n");
	scanf("%d",&duty_cycle_percent);
	printf("Choose the R pins\n");
	scanf("%d",&pin_a);
	IOSetup();

switch(pin_a)
{
case 0: Led_Red = open("/sys/class/gpio/gpio11/value", O_WRONLY);break;
case 1: Led_Red = open("/sys/class/gpio/gpio12/value", O_WRONLY);break;
case 2: Led_Red = open("/sys/class/gpio/gpio13/value", O_WRONLY);break;
case 3: Led_Red = open("/sys/class/gpio/gpio14/value", O_WRONLY);break;
case 4: Led_Red = open("/sys/class/gpio/gpio6/value", O_WRONLY);break;
case 5: Led_Red = open("/sys/class/gpio/gpio0/value", O_WRONLY);break;
case 6: Led_Red = open("/sys/class/gpio/gpio1/value", O_WRONLY);break;
case 7: Led_Red = open("/sys/class/gpio/gpio38/value", O_WRONLY);break;
case 8: Led_Red = open("/sys/class/gpio/gpio40/value", O_WRONLY);break;
case 9: Led_Red = open("/sys/class/gpio/gpio4/value", O_WRONLY);break;
case 10: Led_Red = open("/sys/class/gpio/gpio10/value", O_WRONLY);break;
case 11: Led_Red = open("/sys/class/gpio/gpio5/value", O_WRONLY);break;
case 12: Led_Red = open("/sys/class/gpio/gpio15/value", O_WRONLY);break;
case 13: Led_Red = open("/sys/class/gpio/gpio7/value", O_WRONLY);break;

default: printf("pin_red error\n"); return 0;
}

printf("Choose the G pins\n");
	scanf("%d",&pin_b);

//ocasi
if(pin_b == pin_a){
	printf("pin_green error\n");
	return 0;
}

switch(pin_b)
{
case 0: Led_Green = open("/sys/class/gpio/gpio11/value", O_WRONLY);break;
case 1: Led_Green = open("/sys/class/gpio/gpio12/value", O_WRONLY);break;
case 2: Led_Green = open("/sys/class/gpio/gpio13/value", O_WRONLY);break;
case 3: Led_Green = open("/sys/class/gpio/gpio14/value", O_WRONLY);break;
case 4: Led_Green = open("/sys/class/gpio/gpio6/value", O_WRONLY);break;
case 5: Led_Green = open("/sys/class/gpio/gpio0/value", O_WRONLY);break;
case 6: Led_Green = open("/sys/class/gpio/gpio1/value", O_WRONLY);break;
case 7: Led_Green = open("/sys/class/gpio/gpio38/value", O_WRONLY);break;
case 8: Led_Green = open("/sys/class/gpio/gpio40/value", O_WRONLY);break;
case 9: Led_Green = open("/sys/class/gpio/gpio4/value", O_WRONLY);break;
case 10: Led_Green = open("/sys/class/gpio/gpio10/value", O_WRONLY);break;
case 11: Led_Green = open("/sys/class/gpio/gpio5/value", O_WRONLY);break;
case 12: Led_Green = open("/sys/class/gpio/gpio15/value", O_WRONLY);break;
case 13: Led_Green = open("/sys/class/gpio/gpio7/value", O_WRONLY);break;

default: printf("pin_green error\n");return 0;

}


printf("Choose the B pins\n");
scanf("%d",&pin_c);
//ocasi
if(pin_c == pin_b || pin_c == pin_a){
	printf("pin_blue error\n");
	return 0;
}
switch(pin_c)
{
case 0: Led_Blue = open("/sys/class/gpio/gpio11/value", O_WRONLY);break;
case 1: Led_Blue = open("/sys/class/gpio/gpio12/value", O_WRONLY);break;
case 2: Led_Blue = open("/sys/class/gpio/gpio13/value", O_WRONLY);break;
case 3: Led_Blue = open("/sys/class/gpio/gpio14/value", O_WRONLY);break;
case 4: Led_Blue = open("/sys/class/gpio/gpio6/value", O_WRONLY);break;
case 5: Led_Blue = open("/sys/class/gpio/gpio0/value", O_WRONLY);break;
case 6: Led_Blue = open("/sys/class/gpio/gpio1/value", O_WRONLY);break;
case 7: Led_Blue = open("/sys/class/gpio/gpio38/value", O_WRONLY);break;
case 8: Led_Blue = open("/sys/class/gpio/gpio40/value", O_WRONLY);break;
case 9: Led_Blue = open("/sys/class/gpio/gpio4/value", O_WRONLY);break;
case 10: Led_Blue = open("/sys/class/gpio/gpio10/value", O_WRONLY);break;
case 11: Led_Blue = open("/sys/class/gpio/gpio5/value", O_WRONLY);break;
case 12: Led_Blue = open("/sys/class/gpio/gpio15/value", O_WRONLY);break;
case 13: Led_Blue = open("/sys/class/gpio/gpio7/value", O_WRONLY);break;

default: printf("pin_blue error\n");return 0;
}

	
	int error;
	pthread_t pt_mouse;
	
	
	error = pthread_create(&pt_mouse, NULL, mouse_detect, NULL);
	if(error != 0) printf("Thread mouse detect not create...!?\n");
	//mouse_detect();


	for (;;) 
	{
		int i=0;
		while(i<500)
		{
		write(Led_Red,"1",1);
		usleep(cycle_duration*1000*duty_cycle_percent/100);	
		write(Led_Red, "0", 1);
		usleep(cycle_duration*1000*(1-(duty_cycle_percent/100)));	
                i+=cycle_duration;		

		}
		if(END) break;//ocasi

		while(i<1000)
		{		
		write(Led_Green,"1",1);
		usleep(cycle_duration*1000*duty_cycle_percent/100);
		write(Led_Green, "0", 1);
		usleep(cycle_duration*1000*(1-(duty_cycle_percent/100)));	
                i+=cycle_duration;		
		}
		if(END) break;//ocasi

		while(i<1500)
		{
		write(Led_Blue,"1",1);
		usleep(cycle_duration*1000*duty_cycle_percent/100);
		write(Led_Blue, "0", 1);
		usleep(cycle_duration*1000*(1-(duty_cycle_percent/100)));	
                i+=cycle_duration;
		}
		if(END) break;//ocasi

		while(i<2000)
		{
 		write(Led_Green,"1",1);
		write(Led_Red,"1",1);
		usleep(cycle_duration*1000*duty_cycle_percent/100);
		write(Led_Green,"0",1);
		write(Led_Red,"0",1);
		usleep(cycle_duration*1000*(1-(duty_cycle_percent/100)));	
                i+=cycle_duration;
		}
		if(END) break;//ocasi

		while(i<2500)
		{
		write(Led_Blue,"1",1);
		write(Led_Red,"1",1);
		usleep(cycle_duration*1000*duty_cycle_percent/100);
		write(Led_Blue,"0",1);
		write(Led_Red,"0",1);
		usleep(cycle_duration*1000*(1-(duty_cycle_percent/100)));	
                i+=cycle_duration;
		}
		if(END) break;

		while(i<3000)
		{
		write(Led_Blue,"1",1);
		write(Led_Green,"1",1);
		usleep(cycle_duration*1000*duty_cycle_percent/100);
		write(Led_Blue,"0",1);
		write(Led_Green,"0",1);
		usleep(cycle_duration*1000*(1-(duty_cycle_percent/100)));	
                i+=cycle_duration;
		}
		if(END) break; 

		while(i<3500)
		{
		write(Led_Blue,"1",1);
		write(Led_Green,"1",1);
		write(Led_Red,"1",1);
		usleep(cycle_duration*1000*duty_cycle_percent/100);
		write(Led_Blue,"0",1);
		write(Led_Green,"0",1);
		write(Led_Red,"0",1);
		usleep(cycle_duration*1000*(1-(duty_cycle_percent/100)));	
                i+=cycle_duration;
		}
		if(END) break; 
	}
	pthread_join(pt_mouse,NULL);

	return 0;

}











