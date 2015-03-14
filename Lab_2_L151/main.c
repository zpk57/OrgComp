#include "stm32l1xx.h"
#include "types_P.h"
#include "UART_control.h"
#include "delay.h"
#include "ExtInerruptControl.h"

void LedInit(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	GPIOA->MODER &= ~GPIO_MODER_MODER1;	//bug
	GPIOA->MODER &= ~GPIO_MODER_MODER1;
	GPIOA->MODER |=  GPIO_MODER_MODER1_0;
	GPIOA->OTYPER &= ~GPIO_OTYPER_IDR_1;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR1;
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR1;
}


void Tim6Init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	//Update enable (UDIS == 0)
	//TIM6->CR1 &= ~TIM_CR1_UDIS;
	//clear to reset value
	TIM6->CR2 = 0;
	//set prescaller
	TIM6->PSC = 0x1061;
	//set limit
	TIM6->ARR = 0x3E8;
	//enable interrupt, disable DMA request
	TIM6->DIER = TIM_DIER_UIE & (~TIM_DIER_UDE);

	NVIC_SetPriority (TIM6_IRQn, 3);
	NVIC_EnableIRQ (TIM6_IRQn);

	//enable TIM6
	TIM6->CR1 |= TIM_CR1_CEN;
}

void TIM6_IRQHandler(void)
{
	//clear UIF
	TIM6->SR &= ~TIM_SR_UIF;

	//Do smth
	//Tim6InterruptRoutine();
}

//Place your code here

/*
void RisePA12(void)
{
	//Do smth
}

void FallPA12(void)
{
	//Do smth
}
*/

int main(void)
{
	LedInit();
	UART1_Init();
	InitInerruptPA12();
	Tim6Init();

    while(1)
    {
    	TransmitBufferStart(13, "TEST message.");
    	Delay(1000000);
    }
}
