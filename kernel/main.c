#include "lib.h"
#include "printk.h"
//#include "gate.h"
//#include "trap.h"
#include "ioapic_init.h"
#include "ap_init.h"
#include "acpi_init.h"
#include "pos_init.h"
#include "idt_init.h"
#include "apic_init.h"
#include "memory.h"
#include "gdt_init.h"
#include "tss_init.h"
#include "papg_init.h"

__attribute__((section(".init_text"))) void Kernel_init(void) {

    unsigned int cpu_id;
    __asm__ __volatile__ (
            "movl $0x802,%%ecx   \n\t"
            "rdmsr               \n\t"
            :"=a"(cpu_id)::"%rcx", "%rdx");

    pos_init();                                  //初始化输出控制台
    memory_init();                               //初始化内存管理器
    gdt_init();                                  //初始化GDT
    tss_init(cpu_id);                            //初始化TSS
    idt_init();                                  //初始化IDT
    apic_init();                                 //初始化apic
    acpi_init();                                 //初始化acpi
    ioapic_init();                               //初始化ioapic
    ap_init(cpu_id);                             //初始化ap核
    papg_init();                                 //初始化内核页表

    color_printk(YELLOW, BLACK, "Kernelstart: %#018lX Kernelend: %#018lX \n",
                 memory_management_struct.kernel_start, memory_management_struct.kernel_end);


//    __asm__ __volatile__ ("int $0 \n\t":: :);
//    int i = 1 / 0;

    while (1);
}