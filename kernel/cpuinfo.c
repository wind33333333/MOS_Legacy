#include "cpuinfo.h"



void get_cpuinfo(unsigned int *p){

    // 获取当前CPU id号
    __asm__ __volatile__ (
            "movl $0x802,%%ecx   \n\t"
            "rdmsr               \n\t"
            :"=a"(*p)::"%rcx", "%rdx");

    if(bsp_flags) {
        // 获取CPU厂商
        __asm__ __volatile__(
                "mov $0, %%eax \n\t"
                "cpuid         \n\t"
                "mov %%ebx, (%%rdi) \n\t"
                "mov %%edx, 4(%%edi)\n\t"
                "mov %%ecx, 8(%%edi) \n\t"
                "movb $0, 12(%%edi) \n\t"
                ::"D"(&cpu_info.manufacturer_name):"%rax", "%rbx", "%rcx", "%rdx");

        // 获取CPU型号
        __asm__ __volatile__(
                "mov $0x80000002, %%eax \n\t"
                "cpuid         \n\t"
                "mov %%eax, (%%rdi)   \n\t"
                "mov %%ebx, 4(%%rdi)  \n\t"
                "mov %%ecx, 8(%%edi)  \n\t"
                "mov %%edx, 12(%%edi) \n\t"

                "mov $0x80000003, %%eax \n\t"
                "cpuid         \n\t"
                "mov %%eax, 16(%%rdi)   \n\t"
                "mov %%ebx, 20(%%rdi)  \n\t"
                "mov %%ecx, 24(%%edi)  \n\t"
                "mov %%edx, 28(%%edi) \n\t"

                "mov $0x80000004, %%eax \n\t"
                "cpuid         \n\t"
                "mov %%eax, 32(%%rdi)   \n\t"
                "mov %%ebx, 36(%%rdi)  \n\t"
                "mov %%ecx, 40(%%edi)  \n\t"
                "mov %%edx, 44(%%edi) \n\t"

                "mov $0, 48(%%edi) \n\t"
                ::"D"(&cpu_info.model_name):"%rax", "%rbx", "%rcx", "%rdx");

        // 获取CPU频率
        __asm__ __volatile__(
                "mov $0x16, %%eax \n\t"
                "cpuid         \n\t"
                "shl $32,%%rdx  \n\t"
                "or %%rdx,%%rax \n\t"
                :"=a"(cpu_info.frequency)::"%rbx", "%rcx", "%rdx");
    }
        return;
}
