#include "font-pixellari20.h"
#include "peripherals.h"
#include "system.h"

/*
int main(void)
{
    SystemInit();
    LCD_Init(LCD_ORIENT_VER);
    TP_Init(false);

    LCD_SetBackgroundColor(LCD_COL_BLACK);

    const LCD_FontID font_up = LCD_FMAddFont(Font_Pixellari20);
    TP_ButtonArea welcome_button_area;

    LCD_OBJECT_IMMEDIATE(1, {
        LCD_BUTTON(LCD_GetWidth() / 2 - 50, LCD_GetHeight() / 2 - 50, welcome_button_area, {
            .label = LCD_BUTTON_LABEL({
                .text = "WELCOME POPO!",
                .font = font_up,
                .text_color = LCD_COL_BLACK,
            }),
            .edge_color = LCD_COL_MAGENTA,
            .fill_color = LCD_COL_MAGENTA,
            .padding = {10, 10, 10, 10},
        }),
    });

    POWER_Init(POWR_CFG_SLEEP_ON_EXIT);
    POWER_PowerDownOnWFI();
    POWER_WaitForInterrupts();
}
*/

/*
int main(void)
{
    SystemInit();
    LCD_Init(LCD_ORIENT_VER);

    LCD_SetBackgroundColor(LCD_COL_BLACK);

    const LCD_FontID font_up = LCD_FMAddFont(Font_Pixellari20);

    LCD_Component circle = {
        .type = LCD_COMP_BUTTON,
        .pos = {100, 100},
        .object.button = {
            .label = {
                .text = "WELCOME POPO!",
                .font = font_up,
                .text_color = LCD_COL_BLACK,
            },
            .padding = {3, 5, 3, 5},
            .fill_color = LCD_COL_MAGENTA,
        },
    };

    LCD_Obj o = {
        .comps = (LCD_Component[]){circle},
        .comps_size = 1,
    };

    LCD_RQAddObject(&o);
    LCD_RQRender();

    LCD_CompBBox bbox = LCD_GetComponentBBox(circle);
    LCD_DEBUG_RenderComponentBBox(&bbox);
}
*/

int main(void)
{
    SystemInit();
    LCD_Init(LCD_ORIENT_VER);
    TP_Init(false);

    LCD_SetBackgroundColor(LCD_COL_BLACK);

    const LCD_FontID font_up = LCD_FMAddFont(Font_Pixellari20);

    TP_ButtonArea tp_button_area;

    // clang-format off
    LCD_OBJECT_IMMEDIATE(1, {
        LCD_BUTTON(50, 100, tp_button_area, {
            .label = LCD_BUTTON_LABEL({
                .text = "WELCOME POPO!",
                .font = font_up,
                .text_color = LCD_COL_BLACK,
            }),
            .padding = {3, 5, 3, 5},
            .fill_color = LCD_COL_MAGENTA,
        }),
    });
    // clang-format on

    TP_WaitForButtonPress(tp_button_area);
    LCD_OBJECT_IMMEDIATE(1, {
        LCD_TEXT(50, 150, {
            .text = "Button pressed!",
            .font = font_up,
            .text_color = LCD_COL_WHITE,
        }),
    });
}