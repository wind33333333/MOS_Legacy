#include "page.h"

__attribute__((section(".init_text"))) void page_init(unsigned char bsp_flags) {

    if (bsp_flags) {
        unsigned long pml4_bak[256] = {0};
        unsigned long pml4e_num =
                HADDR_TO_LADDR(memory_management_struct.kernel_end) / (4096UL * 512 * 512 * 512);

        if (HADDR_TO_LADDR(memory_management_struct.kernel_end) % (4096UL * 512 * 512 * 512))
            pml4e_num++;

        for (unsigned int i = 0; i < pml4e_num; i++) {
            pml4_bak[i] = pml4t_vbase[i];  //备份原PML4E
            pml4t_vbase[i] = 0x0UL;        //清除PML4E
        }

        map_pages(0, 0,
                  HADDR_TO_LADDR(memory_management_struct.kernel_end) / 4096,
                  PAGE_G | PAGE_PAT | PAGE_RW | PAGE_P);

        for (unsigned int i = 0; i < pml4e_num; i++) {
            //__PML4T[i] = pml4t_vbase[i];            //修改正式内核PML4T 低
            __PML4T[i + 256] = pml4t_vbase[i];        //修改正式内核PML4T 高
            pml4t_vbase[i] = pml4_bak[i];             //还原PML4E
        }

        color_printk(ORANGE, BLACK, "OS Can Used Total 4K PAGEs: %ld \tAlloc: %ld \tFree: %ld\n",
                     memory_management_struct.total_pages, memory_management_struct.alloc_pages,
                     memory_management_struct.free_pages);
        color_printk(ORANGE, BLACK, "OS StartAddr: %#018lX \tEndAddr: %#018lX \n",
                     memory_management_struct.kernel_start, memory_management_struct.kernel_end);

        SET_CR3(HADDR_TO_LADDR(&__PML4T));

        map_pages(HADDR_TO_LADDR(Pos.FB_addr), Pos.FB_addr, Pos.FB_length / 4096,
                  PAGE_NX | PAGE_G | PAGE_PAT | PAGE_RW | PAGE_P);
        map_pages(HADDR_TO_LADDR(ioapic_baseaddr), (unsigned long) ioapic_baseaddr, 1,
                  PAGE_NX | PAGE_G | PAGE_PAT | PAGE_PCD | PAGE_PWT | PAGE_RW | PAGE_P);
        map_pages(HADDR_TO_LADDR(hpet_attr.baseaddr), hpet_attr.baseaddr, 1,
                  PAGE_NX | PAGE_G | PAGE_PAT | PAGE_PCD | PAGE_PWT | PAGE_RW | PAGE_P);


        map_pages((unsigned long) alloc_pages(516), 0x7FFFFFF000, 516,
                  PAGE_NX | PAGE_PAT | PAGE_P);
        map_pages((unsigned long) alloc_pages(1),0x7FFFE00000,1,PAGE_NX | PAGE_PAT | PAGE_RW | PAGE_P);
        map_pages((unsigned long) alloc_pages(1),0x8000204000,1,PAGE_NX | PAGE_PAT | PAGE_RW | PAGE_P),
//        unmap_pages(0x7FFFFFF000, 516);
//        unmap_pages(0x7FFFE00000, 1);
//        unmap_pages(0x8000204000, 1);

        *(unsigned long*)0x7FFFFFF000 = 0xFFFFFFFF;
    }

    SET_CR3(HADDR_TO_LADDR(&__PML4T));

    return;
}





