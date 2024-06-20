#ifndef __ACPI_H__
#define __ACPI_H__

void apic_init(void);

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


//定时器计数器寄存器
#define APIC_SET_COUNTER(TIME) \
        do {                 \
        __asm__ __volatile(                     \
        "mov   %0,%%rax   \n\t"                \
        "mov   %%rax,%%rdx      \n\t"          \
        "mov   $0xFFFFFFFF,%%rcx  \n\t"               \
        "and   %%rcx,%%rax  \n\t"    \
        "shr   $32,%%rdx        \n\t"                       \
        "mov   $0x838,%%ecx    \n\t"           \
        "wrmsr                  \n\t"           \
        ::"m"(TIME):"%rax","%rcx","%rdx");  \
        } while(0)

#endif
