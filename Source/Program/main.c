#include "includes.h"

TIMER timer1, timer2;

void t1_mr0_int_handler(void)
{
    LED_Output(7);
    for (int i = 100000; i >= 0; i--)
        ;
    LED_Clear();
}

void t2_mr3_int_handler(void)
{
    LED_Output(192);
    for (int i = 100000; i >= 0; i--)
        ;
    LED_Clear();
}

void eint0_handler(void)
{
    TIMER_Enable(timer1);
}

void eint1_handler(void)
{
    TIMER_Enable(timer2);
}

int main(void)
{
    SystemInit();
    LED_Init();
	
    BUTTON_Init(DEF_PRIORITY, DEF_PRIORITY, DEF_PRIORITY);
    BUTTON_SetInterruptHandler(BTN_INT_SRC_EINT0, eint0_handler);
    BUTTON_SetInterruptHandler(BTN_INT_SRC_EINT1, eint1_handler);

    TIMER_Init(&timer1, 1, 100000, NULL);
    TIMER_Init(&timer2, 2, 1000, NULL);

    TIMER_MatchReg mr0_t1 = {.which = 0, .match = 10, .actions = TIM_MR_INT};
    TIMER_MatchReg mr1_t1 = {.which = 1, .match = 32, .actions = TIM_MR_INT | TIM_MR_RES};
    TIMER_SetMatch(timer1, mr0_t1);
    TIMER_SetMatch(timer1, mr1_t1);

    TIMER_MatchReg mr2_t2 = {.which = 2, .match = 200, .actions = TIM_MR_NOP};
    TIMER_MatchReg mr3_t2 = {.which = 3, .match = 300, .actions = TIM_MR_INT | TIM_MR_RES};
    TIMER_SetMatch(timer2, mr2_t2);
    TIMER_SetMatch(timer2, mr3_t2);

    TIMER_SetInterruptHandler(timer1, TIM_INT_SRC_MR0, t1_mr0_int_handler);
    TIMER_SetInterruptHandler(timer2, TIM_INT_SRC_MR3, t2_mr3_int_handler);

    Power_Init(POWR_CFG_SLEEP_ON_EXIT);
    Power_PowerDownOnWFI();

    WFI();
}