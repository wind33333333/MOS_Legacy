#include "papg.h"

__attribute__((section(".init_text"))) void papg_init(unsigned char bsp_flags) {

    if (bsp_flags) {


        color_printk(ORANGE, BLACK, "OS Can Used Total 4K PAGEs: %ld \tAlloc: %ld \tFree: %ld\n",
                     memory_management_struct.total_pages, memory_management_struct.alloc_pages,
                     memory_management_struct.free_pages);
    }

    return;
}