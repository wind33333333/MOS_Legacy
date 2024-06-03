#include "pos_init.h"

//显存起始地址由loader调用int10保存在0x600+0x28地址处
__attribute__((section(".init_text"))) void pos_init(void) {

    if (bsp_flags) {
        Pos.XResolution = 1440;
        Pos.YResolution = 900;

        Pos.XPosition = 0;
        Pos.YPosition = 0;

        Pos.XCharSize = 8;
        Pos.YCharSize = 16;

        Pos.FB_addr = *(unsigned int *)0x628;
        Pos.FB_length = (Pos.XResolution * Pos.YResolution * 4 + PAGE_4K_SIZE - 1) & PAGE_4K_MASK;
        Pos.lock = 0;
    }
    return;
}