#include "stm32l1xx.h"
#include "types_P.h"
#include "UART_control.h"
#include "delay.h"
#include "ADC_control.h"

uint8_t buff[15];

extern volatile uint32_t inBuff;

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

/*
void AdcEndOfConversationRoutine(uint32_t result)
{
	buff[0] = '=';
	buff[1] = ' ';
	buff[2] = (uint8_t)((result >> 8) & 0xFF);
	buff[3] = (uint8_t)(result & 0xFF);
	buff[4] = '\n';
	TransmitBinaryBufferStart(5, buff);
}
*/

void AdcEndOfConversationRoutine(uint32_t result)
{
	//select output data style: binary or text
	if(inBuff < (uint32_t)0x0000000F)
	{
		buff[0] = 'V';
		buff[1] = 'a';
		buff[2] = 'l';
		buff[3] = 'u';
		buff[4] = 'e';
		buff[5] = ' ';
		buff[6] = '=';
		buff[7] = ' ';
		buff[8] =  '0' + result/10000 % 10;
		buff[9] =  '0' + result/1000 % 10;
		buff[10] = '0' + result/100 % 10;
		buff[11] = '0' + result/10 % 10;
		buff[12] = '0' + result % 10;
		buff[13] = '\n';
		buff[14] = '\0';
		TransmitBufferStart(14, buff);
	}
	else
	{
		buff[0] = (uint8_t)(result & (uint32_t)0xFF);
		TransmitBinaryBufferStart(1, buff);
	}
}
int main(void)
{
	inBuff = (uint32_t)0;
	UART1_Init();
	ADC_Init();

    while(1)
    {
    	ADC_StartConversation();
    	Delay(2000000);
    }
}
