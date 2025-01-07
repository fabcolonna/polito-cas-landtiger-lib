#include "peripherals.h"
#include "system.h"

#include "font-upheaval14.h"

int main(void)
{
    SystemInit();
    LCD_Init(LCD_ORIENT_VER);
    LCD_SetBackgroundColor(LCD_COL_BLACK);

    const LCD_FontID font_upheaval14 = LCD_FMAddFont(Font_Upheaval14);
	
    LCD_BEGIN_DRAWING;
    LCD_OBJECT(welcome, 2, {
        LCD_TEXT(79, LCD_GetHeight() / 2 - 10, {
            .text = "Welcome to",
            .font = LCD_DEF_FONT_SYSTEM,
            .text_color = LCD_COL_WHITE,
            .bg_color = LCD_COL_NONE,
        }),
        LCD_TEXT(27, LCD_GetHeight() / 2 + 20, {
            .text = "Press anywhere to start!",
            .font = font_upheaval14,
            .text_color = LCD_COL_WHITE,
            .bg_color = LCD_COL_NONE,
            .char_spacing = 1,
            .line_spacing = LCD_TEXT_DEF_LINE_SPACING,
        }),
    });
    LCD_END_DRAWING;

    POWER_Init(POWR_CFG_SLEEP_ON_EXIT);
    POWER_PowerDownOnWFI();
    POWER_WaitForInterrupts();
}