#ifndef _GET_CPUINFO_H
#define _GET_CPUINFO_H

void get_cpuinfo(unsigned int *cpu_id);

struct {
    unsigned int num;
    char manufacturer_name[13];
    char model_name[41];
    unsigned long frequency;
}cpu_info;

#define GET_CPUID(CPUID) \
        do {             \
        __asm__ __volatile__ (              \
            "movl $0x802,%%ecx   \n\t"      \
            "rdmsr               \n\t"      \
            :"=a"(*CPUID)::"%rcx", "%rdx"); \
            } while(0)

extern unsigned int bsp_flags;

#endif