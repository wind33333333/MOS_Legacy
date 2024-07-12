#ifndef __PAPG_INIT_H__
#define __PAPG_INIT_H__

#include "printk.h"
#include "memory.h"
#include "cpuinfo.h"
#include "hpet.h"

void papg_init(unsigned char bsp_flags);
void mount_page(unsigned long paddr, unsigned long vaddr,unsigned long len, unsigned long attr);
void umount_page(unsigned long addr, unsigned long len);

unsigned long* pml4t_vbase = (unsigned long*)0xFFFFFFFFFFFFF000;  //pml4虚拟地址基址
unsigned long* pdptt_vbase = (unsigned long*)0xFFFFFFFFFFE00000;  //pdpt虚拟地址基址
unsigned long* pdt_vbase = (unsigned long*)0xFFFFFFFFC0000000;    //pd虚拟地址基址
unsigned long* ptt_vbase = (unsigned long*)0xFFFFFF8000000000;    //pt虚拟地址基址

extern unsigned long __PML4T[512];

#define FULSH_TLB_PAGE(addr) \
        do{                     \
        __asm__ __volatile__("invlpg (%0)"::"r"(addr):"memory"); \
        }while(0)

#define PAPG_G      1UL<<8
#define PAPG_PAT    1UL<<7
#define PAPG_D      1UL<<6
#define PAPG_A      1UL<<5
#define PAPG_PCD    1UL<<4
#define PAPG_PWT    1UL<<3
#define PAPG_US     1UL<<2
#define PAPG_RW     1UL<<1
#define PAPG_P      1UL<<0


#endif