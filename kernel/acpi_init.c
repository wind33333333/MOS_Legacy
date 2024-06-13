#include "acpi_init.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wint-to-pointer-cast"

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
            switch (*(unsigned int *) rsdt->Entry[i]) {
                case 0x43495041:        //"APIC"
                    madt = (MADT *) rsdt->Entry[i];
                    color_printk(YELLOW, BLACK, "ACPI: %#018lX  ", madt);
                    break;
                case 0x54455048:        //"HPET"
                    hpet = (HPET *) rsdt->Entry[i];
                    hpet_baseaddr = (unsigned int *) hpet->BaseAddressUpper;
                    color_printk(YELLOW, BLACK, "HPET: %#018lX  \n", hpet);
                    break;
            }
        }

        for (unsigned int i = 0; i < (madt->Length - 44) / 8; i++) {
            if (madt->ioapic[i].type == 1) {
                ioapic_baseaddr = (unsigned int *) madt->ioapic[i].ioapic_address;
                color_printk(YELLOW, BLACK, "IOAPIC ADDR: %#018lX  HPET ADDR: %#018lX\n",
                             ioapic_baseaddr, hpet_baseaddr);
            }

        }
    }

    return;
}

#pragma clang diagnostic pop