#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "printk.h"
#include "lib.h"
#include "cpuinfo.h"

void  memory_init(unsigned char bsp_flags);
void * alloc_pages(unsigned long required_length);
int free_pages(void *pages_addr,unsigned long required_length);
void map_pages(unsigned long paddr, unsigned long vaddr,unsigned long page_num, unsigned long attr);

extern unsigned long kenelstack_top;
extern unsigned long _start_text;

#define E820_SIZE    0x500
#define E820_BASE    0x504

#define PAGE_OFFSET	((unsigned long)0xffff800000000000)
#define PAGE_4K_SHIFT	12
#define PAGE_4K_SIZE	(1UL << PAGE_4K_SHIFT)
#define PAGE_4K_MASK	(~ (PAGE_4K_SIZE - 1))
#define PAGE_4K_ALIGN(addr)	(((unsigned long)(addr) + PAGE_4K_SIZE - 1) & PAGE_4K_MASK)

#define HADDR_TO_LADDR(addr)	((unsigned long)(addr) & (~PAGE_OFFSET))
#define LADDR_TO_HADDR(addr)	((unsigned long *)((unsigned long)(addr) | PAGE_OFFSET))

struct E820
{
    unsigned long address;
    unsigned long length;
    unsigned int	type;
}__attribute__((packed));

typedef struct {
    struct E820 	e820[12];
    unsigned long 	e820_length;

    unsigned long * bits_map;
    unsigned long 	bits_size;
    unsigned long   bits_length;

    unsigned long   total_pages;
    unsigned long   alloc_pages;
    unsigned long   free_pages;

    unsigned long 	kernel_start;
    unsigned long   kernel_end;

    unsigned char   lock;
} Global_Memory_Descriptor;

Global_Memory_Descriptor memory_management_struct = {0};

unsigned long* pml4t_vbase = (unsigned long*)0xFFFFFFFFFFFFF000;  //pml4虚拟地址基址
unsigned long* pdptt_vbase = (unsigned long*)0xFFFFFFFFFFE00000;  //pdpt虚拟地址基址
unsigned long* pdt_vbase = (unsigned long*)0xFFFFFFFFC0000000;    //pd虚拟地址基址
unsigned long* ptt_vbase = (unsigned long*)0xFFFFFF8000000000;    //pt虚拟地址基址

extern unsigned long __PML4T[512];

#define INVLPG(addr) \
        do{                     \
            __asm__ __volatile__("invlpg (%0) \n\t"::"r"(addr):"memory"); \
        }while(0)

#define SET_CR3(addr) \
        do{           \
            __asm__ __volatile__("mov %0,%%cr3 \n\t"::"r"(addr):"memory");              \
        }while(0)

#define GET_CR3(addr) \
        do{           \
            __asm__ __volatile__("mov %%cr3,%0 \n\t":"=r"(addr)::"memory");              \
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
