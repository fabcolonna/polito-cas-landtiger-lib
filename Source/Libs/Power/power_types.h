#ifndef __POWER_TYPES_H
#define __POWER_TYPES_H

typedef enum
{
    POWR_CFG_DEEP = 0x4,         // BIT 2 in the SCB->SCR
    POWR_CFG_SLEEP_ON_EXIT = 0x2 // BIT 1 in the SCB->SCR
} POWER_Config;

// Refer to the user manual @ Table 46 for more peripherals.
// Each enum constant equals to the BIT in the PCONP register associated to that peripheral.
typedef enum
{
    PCTIM0 = 1,
    PCTIM1 = 2,
    PCADC = 12,
    PCGPIO = 15,
    PCRIT = 16,
    PCTIM2 = 22,
    PCTIM3 = 23
} POWER_Peripheral;

#endif