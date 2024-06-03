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

#define CODE64_0 TYPE_CODE64 | DPL_0 | S | P | L
#define DATA64_0 TYPE_DATA64 | DPL_0 | S | P
#define CODE64_3 TYPE_CODE64 | DPL_3 | S | P | L
#define DATA64_3 TYPE_DATA64 | DPL_3 | S | P
#define CODE32_0 TYPE_CODE32 | DPL_0 | S | P | LIMIT32 | DB | G
#define DATA32_0 TYPE_DATA32 | DPL_0 | S | P | LIMIT32 | DB | G

#define TYPE_CODE64 0x8UL <<40
#define TYPE_DATA64 0x2UL <<40
#define TYPE_CODE32 0xAUL <<40
#define TYPE_DATA32 0x2UL <<40
#define TYPE_TSSGATE 0x9UL <<40
#define TYPE_CALLGATE 0xCUL <<40
#define S          1UL << 44
#define DPL_0      0UL << 45
#define DPL_3      3UL <<45
#define P          1UL << 47
#define LIMIT32    (0xFUL << 48) | 0xFFFF
#define L          1UL << 53
#define DB         1UL << 54
#define G          1UL << 55


#endif