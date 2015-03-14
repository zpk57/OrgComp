#include "stm32l1xx.h"
#include "types_P.h"
#include "UART_control.h"
#include "delay.h"
#include "ExtInerruptControl.h"

uint32_t last_pulse, last_positive, irq_cnt;
uint8_t result_message[] = "Period = 0000000000; Hight = 0000000000\n";

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
	result_message[9]  = '0' + last_pulse/1000000000 % 10;
	result_message[10] = '0' + last_pulse/100000000 % 10;
	result_message[11] = '0' + last_pulse/10000000 % 10;
	result_message[12] = '0' + last_pulse/1000000 % 10;
	result_message[13] = '0' + last_pulse/100000 % 10;
	result_message[14] = '0' + last_pulse/10000 % 10;
	result_message[15] = '0' + last_pulse/1000 % 10;
	result_message[16] = '0' + last_pulse/100 % 10;
	result_message[17] = '0' + last_pulse/10 % 10;
	result_message[18] = '0' + last_pulse % 10;

	result_message[29] = '0' + last_pulse/1000000000 % 10;
	result_message[30] = '0' + last_pulse/100000000 % 10;
	result_message[31] = '0' + last_pulse/10000000 % 10;
	result_message[32] = '0' + last_pulse/1000000 % 10;
	result_message[33] = '0' + last_pulse/100000 % 10;
	result_message[34] = '0' + last_pulse/10000 % 10;
	result_message[35] = '0' + last_pulse/1000 % 10;
	result_message[36] = '0' + last_pulse/100 % 10;
	result_message[37] = '0' + last_pulse/10 % 10;
	result_message[38] = '0' + last_pulse % 10;
}

void RisePA12(void)
{
	uint32_t cnt = TIM6->CNT;
	TIM6->CNT = 0;	//clear cnt
	last_pulse = cnt + (irq_cnt << 16);

	build_message();
	TransmitBufferStart(40, result_message);

	irq_cnt = 0;	//clear interrupt counter
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

    while(1)
    {
    	asm volatile ("wfi");
    	//TransmitBufferStart(13, "TEST message.");
    	//Delay(1000000);
    }
}
