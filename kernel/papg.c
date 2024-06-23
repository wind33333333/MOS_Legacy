#include "papg.h"

__attribute__((section(".init_text"))) void papg_init(void){


    color_printk(ORANGE, BLACK, "OS Can Used Total 4K PAGEs: %ld \tAlloc: %ld \tFree: %ld\n",
                 memory_management_struct.total_pages, memory_management_struct.alloc_pages,
                 memory_management_struct.free_pages);

    return;
}