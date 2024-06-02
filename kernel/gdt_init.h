#ifndef __GDT_INIT_H__
#define __GDT_INIT_H__

void    gdt_init(unsigned int bsp_flags);

struct gdt{
    unsigned short limit;
    unsigned long base;
} __attribute__((packed));

struct gdt gdt_ptr={0,0};

#endif