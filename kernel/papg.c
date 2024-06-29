#include "papg.h"

__attribute__((section(".init_text"))) void papg_init(unsigned char bsp_flags) {

    unsigned long addr = 0;
    if (bsp_flags) {
        unsigned long pml4e_num = 0;
        unsigned long pdpte_num = 0;
        unsigned long pde_num = 0;
        unsigned long pte_num = 0;
        unsigned long pml4e_pbaseaddr = 0;
        unsigned long pml4_bak[256]={0};

        pte_num = Virt_To_Phy(memory_management_struct.kernel_end) / 4096;     //计算内核总pte数量
        if (Virt_To_Phy(memory_management_struct.kernel_end) % 4096)
            pte_num++;

        pde_num = pte_num/512;                //计算内核pde数量
        if (pte_num % 512)
            pde_num++;

        pdpte_num = pde_num/512;              //计算内核pdpte数量
        if (pde_num % 512)
            pdpte_num++;

        pml4e_num = pdpte_num/512;             //计算内核pml4e数量
        if (pdpte_num % 512)
            pml4e_num++;

        pml4e_pbaseaddr = (unsigned long)alloc_pages(pml4e_num + pdpte_num + pde_num + pte_num);   //申请内核pml4e、pdpte、pde、pte页表空间

        addr = (unsigned long)pml4e_pbaseaddr;

        for(unsigned int i=0;i<pml4e_num;i++){
            pml4_bak[i] = *(unsigned long*)(PML4_VBASE+i*sizeof(unsigned long));  //备份原PML4E
            *(unsigned long*)(PML4_VBASE+i*sizeof(unsigned long)) = addr | 0x7;  //修改PML4E
            addr += 4096;
        }

        for(unsigned int i=0;i<pdpte_num;i++){
            *(unsigned long*)(PDPT_VBASE+i*sizeof(unsigned long)) = addr | 0x7;  //修改PDPTE
            addr += 4096;
        }

        for(unsigned int i=0;i<pde_num;i++){
            *(unsigned long*)(PD_VBASE+i*sizeof(unsigned long)) = addr | 0x3;  //修改PDE
            addr += 4096;
        }

        for(unsigned long i=0;i<pte_num;i++){
            *(unsigned long*)(PT_VBASE+i*sizeof(unsigned long)) = i * 0x1000 | 0x83;  //修改PTE
        }

        for(unsigned int i=0;i<pml4e_num;i++){
            *(unsigned long*)(PML4_VBASE+i*sizeof(unsigned long)) = pml4_bak[i];  //还原PML4E
            __PML4T[i+256] = pml4e_pbaseaddr+i*4096;                              //修改正式内核PML4T
        }


        color_printk(ORANGE, BLACK, "PML4E: %ld  PDPTE: %ld  PDE: %ld  PTE: %ld  pml4e_pbase: %#018lX\n",pml4e_num,pdpte_num,pde_num,pte_num,pml4e_pbaseaddr);
        color_printk(ORANGE, BLACK, "OS Can Used Total 4K PAGEs: %ld \tAlloc: %ld \tFree: %ld\n",
                     memory_management_struct.total_pages, memory_management_struct.alloc_pages,
                     memory_management_struct.free_pages);
        color_printk(ORANGE, BLACK, "OS StartAddr: %#018lX \tEndAddr: %#018lX \n",
                     memory_management_struct.kernel_start,memory_management_struct.kernel_end);
    }

//    addr = Virt_To_Phy(&__PML4T);
//    __asm__ __volatile__(
//            "mov    %%rax,%%cr3 \n\t"
//            ::"a"(addr):);

    return;
}