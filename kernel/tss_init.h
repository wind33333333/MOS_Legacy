#ifndef __TSS_INIT_H__
#define __TSS_INIT_H__
#include "lib.h"
#include "memory.h"

void    tss_init(unsigned int cpu_id);

struct _tss64_ptr{
    unsigned long limit;
    struct _tss64 *base;
};

__attribute__((section(".init_data"))) struct _tss64_ptr tss64_ptr = {0,0};

struct _tss64{
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

struct _tss64 tss64 = {0};

#endif