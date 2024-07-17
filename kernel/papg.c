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


        map_pages((unsigned long) alloc_pages(516), 0x7FFFFFF000, 516,
                  PAPG_PAT | PAPG_RW | PAPG_P);
        BOCHS_DG();
        map_pages((unsigned long) alloc_pages(1),0x7FFFE00000,1,PAPG_PAT | PAPG_RW | PAPG_P);
        map_pages((unsigned long) alloc_pages(1),0x8000204000,1,PAPG_PAT | PAPG_RW | PAPG_P),
        unmap_pages(0x7FFFFFF000, 516);
        unmap_pages(0x7FFFE00000, 1);
        unmap_pages(0x8000204000, 1);
    }

    SET_CR3(HADDR_TO_LADDR(&__PML4T));

    return;
}


//释放物理内存映射虚拟内存
void unmap_pages(unsigned long vaddr, unsigned long page_num) {
    unsigned long num;
    unsigned long offset = vaddr & 0xFFFFFFFFFFFFUL;

    //释放页表 PT
    free_pages((void *)(ptt_vbase[(offset >> 12)] & PAGE_4K_MASK), page_num);
    memset(&ptt_vbase[(offset >> 12)], 0, page_num << 3);

    //释放页目录 PD
    num = ((page_num + ((vaddr >> 12) - ((vaddr >> 12) & ~(512UL - 1)))) + (512UL - 1)) / 512UL;
    for (unsigned long i = 0; i < num; i++) {
        for (unsigned long j = 0; j < 512; j++) {
            if(ptt_vbase[(offset >> 21 << 9) + i * 512 + j]) {
                break;
            } else if(j == 511){
                free_pages((void *)(pdt_vbase[(offset >> 21) + i] & PAGE_4K_MASK), 1);
                pdt_vbase[(offset >> 21) + i] = 0;
                break;
            }
        }
    }

    //释放页目录表 PDPT
    num = ((page_num + ((vaddr >> 12) - ((vaddr >> 12) & ~(512UL * 512 - 1)))) + (512UL * 512 - 1)) /
        (512UL * 512);
    for (unsigned long i = 0; i < num; i++) {
        for (unsigned long j = 0; j < 512UL; j++) {
            if(pdt_vbase[(offset >> 30 << 9) + i * 512UL + j]) {
                break;
            } else if(j == 511){
                free_pages((void *) (pdptt_vbase[(offset >> 30) + i] & PAGE_4K_MASK), 1);
                pdptt_vbase[(offset >> 30) + i] = 0;
                break;
            }
        }
    }

    //释放页目录表 PML4T
    num = ((page_num + ((vaddr >> 12) - ((vaddr >> 12) & ~(512UL * 512 * 512 - 1)))) +
         (512UL * 512 * 512 - 1)) / (512UL * 512 * 512);
    for (unsigned long i = 0; i < num; i++) {
        for (unsigned long j = 0; j < 512UL; j++) {
            if(pdptt_vbase[(offset >> 39 << 9) + i * 512UL + j]) {
                break;
            } else if(j == 511){
                free_pages((void *) (pml4t_vbase[(offset >> 39) + i] & PAGE_4K_MASK), 1);
                pml4t_vbase[(offset >> 39) + i] = 0;
                break;
            }
        }
    }

    // 刷新 TLB
    for (unsigned long i = 0; i < page_num; i++) {
        INVLPG((vaddr & PAGE_4K_MASK) + i * 4096);
    }

    return;
}




