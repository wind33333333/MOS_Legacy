#include "papg.h"

__attribute__((section(".init_text"))) void papg_init(unsigned char bsp_flags) {

    if (bsp_flags) {
        unsigned long papg_num = 0;
        unsigned long papg_tablenum = 0;
        Virt_To_Phy(memory_management_struct.kernel_end) / 4096;
        if (Virt_To_Phy(memory_management_struct.kernel_end) % 4096)
            papg_num++;

        for (unsigned int i = 0; i < 3; i++) {
            papg_num = papg_num / 512;
            papg_tablenum += papg_num;
            if (papg_num % 512)
                papg_tablenum++;
        }

        color_printk(ORANGE, BLACK, "OS PaPg: %ld  PaPgT: %ld\n",papg_num,papg_tablenum);



        color_printk(ORANGE, BLACK, "OS Can Used Total 4K PAGEs: %ld \tAlloc: %ld \tFree: %ld\n",
                     memory_management_struct.total_pages, memory_management_struct.alloc_pages,
                     memory_management_struct.free_pages);
        color_printk(ORANGE, BLACK, "OS StartAddr: %#018lX \tEndAddr: %#018lX \n",
                     memory_management_struct.kernel_start,memory_management_struct.kernel_end);
    }

    return;
}