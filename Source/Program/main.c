#include "includes.h"

void led(u16 old, u16 new)
{
    LED_Clear();
    const u8 led_index = new / 512; // (4096 / 8)
    LED_On(1 << led_index);
}

int main(void)
{
    SystemInit();
    LED_Init();

    RIT_Init(50, 2);
    RIT_Enable();

    ADC_PMInit(4, INT_PRIO_MAX);
    ADC_PMSetSampleReadyAction(led);

    POWER_Init(POWR_CFG_SLEEP_ON_EXIT);
    POWER_PowerDownOnWFI();

    WFI();
}