#include "idt_init.h"
#include "trap.h"

__attribute__((section(".init_text"))) void idt_init(unsigned long intfnuc) {
    if (bsp_flags) {
        for (int i = 0; i < 256; i++) {
            SET_GATE(IDT_Table, i, ignore_int, IST_1, TYPE_INT);
        }

        SET_GATE(IDT_Table,0,divide_error,IST_1,TYPE_TRAP);

    }

    __asm__ __volatile__(
            "lidt (%0)  \n\t"
            ::"r"(&IDT_POINTER):);
    return;
}