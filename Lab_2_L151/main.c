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

    while(1)
    {
    	TransmitBufferStart(13, "TEST message.");
    	Delay(1000000);
    }
}
