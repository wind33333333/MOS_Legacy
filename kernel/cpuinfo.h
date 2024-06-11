#ifndef _GET_CPUINFO_H
#define _GET_CPUINFO_H

#include "printk.h"
#include "lib.h"

void get_cpuinfo(unsigned int *p);

struct {
    unsigned int num;
    char manufacturer_name[13];
    char model_name[49];
    unsigned long frequency;
}cpu_info;


extern unsigned int bsp_flags;

#endif