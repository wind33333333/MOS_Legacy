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
            switch (*(unsigned int *)rsdt->Entry[i]) {
                case 0x43495041:        //"APIC"
                    madt = (MADT *)rsdt->Entry[i];
                    color_printk(YELLOW, BLACK, "ACPI: %#018lX  ", madt);
                    break;
                case 0x54455048:   //"HPET"
                    hpet = (HPET *)rsdt->Entry[i];
                    color_printk(YELLOW, BLACK, "HPET: %#018lX  \n", hpet);
                    break;
            }


        }

    }
    return;
}