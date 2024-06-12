#include "acpi_init.h"

__attribute__((section(".init_text"))) void acpi_init(void) {
    if (bsp_flags) {
        RSDT *rsdt;
        MADT *madt;
        HPET *hpet;

        for (RSDP *rsdp = (RSDP *) 0xe0000; rsdp < 0x100000; rsdp = (unsigned long *) rsdp + 2) {
            if (*(unsigned long *) rsdp == 0x2052545020445352) {  //'RSD PTR '
                rsdt = (RSDT *) rsdp->RsdtAddress;
                color_printk(YELLOW, BLACK, "RSDP: %#018lX  RSDT: %#018lX  ", rsdp,
                             rsdt);
                break;
            }
        }

        for (unsigned int i = 0; i < (rsdt->Length - 36) / 4; i++) {
            unsigned int *entry = (unsigned int *) rsdt->Entry[i];
            if (*entry == 0x43495041) {  //"ACPI"
                madt = (MADT *) entry;
                color_printk(YELLOW, BLACK, "ACPI: %#018lX  ", madt);

            } else if (*entry == 0x54455048) {  //"HPET"
                hpet = (HPET *) entry;
                color_printk(YELLOW, BLACK, "HPET: %#018lX  \n", hpet);
            }
        }

    }
    return;
}