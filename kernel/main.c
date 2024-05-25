#include "lib.h"
#include "printk.h"
#include "gate.h"
#include "trap.h"
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

    extern unsigned int bsp_flags;

    unsigned int cpu_id = 0;
    __asm__ __volatile__ (
            "movl $0x802,%%ecx   \n\t"
            "rdmsr               \n\t"
            :"=a"(cpu_id)::"%rcx", "%rdx");

    pos_init(bsp_flags);            //初始化输出控制台

    memory_init(bsp_flags);         //初始化内存管理器

    gdt_init(bsp_flags);            //初始化GDT

    tss_init(cpu_id);               //初始化TSS

    papg_init(bsp_flags);           //初始化内核页表

    idt_init(bsp_flags, ignore_int);      //初始化中断描述符表

    apic_init();                    //初始化apic

    sys_vector_init(bsp_flags);     //初始化系中断列程

    acpi_init(bsp_flags);           //初始化acpi

    ioapic_init(bsp_flags);         //初始化ioapic

    ap_init(bsp_flags);             //初始化ap核

    color_printk(GREEN, BLACK, "CPU%d init successful\n", cpu_id);


//  __asm__ __volatile__ ("int $0 \n\t":: :);

//  int i = 1/0;

    while (1);
}