#include "hpet.h"

void hpet_init(void) {

    if (bsp_flags) {
        unsigned long i = *hpet_baseaddr;
        color_printk(YELLOW, BLACK, "HPET Clock Frequency: %dMhz \n", (i >> 32) / 1000 / 1000);
        return;
    }

}