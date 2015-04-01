#ifndef led_blink
#define led_blink

#include "stm32f30x.h"

void ledInit(void);

void RedLedOn(void);

void BlueLedOn(void);

void YellowLedOn(void);

void WhiteLedOn(void);

void GreenHighLedOn(void);

void GreenLowLedOn(void);

void RedLedOff(void);

void BlueLedOff(void);

void YellowLedOff(void);

void WhiteLedOff(void);

void GreenHighLedOff(void);

void GreenLowLedOff(void);

void GreenHighLedChangeState(void);

void RedLedChangeState(void);

#endif
