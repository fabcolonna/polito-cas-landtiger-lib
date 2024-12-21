#include "includes.h"

void led(u16 old, u16 new)
{
    LED_Clear();
    const u8 led_index = new / 512; // (4096 / 8)
    LED_On(1 << led_index);
}

void play(void)
{
    DAC_BUZPlay(DAC_NOTE_A, DAC_OCT_4, DAC_NOTE_WHOLE, 60, 10);
}

int main(void)
{
    SystemInit();
    LED_Init();

    ADC_PMInit(4, INT_PRIO_DEF);
    ADC_PMSetSampleReadyAction(led);

    BUTTON_Init(INT_PRIO_DEF, INT_PRIO_DEF, INT_PRIO_DEF);
    BUTTON_SetInterruptHandler(BTN_INT_SRC_EINT0, play);

    // TODO: Make RIT accept tasks from the outside, possibly with different timings
    RIT_Init(50, 2);
    RIT_Enable();

    DAC_BUZInit(2, 3, INT_PRIO_MAX);

    POWER_Init(POWR_CFG_SLEEP_ON_EXIT);
    POWER_PowerDownOnWFI();

    WFI();
}