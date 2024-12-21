#include "dac.h"

#ifdef DAC_USE_DMA

#include "GPDMA_LPC17xx.h"
#include "LPC17xx.h"
#include <stdbool.h>

_PRIVATE u8 DAC_DMAChannel;

_PRIVATE bool DAC_DMAWasSuccessful;

enum
{
    /// @brief By using a double buffer, we avoid possible glitches when the DAC is playing, and we are
    ///        feeding it with new samples at a high rate.
    // DAC_CFG_DOUBLEBUF = 0x2,

    /// @brief When enabled, the timer generates a periodic interrupt (or DMA request if enabled). Without this,
    ///        you would need another mechanism for feeding the DAC with samples (like using a timer).
    /// @note  This option is highly recommended for DMA mode.
    DAC_CFG_USE_CNT = 0x4,

    /// @brief Use DMA for the DAC peripheral. This makes the DAC work independently
    ///        from the CPU, which can perform other tasks while the DAC is playing. DAC automatically
    ///        retrives new samples from the memory and puts them in the DAC register.
    DAC_CFG_USE_DMA = 0x8,
} DAC_DMAConfig;

void DAC_InitDMA(u32 sample_rate_hz)
{
    LPC_DAC->DACCTRL |= (DAC_CFG_USE_DMA | DAC_CFG_USE_CNT);
    LPC_DAC->DACCNTVAL = sample_rate_hz;

    GPDMA_Initialize();
    DAC_DMAChannel = 1;
    DAC_DMAWasSuccessful = false;
}

void DAC_DeinitDMA(void)
{
    LPC_DAC->DACR = 0;
    LPC_DAC->DACCTRL &= ~(DAC_CFG_USE_DMA | DAC_CFG_USE_CNT);
    LPC_DAC->DACCNTVAL = 0;
    GPDMA_Uninitialize();
}

// PLAY FUNCTION & PRIVATE STUFF

_PRIVATE void dma_transfer_complete_callback(u32 event)
{
    DAC_DMAWasSuccessful = (event & GPDMA_EVENT_TERMINAL_COUNT_REQUEST) != 0;
    DAC_StopDMA();
}

void DAC_PlayDMA(const u16 *const pcm_samples, u32 sample_count)
{
    if (!pcm_samples || sample_count == 0)
        return;

    const u32 status = GPDMA_ChannelConfigure(DAC_DMAChannel, (u32)pcm_samples, (u32)&LPC_DAC->DACR, sample_count,
                                              GPDMA_CH_CONTROL_SI | GPDMA_CH_CONTROL_DI | GPDMA_WIDTH_HALFWORD,
                                              GPDMA_CONFIG_E, dma_transfer_complete_callback);

    if (status != 0)
        return;

    if (GPDMA_ChannelEnable(DAC_DMAChannel) != 0)
        return;
}

void DAC_StopDMA(void)
{
    GPDMA_ChannelDisable(DAC_DMAChannel);
}

#endif