#include "memory.h"
#include "printk.h"

void memory_init(unsigned int bsp_flags) {

    if (bsp_flags) {
        unsigned int x = 0;
        unsigned long totalmem = 0;
        unsigned long page4knum = 0;
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
                page4knum += memory_management_struct.e820[x].length;
                x++;
            }
            p++;
        }
        color_printk(ORANGE, BLACK, "OS Can Used Total RAM: %#018lX=%ldMB\n", totalmem,
                     totalmem / 1024 / 1024);
        color_printk(ORANGE, BLACK, "OS Can Used Total 4K PAGEs: %#018lX=%ld\n",
                     page4knum >> PAGE_4K_SHIFT, page4knum >> PAGE_4K_SHIFT);

        //bits map construction init
        totalmem = memory_management_struct.e820[memory_management_struct.e820_length - 1].address +
                   memory_management_struct.e820[memory_management_struct.e820_length - 1].length;
        memory_management_struct.bits_map = (unsigned long *) kernel_memend;
        memory_management_struct.bits_size = totalmem >> PAGE_4K_SHIFT;
        memory_management_struct.bits_length =
                (memory_management_struct.bits_size + 63) / 8 & 0xFFFFFFFFFFFFFFF8;
//      memory_management_struct.end_brk=kernel_memend+(memory_management_struct.bits_length+0xfff)&0xFFFFFFFFFFFFF000;
        memset(memory_management_struct.bits_map, 0xff, memory_management_struct.bits_length);

        //pages construction init
        memory_management_struct.pages_struct = (struct Page *) (
                ((unsigned long) memory_management_struct.bits_map +
                 memory_management_struct.bits_length + PAGE_4K_SIZE - 1) & PAGE_4K_MASK);
        memory_management_struct.pages_size = totalmem >> PAGE_4K_SHIFT;
        memory_management_struct.pages_length =
                ((totalmem >> PAGE_4K_SHIFT) * sizeof(struct Page) + sizeof(long) - 1) &
                (~(sizeof(long) - 1));
        memset(memory_management_struct.pages_struct, 0x00,
               memory_management_struct.pages_length);    //init pages memory

        //zones construction init
        memory_management_struct.zones_struct = (struct Zone *) (
                ((unsigned long) memory_management_struct.pages_struct +
                 memory_management_struct.pages_length + PAGE_4K_SIZE - 1) & PAGE_4K_MASK);
        memory_management_struct.zones_size = 0;
        memory_management_struct.zones_length =
                (5 * sizeof(struct Zone) + sizeof(long) - 1) & (~(sizeof(long) - 1));
        memset(memory_management_struct.zones_struct, 0x00,
               memory_management_struct.zones_length);    //init zones memory

        for (unsigned int i = 0; i < memory_management_struct.e820_length; i++) {
            struct Zone *z;
            struct Page *p;

            //zone init
            z = memory_management_struct.zones_struct + memory_management_struct.zones_size;
            memory_management_struct.zones_size++;

            z->zone_start_address = memory_management_struct.e820[i].address;
            z->zone_end_address = memory_management_struct.e820[i].address +
                                  memory_management_struct.e820[i].length;
            z->zone_length = memory_management_struct.e820[i].length;
            z->page_using_count = 0;
            z->page_free_count = memory_management_struct.e820[i].length >> PAGE_4K_SHIFT;
            z->total_pages_link = 0;
            z->attribute = 0;
            z->GMD_struct = &memory_management_struct;

            z->pages_length = memory_management_struct.e820[i].length >> PAGE_4K_SHIFT;
            z->pages_group = (struct Page *) (memory_management_struct.pages_struct +
                                              (memory_management_struct.e820[i].address
                                                      >> PAGE_2M_SHIFT));

            //page init
            p = z->pages_group;
            for (unsigned int j = 0; j < z->pages_length; j++, p++) {
                p->zone_struct = z;
                p->PHY_address = memory_management_struct.e820[i].address + PAGE_4K_SIZE * j;
                p->attribute = 0;
                p->reference_count = 0;
                p->age = 0;

                *(memory_management_struct.bits_map + ((p->PHY_address >> PAGE_4K_SHIFT) >> 6)) ^=
                        1UL << (p->PHY_address >> PAGE_4K_SHIFT) % 64;
            }
        }
        memory_management_struct.zones_length =
                (memory_management_struct.zones_size * sizeof(struct Zone) + sizeof(long) - 1) &
                (~(sizeof(long) - 1));
        color_printk(ORANGE, BLACK, "bits_map:%#018lx \tbits_size:%#018lx \tbits_length:%#018lx\n",
                     memory_management_struct.bits_map, memory_management_struct.bits_size,
                     memory_management_struct.bits_length);
        color_printk(ORANGE, BLACK,
                     "pages_struct:%#018lx \tpages_size:%#018lx \tpages_length:%#018lx\n",
                     memory_management_struct.pages_struct, memory_management_struct.pages_size,
                     memory_management_struct.pages_length);
        color_printk(ORANGE, BLACK,
                     "zones_struct:%#018lx \tzones_size:%#018lx \tzones_length:%#018lx\n",
                     memory_management_struct.zones_struct, memory_management_struct.zones_size,
                     memory_management_struct.zones_length);

    }
    return;
}