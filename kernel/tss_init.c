#include "tss_init.h"

__attribute__((section(".init_text"))) void tss_init(unsigned int bsp_flags,unsigned int cpu_id) {
    if(bsp_flags) {
        unsigned long *tss_table;
        unsigned long tss_limit = ((cpu_num * 104 + 0xFFF) & PAGE_4K_MASK) - 0x1;
        for(unsigned int i = (tss_limit+1) >> PAGE_4K_SHIFT; i != 0; i--){
            tss_table = Phy_To_Virt(alloc_pages());                      //分配tss_tables内存
        }
        memory_management_struct.kernel_end = tss_table;
        tss_table -= (tss_limit& PAGE_4K_MASK)/sizeof(long);             //创建tss_tables指针


    }

    return;
}