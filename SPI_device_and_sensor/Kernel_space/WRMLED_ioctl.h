#ifndef _WRMLED_H
#define _WRMLED_H

#include <linux/ioctl.h>
//#include "pattern.h"

struct PATTERN_list{
	PATTERN pattern_num[10];
};

/* Reference Documentation/ioctl/ioctl-number.txt for an unused number*/
#define WRMLED_NUM '\x66'

/* Behavior desired */
#define IOCTL_VALSET      _IOW(WRMLED_NUM, 0, struct PATTERN_list)

#define IOCTL_VAL_MAXNR 0

#endif
