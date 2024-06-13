#ifndef __IOAPIC_H__
#define __IOAPIC_H__
#include "lib.h"
#include "cpuinfo.h"
#include "hpet.h"

void ioapic_init(void);

unsigned int *ioapic_baseaddr= 0;

#endif