#ifndef __PAPG_INIT_H__
#define __PAPG_INIT_H__

#include "printk.h"
#include "memory.h"
#include "cpuinfo.h"

void papg_init(unsigned char bsp_flags);

unsigned long* pml4t_vbase = (unsigned long*)0xFFFFFFFFFFFFF000;  //pml4虚拟地址基址
unsigned long* pdptt_vbase = (unsigned long*)0xFFFFFFFFFFE00000;  //pdpt虚拟地址基址
unsigned long* pdt_vbase = (unsigned long*)0xFFFFFFFFC0000000;    //pd虚拟地址基址
unsigned long* ptt_vbase = (unsigned long*)0xFFFFFF8000000000;    //pt虚拟地址基址

extern unsigned long __PML4T[512];

#endif