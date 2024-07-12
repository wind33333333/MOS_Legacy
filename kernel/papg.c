#include "papg.h"

__attribute__((section(".init_text"))) void papg_init(unsigned char bsp_flags) {

    unsigned long addr = 0;

    if (bsp_flags) {
        unsigned long pml4_bak[256] = {0};
        unsigned long pml4e_num =
                Virt_To_Phy(memory_management_struct.kernel_end) / (4096UL * 512 * 512 * 512);

        if (Virt_To_Phy(memory_management_struct.kernel_end) % (4096UL * 512 * 512 * 512))
            pml4e_num++;

        for (unsigned int i = 0; i < pml4e_num; i++) {
            pml4_bak[i] = pml4t_vbase[i];  //备份原PML4E
            pml4t_vbase[i] = 0x0UL;        //清除PML4E
        }

        mount_page(0, Virt_To_Phy(memory_management_struct.kernel_end),
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

        addr = Virt_To_Phy(&__PML4T);
        __asm__ __volatile__(
                "mov    %%rax,%%cr3 \n\t"
                ::"a"(addr):);

        mount_page(Pos.FB_addr, Pos.FB_length, PAPG_G | PAPG_PAT | PAPG_RW | PAPG_P);
        mount_page((unsigned long) ioapic_baseaddr, 4096,
                   PAPG_G | PAPG_PAT | PAPG_PCD | PAPG_PWT | PAPG_RW | PAPG_P);
        mount_page(hpet_attr.baseaddr, 4096,
                   PAPG_G | PAPG_PAT | PAPG_PCD | PAPG_PWT | PAPG_RW | PAPG_P);


        umount_page(hpet_attr.baseaddr, 4096);
    }

    addr = Virt_To_Phy(&__PML4T);
    __asm__ __volatile__(
            "mov    %%rax,%%cr3 \n\t"
            ::"a"(addr):);

    return;
}

void mount_page(unsigned long addr, unsigned long len, unsigned long attr) {

    unsigned long y;
    unsigned long offset = addr & 0xFFFFFFFFFFFFUL;
    unsigned long paddr = Virt_To_Phy(addr);

    y = len / (4096UL * 512 * 512 * 512);
    if (len % (4096UL * 512 * 512 * 512))
        y++;
    for (unsigned long i = 0; i < y; i++) {
        if (pml4t_vbase[(offset >> 39) + i] == 0) {
            pml4t_vbase[(offset >> 39) + i] = (unsigned long) alloc_pages(1) | (attr & 0x3F);
            memset(&pdptt_vbase[(offset >> 39 << 9) + i * 512], 0x0, 4096);
        }
    }

    y = len / (4096UL * 512 * 512);
    if (len % (4096UL * 512 * 512))
        y++;
    for (unsigned long i = 0; i < y; i++) {
        if (pdptt_vbase[(offset >> 30) + i] == 0) {
            pdptt_vbase[(offset >> 30) + i] = (unsigned long) alloc_pages(1) | (attr & 0x3F);
            memset(&pdt_vbase[(offset >> 30 << 9) + i * 512], 0x0, 4096);
        }
    }

    y = len / (4096UL * 512);
    if (len % (4096UL * 512))
        y++;
    for (unsigned long i = 0; i < y; i++) {

        if (pdt_vbase[(offset >> 21) + i] == 0) {
            pdt_vbase[(offset >> 21) + i] = (unsigned long) alloc_pages(1) | (attr & 0x3F);
            memset(&ptt_vbase[(offset >> 21 << 9) + i * 512], 0x0, 4096);
        }
    }

    y = len / 4096;
    if (len % 4096)
        y++;
    for (unsigned long i = 0; i < y; i++) {

        if (ptt_vbase[(offset >> 12) + i] == 0)
            ptt_vbase[(offset >> 12) + i] = paddr + i * 4096 | attr;
    }

    for (unsigned long i = 0; i < (len / 4096); i++) {
        FULSH_TLB_PAGE(addr + i * 4096);
    }

    return;
}


void umount_page(unsigned long addr, unsigned long len) {

    unsigned long y;
    unsigned long offset = addr & 0xFFFFFFFFFFFFUL;

    y = len / 4096;
    if (len % 4096)
        y++;
    for (unsigned long i = 0; i < y; i++) {
        ptt_vbase[(offset >> 12) + i] = 0;
    }

    for (unsigned long i = 0; i < (len / 4096); i++) {
        FULSH_TLB_PAGE(addr + i * 4096);
    }

    y = len / (4096UL * 512);
    if (len % (4096UL * 512))
        y++;
    for (unsigned long i = 0; i < y; i++) {
        free_pages((void*)(pdt_vbase[(offset >> 21) + i] & PAGE_4K_MASK), 1);
    }

    y = len / (4096UL * 512 * 512);
    if (len % (4096UL * 512 * 512))
        y++;
    for (unsigned long i = 0; i < y; i++) {
        free_pages((void *)(pdptt_vbase[(offset >> 30) + i] & PAGE_4K_MASK), 1);
    }

    y = len / (4096UL * 512 * 512 * 512);
    if (len % (4096UL * 512 * 512 * 512))
        y++;
    for (unsigned long i = 0; i < y; i++) {
        free_pages((void *)(pml4t_vbase[(offset >> 39) + i] & PAGE_4K_MASK), 1);
    }


    return;
}
