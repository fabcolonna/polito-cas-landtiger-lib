#include "includes.h"

void eint0_handler(void)
{
    LED_Output(248);
    Delay(5);
    LED_Output(7);
}

int main(void)
{
    LED_Init();
    DEB_Init(50, INT_PRIO_MAX);
    DEB_Enable();

    BUTTON_Init(2, 2, 2);
    BUTTON_SetInterruptHandler(BTN_INT_SRC_EINT0, eint0_handler);

    Power_Init(POWR_CFG_SLEEP_ON_EXIT);
    Power_PowerDownOnWFI();

    WFI();
}