#ifndef __ACPI_H__
#define __ACPI_H__

void apic_init(void);

struct cpu_info {
    unsigned char name[17];
};

#endif
