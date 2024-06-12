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
    char Signature[4];                      // 必须为 "RSDT"
    unsigned int Length;                     // 表的长度，包括表头
    unsigned char Revision;                  // 表的修订版本
    unsigned char Checksum;                 // 检验和
    char OEMID[6];                          // OEM ID
    char OEMTableID[8];                     // OEM表ID
    unsigned int OEMRevision;               // OEM表修订版
    unsigned int CreatorID;                 // 表的创建者ID
    unsigned int CreatorRevision;           // 表的创建者修订版
    unsigned int Entry[];                   // ACPI表指针数组（RSDT为32位指针，XSDT为64位指针）
} __attribute__((packed)) rsdt_struct;

// 定义XSDT结构
typedef struct {
    char Signature[4];                  // 必须为 "XSDT"
    unsigned int Length;                // 表的长度，包括表头
    unsigned char Revision;             // 表的修订版本
    unsigned char Checksum;             // 检验和
    char OEMID[6];                      // OEM ID
    char OEMTableID[8];                 // OEM表ID
    unsigned int OEMRevision;           // OEM表修订版
    unsigned int CreatorID;             // 表的创建者ID
    unsigned int CreatorRevision;       // 表的创建者修订版
    unsigned long Entry[];              // ACPI表指针数组（64位指针）
} __attribute__((packed)) xsdt_struct;

#endif