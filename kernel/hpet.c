#include "hpet.h"

void hpet_init(void) {

    if (bsp_flags) {

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

        color_printk(YELLOW, BLACK, "HPET: %#018lX \n", *hpetRegisters.MAIN_CNT);
        color_printk(YELLOW, BLACK, "HPET Clock Frequency: %dMhz \n", (*hpetRegisters.GCAP_ID >> 32) / 1000 / 1000);
        return;
    }

}