#include "tss_init.h"

__attribute__((section(".init_text"))) void tss_init(unsigned int cpu_id) {
    if (bsp_flags) {
        tss_ptr.limit = ((cpu_num * 104 + 0xFFF) & PAGE_4K_MASK) - 0x1;
        for (unsigned int i = (tss_ptr.limit + 1) >> PAGE_4K_SHIFT; i != 0; i--) {
            tss_ptr.base = Phy_To_Virt(alloc_pages());                      //分配tss_tables内存
        }
        memory_management_struct.kernel_end = tss_ptr.base;
        tss_ptr.base -= (tss_ptr.limit & PAGE_4K_MASK) / sizeof(long);      //创建tss_tables指针
    }

    *(gdt_ptr.base + GDT_HEADER_SIZE + cpu_id) = TSS_DESCRIPTOR_L(tss_ptr.base);
    *(gdt_ptr.base + GDT_HEADER_SIZE + cpu_id+1) = TSS_DESCRIPTOR_H(tss_ptr.base);

    while (1)

    return;
}