#ifndef __IOAPIC_H__
#define __IOAPIC_H__
#include "lib.h"
#include "cpuinfo.h"

void ioapic_init(void);

void *ioapic_baseaddr=0;

#endif