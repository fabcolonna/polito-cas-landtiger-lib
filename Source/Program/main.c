#include "includes.h"

int main(void)
{
    SystemInit();
    LED_Init();

    RIT_Init(50, INT_PRIO_MAX);
    RIT_Enable();

    Power_Init(POWR_CFG_SLEEP_ON_EXIT);
    Power_PowerDownOnWFI();

    WFI();
}