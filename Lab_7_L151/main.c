#include "stm32l1xx.h"
#include "types_P.h"
#include "UART_control.h"
#include "I2C_control.h"
#include "delay.h"

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

int main(void)
{
	uint32_t inBuff;
	uint8_t outBuff[2];
	LedInit();
	UART1_Init();
	I2C1_Init();
	I2C_init(I2C1, 100000);
	BH1750_StartContinuouslyLowResolution();
    while(1)
    {
    	GPIOA->ODR ^= GPIO_ODR_ODR_1;
    	BH1750_ReadData(&inBuff);
    	outBuff[0] = (inBuff >= 4095) ? (uint8_t)0xFF : (uint8_t)((inBuff >> 4) & (0xFF));
//    	outBuff[0] = (uint8_t)(inBuff / 257);
//    	outBuff[0] = (uint8_t)((inBuff >> 4) & (0xFF));
//    	outBuff[1] = (uint8_t)((inBuff) & (0xFF));
    	TransmitBinaryBufferStart(1, outBuff);
    	Delay(100000);
    }
}
