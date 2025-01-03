#include "includes.h"

#include "font_pixellari16.h"
#include "font_pixellari8.h"

int main(void)
{
    SystemInit();
    LCD_Init(LCD_ORIENT_VER);
    //TP_Init();

    const LCD_FontID pixellari_font_id = LCD_FMAddFont(Font_Pixellari8);
    const LCD_FontID pixellari_16_font_id = LCD_FMAddFont(Font_Pixellari16);

    // clang-format off

    LCD_BEGIN_DRAWING;
    const LCD_ObjID cross_id = LCD_OBJECT(cross, 3, {
        LCD_TEXT(40, 40, {
            .text = "This is Pixellari 8 font",
            .text_color = LCD_COL_RED,
            .bg_color = LCD_COL_NONE,
            .font = pixellari_16_font_id,
        })
    });
    LCD_END_DRAWING;

    // clang-format on

    POWER_Init(POWR_CFG_SLEEP_ON_EXIT);
    POWER_PowerDownOnWFI();
    POWER_WaitForInterrupts();
}