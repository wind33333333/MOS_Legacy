#ifndef _HPET_H_
#define _HPET_H_

#include "cpuinfo.h"
#include "lib.h"
#include "printk.h"

unsigned long *hpet_baseaddr=0;

void hpet_init(void);

#endif