#ifndef __PAPG_INIT_H__
#define __PAPG_INIT_H__

#include "printk.h"
#include "memory.h"
#include "cpuinfo.h"
#include "hpet.h"

void papg_init(unsigned char bsp_flags);
void map_pages(unsigned long paddr, unsigned long vaddr,unsigned long page_num, unsigned long attr);
void unmap_pages(unsigned long paddr, unsigned long page_num);


#endif