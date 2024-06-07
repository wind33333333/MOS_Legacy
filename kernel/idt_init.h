#ifndef __IDT_H__
#define __IDT_H__
#include "lib.h"

struct idt_desc_struct {
    unsigned short offset_low;  // Offset bits 0-15
    unsigned short selector;    // Selector
    unsigned char ist;         // This will always be zero
    unsigned char type_attr;    // Type and attributes
    unsigned short offset_mid;  // Offset bits 16-31
    unsigned int offset_high;   // Offset bits 32-63
    unsigned int zero;         // This will always be zero
}__attribute__((packed));

extern struct idt_desc_struct IDT_Table[256];

extern void ignore_int(void);

extern int IDT_POINTER;

void idt_init(unsigned long intfnuc);

#endif