#include "includes.h"

int main(void)
{
    SystemInit();
    LCD_Init();

    RIT_Init(50, 0);
    RIT_Enable();
	
		LCD_ClearWith(COL_BLACK);
	
		const char *string = "Hello!";
		LCD_PrintString(string, COL_WHITE, LCD_FONT_SYSTEM, COL_RED, (LCD_Coordinate){0, 280});

    // Drawing a square
		LCD_DrawRectangle((LCD_Coordinate){10, 10}, (LCD_Coordinate){100, 100}, COL_RED, COL_RED);
		
		LCD_DrawCircle((LCD_Coordinate){130, 130}, 15, COL_BLUE2, COL_BLUE2);

    POWER_Init(POWR_CFG_SLEEP_ON_EXIT);
    POWER_PowerDownOnWFI();
    POWER_WaitForInterrupts();
} 