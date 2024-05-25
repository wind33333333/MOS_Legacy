#include "memory.h"
#include "printk.h"

void memory_init(unsigned int bsp_flags) {

    if (bsp_flags) {
        unsigned totalmem = 0;
        unsigned start=0;
        unsigned long end=0;
        struct E820 *p = (struct E820 *) E820_BASE;
        for (unsigned int i = 0; i < *(unsigned int *) E820_SIZE; i++) {
            color_printk(YELLOW, BLACK, "Addr: %#018lX\t Len: %#018lX\t Type: %d\n", p->address,
                         p->length, p->type);
            if (p->type == 1) {
                start = p->address&PAGE_4K_MASK;
                end = start+(p->length&PAGE_4K_MASK);
                totalmem += (end-start)>>PAGE_4K_SHIFT;
            }
            p++;
        }
        color_printk(YELLOW, BLACK, "Total Memory: %#018lX\n", totalmem);
    }
    return;
}