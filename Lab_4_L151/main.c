#include "stm32l1xx.h"
#include "types_P.h"
#include "UART_control.h"
#include "delay.h"
#include "ADC_control.h"

uint8_t buff[5];

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

void AdcEndOfConversationRoutine(uint32_t result)
{
	buff[0] = '=';
	buff[1] = ' ';
	buff[2] = (uint8_t)((result >> 8) & 0xFF);
	buff[3] = (uint8_t)(result & 0xFF);
	buff[4] = '\n';
	TransmitBinaryBufferStart(5, buff);
}

int main(void)
{
	UART1_Init();
	ADC_Init();

    while(1)
    {
    	ADC_StartConversation();
    	Delay(5000000);
    }
}
