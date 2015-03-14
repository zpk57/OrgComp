#include "stm32f30x.h"

#include "types_P.h"
#include "UART_control.h"
#include "led_blink.h"
#include "delay.h"

#define TIM_7_PREDIV		(uint32_t)0x1F40
#define TIM_7_COUNTER		(uint32_t)0x3E8

uint8_t stringPointer[] = "Time xx:xx\n";
uint8_t sec, min;

void TIM7_IRQHandler(void)
{
	//clear flag
	TIM7->SR = ~TIM_SR_UIF;

	//calculate time
	++sec;
	if(sec >= 60)
	{
		min++;
		if(min >= 60) min = 0;
		sec=0;
		stringPointer[5] = min/10 + '0';
		stringPointer[6] = min%10 + '0';
		stringPointer[8] = '0';
		stringPointer[9] = '0';
	}
	else
	{
		stringPointer[8] = sec/10 + '0';
		stringPointer[9] = sec%10 + '0';
	}

	GreenHighLedChangeState();
	TransmitBufferStart(11, stringPointer);
}

int main(void)
{
	ledInit();
	UART1_Init();
	sec = 0;
	min = 0;

	RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;

	//dma course off
	TIM7->DIER &= ~ TIM_DIER_UDE;

	//interrupt enable
	TIM7->DIER |= TIM_DIER_UIE;

	//prescaller
	TIM7->PSC = TIM_7_PREDIV;

	//auto reload
	TIM7->ARR = TIM_7_COUNTER;

	//enable
	TIM7->CR1 |= TIM_CR1_CEN;

	NVIC_SetPriority (TIM7_IRQn, 1);
	NVIC_EnableIRQ (TIM7_IRQn);

    while(1)
    {
    	asm volatile ("wfi");
    }
}
