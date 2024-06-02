#include "memory.h"
#include "printk.h"


__attribute__((section(".init_text"))) void memory_init(unsigned int bsp_flags) {

    if (bsp_flags) {
        unsigned int x = 0;
        unsigned long totalmem = 0;
        struct E820 *p = (struct E820 *) E820_BASE;
        for (unsigned int i = 0; i < *(unsigned int *) E820_SIZE; i++) {
            color_printk(ORANGE, BLACK, "Addr: %#018lX\t Len: %#018lX\t Type: %d\n", p->address,
                         p->length, p->type);
            if (p->type == 1) {
                memory_management_struct.e820[x].address = p->address & PAGE_4K_MASK;
                memory_management_struct.e820[x].length = p->length & PAGE_4K_MASK;
                memory_management_struct.e820[x].type = p->type;
                memory_management_struct.e820_length++;
                totalmem += p->length;
                memory_management_struct.total_pages +=
                        memory_management_struct.e820[x].length >> PAGE_4K_SHIFT;
                x++;
            }
            p++;
        }
        color_printk(ORANGE, BLACK, "OS Can Used Total RAM: %#018lX=%ldMB\n", totalmem,
                     totalmem / 1024 / 1024);

        //bits map construction init
        totalmem = memory_management_struct.e820[memory_management_struct.e820_length - 1].address +
                   memory_management_struct.e820[memory_management_struct.e820_length - 1].length;
        memory_management_struct.bits_map = (unsigned long *) kernel_memend;
        memory_management_struct.bits_size = totalmem >> PAGE_4K_SHIFT;
        memory_management_struct.bits_length =
                (memory_management_struct.bits_size + 63) / 8 & 0xFFFFFFFFFFFFFFF8;
        memset(memory_management_struct.bits_map, 0xff, memory_management_struct.bits_length);

        //bit map 1M以上可用空间置0，i=1跳过1M保持使用置1，等全部初始化后再释放
        for (unsigned int i = 1; i < memory_management_struct.e820_length; i++) {
            memset(memory_management_struct.bits_map +
                   ((memory_management_struct.e820[i].address >> PAGE_4K_SHIFT) >> 6), 0,
                   (memory_management_struct.e820[i].length >> PAGE_4K_SHIFT) >> 3);
            totalmem = memory_management_struct.e820[i].address +
                       memory_management_struct.e820[i].length & 0xFFFFFFFFFFFF8000;
            for (; totalmem < (memory_management_struct.e820[i].address +
                               memory_management_struct.e820[i].length); totalmem += PAGE_4K_SIZE) {
                *(memory_management_struct.bits_map + (totalmem >> PAGE_4K_SHIFT >> 6)) ^=
                        1UL << (totalmem >> PAGE_4K_SHIFT) % 64;
            }
        }

        //kernel_end结束地址加上bit map对齐4K地址
        memory_management_struct.kernel_start = &_start_text;
        memory_management_struct.kernel_end =
                kernel_memend + (memory_management_struct.bits_length + 0xfff) & 0xFFFFFFFFFFFFF000;

        //把内核1M开始到kernel_end地址bit map置1，标记为已使用
        memset(memory_management_struct.bits_map + ((0x100000 >> PAGE_4K_SHIFT) >> 6), 0xFF,
               (Virt_To_Phy(memory_management_struct.kernel_end) - 0x100000) >> PAGE_4K_SHIFT >> 3);
        totalmem = Virt_To_Phy(memory_management_struct.kernel_end) & 0xFFFFFFFFFFFF8000;
        for (; totalmem <
               Virt_To_Phy(memory_management_struct.kernel_end); totalmem += PAGE_4K_SIZE) {
            *(memory_management_struct.bits_map + (totalmem >> PAGE_4K_SHIFT >> 6)) ^=
                    1UL << ((totalmem - 0x100000) >> PAGE_4K_SHIFT) % 64;
        }

        memory_management_struct.alloc_pages += (memory_management_struct.e820[0].length
                >> PAGE_4K_SHIFT);
        memory_management_struct.alloc_pages += (
                (Virt_To_Phy(memory_management_struct.kernel_end) - 0x100000) >> PAGE_4K_SHIFT);
        memory_management_struct.free_pages =
                memory_management_struct.total_pages - memory_management_struct.alloc_pages;

        color_printk(ORANGE, BLACK,
                     "bits_map: %#018lx \tbits_size: %#018lx \tbits_length: %#018lx\n",
                     memory_management_struct.bits_map, memory_management_struct.bits_size,
                     memory_management_struct.bits_length);
        color_printk(ORANGE, BLACK, "OS Can Used Total 4K PAGEs: %ld \tAlloc: %ld \tFree: %ld\n",
                     memory_management_struct.total_pages, memory_management_struct.alloc_pages,
                     memory_management_struct.free_pages);
        color_printk(ORANGE, BLACK, "Kernel Start Addr: %#018lX \tKernel End Addr: %#018lX\n",
                     memory_management_struct.kernel_start, memory_management_struct.kernel_end);

    }
    return;
}


////物理页分配器
void *alloc_pages(void) {
    SPIN_LOCK(memory_management_struct.lock);
    if (0 != memory_management_struct.free_pages) {
        void *page_addr;
        for (unsigned long i = 0; i < (memory_management_struct.bits_length >> 3); i++) {
            if (*(memory_management_struct.bits_map + i) != 0xFFFFFFFFFFFFFFFF) {
                for (unsigned long j = 0; j < 64; j++) {
                    if ((*(memory_management_struct.bits_map + i) & (1UL << j)) == 0) {
                        page_addr = (void *) ((i << 18) + (j << PAGE_4K_SHIFT));
                        *(memory_management_struct.bits_map + i) |= (1UL << j);
                        memory_management_struct.alloc_pages++;
                        memory_management_struct.free_pages--;
                        memory_management_struct.lock = 0;                                    //解锁
                        return page_addr;
                    }
                }
            }
        }
    }
    memory_management_struct.lock = 0;
    return (void *) -1;
}

///物理页释放器
unsigned long free_pages(void *page_addr) {
    SPIN_LOCK(memory_management_struct.lock);
    if (page_addr > (memory_management_struct.e820[memory_management_struct.e820_length-1].address+memory_management_struct.e820[memory_management_struct.e820_length-1].length))
        return -1;

    *(memory_management_struct.bits_map + ((unsigned long) page_addr >> PAGE_4K_SHIFT >> 6)) ^=
            1UL << ((unsigned long) page_addr >> PAGE_4K_SHIFT) % 64;
    memory_management_struct.alloc_pages--;
    memory_management_struct.free_pages++;
    return 0;
}