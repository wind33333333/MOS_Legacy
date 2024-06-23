#ifndef __ACPI_H__
#define __ACPI_H__

#include "lib.h"

void apic_init(void);

//中断结束发送EOI
#define EOI() \
        do {  \
          __asm__ __volatile__( \
           "xor     %%edx,%%edx \n\t" \
           "xor     %%eax,%%eax \n\t" \
           "mov     $0x80B,%%ecx \n\t" \
           "wrmsr      \n\t"  \
            :::"%rdx","%rax");    \
          } while(0)


#define DISABLE_APIC_TIME() \
        do {  \
          __asm__ __volatile__( \
         "xorl   %%edx,%%edx           \n\t"          \
         "movl   $0x10000,%%eax        \n\t"         /*bit0-7中断向量号,bit16屏蔽标志 0未屏蔽 1屏蔽,bit17 18 00/一次计数 01/周期计数 10/TSC-Deadline*/ \
         "movl   $0x832,%%ecx          \n\t"         /*定时器模式配置寄存器*/ \
         "wrmsr                        \n\t"          \
          :::"%rdx","%rax");    \
          } while(0)


void enable_apic_time (unsigned long time,unsigned int model,unsigned int ivt);


#define APIC_ONESHOT 0
#define APIC_PERIODIC  0x20000
#define APIC_TSC_DEADLINE 0x40000

#endif
