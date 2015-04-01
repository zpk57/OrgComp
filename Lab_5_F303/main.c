#include "stm32f30x.h"

#include "types_P.h"
#include "led_blink.h"
#include "delay.h"
#include "UART_control.h"

#define TIM_7_PREDIV		(uint32_t)797	//100ns

#define events_size 100

#define hyst_up 6
#define hyst_down 2

int buttons_hyst[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};	//hysteresis counters
int buttons_map[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};	//last state
int buttons_press_events[events_size];

int events_begin = 0;
int events_end = 0;

uint8_t result_message[40];
int string_ptr = 0;

void process_button(int pos, int value)
{
	if(value)
	{
		if(buttons_hyst[pos] < 7)
		{
			buttons_hyst[pos]++;
			if(buttons_hyst[pos] > 5 && !buttons_map[pos])
			{
				buttons_map[pos] = 1;

				//generate push on pos
				if(events_end < events_size-1)
				{
					buttons_press_events[events_end] = pos;
					events_end++;
				}
			}
		}
	}
	else
	{
		if(buttons_hyst[pos] > 0)
		{
			buttons_hyst[pos]--;
			if(buttons_hyst[pos] < 2 && buttons_map[pos])
			{
				buttons_map[pos] = 0;
				//generate down on pos
			}

		}
	}
}

/*
 * 0	down
 * 1	down
 * 2	~
 * 3	~
 * 4	~
 * 5	~
 * 6	push
 * 7	push
 */

void TIM7_IRQHandler(void)
{
	uint32_t temp1, temp2, temp3;
	//clear flag
	TIM7->SR = ~TIM_SR_UIF;

	GPIOB->ODR = (uint32_t)0;

	GPIOB->BSRR = GPIO_BSRR_BS_13;
		Delay(100);
		temp1 = GPIOA->IDR;
	GPIOB->BSRR = GPIO_BSRR_BR_13;

	GPIOB->BSRR = GPIO_BSRR_BS_14;
		//delay by process previous line
		process_button(0, temp1&3);
		process_button(1, temp1&4);
		process_button(2, temp1&5);
		process_button(3, temp1&6);
		process_button(4, temp1&7);

		temp2 = GPIOA->IDR;
	GPIOB->BSRR = GPIO_BSRR_BR_14;

	GPIOB->BSRR = GPIO_BSRR_BS_15;
		//delay by process previous line
		process_button(5, temp2&3);
		process_button(6, temp2&4);
		process_button(7, temp2&5);
		process_button(8, temp2&6);
		process_button(9, temp2&7);

		temp3 = GPIOA->IDR;

		process_button(10, temp3&3);
		process_button(11, temp3&4);
		process_button(12, temp3&5);
		process_button(13, temp3&6);
		process_button(14, temp3&7);
	GPIOB->BSRR = GPIO_BSRR_BR_15;
}

void tim7_init()
{
	//enable clock
	RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
	//dma course off
	TIM7->DIER &= ~ TIM_DIER_UDE;
	//interrupt enable
	TIM7->DIER |= TIM_DIER_UIE;
	//prescaller
	TIM7->PSC = TIM_7_PREDIV;
	//auto reload
	TIM7->ARR = 9;	//1 ms
	//enable
	TIM7->CR1 |= TIM_CR1_CEN;

	NVIC_SetPriority (TIM7_IRQn, 1);
	NVIC_EnableIRQ (TIM7_IRQn);
}

//read: pa 3,4,5,6,7	(input)
//line: pb 13,14,15		(output)
void port_init()
{
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	GPIOB->MODER &= ~( GPIO_MODER_MODER0 );
	GPIOB->MODER |= ( GPIO_MODER_MODER0_0 );
	GPIOB->OTYPER &= ~( GPIO_OTYPER_OT_0);
	GPIOB->OSPEEDR |= ( GPIO_OSPEEDER_OSPEEDR0 );
	//no pull-up/pull-down
	GPIOB->PUPDR &= ~( GPIO_PUPDR_PUPDR0 );


	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	//input
	GPIOA->MODER &= ~( GPIO_MODER_MODER2 );

	GPIOA->PUPDR &= ~( GPIO_PUPDR_PUPDR2 );
	//pull-down
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR2_1;
	//or pull-up
	//GPIOA->PUPDR |= GPIO_PUPDR_PUPDR2_0;

}

int main(void)
{
	tim7_init();
	UART1_Init();

    while(1)
    {
    	asm volatile ("wfi");
    	if(events_begin < events_end)
    	{
			while(events_begin < events_end)
			{
				if(buttons_press_events[events_begin]<10)
				{
					result_message[string_ptr++] = '0' + buttons_press_events[events_begin];
				}
				else if(buttons_press_events[events_begin] == 10)
				{
					result_message[string_ptr++] = ',';
				}
				else if(buttons_press_events[events_begin] == 11)
				{
					result_message[string_ptr++] = '-';
				}
				else if(buttons_press_events[events_begin] == 12)
				{
					string_ptr = 0;
				}
				else if(buttons_press_events[events_begin] == 13)
				{
					result_message[string_ptr++] = '.';
				}
				else if(buttons_press_events[events_begin] == 14)
				{
					result_message[string_ptr++] = 'e';
				}
				events_begin++;
			}
			//critical section
			TIM7->DIER &= ~TIM_DIER_UIE;
			events_begin = 0;
			events_end = 0;
			TIM7->DIER |= TIM_DIER_UIE;
			//end of critical
			TransmitBufferStart(string_ptr, result_message);
    	}
    }
}
