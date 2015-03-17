#include "UART_control.h"

volatile uint32_t inBuff;

volatile static uint8_t outBuff[MAX_TRANSMIT_BUFF_SIZE];

volatile static uint32_t transmitPointer;

volatile static uint32_t dataToSend;

/**
 * @param  none   none.
 */
ErrorStateEnum UART1_Init()
{
	uint32_t tempReg;

	//Enable GPIO "A" port
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	//RX (PA10) init
	GPIOA->MODER &= ~GPIO_MODER_MODER10;
	//alternate function
	GPIOA->MODER |= GPIO_MODER_MODER10_1;
	//speed
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR10;
	//NO pull up/down
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR10;

	//Selection of alternate function for PA10
	GPIOA->AFR[1] &= ~GPIO_AFRH_AFRH10;
	GPIOA->AFR[1] |= (ALTER_FUNC_7 & GPIO_AFRH_AFRH10);


	//TX (PA9) init
	GPIOA->MODER &= ~GPIO_MODER_MODER9;
	//alternate function
	GPIOA->MODER |= GPIO_MODER_MODER9_1;
	//push-pull
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT_9;
	//speed
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR9;
	//pull up
	tempReg = GPIOA->PUPDR;
	tempReg &= ~GPIO_PUPDR_PUPDR9;
	tempReg |= GPIO_PUPDR_PUPDR9_0;
	GPIOA->PUPDR = tempReg;

	//Selection of alternate function for PA9
	GPIOA->AFR[1] &= ~GPIO_AFRH_AFRH9;
	GPIOA->AFR[1] |= (ALTER_FUNC_7 & GPIO_AFRH_AFRH9);


	//UART init
	//USART1 clock enable //Включаем тактирование модуля USART1
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	//Включаем USART1
	USART1->CR1 |= USART_CR1_UE;
	//Длина слова - 8 бит
	USART1->CR1 &= ~USART_CR1_M;
	//1 стоп-бит
	USART1->CR2 &= ~USART_CR2_STOP;
	//oversampling by 16
	USART1->CR1 &= ~USART_CR1_OVER8;

	//baud rate = Fclk(4,194MHz)/(16*USARTDIV)
	//baud rate = 38400
	//USART1->BRR = 0x6D;

	//baud rate = Fclk(16MHz)/(16*USARTDIV)
	//baud rate = 38400
	USART1->BRR = 0x1A1;

	//Разрешаем передачу и приём данных
	USART1->CR1 |=	(	USART_CR1_TE
					|	USART_CR1_RE);


	NVIC_SetPriority (USART1_IRQn, 2);
	NVIC_EnableIRQ (USART1_IRQn);


	//enable USART1 interrupt
	USART1->CR1 |=  (	USART_CR1_RXNEIE);

	return SUCCESS_P;
}

void USART1_IRQHandler(void)
{
	volatile int temp;
	if(USART1->SR & USART_SR_RXNE) //data received
	{
		RecieveBufferInterruptRoutine();
	}
	if(USART1->SR & USART_SR_ORE) //data overrun error
	{
		temp = USART1->DR; //reset ORE flag by reading DR register
	}
	if(USART1->SR & USART_SR_TXE)
	{
		TransmitBufferInterruptRoutine();
	}
}

void TransmitBufferInterruptRoutine(void)
{
	if(transmitPointer < dataToSend)
	{
		USART1->DR = outBuff[transmitPointer];
		transmitPointer++;
	}
	else
	{
		USART1->CR1 &=  ~USART_CR1_TXEIE;
	}
}

void RecieveBufferInterruptRoutine(void)
{
	uint32_t temp;
	temp = (uint32_t)(USART1->DR);
	inBuff = temp;
}

/**
 * @param  stringLengh	String length without null character.
 * @param  strPointer   String pointer.
 */
ErrorStateEnum TransmitBufferStart(uint32_t stringLengh, uint8_t * strPointer)
{
	ErrorStateEnum retStatus = SUCCESS_P;
	uint32_t i;
	transmitPointer = 0;
	if(stringLengh > MAX_TRANSMIT_BUFF_SIZE)
	{
		retStatus = ERROR_P;
		dataToSend = MAX_TRANSMIT_BUFF_SIZE;
		for(i = 0; i < MAX_TRANSMIT_BUFF_SIZE; i++)
		{
			outBuff[i] = (uint8_t)*(strPointer + i);
			if (outBuff[i] == (uint8_t)0)
			{
				dataToSend = (uint32_t)(i + 1);
				break;
			}
		}
	}
	else
	{
		dataToSend = stringLengh;
		for(i = 0; i < dataToSend; i++)
		{
			outBuff[i] = (uint8_t)*(strPointer + i);
			if (outBuff[i] == (uint8_t)0)
			{
				dataToSend = (uint32_t)(i + 1);
				retStatus = ERROR_P;
				break;
			}
		}
	}
	TransmitBufferInterruptRoutine();
	//enable USART1 interrupt
	USART1->CR1 |=  USART_CR1_TXEIE;
	return retStatus;
}

void TransmitBufferErrase(void)
{
	uint32_t i;
	for(i = 0; i < MAX_TRANSMIT_BUFF_SIZE; i++)
	{
		outBuff[i] = (uint8_t)0;
	}
	transmitPointer = (uint32_t)0;
	dataToSend = (uint32_t)0;
}

void RecieveBufferErrase(void)
{
	inBuff = (uint32_t)0;
}
