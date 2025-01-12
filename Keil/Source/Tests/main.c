#include "pacman.h"
#include "peripherals.h"
#include "system.h"

// Memory arena for PacMan: 1KB
LCD_MA_ALLOC_STATIC_MEM(lcd_memory, 10192);

int main(void)
{
    SystemInit();
    RIT_Init(50, INT_PRIO_DEF);
    RIT_Enable();

    // Initializing the LCD
    LCD_MemoryArena *lcd_arena = LCD_MAUseMemory(lcd_memory, sizeof(lcd_memory));
    if (!lcd_arena)
        return EXIT_FAILURE;

    if (LCD_Init(LCD_ORIENT_VER, lcd_arena) != LCD_ERR_OK)
        return EXIT_FAILURE;

    // TP_Init(false);

    PACMAN_Init();
    POWER_Init(POWR_CFG_SLEEP_ON_EXIT);
    POWER_SleepOnWFI();
    POWER_WaitForInterrupts();
}
