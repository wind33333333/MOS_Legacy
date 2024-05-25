#include "idt_init.h"

void idt_init(unsigned int bsp_flags, unsigned long intfnuc) {
    if (bsp_flags) {
    struct idt_desc_struct *p_idt=IDT_Table;
        for (int i=0;i<256;i++) {
            p_idt[i].offset_low=intfnuc&0xFFFF;
            p_idt[i].selector=0x08;
            p_idt[i].ist=0;
            p_idt[i].type_attr=0x8E;
            p_idt[i].offset_mid=(intfnuc>>16)&0xFFFF;
            p_idt[i].offset_high=(intfnuc>>32);
            p_idt[i].zero=0;
        }
    }
    return;
}