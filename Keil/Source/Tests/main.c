#include "peripherals.h"
#include "pm.h"
#include "system.h"

// Memory arena for PacMan: 4K
LCD_MA_ALLOC_STATIC_MEM(lcd_memory, 4096);

int main(void)
{
    SystemInit();
    RIT_Init(50, INT_PRIO_DEF);
    RIT_Enable();

    // Initializing the LCD
    const LCD_MemoryArena *const lcd_arena = LCD_MAUseMemory(lcd_memory, sizeof(lcd_memory));
    if (!lcd_arena)
        return EXIT_FAILURE;

    if (LCD_Init(LCD_ORIENT_VER, lcd_arena, NULL) != LCD_ERR_OK)
        return EXIT_FAILURE;

    TP_Init(false);

    if (!PACMAN_Init())
        return EXIT_FAILURE;

    PACMAN_GameLoop(PM_SPEED_NORMAL);
    POWER_Init(POWR_CFG_SLEEP_ON_EXIT);
    POWER_SleepOnWFI();
    POWER_WaitForInterrupts();
}
