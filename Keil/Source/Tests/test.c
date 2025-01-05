#include "peripherals.h"
#include "system.h"

int main(void)
{
    SystemInit();

    LCD_Init(LCD_ORIENT_VER);
    TP_Init();

    POWER_Init(POWR_CFG_SLEEP_ON_EXIT);
    POWER_PowerDownOnWFI();
    POWER_WaitForInterrupts();
}