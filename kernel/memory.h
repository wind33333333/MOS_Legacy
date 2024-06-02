#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "printk.h"
#include "lib.h"

void  memory_init(unsigned int bsp_flags);
void * alloc_pages(void);

#define E820_SIZE    0x500
#define E820_BASE    0x504

#define PAGE_OFFSET	((unsigned long)0xffff800000000000)
#define PAGE_4K_SHIFT	12
#define PAGE_4K_SIZE	(1UL << PAGE_4K_SHIFT)
#define PAGE_4K_MASK	(~ (PAGE_4K_SIZE - 1))
#define PAGE_4K_ALIGN(addr)	(((unsigned long)(addr) + PAGE_4K_SIZE - 1) & PAGE_4K_MASK)

#define Virt_To_Phy(addr)	((unsigned long)(addr) - PAGE_OFFSET)
#define Phy_To_Virt(addr)	((unsigned long *)((unsigned long)(addr) + PAGE_OFFSET))

struct E820
{
    unsigned long address;
    unsigned long length;
    unsigned int	type;
}__attribute__((packed));

struct Global_Memory_Descriptor
{
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
};

struct Global_Memory_Descriptor memory_management_struct = {{0},0};

#endif
