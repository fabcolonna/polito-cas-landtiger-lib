#include "font-pixellari20.h"
#include "peripherals.h"
#include "system.h"

// TODO: Update sample project branch with new changes (Scripts, new stack size in System.s etc.)
// TODO: Replace every .sh script with a .ps1 script
// TODO: Update this project's README.md
// TODO: Update docs with new changes
// TODO: Implement GetObjectBBox (public API)
// TODO: Make the component macros assign their value to an extern variable, if specified, so that i can
// TODO: Readme, added RIT timeout dividers support
// TODO: Implemented bounding box for objects with selective re-rendering upon removal based on bounding box overlapping

// BUG: Touch Area sensitivity not correctly set when fill_color is set but edge_color is not
// BUG: RQ_RemoveObject does not completely delete the Button

LCD_MA_ALLOC_STATIC_MEM(lcd_arena, 1024);

// clang-format off

int main(void)
{
    SystemInit();

    LCD_MemoryArena *arena = LCD_MAUseMemory(lcd_arena, sizeof(lcd_arena));
    if (LCD_Init(LCD_ORIENT_VER, arena) != LCD_ERR_OK)
        return 1;

    LCD_FontID font;
    LCD_FMAddFont(Font_Pixellari20, &font);

    LCD_SetBackgroundColor(LCD_COL_BLACK);

    LCD_ObjID o1, o2;
    LCD_Component c1, c2, c3, c4;

    LCD_OBJECT(&o1, {
        (c1 = LCD_RECT2(100, 100, {
            .width = 100,
            .height = 50,
            .fill_color = LCD_COL_RED,
            .edge_color = LCD_COL_GREEN,
        })),
        (c3 = LCD_TEXT2(50, 50, {
            .text = "Hello, hello World!",
            .font = font,
            .text_color = LCD_COL_WHITE,
        })),
    });

    LCD_OBJECT(&o2, {
        (c2 = LCD_CIRCLE({
            .center = {120, 140},
            .radius = 20,
            .fill_color = LCD_COL_BLUE,
            .edge_color = LCD_COL_YELLOW,
        })),
    });

    LCD_RQRender();
    LCD_DEBUG_RenderBBox(LCD_GetObjBBoxWithID(o1));
    LCD_DEBUG_RenderBBox(LCD_GetObjBBoxWithID(o2));

    LCD_RQRemoveObject(o2, true);

    POWER_Init(POWR_CFG_SLEEP_ON_EXIT);
    POWER_PowerDownOnWFI();
    POWER_WaitForInterrupts();
}