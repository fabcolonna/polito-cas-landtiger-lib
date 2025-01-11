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
//       call GetComponentBBOx on them

// BUG: Touch Area sensitivity not correctly set when fill_color is set but edge_color is not
// BUG: RQ_RemoveObject does not completely delete the Button

LCD_MA_ALLOC_STATIC_MEM(lcd_arena, 1024);

void increment(void)
{
    static int x = 0;
    x++;
}

void slower_increment(void)
{
    static int slower = 0;
    slower++;
}

int main(void)
{
    SystemInit();
    RIT_Init(50, INT_PRIO_DEF);
    RIT_Enable();

    LCD_MemoryArena *arena = LCD_MAUseMemory(lcd_arena, sizeof(lcd_arena));
    if (LCD_Init(LCD_ORIENT_VER, arena) != LCD_ERR_OK)
        return 1;

    LCD_SetBackgroundColor(LCD_COL_BLACK);

    LCD_FontID font_pixellari;
    LCD_FMAddFont(Font_Pixellari20, &font_pixellari);

    LCD_ObjID id;
    LCD_OBJECT(&id, {
                        LCD_RECT2(100, 100,
                                  {
                                      .width = 100,
                                      .height = 50,
                                      .fill_color = LCD_COL_RED,
                                      .edge_color = LCD_COL_GREEN,
                                  }),
                    });

    RIT_AddJob(increment, 2);         // 100ms interval
    RIT_AddJob(slower_increment, 20); // 1s interval

    LCD_ObjID id2;
    LCD_OBJECT(&id2, {
                         LCD_RECT2(80, 80,
                                   {
                                       .width = 100,
                                       .height = 50,
                                       .fill_color = LCD_COL_RED,
                                       .edge_color = LCD_COL_GREEN,
                                   }),
                     });

    LCD_RQRender();

    LCD_RQRemoveObject(id2, true);
    POWER_Init(POWR_CFG_SLEEP_ON_EXIT);
    POWER_PowerDownOnWFI();
    POWER_WaitForInterrupts();
}