#ifndef __PAPG_INIT_H__
#define __PAPG_INIT_H__

#include "printk.h"
#include "memory.h"
#include "cpuinfo.h"

void papg_init(unsigned char bsp_flags);
void kernel_mount_page(unsigned long phy_addr, unsigned long phy_len);
void user_mount_page(unsigned long phy_addr, unsigned long phy_len);

unsigned long* upml4t_vbase = (unsigned long*)0xFFFFFFFFFFFFF000;  //用户pml4虚拟地址基址
unsigned long* updptt_vbase = (unsigned long*)0xFFFFFFFFFFE00000;  //用户pdpt虚拟地址基址
unsigned long* updt_vbase = (unsigned long*)0xFFFFFFFFC0000000;    //用户pd虚拟地址基址
unsigned long* uptt_vbase = (unsigned long*)0xFFFFFF8000000000;    //用户pt虚拟地址基址

unsigned long* kpml4t_vbase = (unsigned long*)0xFFFFFFFFFFFFF800;  //内核pml4虚拟地址基址
unsigned long* kpdptt_vbase = (unsigned long*)0xFFFFFFFFFFF00000;  //内核pdpt虚拟地址基址
unsigned long* kpdt_vbase = (unsigned long*)0xFFFFFFFFE0000000;    //内核pd虚拟地址基址
unsigned long* kptt_vbase = (unsigned long*)0xFFFFFFC000000000;    //内核pt虚拟地址基址

extern unsigned long __PML4T[512];

#endif