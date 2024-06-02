#include "gdt_init.h"

__attribute__((section(".init_text"))) void gdt_init(unsigned int bsp_flags) {
    if (bsp_flags) {
        gdt_ptr.limit = ((cpu_num * 16 + 0x50) + 0xFFF) & PAGE_4K_MASK - 1;
        gdt_ptr.limit = 0x3fff;

        for(unsigned int i = (gdt_ptr.limit+1) >> PAGE_4K_SHIFT; i != 0; i--){
            gdt_ptr.base = alloc_pages();
        }
        memory_management_struct.kernel_end = gdt_ptr.base;
        gdt_ptr.base -= (gdt_ptr.limit & PAGE_4K_MASK)/sizeof(long);



    }
    return;
}