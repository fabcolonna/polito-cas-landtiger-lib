#include "dac.h"
#include "LPC17xx.h"
#include "timer.h"

_PRIVATE TIMER DAC_SamplesTimer;
_PRIVATE TIMER DAC_SecondsTimer;

_PRIVATE const u32 DAC_CLKHz = 25000000;

_PRIVATE inline u16 get_tone_frequency(u16 tone, u8 octave)
{
    if (octave == DAC_OCT_4)
        return tone;

    return (octave < DAC_OCT_4) ? tone >> (DAC_OCT_4 - octave) : tone << (octave - DAC_OCT_4);
}

// TIMER INTERRUPTS & SINE WAVE

_PRIVATE u8 requested_volume;
_PRIVATE u32 sin_table_index = 0;

#define DAC_SIN_TABLE_SIZE 45
const u16 DAC_Sin[DAC_SIN_TABLE_SIZE] = {410, 467, 523, 576, 627, 673, 714, 749, 778, 799, 813, 819, 817, 807, 789,
                                         764, 732, 694, 650, 602, 550, 495, 438, 381, 324, 270, 217, 169, 125, 87,
                                         55,  30,  12,  2,   0,   6,   20,  41,  70,  105, 146, 193, 243, 297, 353};

_PRIVATE void next(void)
{
    if (requested_volume == 0)
    {
        LPC_DAC->DACR = 0;
        return;
    }

    u16 value = DAC_Sin[sin_table_index++];
    value -= 410; // Normalization
    value /= (10 / requested_volume);
    value += 410;

    LPC_DAC->DACR = (value << 6);
    if (sin_table_index == DAC_SIN_TABLE_SIZE)
        sin_table_index = 0;
}

// PUBLIC FUNCTIONS

void DAC_BUZInit(u8 timer_a, u8 timer_b, u8 int_priority)
{
    TIMER_Init(&DAC_SamplesTimer, timer_a, NO_PRESCALER, int_priority);
    TIMER_Init(&DAC_SecondsTimer, timer_b, NO_PRESCALER, int_priority);
}

void DAC_BUZDeinit(void)
{
    TIMER_Disable(DAC_SamplesTimer);
    TIMER_Reset(DAC_SamplesTimer);
    TIMER_Deinit(DAC_SamplesTimer);

    TIMER_Disable(DAC_SecondsTimer);
    TIMER_Reset(DAC_SecondsTimer);
    TIMER_Deinit(DAC_SecondsTimer);
}

void DAC_BUZPlay(u16 tone, u8 octave, u8 type, u8 bpm, u8 volume)
{
    requested_volume = volume;
    const u16 tone_freq = get_tone_frequency(tone, octave);

    // Frequency at which I send the next sample to the DAC
    const float sample_freq = tone_freq / DAC_SIN_TABLE_SIZE;
    TIMER_MatchRegister samples_mr0 = {
        .which = 0, .actions = TIM_MR_INT | TIM_MR_RES, .match = (u32)(DAC_CLKHz / sample_freq)};
    TIMER_SetMatch(DAC_SamplesTimer, samples_mr0);
    TIMER_SetInterruptHandler(DAC_SamplesTimer, TIM_INT_SRC_MR0, next);

    // How many seconds does the tone last
    const float tone_duration_sec = (60 * type) / bpm;
    TIMER_MatchRegister seconds_mr0 = {
        .which = 0, .actions = TIM_MR_INT | TIM_MR_RES, .match = (u32)(DAC_CLKHz * tone_duration_sec)};
    TIMER_SetMatch(DAC_SecondsTimer, seconds_mr0);
    TIMER_SetInterruptHandler(DAC_SecondsTimer, TIM_INT_SRC_MR0, DAC_BUZStop);

    TIMER_Enable(DAC_SamplesTimer);
    TIMER_Enable(DAC_SecondsTimer);
}

void DAC_BUZStop(void)
{
    TIMER_Disable(DAC_SamplesTimer);
    TIMER_Reset(DAC_SamplesTimer);

    TIMER_Reset(DAC_SecondsTimer);
    TIMER_Disable(DAC_SecondsTimer);

    sin_table_index = 0;
    requested_volume = 1;

    LPC_DAC->DACR = 0;
}
