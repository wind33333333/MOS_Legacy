#include "gdt_init.h"

__attribute__((section(".init_text"))) void gdt_init(unsigned int bsp_flags) {
    if (bsp_flags) {
gdt.length=0;
gdt.base=0;

    }
    return;
}