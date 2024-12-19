#include "includes.h"

#include "wave.h"

void led(u16 old, u16 new)
{
    LED_Clear();
    const u8 led_index = new / 512; // (4096 / 8)
    LED_On(1 << led_index);
}

void play(void)
{
    DAC_Play(sine_wave, 44100 * 3); // 5 seconds of audio
}

int main(void)
{
    SystemInit();
    LED_Init();

    ADC_PMInit(4, INT_PRIO_MAX);
    ADC_PMSetSampleReadyAction(led);

    BUTTON_Init(INT_PRIO_DEF, INT_PRIO_DEF, INT_PRIO_DEF);
    BUTTON_SetInterruptHandler(BTN_INT_SRC_EINT0, play);

    // TODO: Make RIT accept tasks from the outside, possibly with different timings
    RIT_Init(50, 2);
    RIT_Enable();

    POWER_Init(POWR_CFG_SLEEP_ON_EXIT);
    POWER_PowerDownOnWFI();

    DAC_Init(TIMER_3, 44100);

    WFI();
}