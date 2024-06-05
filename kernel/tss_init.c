#include "tss_init.h"

__attribute__((section(".init_text"))) void tss_init(unsigned int cpu_id) {
    if (bsp_flags) {
        tss_ptr.limit = ((cpu_num * 104 + 0xFFF) & PAGE_4K_MASK) - 0x1;
        for (unsigned int i = (tss_ptr.limit + 1) >> PAGE_4K_SHIFT; i != 0; i--) {
            tss_ptr.base = Phy_To_Virt(alloc_pages());                      //分配tss_tables内存
        }
        memory_management_struct.kernel_end = (unsigned long)tss_ptr.base;
        tss_ptr.base -= (tss_ptr.limit & PAGE_4K_MASK) / sizeof(long);      //创建tss_tables指针
    }

    tss_ptr.base[cpu_id].reserved0 = 0 ;
    for (unsigned int i = 0; i < 4; i++) {
        tss_ptr.base[cpu_id].rsp0 = (unsigned long)Phy_To_Virt(alloc_pages());
    }
    tss_ptr.base[cpu_id].rsp0 += PAGE_4K_SIZE;
    tss_ptr.base[cpu_id].rsp1 = 0;
    tss_ptr.base[cpu_id].rsp2 = 0;
    tss_ptr.base[cpu_id].reserved1 = 0;
    for (unsigned int i = 0; i < 4; i++) {
        tss_ptr.base[cpu_id].ist1 = (unsigned long)Phy_To_Virt(alloc_pages());
    }
    tss_ptr.base[cpu_id].ist1 += PAGE_4K_SIZE;
    tss_ptr.base[cpu_id].ist2 = 0;
    tss_ptr.base[cpu_id].ist3 = 0;
    tss_ptr.base[cpu_id].ist4 = 0;
    tss_ptr.base[cpu_id].ist5 = 0;
    tss_ptr.base[cpu_id].ist6 = 0;
    tss_ptr.base[cpu_id].ist7 = 0;
    tss_ptr.base[cpu_id].reserved2 = 0;
    tss_ptr.base[cpu_id].reserved3 = 0;
    tss_ptr.base[cpu_id].iomap_base = 0;

    *(gdt_ptr.base + GDT_HEADER_SIZE + cpu_id*2) = TSS_DESCRIPTOR_L(tss_ptr.base+cpu_id);
    *(gdt_ptr.base + GDT_HEADER_SIZE + cpu_id*2 + 1) = TSS_DESCRIPTOR_H(tss_ptr.base+cpu_id);

    __asm__ __volatile__(
            "ltr    %w0 \n\t"
            ::"r"((cpu_id<<4)+GDT_HEADER_SIZE*8):);

    return;
}