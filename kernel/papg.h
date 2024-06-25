#ifndef __PAPG_INIT_H__
#define __PAPG_INIT_H__

#include "printk.h"
#include "memory.h"
#include "cpuinfo.h"

void papg_init(unsigned char bsp_flags);

#define PML4_VBASE    0xFFFFFFFFFFFFF000     //pml4虚拟地址基址
#define PDPT_VBASE    0xFFFFFFFFFFE00000     //pdpt虚拟地址基址
#define PD_VBASE      0xFFFFFFFFC0000000     //pd虚拟地址基址
#define PT_VBASE      0xFFFFFF8000000000     //pt虚拟地址基址

#endif