# KDIR:=/opt/clanton-full/1.4.2/sysroots/i586-poky-linux/usr/src/kernel
# PWD:= $(shell pwd)
# PATH=${PATH}:/home/Gen2/IoT_downloads/iotdk-ide-linus//devkit-x86/sysroots/x86_64-pokysdk-linux/usr/bin/i586-poky-linux/

#IOT_HOME = /home/yann-hang/Gen2/IoT_downloads/iotdk-ide-linux/devkit-x86/sysroots
IOT_HOME = /opt/iot-devkit/1.7.2/sysroots

PATH := $(PATH):$(IOT_HOME)/x86_64-pokysdk-linux/usr/bin/i586-poky-linux

CC = i586-poky-linux-gcc
ARCH = x86
CROSS_COMPILE = i586-poky-linux-
SROOT=$(IOT_HOME)/i586-poky-linux/
TEMP_PATH = /home/yuchun/Desktop/temp/

APP = WRM
obj-m += WRM_kernel.o HCSR04_Driver.o
#obj-m += WRM_kernel.o

all:
	make ARCH=x86 CROSS_COMPILE=i586-poky-linux- -C $(SROOT)/usr/src/kernel M=$(PWD) modules
	#i586-poky-linux-gcc -Wall RGBLed_usr_tst.c -pthread -lm -o test.o --sysroot=$(SROOT)
	$(CC) -o $(APP) -Wall -lm -lpthread --sysroot=$(SROOT) WalkingRedMan.c 
	#$(CC) -o $(APP) -Wall -pthread --sysroot=$(SROOT) WalkingRedMan.c 
clear:
	rm -f *.o
	rm -f *.ko
	rm -f *.mod.c
	rm -f Module.symvers
	rm -f modules.order
	rm -rf .tmp_versions
	rm -f Module.markers
	rm -f *.log
	rm -f $(APP) 
