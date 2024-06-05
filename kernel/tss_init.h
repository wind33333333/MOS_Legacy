#ifndef __TSS_INIT_H__
#define __TSS_INIT_H__
#include "lib.h"
#include "memory.h"
#include "gdt_init.h"

#define TSS_DESCRIPTOR_L(ADDR)  \
    (TSS_TYPE | P | TSS_LIMIT | DPL_0 | \
    ((unsigned long)(ADDR) & 0x000000000000FFFF) | \
    (((unsigned long)(ADDR) >> 16) & 0x00000000000000FF) << 32 | \
    (((unsigned long)(ADDR) >> 24) & 0x00000000000000FF) << 56)

#define TSS_DESCRIPTOR_H(ADDR)  (unsigned long)(ADDR) >> 32
#define TSS_TYPE    0x9UL << 40
#define TSS_LIMIT   (0x67UL & 0xFFFF) | ((0x67UL >> 16)<<48)





void    tss_init(unsigned int cpu_id);

struct _tss_ptr{
    unsigned long limit;
    struct _tss *base;
};

__attribute__((section(".init_data"))) struct _tss_ptr tss_ptr = {0,0};

struct _tss{
    unsigned int    reserved0;
    unsigned long   rsp0;
    unsigned long   rsp1;
    unsigned long   rsp2;
    unsigned long   reserved1;
    unsigned long   ist1;
    unsigned long   ist2;
    unsigned long   ist3;
    unsigned long   ist4;
    unsigned long   ist5;
    unsigned long   ist6;
    unsigned long   ist7;
    unsigned long   reserved2;
    unsigned short  reserved3;
    unsigned short  iomap_base;
} __attribute__((packed));




#endif