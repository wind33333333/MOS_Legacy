#include "lib.h"
#include "printk.h"
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
#include "cpuinfo.h"
#include "hpet.h"

__attribute__((section(".init_text"))) void Kernel_init(void) {

    unsigned int cpu_id;
    pos_init();                                  //初始化输出控制台
    get_cpuinfo(&cpu_id);                     //获取cpu信息
    memory_init();                               //初始化内存管理器
    gdt_init();                                  //初始化GDT
    tss_init(cpu_id);                            //初始化TSS
    idt_init();                                  //初始化IDT
    acpi_init();                                 //初始化acpi
    ioapic_init();                               //初始化ioapic
    hpet_init();
    apic_init();                                 //初始化apic
    ap_init(cpu_id);                             //初始化ap核
    papg_init();                                 //初始化内核页表



    sti();

//    color_printk(YELLOW, BLACK, "Kernelstart: %#018lX Kernelend: %#018lX \n",memory_management_struct.kernel_start, memory_management_struct.kernel_end);
//    __asm__ __volatile__ ("int $0 \n\t":: :);
//    int i = 1 / 0;

    while (1);
}