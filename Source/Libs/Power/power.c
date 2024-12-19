#include "power.h"
#include "LPC17xx.h"

void POWER_Init(u8 config)
{
    SCB->SCR |= (config & POWR_CFG_SLEEP_ON_EXIT);
    SCB->SCR |= (config & POWR_CFG_DEEP);
}

void POWER_SleepOnWFI(void)
{
    LPC_SC->PCON &= ~0x3; // Sets B[1,0] = 00
}

void POWER_PowerDownOnWFI(void)
{
    SET_BIT(LPC_SC->PCON, 0); // 01
}

void POWER_DeepPowerDownOnWFI(void)
{
    LPC_SC->PCON |= 0x3; // 11
}

void POWER_TurnOnPeripheral(u8 bit)
{
    SET_BIT(LPC_SC->PCONP, bit);
}

void POWER_TurnOffPeripheral(u8 bit)
{
    CLR_BIT(LPC_SC->PCONP, bit);
}