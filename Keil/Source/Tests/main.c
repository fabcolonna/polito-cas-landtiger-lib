#include "font-pixellari20.h"
#include "peripherals.h"
#include "system.h"

// TODO: Update sample project branch with new changes (Scripts, new stack size in System.s etc.)
// TODO: Replace every .sh script with a .ps1 script
// TODO: Update this project's README.md
// TODO: Update docs with new changes
// TODO: Implement GetObjectBBox (public API)
// TODO: Make the component macros assign their value to an extern variable, if specified, so that i can
//       call GetComponentBBOx on them

// BUG: Touch Area sensitivity not correctly set when fill_color is set but edge_color is not
// BUG: RQ_RemoveObject does not completely delete the Button

LCD_MA_ALLOC_STATIC_MEMORY(lcd_arena, 128);

int main(void)
{
    SystemInit();
    LCD_Init(LCD_ORIENT_VER);

    // LCD_SetBackgroundColor(LCD_COL_BLACK);

    LCD_MemoryArena *arena = LCD_MAUseMemory(lcd_arena, sizeof(lcd_arena));
    if (!arena)
        return 1;

    const LCD_FontID font_up = LCD_FMAddFont(Font_Pixellari20);

    LCD_Obj *obj;
    const LCD_MAError err = LCD_MAAllocObject(1, &obj);
    if (err != LCD_MA_ERR_NONE)
        return 1;

    obj->comps = (LCD_Component[]){
        LCD_TEXT(50, 150,
                 {
                     .text = "Button pressed!",
                     .font = font_up,
                     .text_color = LCD_COL_BLACK,
                     .bg_color = LCD_COL_RED,
                 }),
    };

    LCD_MAFreeObject(obj);

    LCD_RQAddObject(obj);
    LCD_RQRender();

    LCD_MAAllocObject(2, &obj);

    TP_ButtonArea area;
    obj->comps = (LCD_Component[]){
        LCD_BUTTON(LCD_GetWidth() / 2 - 20, LCD_GetHeight() / 2 - 10, area, {
            .label = LCD_BUTTON_LABEL({
                .text = "START!",
                .font = font_up,
                .text_color = LCD_COL_WHITE,
            }),
            .padding = {3, 5, 3, 5},
            .fill_color = rgb888_to_rgb565(0xe27417),
        })
    };

    LCD_RQAddObject(obj);
    LCD_RQRender();

    POWER_Init(POWR_CFG_SLEEP_ON_EXIT);
    POWER_PowerDownOnWFI();
    POWER_WaitForInterrupts();
}