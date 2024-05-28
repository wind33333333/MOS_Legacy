#include "memory.h"
#include "printk.h"

void memory_init(unsigned int bsp_flags) {

    if (bsp_flags) {
        unsigned int x=0;
        unsigned long totalmem = 0;
        unsigned long page4knum = 0;
        struct E820 *p = (struct E820 *) E820_BASE;
        for (unsigned int i = 0; i < *(unsigned int *) E820_SIZE; i++) {
            color_printk(YELLOW, BLACK, "Addr: %#018lX\t Len: %#018lX\t Type: %d\n", p->address,
                         p->length, p->type);
            if (p->type == 1) {
                memory_management_struct.e820[x].address = p->address&PAGE_4K_MASK;
                memory_management_struct.e820[x].length = p->length&PAGE_4K_MASK;
                memory_management_struct.e820[x].type = p->type;
                memory_management_struct.e820_length++;
                totalmem += p->length;
                page4knum += memory_management_struct.e820[x].length;
                x++;
            }
            p++;
        }
        color_printk(ORANGE,BLACK,"OS Can Used Total RAM: %#018lX=%ldMB\n",totalmem,totalmem/1024/1024);
        color_printk(ORANGE,BLACK, "OS Can Used Total 4K PAGEs: %#018lX=%ld\n", page4knum>>PAGE_4K_SHIFT,page4knum>>PAGE_4K_SHIFT);

        //bits map construction init
        totalmem = memory_management_struct.e820[memory_management_struct.e820_length-1].address + memory_management_struct.e820[memory_management_struct.e820_length-1].length;
        memory_management_struct.bits_map=(unsigned long*)kernel_memend;
        memory_management_struct.bits_size=totalmem>>PAGE_4K_SHIFT;
        memory_management_struct.bits_length=(memory_management_struct.bits_size+63)/8&0xFFFFFFFFFFFFFFF8;
        memory_management_struct.end_brk=kernel_memend+(memory_management_struct.bits_length+0xfff)&0xFFFFFFFFFFFFF000;
        memset(memory_management_struct.bits_map,0xff,memory_management_struct.bits_length);

    }
    return;
}