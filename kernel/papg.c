#include "papg.h"

__attribute__((section(".init_text"))) void papg_init(unsigned char bsp_flags) {

    unsigned long addr = 0;

    if (bsp_flags) {
        unsigned long pml4_bak[256] = {0};
        unsigned long pml4e_num = Virt_To_Phy(memory_management_struct.kernel_end) / (4096UL * 512 * 512 * 512);
        if(Virt_To_Phy(memory_management_struct.kernel_end) % (4096UL * 512 * 512 * 512))
            pml4e_num++;

        for (unsigned int i = 0; i < pml4e_num; i++) {
            pml4_bak[i] = upml4t_vbase[i];  //备份原PML4E
            upml4t_vbase[i] = 0x0UL;        //清除PML4E
        }

        user_mount_page(0, Virt_To_Phy(memory_management_struct.kernel_end));

        for (unsigned int i = 0; i < pml4e_num; i++) {
            //   __PML4T[i] = upml4t_vbase[i];            //修改正式内核PML4T 低
            __PML4T[i + 256] = upml4t_vbase[i];        //修改正式内核PML4T 高
            upml4t_vbase[i] = pml4_bak[i];           //还原PML4E
        }

/*

        unsigned long pml4e_num = 0;
        unsigned long pdpte_num = 0;
        unsigned long pde_num = 0;
        unsigned long pte_num = 0;
        unsigned long pml4e_pbaseaddr = 0;
        unsigned long pml4_bak[256] = {0};

        pte_num = Virt_To_Phy(memory_management_struct.kernel_end) / 4096;     //计算内核总pte数量
        if (Virt_To_Phy(memory_management_struct.kernel_end) % 4096)
            pte_num++;

        pde_num = pte_num / 512;                //计算内核pde数量
        if (pte_num % 512)
            pde_num++;

        pdpte_num = pde_num / 512;              //计算内核pdpte数量
        if (pde_num % 512)
            pdpte_num++;

        pml4e_num = pdpte_num / 512;             //计算内核pml4e数量
        if (pdpte_num % 512)
            pml4e_num++;

        pml4e_pbaseaddr = (unsigned long) alloc_pages(
                pml4e_num + pdpte_num + pde_num);   //申请内核pml4e、pdpte、pde、pte页表空间

        addr = (unsigned long) pml4e_pbaseaddr;

        for (unsigned int i = 0; i < pml4e_num; i++) {
            pml4_bak[i] = upml4t_vbase[i];  //备份原PML4E
            upml4t_vbase[i] = addr | 0x7;  //修改PML4E
            addr += 4096;
        }

        for (unsigned int i = 0; i < pdpte_num; i++) {
            updptt_vbase[i] = addr | 0x7;  //修改PDPTE
            addr += 4096;
        }

        for (unsigned int i = 0; i < pde_num; i++) {
            updt_vbase[i] = addr | 0x3;  //修改PDE
            addr += 4096;
        }

        for (unsigned long i = 0; i < pte_num; i++) {
            uptt_vbase[i] = i * 0x1000 | 0x83;  //修改PTE
        }

        for (unsigned int i = 0; i < pml4e_num; i++) {
            //   __PML4T[i] = upml4t_vbase[i];            //修改正式内核PML4T 低
            __PML4T[i + 256] = upml4t_vbase[i];        //修改正式内核PML4T 高
            upml4t_vbase[i] = pml4_bak[i];           //还原PML4E
        }
*/


/*        color_printk(ORANGE, BLACK,
                     "PML4E: %ld  PDPTE: %ld  PDE: %ld  PTE: %ld  pml4e_pbase: %#018lX\n",
                     pml4e_num, pdpte_num, pde_num, pte_num, pml4e_pbaseaddr);*/
        color_printk(ORANGE, BLACK, "OS Can Used Total 4K PAGEs: %ld \tAlloc: %ld \tFree: %ld\n",
                     memory_management_struct.total_pages, memory_management_struct.alloc_pages,
                     memory_management_struct.free_pages);
        color_printk(ORANGE, BLACK, "OS StartAddr: %#018lX \tEndAddr: %#018lX \n",
                     memory_management_struct.kernel_start, memory_management_struct.kernel_end);

        addr = Virt_To_Phy(&__PML4T);
        __asm__ __volatile__(
                "mov    %%rax,%%cr3 \n\t"
                ::"a"(addr):);

        kernel_mount_page(Pos.FB_addr, Pos.FB_length);

    }

    addr = Virt_To_Phy(&__PML4T);
    __asm__ __volatile__(
            "mov    %%rax,%%cr3 \n\t"
            ::"a"(addr):);

    return;
}

