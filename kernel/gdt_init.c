#include "gdt_init.h"

__attribute__((section(".init_text"))) void gdt_init(unsigned int bsp_flags) {
    if (bsp_flags) {
        gdt_ptr.limit = ((cpu_num << 4 + 0x50) + 0xFFF) & PAGE_4K_MASK - 1;
        for(unsigned int i = (gdt_ptr.limit+1) >> PAGE_4K_SHIFT; i != 0; i--){
            gdt_ptr.base = Phy_To_Virt(alloc_pages());              //分配GDT内存
        }
        memory_management_struct.kernel_end = gdt_ptr.base;
        gdt_ptr.base -= (gdt_ptr.limit & PAGE_4K_MASK)/sizeof(long);     //创建gdt指针结构

        *(gdt_ptr.base+0) = 0;        /*0	NULL descriptor		       	00*/
        *(gdt_ptr.base+1) = CODE64_0;        /*1	KERNEL	Code	64-bit	Segment	08*/
        *(gdt_ptr.base+2) = DATA64_0;        /*2	KERNEL	Data	64-bit	Segment	10*/
        *(gdt_ptr.base+3) = CODE64_3;        /*3	USER	Code	64-bit	Segment	18*/
        *(gdt_ptr.base+4) = DATA64_3;        /*4	USER	Data	64-bit	Segment	20*/
        *(gdt_ptr.base+5) = CODE32_0;        /*5	KERNEL	Code	32-bit	Segment	28*/
        *(gdt_ptr.base+6) = DATA32_0;        /*6	KERNEL	Data	32-bit	Segment	30*/
        *(gdt_ptr.base+7) = 0;
    }

    __asm__ __volatile__(
            "lgdt (%0)  \n\t"
            ::"r"(&gdt_ptr):);
    return;
}