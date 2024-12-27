#include "includes.h"

int main(void)
{
    SystemInit();
    LCD_Init();

    RIT_Init(50, 0);
    RIT_Enable();

    LCD_ClearWith(COL_BLACK);
    // LCD_DrawImage32(apple_png_alpha, 96, 54, (LCD_Coordinate){50, 50});

    POWER_Init(POWR_CFG_SLEEP_ON_EXIT);
    POWER_PowerDownOnWFI();
    POWER_WaitForInterrupts();
}