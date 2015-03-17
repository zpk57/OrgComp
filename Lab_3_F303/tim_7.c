#include "tim_7.h"


ErrorStateEnum Tim7Init(void)
{
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

	return SUCCESS_P;
}


void Tim7InerruptRoutine(void)
{
	RedLedChangeState();
}
