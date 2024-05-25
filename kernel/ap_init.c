#include "ap_init.h"

//多核处理器初始化
void ap_init(unsigned int bsp_flags) {
    if(bsp_flags) {
        __asm__ __volatile__ (
                "mov $0x00,	%%rdx	\n\t"
                "mov $0xC4500,%%rax	\n\t"   //bit8-10投递模式init101 ，bit14 1 ，bit18-19投递目标11所有处理器（不包括自身）
                "mov $0x830,	%%rcx	\n\t"    //INIT IPI
                "wrmsr	\n\t"
                "mov $0x5000,%%rcx	\n\t"       //延时
                "1:\tloop 1b	\n\t"
                "mov $0x830,	%%rcx	\n\t"
                "mov $0xC4610,%%rax	\n\t"   //Start-up IPI //bit0-7处理器启动地址000VV000的中间两位 ，bit8-10投递模式start-up110 ，bit14 1 ，bit18-19投递目标11所有处理器（不包括自身）
                "wrmsr	\n\t"
                "mov $0x830,	%%rcx	\n\t"
                "mov $0x50000,%%rcx	\n\t"       //延时
                "2:\tloop 2b	\n\t"
                "wrmsr	\n\t"

                :: :"%rax", "%rcx", "%rdx");
    }
    return;
}