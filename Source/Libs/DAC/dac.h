#ifndef __DAC_H
#define __DAC_H

#include "utils.h"

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

#include "timer_types.h"

/// @brief Initializes the DAC peripheral.
/// @param sample_rate_hz The sample rate of the DAC.
/// @param timer This implementation requires the use of a timer to feed the DAC with samples.
///              Make sure you're not using that timer for other purposes. Take this value
///              from the TIMER_Which enum.
void DAC_Init(u8 timer, u32 sample_rate_hz);

void DAC_Deinit(void);

void DAC_Play(const u16 *const pcm_samples, u32 sample_count);

void DAC_Stop(void);

#endif

#endif