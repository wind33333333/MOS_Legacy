#include "printk.h"
void acpi_init(unsigned int bsp_flags){
    if(bsp_flags) {
        unsigned long rsdpstr = 0x2052545020445352;   //'RSD PTR '
        unsigned long *prsdp = (long *) 0xe0000;
        for (; prsdp < (unsigned long *) 0x100000; prsdp += 2) {
            if (*prsdp == rsdpstr) {
                color_printk(YELLOW, BLACK, "RSDP: %#018lX\t RSDT: %#018lX\t\n", prsdp,
                             *(int *) (prsdp + 2));
                break;

            }
        }
    }
    return;
}