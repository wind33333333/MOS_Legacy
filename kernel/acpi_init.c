#include "acpi_init.h"

__attribute__((section(".init_text"))) void acpi_init(void) {
    if (bsp_flags) {
        RSDP *rsdp = (RSDP *) 0xe0000;
        RSDT *rsdt = (RSDT *) 0;
        MADT *madt = (MADT *) 0;
        HPET *hpet = (HPET *) 0;

        for (; rsdp < 0x100000; rsdp = (unsigned long *) rsdp + 2) {
            if (*(unsigned long *) rsdp == 0x2052545020445352) {  //'RSD PTR '
                rsdt = (RSDT *) rsdp->RsdtAddress;
                break;
            }
        }

        for (unsigned int i = 0; i < (rsdt->Length - 36) / 4; i++) {
            switch (*(unsigned int *) rsdt->Entry[i]) {
                case 0x43495041:        //"APIC"
                    madt = (MADT *) rsdt->Entry[i];
                    break;
                case 0x54455048:        //"HPET"
                    hpet = (HPET *) rsdt->Entry[i];
                    hpet_baseaddr = (unsigned int *) hpet->BaseAddressUpper;
                    break;
            }
        }

        for (unsigned int i = 0; i < (madt->Length - 44) / 8; i++) {
            if (madt->ioapic[i].type == 1) {
                ioapic_baseaddr = (unsigned int *) madt->ioapic[i].ioapic_address;
            }
        }

        color_printk(YELLOW, BLACK, "RSDP: %#018lX \tRSDT: %#018lX\n", rsdp,
                     rsdt);
        color_printk(YELLOW, BLACK, "APIC: %#018lX \tIOAPIC ADDR: %#018lX\n", madt,
                     ioapic_baseaddr);
        color_printk(YELLOW, BLACK, "HPET: %#018lX \tHPET ADDR: %#018lX\n", hpet, hpet_baseaddr);
    }

    return;
}

#pragma clang diagnostic pop