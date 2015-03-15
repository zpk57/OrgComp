#include "ADC_control.h"

/**
 * @note  you need enable HSI clock before use ADC
 */
ErrorStateEnum ADC_Init(void)
{
	ErrorStateEnum result = SUCCESS_P;
	uint32_t timeout = 0;
	if(RCC->CR & RCC_CR_HSIRDY)
	{
		AdcInitIoB12();

		//clock enable
		RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

		//disable ADC1
		ADC1->CR2 &= ~ADC_CR2_ADON;
		//wait for disable ADC1
		while((ADC1->SR & ADC_SR_ADONS) && (timeout < ADC_ENABLE_TIMEOUT))
		{
			timeout++;
		}
		if(timeout >= ADC_ENABLE_TIMEOUT)
		{
			result = ERROR_P;
		}

		//frequency of the clock to the ADC == HSI divided by 1
		ADC->CCR &= ~ ADC_CCR_ADCPRE;

		//select ADC_IN_18 (GPIO B12) for 1st conversion in regular sequence
		ADC1->SQR5 |= 	( ADC_SQR5_SQ1_1
						| ADC_SQR5_SQ1_4);

		//Single conversion mode
		ADC1->CR2 &= ~ADC_CR2_CONT;

		//12 bit conversation
		ADC1->CR1 &= ~ADC_CR1_RES;

		//right align
		ADC1->CR2 &= ~ADC_CR2_ALIGN;

		//ADC is powered down when waiting for a start event
		ADC1->CR1 |= ADC_CR1_PDI;

		//Interrupt enable for EOC
		ADC1->CR1 |= ADC_CR1_EOCIE;

		NVIC_SetPriority (ADC1_IRQn, 3);
		NVIC_EnableIRQ (ADC1_IRQn);


		timeout = 0;
		//enable ADC1
		ADC1->CR2 |= ADC_CR2_ADON;
		//wait for enable ADC1
		while((!(ADC1->SR & ADC_SR_ADONS)) && (timeout < ADC_ENABLE_TIMEOUT))
		{
			timeout++;
		}
		if(timeout >= ADC_ENABLE_TIMEOUT)
		{
			result = ERROR_P;
		}
	}
	else
	{
		result = ERROR_P;
	}
	return result;
}

/**
 * @note  you need enable HSI clock before use ADC
 */
ErrorStateEnum ADC_StartConversation(void)
{
	if(RCC->CR & RCC_CR_HSIRDY)
	{
		//start conversation
		ADC1->CR2 |= ADC_CR2_SWSTART;

		return SUCCESS_P;
	}
	else
	{
		return ERROR_P;
	}
}


void AdcInitIoB12(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

	//analog io
	GPIOB->MODER |= GPIO_MODER_MODER12;
	//no pull
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR12;
}

void ADC1_IRQHandler(void)
{
	AdcEndOfConversationRoutine(ADC1->DR);
}

void __attribute__ ((weak)) AdcEndOfConversationRoutine(uint32_t result)
{

}
