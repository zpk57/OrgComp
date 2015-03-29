#include "stm32l1xx.h"
#include "types_P.h"
#include "UART_control.h"
#include "delay.h"
#include "ExtInerruptControl.h"

uint32_t last_pulse, last_positive, irq_cnt;
uint32_t received_message;
uint8_t result_message[30];
uint8_t received_bits;

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
	//reset counter
	TIM6->CNT = 0;
	//set prescaller
	TIM6->PSC = 15;
	//set limit
	TIM6->ARR = 0xffff;
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

	irq_cnt++;
}

void build_message(void)
{

	result_message[14] = '0' + ((received_message >> 7) & 1);
	result_message[15] = '0' + ((received_message >> 6) & 1);
	result_message[16] = '0' + ((received_message >> 5) & 1);
	result_message[17] = '0' + ((received_message >> 4) & 1);
	result_message[18] = '0' + ((received_message >> 3) & 1);
	result_message[19] = '0' + ((received_message >> 2) & 1);
	result_message[20] = '0' + ((received_message >> 1) & 1);
	result_message[21] = '0' + ((received_message >> 0) & 1);
}

void RisePA12(void)
{
	uint32_t cnt = TIM6->CNT;
	TIM6->CNT = 0;	//clear cnt
	last_pulse = cnt + (irq_cnt << 16);
	irq_cnt = 0;	//clear interrupt counter
	received_bits++;

	received_message = received_message << 1;
	if(last_positive > 5000)
	{
		received_message &= ~((uint32_t)0x1);
	}
	else
	{
		received_message |= 1;
	}

	if(received_bits >= 8)
	{
		build_message();
		TransmitBufferStart(40, result_message);
	}

}

void FallPA12(void)
{
	last_positive = TIM6->CNT + (irq_cnt << 16);
}

int main(void)
{
	LedInit();
	UART1_Init();
	InitInerruptPA12();
	Tim6Init();

	result_message[0] = 'L';
	result_message[1] = 'a';
	result_message[2] = 's';
	result_message[3] = 't';
	result_message[4] = ' ';
	result_message[5] = 'm';
	result_message[6] = 'e';
	result_message[7] = 's';
	result_message[8] = 's';
	result_message[9] = 'a';
	result_message[10] = 'g';
	result_message[11] = 'e';
	result_message[12] = ':';
	result_message[13] = ' ';

	result_message[22] = '\n';

	received_message = 0;
	received_bits = 0;

    while(1)
    {
    	asm volatile ("wfi");
    	//TransmitBufferStart(13, "TEST message.");
    	//Delay(1000000);
    }
}
