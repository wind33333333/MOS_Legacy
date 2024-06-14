#include "hpet.h"

void hpet_init(void) {

    if (bsp_flags) {

        __asm__ __volatile__ (
                "mov    $0x34,%%al \n\t"
                "out    %%al,$0x43 \n\t"

                "mov    $0,%%al \n\t"
                "out    %%al,$0x40 \n\t"
                "out    %%al,$0x40 \n\t"
                "out    %%al,$0x41 \n\t"
                "out    %%al,$0x41 \n\t"
                "out    %%al,$0x42 \n\t"
                "out    %%al,$0x42 \n\t"
                :::"%rax");

        HPET_Registers hpetRegisters = {
                .GCAP_ID = (unsigned long *)Phy_To_Virt(hpet_baseaddr+0),
                .GEN_CONF = (unsigned long*)Phy_To_Virt(hpet_baseaddr+0x10),
                .GINTR_STA = (unsigned long *)Phy_To_Virt(hpet_baseaddr+0x20),
                .MAIN_CNT = (unsigned long *)Phy_To_Virt(hpet_baseaddr+0xF0),
                .TIM0_CONF = (unsigned long *)Phy_To_Virt(hpet_baseaddr+0x100),
                .TIM0_COMP = (unsigned long *)Phy_To_Virt(hpet_baseaddr+0x108),
                .TIM1_CONF = (unsigned long *)Phy_To_Virt(hpet_baseaddr+0x120),
                .TIM1_COMP = (unsigned long *)Phy_To_Virt(hpet_baseaddr+0x128),
                .TIM2_CONF = (unsigned long *)Phy_To_Virt(hpet_baseaddr+0x140),
                .TIM2_COMP = (unsigned long *)Phy_To_Virt(hpet_baseaddr+0x148),
                .TIM3_CONF = (unsigned long *)Phy_To_Virt(hpet_baseaddr+0x160),
                .TIM3_COMP = (unsigned long *)Phy_To_Virt(hpet_baseaddr+0x168),
                .TIM4_CONF = (unsigned long *)Phy_To_Virt(hpet_baseaddr+0x180),
                .TIM4_COMP = (unsigned long *)Phy_To_Virt(hpet_baseaddr+0x188),
                .TIM5_CONF = (unsigned long *)Phy_To_Virt(hpet_baseaddr+0x1A0),
                .TIM5_COMP = (unsigned long *)Phy_To_Virt(hpet_baseaddr+0x1A8),
                .TIM6_CONF = (unsigned long *)Phy_To_Virt(hpet_baseaddr+0x1C0),
                .TIM6_COMP = (unsigned long *)Phy_To_Virt(hpet_baseaddr+0x1C8),
                .TIM7_CONF = (unsigned long *)Phy_To_Virt(hpet_baseaddr+0x1E0),
                .TIM7_COMP = (unsigned long *)Phy_To_Virt(hpet_baseaddr+0x1E8),
                };

        color_printk(YELLOW, BLACK, "HPET Clock Frequency: %dMhz \n", (*hpetRegisters.GCAP_ID >> 32) / 1000 / 1000);

        *hpetRegisters.TIM0_COMP = 0xFFFFFFF;
        *hpetRegisters.TIM0_CONF = ((2UL << 9) | (1UL << 6) | (1Ul << 3) | (1UL << 2));
        *hpetRegisters.GEN_CONF = 0;



    }
    return;
}