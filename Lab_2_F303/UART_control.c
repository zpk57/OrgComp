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

	//initiation of GPIOs (PA10;PA9)

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
	GPIOA->AFR[1] &= ~GPIO_AFRH_AFRH2;
	GPIOA->AFR[1] |= (ALTER_FUNC_7 & GPIO_AFRH_AFRH2);


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
	GPIOA->AFR[1] &= ~GPIO_AFRH_AFRH1;
	GPIOA->AFR[1] |= (ALTER_FUNC_7 & GPIO_AFRH_AFRH1);


	//UART1 init

	//USART1 clock enable
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	//Enable USART1
	USART1->CR1 |= USART_CR1_UE;
	//word length == 8 bit
	USART1->CR1 &= ~USART_CR1_M;
	//1 stop bit
	USART1->CR2 &= ~USART_CR2_STOP;
	//oversampling by 16
	USART1->CR1 &= ~USART_CR1_OVER8;
	//baud rate 19200 ןנט 32לדצ
	USART1->BRR = 0x1A3;
	//Receive, transmit on
	USART1->CR1 |=	(	USART_CR1_TE
					|	USART_CR1_RE);

	//Buffers clear

	TransmitBufferErrase();
	RecieveBufferErrase();

	//Interrupts config

	NVIC_SetPriority (USART1_IRQn, 2);
	NVIC_EnableIRQ (USART1_IRQn);


	//enable USART1 interrupt

	USART1->CR1 |=  (	USART_CR1_RXNEIE);
	//				|	USART_CR1_TXEIE


	return SUCCESS_P;
}

void USART1_IRQHandler(void)
{
	volatile int temp;
	if(USART1->ISR & USART_ISR_RXNE) //data received
	{
		RecieveBufferInterruptRoutine();
	}
	if(USART1->ISR & USART_ISR_ORE) //data overrun error
	{
		USART1->ICR = USART_ICR_ORECF;
		temp = USART1->RDR; //reset ORE flag by reading DR register
	}
	if(USART1->ISR & USART_ISR_TXE)
	{
		TransmitBufferInterruptRoutine();
	}
}

void TransmitBufferInterruptRoutine(void)
{
	if(transmitPointer < dataToSend)
	{
		USART1->TDR = outBuff[transmitPointer];
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
	temp = (uint32_t)(USART1->RDR);
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
