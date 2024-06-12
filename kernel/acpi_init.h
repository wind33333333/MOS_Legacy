#ifndef __acpi_init__
#define __acpi_init__
#include "lib.h"
#include "printk.h"
#include "cpuinfo.h"

void acpi_init(void);

// 定义RSDP结构
typedef struct {
    char Signature[8];                   // 必须为 "RSD PTR "
    unsigned char Checksum;              // 检验和，确保整个表的字节和为0
    char OEMID[6];                       // OEM ID
    unsigned char Revision;              // ACPI版本号
    unsigned int RsdtAddress;           // RSDT的物理地址（32位）
    unsigned int Length;                // RSDP结构长度（仅在ACPI 2.0中使用）
    unsigned long XsdtAddress;           // XSDT的物理地址（仅在ACPI 2.0中使用）
    unsigned char ExtendedChecksum;      // 扩展校验和（仅在ACPI 2.0中使用）
    unsigned char Reserved[3];           // 保留，必须为0
} __attribute__((packed)) rsdp_struct;


// 定义RSDT结构
typedef struct {
    char Signature[4];
    unsigned int Length;
    unsigned char Revision;
    unsigned char Checksum;
    char OEMID[6];
    char OEMTableID[8];
    unsigned int OEMRevision;
    unsigned int CreatorID;
    unsigned int CreatorRevision;
    unsigned int Entry[];
} __attribute__((packed)) rsdt_struct;



#endif