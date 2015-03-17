#include "stm32f30x.h"

#include "types_P.h"
#include "led_blink.h"
#include "delay.h"

#define TIM_7_PREDIV		(uint32_t)7972

#define SIG_HIGHT_0			(uint32_t)7
#define SIG_LOW_0			(uint32_t)3
#define SIG_HIGHT_1			(uint32_t)3
#define SIG_LOW_1			(uint32_t)7

uint32_t current_hight;
uint32_t current_low;
uint8_t message;
uint8_t pointer;


void transmited_bit_init()
{
	if(pointer > 7)
	{
		TIM7->CR1 &= ~ TIM_CR1_CEN;
		return;
	}

	if((message >> pointer) & 0b1)
	{
		current_hight = SIG_HIGHT_1;
		current_low = SIG_LOW_1;
	}
	else
	{
		current_hight = SIG_HIGHT_0;
		current_low = SIG_LOW_0;
	}
	pointer++;
}

void TIM7_IRQHandler(void)
{
	//clear flag
	TIM7->SR = ~TIM_SR_UIF;

	if(TIM7->ARR == current_hight)
	{
		TIM7->ARR = current_low;
		GreenHighLedOff();
	}
	else
	{
		transmited_bit_init();
		TIM7->ARR = current_hight;
		GreenHighLedOn();
	}
}

void start_transmit()
{
	pointer = 0;

	//auto reload
	TIM7->ARR = 20;

	GreenHighLedOff();
	//enable
	TIM7->CR1 |= TIM_CR1_CEN;
}

int main(void)
{
	message = 0b11010011;

	ledInit();

	//enable clock
	RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
	//dma course off
	TIM7->DIER &= ~ TIM_DIER_UDE;
	//interrupt enable
	TIM7->DIER |= TIM_DIER_UIE;
	//prescaller
	TIM7->PSC = TIM_7_PREDIV;

	NVIC_SetPriority (TIM7_IRQn, 1);
	NVIC_EnableIRQ (TIM7_IRQn);

	start_transmit();

    while(1)
    {
    	asm volatile ("wfi");
    }
}
