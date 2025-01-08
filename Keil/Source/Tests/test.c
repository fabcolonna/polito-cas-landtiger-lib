#include "peripherals.h"
#include "system.h"

#include "font-upheaval14.h"

int main(void)
{
    SystemInit();
    LCD_Init(LCD_ORIENT_VER);
    TP_Init(false);

    LCD_SetBackgroundColor(LCD_COL_BLACK);

    const LCD_FontID font_upheaval14 = LCD_FMAddFont(Font_Upheaval14);
	
    const TP_Button start_button = {
        .pos = {
            .x = 27,
            .y = LCD_GetHeight() / 2 + 40,
        },
        .width = 75,
        .height = 20,
    };

    LCD_ObjID welcome_id;

    LCD_BEGIN_DRAWING;
    welcome_id = LCD_OBJECT(welcome, 3, {
        LCD_TEXT(79, LCD_GetHeight() / 2 - 10, {
            .text = "Welcome to",
            .font = LCD_DEF_FONT_SYSTEM,
            .text_color = LCD_COL_WHITE,
            .bg_color = LCD_COL_NONE,
        }),

        // Button
        LCD_RECT(start_button.pos.x, start_button.pos.y, {
            .width = start_button.width,
            .height = start_button.height,
            .edge_color = LCD_COL_YELLOW,
            .fill_color = LCD_COL_YELLOW,
        }),
        LCD_TEXT(start_button.pos.x + start_button.width / 2 - 10, start_button.pos.y + start_button.height / 2, {
            .text = "Start",
            .font = font_upheaval14,
            .text_color = LCD_COL_WHITE,
            .bg_color = LCD_COL_NONE,
        }),
    });
    LCD_END_DRAWING;

    TP_WaitForButtonPress(start_button);
    LCD_RQRemoveObject(welcome_id, true);

    POWER_Init(POWR_CFG_SLEEP_ON_EXIT);
    POWER_PowerDownOnWFI();
    POWER_WaitForInterrupts();
}