void kernel_mount_page(unsigned long phy_addr, unsigned long phy_len) {

    unsigned long y;
    unsigned long addr = Virt_To_Phy(phy_addr);

    y = phy_len / (4096UL * 512 * 512 * 512);
    if (phy_len % (4096UL * 512 * 512 * 512))
        y++;
    for (unsigned long i = 0; i < y; i++) {
        if (kpml4t_vbase[(addr >> 39) + i] == 0) {
            kpml4t_vbase[(addr >> 39) + i] = (unsigned long) alloc_pages(1) | 0x7;
            memset(&kpdptt_vbase[(addr >> 30) + i * 512],0x0,4096);
        }
    }

    y = phy_len / (4096UL * 512 * 512);
    if (phy_len % (4096UL * 512 * 512))
        y++;
    for (unsigned long i = 0; i < y; i++) {
        if (kpdptt_vbase[(addr >> 30) + i] == 0) {
            kpdptt_vbase[(addr >> 30) + i] = (unsigned long) alloc_pages(1) | 0x7;
            memset(&kpdt_vbase[(addr >> 30 <<9) + i * 512],0x0,4096);
        }
    }

    y = phy_len / (4096UL * 512);
    if (phy_len % (4096UL * 512))
        y++;
    for (unsigned long i = 0; i < y; i++) {

        if (kpdt_vbase[(addr >> 21) + i] == 0) {
            kpdt_vbase[(addr >> 21) + i] = (unsigned long) alloc_pages(1) | 0x3;
            memset(&kptt_vbase[(addr >> 21 << 9) + i * 512],0x0,4096);
        }
    }

    y = phy_len / 4096;
    if (phy_len % 4096)
        y++;
    for (unsigned long i = 0; i < y; i++) {

        if (kptt_vbase[(addr >> 12) + i] == 0)
            kptt_vbase[(addr >> 12) + i] = addr + i * 4096 | 0x83;
    }

    return;
}


void user_mount_page(unsigned long phy_addr, unsigned long phy_len) {

    unsigned long y;
    unsigned long addr = Virt_To_Phy(phy_addr);

    y = phy_len / (4096UL * 512 * 512 * 512);
    if (phy_len % (4096UL * 512 * 512 * 512))
        y++;
    for (unsigned long i = 0; i < y; i++) {
        if (upml4t_vbase[(addr >> 39) + i] == 0) {
            upml4t_vbase[(addr >> 39) + i] = (unsigned long) alloc_pages(1) | 0x7;
            memset(&updptt_vbase[(addr >> 30) + i * 512],0x0,4096);
        }
    }

    y = phy_len / (4096UL * 512 * 512);
    if (phy_len % (4096UL * 512 * 512))
        y++;
    for (unsigned long i = 0; i < y; i++) {
        if (updptt_vbase[(addr >> 30) + i] == 0) {
            updptt_vbase[(addr >> 30) + i] = (unsigned long) alloc_pages(1) | 0x7;
            memset(&updt_vbase[(addr >> 30 <<9) + i * 512],0x0,4096);
        }
    }

    y = phy_len / (4096UL * 512);
    if (phy_len % (4096UL * 512))
        y++;
    for (unsigned long i = 0; i < y; i++) {

        if (updt_vbase[(addr >> 21) + i] == 0) {
            updt_vbase[(addr >> 21) + i] = (unsigned long) alloc_pages(1) | 0x3;
            memset(&uptt_vbase[(addr >> 21 << 9) + i * 512],0x0,4096);
        }
    }

    y = phy_len / 4096;
    if (phy_len % 4096)
        y++;
    for (unsigned long i = 0; i < y; i++) {

        if (uptt_vbase[(addr >> 12) + i] == 0)
            uptt_vbase[(addr >> 12) + i] = addr + i * 4096 | 0x83;
    }

    return;
}


