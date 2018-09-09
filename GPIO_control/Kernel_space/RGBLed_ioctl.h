#ifndef _RGBLED_H
#define _RGBLED_H

#include <linux/ioctl.h>

struct RGB_info{
	int duty_cycle;
	int R_pin;
	int G_pin;
	int B_pin;
};

/* Reference Documentation/ioctl/ioctl-number.txt for an unused number*/
#define RGBLED_NUM '\x66'

/* Behavior desired */
#define IOCTL_VALSET      _IOW(RGBLED_NUM, 0, struct RGB_info)

#define IOCTL_VAL_MAXNR 0

#define cycle_duration 20
#define period 500

#endif
