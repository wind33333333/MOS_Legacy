#ifndef __TSS_INIT_H__
#define __TSS_INIT_H__
#include "lib.h"
#include "memory.h"

void    tss_init(unsigned int bsp_flags,unsigned int cpu_id);

struct _tss64_ptr{
    unsigned long limit;
    unsigned long *base;
};

struct _tss64_ptr tss64_ptr = {0,0};

struct _tss64{


} __attribute__((packed));

#endif