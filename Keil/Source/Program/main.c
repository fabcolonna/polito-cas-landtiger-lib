#include "system.h"
#include "peripherals.h"

int main(void)
{
    SystemInit();

    // Your code...

    POWER_Init(POW_SLEEPONEXIT);
    POWER_SleepOnWFI();
    POWER_WaitForInterrupts();

    return 0;
}