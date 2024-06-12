#include "acpi_init.h"

__attribute__((section(".init_text"))) void acpi_init(void){
    if(bsp_flags) {
        rsdt_struct* rsdt;

        for (rsdp_struct *rsdp = (rsdp_struct *) 0xe0000; rsdp < 0x100000; rsdp = (unsigned long*)rsdp+2) {
            if (*(unsigned long*)rsdp == 0x2052545020445352) {  //'RSD PTR '
                rsdt = rsdp->RsdtAddress;
                color_printk(YELLOW, BLACK, "RSDP: %#018lX  RSDT: %#018lX  ", rsdp,
                            rsdt);
                break;
            }
        }

        for(unsigned int i =0;i < (rsdt->Length - 36)/4;i++){
            unsigned int *entry = (unsigned int *)rsdt->Entry[i];
            if(*entry == 0x43495041){  //"ACPI"

                color_printk(YELLOW, BLACK, "ACPI: %#018lX  ", entry);

            }else if(*entry == 0x54455048){  //"HPET"

                color_printk(YELLOW, BLACK, "HPET: %#018lX  \n", entry);
            }
        }

    }
    return;
}