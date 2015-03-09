#ifndef TIM_7_
#define TIM_7_

#include "stm32f30x.h"
#include "types_P.h"

#include "led_blink.h"


ErrorStateEnum Tim7Init(void);
void Tim7InerruptRoutine(void);

#endif
