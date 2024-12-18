#include "includes.h"

void up0_action(void)
{
    LED_Output(5);
    Delay(5);
    LED_Output(7);
    Delay(5);
    LED_Clear();
}

int main(void)
{
    LED_Init();
	
	  RIT_Init(50, INT_PRIO_MAX);
    RIT_Enable();

    JOYSTICK_Init();
    JOYSTICK_SetFunction(JOY_ACTION_UP, up0_action);

    Power_Init(POWR_CFG_SLEEP_ON_EXIT);
    Power_PowerDownOnWFI();

    WFI();
}