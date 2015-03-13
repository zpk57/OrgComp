#include "ExtInerruptControl.h"

void InitInerruptPA12(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	GPIOA->MODER &= ~GPIO_MODER_MODER12;

	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR12;
	//pull-up
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR12_0;

	//interrupt mask register
	EXTI->IMR |= EXTI_IMR_MR12;
	//rise
	EXTI->RTSR |= EXTI_RTSR_TR12;
	//fall
	EXTI->FTSR |= EXTI_FTSR_TR12;

	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	//clear
	SYSCFG->EXTICR[4] &= ~SYSCFG_EXTICR4_EXTI12;
	//Connect EXTI line 12 to PA.12
	SYSCFG->EXTICR[4] |= SYSCFG_EXTICR4_EXTI12_PA;

	NVIC_EnableIRQ(EXTI15_10_IRQn); //разрешение прерывания EXTI0
	NVIC_SetPriority(EXTI15_10_IRQn, 1); //задаем приоритет прерывания
}


void EXTI15_10_IRQHandler(void)
{
	if(EXTI->PR & EXTI_PR_PR12)
	{
		if(GPIOA->IDR & GPIO_IDR_IDR_12)
		{
			RisePA12();
		}
		else
		{
			FallPA12();
		}
		EXTI->PR = EXTI_PR_PR12;
	}
	else //non expected interrupt
	{
		EXTI->PR = 0x00ffffff;
	}
}


void __attribute__ ((weak)) RisePA12(void)
{
	//led on
	GPIOA->BSRRL = GPIO_BSRR_BS_1;
}

void __attribute__ ((weak)) FallPA12(void)
{
	//led off
	GPIOA->BSRRH = GPIO_BSRR_BS_1;
}
