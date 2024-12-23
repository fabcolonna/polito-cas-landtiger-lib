#include "dac.h"
#include "timer.h"
#include <LPC17xx.h>

#define DAC_CLK_HZ 25000000
#define DAC_SIN_TABLE_SZ 45

_PRIVATE TIMER DAC_SamplesTimer;
_PRIVATE TIMER DAC_SecondsTimer;

_PRIVATE u8 DAC_SinVolume = DAC_VOL_DEFAULT;
_PRIVATE u32 DAC_SinTableIndex = 0;

/// @brief Sine wave samples for the DAC. They represent one complete period. Each sample needs to be sent to the DAC
//         at a specific frequency in order to generate the desired tone.
const u16 DAC_Sin[] = {410, 467, 523, 576, 627, 673, 714, 749, 778, 799, 813, 819, 817, 807, 789,
                       764, 732, 694, 650, 602, 550, 495, 438, 381, 324, 270, 217, 169, 125, 87,
                       55,  30,  12,  2,   0,   6,   20,  41,  70,  105, 146, 193, 243, 297, 353};

_PRIVATE inline u32 get_tone_timing(u16 note, u8 octave)
{
    // Gets the Hz of the tone at the specified octave
    const u32 freq = (octave <= DAC_OCT_4) ? note << (DAC_OCT_4 - octave) : note >> (octave - DAC_OCT_4);

    /// In order to play the tone, every sample needs to be sent to the DAC at a specific rate
    /// that depends on the frequency of the tone, the number of samples in the sine wave table
    /// and the frequency of the TIMER peripheral that is responsible for sending the samples to the DAC
    /// @example 440Hz tone, 45 samples, 25MHz timer:
    ///          In one second (25 million cycles), 440 full periods (i.e. 45 samples) need to be sent to the DAC.
    ///          Hence, Dac_SamplesTimer needs to be set to 25MHz (duration of a second) / (440 * 45) = 1250Hz
    return DAC_CLK_HZ / (freq * DAC_SIN_TABLE_SZ);
}

_PRIVATE inline u16 adjust_sample_volume(u16 sample, u8 volume)
{
    if (volume == 0)
        return 0; // Mute

    sample -= 410;
    sample /= (10 / volume); // If volume is 1 -> sample /= 10 (decrease volume), if 10 -> sample /= 1 (increase volume)
    sample += 410;

    return sample;
}

// TIMER INTERRUPTS & SINE WAVE

_PRIVATE void next_sample(void)
{
    if (DAC_SinVolume == 0)
    {
        LPC_DAC->DACR = 0;
        return;
    }

    // Sending next sample
    u16 volumed_sample = adjust_sample_volume(DAC_Sin[DAC_SinTableIndex++], DAC_SinVolume);
    LPC_DAC->DACR = (volumed_sample << 6);

    if (DAC_SinTableIndex == DAC_SIN_TABLE_SZ)
        DAC_SinTableIndex = 0;
}

// PUBLIC FUNCTIONS

void DAC_BUZInit(u8 timer_a, u8 timer_b, u8 int_priority)
{
    // Enable Analog OUT (function 10) on PIN on P0.26, located in PINSEL1
    LPC_PINCON->PINSEL1 |= (2 << 20);
    LPC_GPIO0->FIODIR |= (1 << 26);

    TIMER_Init(&DAC_SamplesTimer, timer_a, NO_PRESCALER, int_priority);
    TIMER_Init(&DAC_SecondsTimer, timer_b, NO_PRESCALER, int_priority);
}

void DAC_BUZDeinit(void)
{
    LPC_PINCON->PINSEL1 &= ~(3 << 20);
    LPC_GPIO0->FIODIR &= ~(1 << 26);

    DAC_BUZStop();
    TIMER_Deinit(DAC_SamplesTimer);
    TIMER_Deinit(DAC_SecondsTimer);
}

void DAC_BUZSetVolume(u8 volume)
{
    DAC_SinVolume = (IS_BETWEEN_EQ(volume, 0, 10)) ? volume : DAC_VOL_DEFAULT;
}

void DAC_BUZPlay(DAC_Tone tone, u16 bpm)
{
    // How many samples need to be sent to the DAC in a second in order to play the tone
    TIMER_SetInterruptHandler(DAC_SamplesTimer, TIM_INT_SRC_MR0, next_sample);
    TIMER_SetMatch(DAC_SamplesTimer, (TIMER_MatchRegister){.which = TIM_MR0,
                                                           .actions = TIM_MR_INT | TIM_MR_RES,
                                                           .match = get_tone_timing(tone.note, tone.octave)});

    // Since a tone has a type (i.e. a duration which depends on the BPM), the duration of the tone
    // needs to be controlled by another timer, which, on match, will stop & clear the samples timer.
    // The duration (in seconds) of the tone is calculated by the formula: (60 * type) / bpm. Since
    // each second the seconds timer ticks 25 million times, we need to multiply that by the duration
    const u32 tone_duration = DAC_CLK_HZ * ((60 * tone.type) / bpm);
    TIMER_SetInterruptHandler(DAC_SecondsTimer, TIM_INT_SRC_MR0, DAC_BUZStop);
    TIMER_SetMatch(DAC_SecondsTimer, (TIMER_MatchRegister){.which = TIM_MR0,
                                                           .actions = TIM_MR_INT | TIM_MR_RES | TIM_MR_STP,
                                                           .match = tone_duration});

    // Starts the timers (i.e. plays)
    TIMER_Enable(DAC_SamplesTimer);
    TIMER_Enable(DAC_SecondsTimer);
}

bool DAC_BUZIsPlaying(void)
{
    return TIMER_IsEnabled(DAC_SamplesTimer) && TIMER_IsEnabled(DAC_SecondsTimer);
}

void DAC_BUZStop(void)
{
    DAC_SinVolume = DAC_VOL_DEFAULT;
    DAC_SinTableIndex = 0;
    LPC_DAC->DACR &= ~(0x3FF << 6);

    TIMER_Disable(DAC_SamplesTimer);
    TIMER_Reset(DAC_SamplesTimer);

    TIMER_Reset(DAC_SecondsTimer);
    TIMER_Disable(DAC_SecondsTimer);
}
