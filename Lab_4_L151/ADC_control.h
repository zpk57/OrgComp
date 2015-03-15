#ifndef ADC_control
#define ADC_control

#include "stm32l1xx.h"
#include "types_P.h"

#define ALTER_FUNC_7 0x77777777

#define ADC_ENABLE_TIMEOUT		0x0FFFFFFF
#define ADC_DISABLE_TIMEOUT		0x0FFFFFFF

ErrorStateEnum ADC_Init(void);
ErrorStateEnum ADC_StartConversation(void);

void AdcInitIoB12(void);

void __attribute__ ((weak)) AdcEndOfConversationRoutine(uint32_t result);

#endif
