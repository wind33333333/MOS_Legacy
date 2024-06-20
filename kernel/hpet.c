#include "hpet.h"

void hpet_init(void) {

    if (bsp_flags) {

        hpetRegisters.GCAP_ID = (unsigned long *) Phy_To_Virt(hpet_baseaddr + 0);
        hpetRegisters.GEN_CONF = (unsigned long *) Phy_To_Virt(hpet_baseaddr + 0x10);
        hpetRegisters.GINTR_STA = (unsigned long *) Phy_To_Virt(hpet_baseaddr + 0x20);
        hpetRegisters.MAIN_CNT = (unsigned long *) Phy_To_Virt(hpet_baseaddr + 0xF0);
        hpetRegisters.TIM0_CONF = (unsigned long *) Phy_To_Virt(hpet_baseaddr + 0x100);
        hpetRegisters.TIM0_COMP = (unsigned long *) Phy_To_Virt(hpet_baseaddr + 0x108);
        hpetRegisters.TIM1_CONF = (unsigned long *) Phy_To_Virt(hpet_baseaddr + 0x120);
        hpetRegisters.TIM1_COMP = (unsigned long *) Phy_To_Virt(hpet_baseaddr + 0x128);
        hpetRegisters.TIM2_CONF = (unsigned long *) Phy_To_Virt(hpet_baseaddr + 0x140);
        hpetRegisters.TIM2_COMP = (unsigned long *) Phy_To_Virt(hpet_baseaddr + 0x148);
        hpetRegisters.TIM3_CONF = (unsigned long *) Phy_To_Virt(hpet_baseaddr + 0x160);
        hpetRegisters.TIM3_COMP = (unsigned long *) Phy_To_Virt(hpet_baseaddr + 0x168);
        hpetRegisters.TIM4_CONF = (unsigned long *) Phy_To_Virt(hpet_baseaddr + 0x180);
        hpetRegisters.TIM4_COMP = (unsigned long *) Phy_To_Virt(hpet_baseaddr + 0x188);
        hpetRegisters.TIM5_CONF = (unsigned long *) Phy_To_Virt(hpet_baseaddr + 0x1A0);
        hpetRegisters.TIM5_COMP = (unsigned long *) Phy_To_Virt(hpet_baseaddr + 0x1A8);
        hpetRegisters.TIM6_CONF = (unsigned long *) Phy_To_Virt(hpet_baseaddr + 0x1C0);
        hpetRegisters.TIM6_COMP = (unsigned long *) Phy_To_Virt(hpet_baseaddr + 0x1C8);
        hpetRegisters.TIM7_CONF = (unsigned long *) Phy_To_Virt(hpet_baseaddr + 0x1E0);
        hpetRegisters.TIM7_COMP = (unsigned long *) Phy_To_Virt(hpet_baseaddr + 0x1E8);

        color_printk(YELLOW, BLACK, "HPET Clock Frequency: %dMhz \n",
                     (*hpetRegisters.GCAP_ID >> 32) / 1000 / 1000);

        *hpetRegisters.GEN_CONF = 0;
        io_mfence();

        *hpetRegisters.TIM0_CONF = ((0UL << 9) | (1UL << 6) | (1Ul << 3) | (1UL << 2));
        io_mfence();

        *hpetRegisters.TIM0_COMP = 0xFFFFFFF;
        io_mfence();

        *hpetRegisters.MAIN_CNT = 0;
        io_mfence();


    }
    return;
}