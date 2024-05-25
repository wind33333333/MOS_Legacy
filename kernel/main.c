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

    pos_init(bsp_flags);

    memory_init(bsp_flags);

    gdt_init(bsp_flags);

    tss_init(cpu_id);

    papg_init(bsp_flags);

    idt_init(bsp_flags, (unsigned long) &ignore_int);

    apic_init();

    sys_vector_init(bsp_flags);

    ioapic_init(bsp_flags);

    acpi_init(bsp_flags);

    ap_init(bsp_flags);

    color_printk(GREEN, BLACK, "CPU%d init successful\n", cpu_id);


//  __asm__ __volatile__ ("int $0 \n\t":: :);

//  int i = 1/0;

    while (1);
}