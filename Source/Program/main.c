#include "includes.h"

void led(u16 old, u16 new)
{
    LED_Clear();
    const u8 led_index = new / 512; // (4096 / 8)
    LED_On(1 << led_index);
}

void play(void)
{
    if (DAC_BUZIsPlaying())
        DAC_BUZStop();
    else
        DAC_BUZPlay((DAC_Tone){.note = DAC_NOTE_A, .octave = DAC_OCT_4, .type = DAC_NOTE_WHOLE}, 60);
}

int main(void)
{
    SystemInit();
    LED_Init();

    RIT_Init(50, INT_PRIO_DEF);
    RIT_Enable();

    ADC_PMInit(ADC_PM_SAMPLE_WITH_RIT, 4, INT_PRIO_DEF);
    ADC_PMSetSampleReadyAction(led);

    DAC_BUZInit(TIM_2, TIM_3, INT_PRIO_DEF);
    DAC_BUZSetVolume(10);

    BUTTON_Init(BTN_DEBOUNCE_WITH_RIT, INT_PRIO_DEF, INT_PRIO_DEF, INT_PRIO_DEF);
    BUTTON_SetInterruptHandler(BTN_INT_SRC_EINT0, play);

    POWER_Init(POWR_CFG_SLEEP_ON_EXIT);
    POWER_PowerDownOnWFI();
    WFI();
}