#include "includes.h"

#include "font_pixellari8.h"

int main(void)
{
    SystemInit();
    LCD_Init(LCD_ORIENT_VER);
    // TP_Init();

    const LCD_FontID pixellari_font_id = LCD_FMAddFont(Font_Pixellari8);

    LCD_SetBackgroundColor(LCD_COL_BLACK);

    // TODO: Fix touch screen drawing stuff and readd it to Keil (uncommet in includes)
    // TODO: Add shapes for complex components

    LCD_ComponentID text_id = LCD_CMAddComp(LCD_MAKE_TEXT(10, 10,
                                                          {
                                                              .text = "Hello, world!",
                                                              .text_color = LCD_COL_WHITE,
                                                              .bg_color = LCD_COL_NONE,
                                                              .font = LCD_DEF_FONT_SYSTEM,
                                                          }),
                                            false);

    LCD_ComponentID rect_id = LCD_CMAddComp(LCD_MAKE_RECT(70, 130,
                                                          {
                                                              .width = 80,
                                                              .height = 80,
                                                              .edge_color = LCD_COL_RED,
                                                              .fill_color = LCD_COL_RED,
                                                          }),
                                            false);

    // Making circle

    LCD_ComponentID circle_id = LCD_CMAddComp(LCD_MAKE_CIRCLE({
                                                  .center = {120, 170},
                                                  .radius = 20,
                                                  .edge_color = LCD_COL_BLUE,
                                                  .fill_color = LCD_COL_BLUE,
                                              }),
                                              false);

    // Removing the text component
    LCD_CMRender();
    LCD_CMRemoveComp(rect_id, true);

    POWER_Init(POWR_CFG_SLEEP_ON_EXIT);
    POWER_PowerDownOnWFI();
    POWER_WaitForInterrupts();
}