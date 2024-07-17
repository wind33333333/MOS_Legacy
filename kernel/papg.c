#include "papg.h"

__attribute__((section(".init_text"))) void papg_init(unsigned char bsp_flags) {

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
                  PAPG_G | PAPG_PAT | PAPG_RW | PAPG_P);

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
                  PAPG_G | PAPG_PAT | PAPG_RW | PAPG_P);
        map_pages(HADDR_TO_LADDR(ioapic_baseaddr), (unsigned long) ioapic_baseaddr, 1,
                  PAPG_G | PAPG_PAT | PAPG_PCD | PAPG_PWT | PAPG_RW | PAPG_P);
        map_pages(HADDR_TO_LADDR(hpet_attr.baseaddr), hpet_attr.baseaddr, 1,
                  PAPG_G | PAPG_PAT | PAPG_PCD | PAPG_PWT | PAPG_RW | PAPG_P);


        map_pages((unsigned long) alloc_pages(0x516), 0x7FFFFFF000, 516,
                  PAPG_G | PAPG_PAT | PAPG_RW | PAPG_P);
        map_pages((unsigned long) alloc_pages(1),0x7FFFE00000,1,PAPG_G | PAPG_PAT | PAPG_RW | PAPG_P);
        map_pages((unsigned long) alloc_pages(1),0x8000204000,1,PAPG_G | PAPG_PAT | PAPG_RW | PAPG_P),
        unmap_pages(0x7FFFFFF000, 516);
    }

    SET_CR3(HADDR_TO_LADDR(&__PML4T));

    return;
}


void unmap_pages(unsigned long vaddr, unsigned long page_num) {
    unsigned long headlength, taillength, x, y;
    unsigned long offset = vaddr & 0xFFFFFFFFFFFFUL;

    //PT 页表
    headlength = &ptt_vbase[(offset >> 12)] - &ptt_vbase[(offset >> 21 << 9)];
    for (int i = 0; i < headlength; i++) {
        if (ptt_vbase[(offset >> 21 << 9) + i] != 0) {
            x = 1;
            break;
        }
        x = 0;
    }

    if(x){
        for (unsigned long i = 0; i < (512-headlength); i++) {
            ptt_vbase[(offset >> 12) + i] = 0;
        }
    }


    taillength = (((unsigned long) &ptt_vbase[(offset >> 12) + page_num + 0x1FF] & ~0xFFFUL) -
                  (unsigned long) &ptt_vbase[(offset >> 12) + page_num]) >> 3;
    for (int i = 0; i < taillength; i++) {
        if (ptt_vbase[(offset >> 12) + page_num + i] != 0) {
            x = 1;
            break;
        }
        x = 0;
    }

    if(x){
        for (unsigned long i = 0; i < (512-taillength); i++) {
            ptt_vbase[(offset >> 21 <<9) + page_num + i] = 0;
        }
    }


    for (unsigned long i = 0; i < page_num; i++) {
        ptt_vbase[(offset >> 12) + i] = 0;
    }


    y = ((page_num + ((vaddr >> 12) - ((vaddr >> 12) & ~(512UL - 1)))) + (512UL - 1)) / 512UL;
    for (unsigned long i = 0; i < y; i++) {
        if (ptt_vbase[(offset >> 21 << 9) + i])


            if (pdt_vbase[(offset >> 21) + i] != 0) {
                free_pages((void *) (pdt_vbase[(offset >> 21) + i] & PAGE_4K_MASK), 1);
                pdt_vbase[(offset >> 21) + i] = 0;
            }
    }

    y = ((page_num + ((vaddr >> 12) - ((vaddr >> 12) & ~(512UL * 512 - 1)))) + (512UL * 512 - 1)) /
        (512UL * 512);
    for (unsigned long i = 0; i < y; i++) {
        if (pdptt_vbase[(offset >> 30) + i] != 0) {
            free_pages((void *) (pdptt_vbase[(offset >> 30) + i] & PAGE_4K_MASK), 1);
            pdptt_vbase[(offset >> 30) + i] = 0;
        }
    }

    y = ((page_num + ((vaddr >> 12) - ((vaddr >> 12) & ~(512UL * 512 * 512 - 1)))) +
         (512UL * 512 * 512 - 1)) / (512UL * 512 * 512);
    for (unsigned long i = 0; i < y; i++) {
        if (pml4t_vbase[(offset >> 39) + i] != 0) {
            free_pages((void *) (pml4t_vbase[(offset >> 39) + i] & PAGE_4K_MASK), 1);
            pml4t_vbase[(offset >> 39) + i] = 0;
        }
    }

    // 刷新 TLB
    for (unsigned long i = 0; i < page_num; i++) {
        INVLPG((vaddr & PAGE_4K_MASK) + i * 4096);
    }

    return;
}




