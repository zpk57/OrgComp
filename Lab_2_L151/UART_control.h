#ifndef UART_CONTROL
#define UART_CONTROL

#include "stm32l1xx.h"
#include "types_P.h"

#define ALTER_FUNC_7 0x77777777

#define MAX_TRANSMIT_BUFF_SIZE 30


//		asm volatile ("" ::: "memory");


ErrorStateEnum UART1_Init(void);
ErrorStateEnum TransmitBufferStart(uint32_t stringLengh, uint8_t * strPointer);
void TransmitBufferInterruptRoutine(void);
void RecieveBufferInterruptRoutine(void);
void TransmitBufferErrase(void);
void RecieveBufferErrase(void);


#endif
