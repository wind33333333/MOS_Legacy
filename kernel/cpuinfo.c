#include "cpuinfo.h"



void get_cpuinfo(unsigned int *cpu_id){

    GET_CPUID(cpu_id);                           //获取当前cpu的id

    if(bsp_flags)

        __asm__ __volatile__(
                "mov $0, %%eax \n\t"
                "cpuid         \n\t"
                "mov %%ebx, (%%rdi) \n\t"
                "mov %%edx, 4(%%edi)\n\t"
                "mov %%ecx, 8(%%edi) \n\t"
                "movb $0, 12(%%edi) \n\t"
                ::"D"(&cpu_info.manufacturer_name):"%rax","%rbx","%rcx","%rdx");

        return;
}

