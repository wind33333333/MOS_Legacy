#ifndef __GDT_INIT_H__
#define __GDT_INIT_H__
#include "lib.h"
#include "memory.h"

void    gdt_init(unsigned int bsp_flags);

struct gdt{
    unsigned short limit;
    unsigned long *base;
} __attribute__((packed));

__attribute__((section(".init_data"))) struct gdt gdt_ptr={0,0};

#endif