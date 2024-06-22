#ifndef __ACPI_H__
#define __ACPI_H__

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

//IA32_TSC_DEADLINE寄存器 TSC-Deadline定时模式
#define APIC_SET_TSCDEADLINE(TIME) \
        do {                \
          __asm__ __volatile__( \
          "rdtscp \n\t"   \
          "shl $32,%%rdx \n\t"    \
          "or %%rdx,%%rax \n\t"   \
          "add %0,%%rax \n\t"    \
          "mov %%rax,%%rdx \n\t"  \
          "mov $0xFFFFFFFF,%%rcx \n\t"    \
          "and %%rcx,%%rax \n\t"  \
          "shr $32,%%rdx \n\t"    \
          "mov $0x6E0,%%ecx \n\t"         \
          "wrmsr \n\t"    \
          ::"m"(TIME):"%rax","%rcx","%rdx");  \
          } while(0)

#define ENABLE_APIC_TIME(TIME,MODEL,IVT) \
        do {                 \
         __asm__ __volatile(   \
         "movl   $0x832,%%ecx    \n\t"         //定时器配置寄存器  \
         "xorl   %%edx,%%edx     \n\t"  \
         "movl   $1,%%eax  \n\t"         //bit0-7中断向量号,bit16屏蔽标志 0未屏蔽 1屏蔽,bit17 18 00/一次计数 01/周期计数 10/TSC-Deadline  \
         "wrmsr                  \n\t"                      \
                               \
         "movl   $0x83E,%%ecx    \n\t"         //分频器寄存器  \
         "xorl   %%edx,%%edx     \n\t"              \
         "movl   $0xA,%%eax      \n\t"         //bit013 0:2 1:4 2:8 3:16 8:32 9:64 0xA:128 0xB:1   \
         "wrmsr                  \n\t"        \
                                \
     #if (MODEL==TSC_DEADLINE)      \
         "rdtscp \n\t"   \
         "shl $32,%%rdx \n\t"    \
         "or %%rdx,%%rax \n\t"   \
         "add %0,%%rax \n\t"    \
         "mov %%rax,%%rdx \n\t"  \
         "mov $0xFFFFFFFF,%%rcx \n\t"    \
         "and %%rcx,%%rax \n\t"  \
         "shr $32,%%rdx \n\t"    \
         "mov $0x6E0,%%ecx \n\t"         \
         "wrmsr \n\t"    \ \
                            \
      #else                 \
         "mov   %0,%%eax   \n\t"                \
         "xor   %%rdx ,%%rdx        \n\t"        \
         "mov   $0x838,%%ecx    \n\t"           //定时器计数器寄存器          \
         "wrmsr                  \n\t"         \
      #endif  \
         ::"m"(TIME),"m"((MODEL)|(IVT)):"%rax","%rcx","%rdx");  \
           } while(0)


#define ONESHOT 0
#define PERIODIC  0x20000
#define TSC_DEADLINE 0x40000

#endif
