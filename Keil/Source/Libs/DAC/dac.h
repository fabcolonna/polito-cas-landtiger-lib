#ifndef __DAC_H
#define __DAC_H

#include "dac_types.h"
#include "utils.h"

#include <stdbool.h>

#ifdef DAC_USE_DMA // DMA is not currently working due to "write permission error 65"

/// @brief Initializes the DAC peripheral.
/// @param sample_rate_hz The sample rate of the DAC.
void DAC_InitDMA(u32 sample_rate_hz);

/// @brief Deinitializes the DAC peripheral.
void DAC_DeinitDMA(void);

/// @brief Plays a sequence of 10 bits samples (up to 1024 quantization levels), at a given sample rate.
/// @param pcm_samples The samples to be played.
/// @param sample_count The number of samples to be played.
void DAC_PlayDMA(const u16 *const pcm_samples, u32 sample_count);

void DAC_StopDMA(void);

#else

/// @brief Initializes the DAC peripheral.
/// @param int_priority The interrupt priority of the timer. If set to INT_PRIO_DEF, the default
///                     interrupt priority will be used.
/// @note  This implementation requires the use of 2 timers to feed the DAC with samples
///        and to play the note for the given duration. By giving these parameters to this
///        function, you ensure that those timers are not and won't be used for other purposes.
void DAC_BUZInit(u8 timer_a, u8 timer_b, u8 int_priority);

// TODO: Instead of using standard timers, use RIT for seconds, and SYSTICK for sample feeding

void DAC_BUZDeinit(void);

/// @brief Plays the given note for the given duration.
/// @param tone The tone to be played
/// @param bpm The tempo of the note (i.e. 60, 120, 240, etc.)
void DAC_BUZPlay(DAC_Tone tone, u16 bpm);

// TODO: Add multiple tone playback using RIT

/// @brief Sets the volume of the buzzer.
/// @param volume The volume of the buzzer (between 0 and 10).
void DAC_BUZSetVolume(u8 volume);

bool DAC_BUZIsPlaying(void);

void DAC_BUZStop(void);

#endif

#endif