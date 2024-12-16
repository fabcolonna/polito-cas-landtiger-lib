#ifndef __INCLUDES_H
#define __INCLUDES_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "button.h"
#include "debouncer.h"
#include "led.h"
#include "power.h"
#include "delay.h"
#include "timer.h"

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // Needs to be visible in order to become visible by the simulator
#endif

#endif