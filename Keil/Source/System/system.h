#ifndef _SYSTEM_H
#define _SYSTEM_H

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // Needs to be visible in order to become visible by the simulator
#endif

extern void SystemInit();

#endif