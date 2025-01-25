#include "system.h"
#include "peripherals.h"

int main(void)
{
    SystemInit();

    // Your code...

    POWER_Init(POWR_CFG_SLEEP_ON_EXIT);
    POWER_SleepOnWFI();
    POWER_WaitForInterrupts();

    return 0;
}