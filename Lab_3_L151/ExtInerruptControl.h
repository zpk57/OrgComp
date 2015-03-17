#ifndef EXT_INT_CONTROL
#define EXT_INT_CONTROL

#include "stm32l1xx.h"
#include "types_P.h"

void InitInerruptPA12(void);

void __attribute__ ((weak)) RisePA12(void);

void __attribute__ ((weak)) FallPA12(void);

#endif
