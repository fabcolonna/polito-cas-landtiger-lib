#include "pacman.h"
#include "peripherals.h"
#include "system.h"

int main(void)
{
    SystemInit();

    PACMAN_Init();
    PACMAN_Play(PM_SPEED_NORMAL);

    POWER_Init(POWR_CFG_SLEEP_ON_EXIT);
    POWER_SleepOnWFI();
    POWER_WaitForInterrupts();
}
