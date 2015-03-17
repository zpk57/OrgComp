#include "stm32f30x.h"

#include "types_P.h"
#include "led_blink.h"
#include "delay.h"

#define TIM_7_PREDIV		(uint32_t)7972

#define SIG_HIGHT			(uint32_t)70
#define SIG_LOW			    (uint32_t)30

void TIM7_IRQHandler(void)
{
	//clear flag
	TIM7->SR = ~TIM_SR_UIF;

	if(TIM7->ARR == SIG_LOW)
	{
		TIM7->ARR = SIG_HIGHT;
		GreenHighLedOn();
	}
	else
	{
		TIM7->ARR = SIG_LOW;
		GreenHighLedOff();
	}
}

int main(void)
{
	ledInit();

	RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;

	//dma course off
	TIM7->DIER &= ~ TIM_DIER_UDE;

	//interrupt enable
	TIM7->DIER |= TIM_DIER_UIE;

	//prescaller
	TIM7->PSC = TIM_7_PREDIV;

	//auto reload
	TIM7->ARR = SIG_LOW;

	//enable
	TIM7->CR1 |= TIM_CR1_CEN;

	NVIC_SetPriority (TIM7_IRQn, 1);
	NVIC_EnableIRQ (TIM7_IRQn);

	GreenHighLedOff();

    while(1)
    {
    	asm volatile ("wfi");
    }
}
