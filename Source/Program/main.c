#include "includes.h"

int main(void)
{
    SystemInit();
    LCD_Init();

    RIT_Init(50, 0);
    RIT_Enable();
	
		LCD_ClearWith(COL_GREEN);
	
		const char *string = "Hello!";
		LCD_PrintString(string, COL_WHITE, LCD_FONT_MSGOTHIC, COL_RED, (LCD_Coordinate){0, 280});

    POWER_Init(POWR_CFG_SLEEP_ON_EXIT);
    POWER_PowerDownOnWFI();
    POWER_WaitForInterrupts();
} 