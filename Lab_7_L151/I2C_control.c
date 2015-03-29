#include "I2C_control.h"


ErrorStateEnum I2C1_Init(void)
{
	RCC->AHBENR |= 		RCC_AHBENR_GPIOBEN;
	RCC->APB1ENR |= 	RCC_APB1ENR_I2C1EN;


	GPIOB->MODER &= ~(	GPIO_MODER_MODER6
					|	GPIO_MODER_MODER7);
	//AF
	GPIOB->MODER |=	(	GPIO_MODER_MODER6_1
					|	GPIO_MODER_MODER7_1);

	GPIOB->PUPDR &= ~(	GPIO_PUPDR_PUPDR6
					|	GPIO_PUPDR_PUPDR7);
	//Pull-up
	GPIOB->PUPDR |=	(	GPIO_PUPDR_PUPDR6_0
					|	GPIO_PUPDR_PUPDR7_0);

	GPIOB->OSPEEDR |= (	GPIO_OSPEEDER_OSPEEDR6
					|	GPIO_OSPEEDER_OSPEEDR7);

	//open-drain
	GPIOB->OTYPER |= (	GPIO_OTYPER_OT_7
					|	GPIO_OTYPER_OT_6);

	GPIOB->AFR[0] &= ~( GPIO_AFRL_AFRL6
					|	GPIO_AFRL_AFRL7);
	//pin 6
	GPIOB->AFR[0] |=	(GPIO_AFRL_AFRL6 & ALTER_FUNC_4);
	//pin 7
	GPIOB->AFR[0] |=	(GPIO_AFRL_AFRL7 & ALTER_FUNC_4);

	//PCLK1 frequency	@CLK=16MHz
	I2C1->CR2 &= ~I2C_CR2_FREQ;
	I2C1->CR2 |= 16;

	//16 ћ√ц / 2 / 100 к√ц = 80;	@CLK=16MHz
	I2C1->CCR |= 80;

	//slow mose (100KHz)
	I2C1->CCR &= ~ I2C_CCR_FS;

	//period == 1/16 MHz = 62.5 ns	@CLK=16MHz
	//rise time of I2C line (see spec) == 1000 ns
	//1000 ns / 62.5 ns = 16 + 1(safe addition) = 17
	I2C1->TRISE = 17;

	//7-bit slave address
	I2C1->OAR1 &= I2C_OAR1_ADDMODE;

	//I2C bus
	I2C1->CR1 &=~ I2C_CR1_SMBUS;

	return SUCCESS_P;
}

void inline I2C1_SendStart(void)
{
	I2C1->CR1 |= I2C_CR1_START;
}

void inline I2C1_SendStop(void)
{
	I2C1->CR1 |= I2C_CR1_STOP;
}

void inline I2C1_Enable(void)
{
	//enable
	I2C1->CR1 |= I2C_CR1_PE;
}

void inline I2C1_Disable(void)
{
	I2C1->CR1 &= ~I2C_CR1_PE;
}


ErrorStateEnum BH1750_StartContinuouslyLowResolution(void)
{
	uint32_t timeout = 0;
	I2C1_Enable();

	I2C1_SendStart();
	//wait for start flag set
	while (!(I2C1->SR1 & I2C_SR1_SB))
	{
		timeout++;
		if(timeout > I2C_FAILURE_TIMEOUT) break;
	}
	(void) I2C1->SR1;

	I2C1->DR = BH1750_I2C_ADDRES_L | I2C_WRITE;

	timeout = 0;

	//wait for address transmit
	while (!(I2C1->SR1 & I2C_SR1_ADDR))
	{
		timeout++;
		if(timeout > I2C_FAILURE_TIMEOUT) break;
	}
	(void) I2C1->SR1;
	(void) I2C1->SR2;

	timeout = 0;

	I2C1->DR = BH1750_CONTINUOUSLY_LOW_RESOLUTION_COMAND;
	//wait for data (command) transmit
	while (!(I2C1->SR1 & I2C_SR1_BTF))
	{
		timeout++;
		if(timeout > I2C_FAILURE_TIMEOUT) break;
	}

	I2C1_SendStop();

	I2C1_Disable();

	return SUCCESS_P; 	//#TODO: написать обработку ошибок
}


/*
ErrorStateEnum BH1750_ReadData(uint32_t * data)
{
	uint32_t timeout = 0;
	uint32_t tempDataBuff;

	//write ASK after read
	I2C1->CR1 |= I2C_CR1_ACK;

	I2C1_Enable();

	I2C1_SendStart();

	//wait for start flag set
	while (!(I2C1->SR1 & I2C_SR1_SB))
	{
		timeout++;
		if(timeout > I2C_FAILURE_TIMEOUT) break;
	}
	timeout = 0;

	I2C1->DR = BH1750_I2C_ADDRES_L | I2C_READ;
	//wait for address transmit
	while (!(I2C1->SR1 & I2C_SR1_ADDR))
	{
		timeout++;
		if(timeout > I2C_FAILURE_TIMEOUT) break;
	}
	(void) I2C1->SR1;
	(void) I2C1->SR2;

	timeout = 0;
	// ожидаем окончани€ приема данных
	while (!(I2C1->SR1 & I2C_SR1_RXNE))
	{
		timeout++;
		if(timeout > I2C_FAILURE_TIMEOUT) break;
	}
	tempDataBuff = ((I2C1->DR) << 8);

	//not write ASK after read
	I2C1->CR1 &= ~ I2C_CR1_ACK;

	timeout = 0;

	// ожидаем окончани€ приема данных
	while (!(I2C1->SR1 & I2C_SR1_RXNE))
	{
		timeout++;
		if(timeout > I2C_FAILURE_TIMEOUT) break;
	}
	tempDataBuff |= I2C1->DR;

	I2C1_SendStop();

	I2C1_Disable();

	return SUCCESS_P; 	//#TODO: написать обработку ошибок
}
*/


void I2C_init(I2C_TypeDef* I2Cx, uint32_t speed)
{
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	I2C_InitTypeDef I2C_InitStructure;
	I2C_StructInit(&I2C_InitStructure);
	I2C_InitStructure.I2C_ClockSpeed = speed;
	I2C_InitStructure.I2C_OwnAddress1 = 1;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_Init(I2Cx, &I2C_InitStructure);
	I2C_Cmd(I2Cx, ENABLE);
}

void I2C_single_write(I2C_TypeDef* I2Cx, uint8_t HW_address, uint8_t addr, uint8_t data)
{
    I2C_GenerateSTART(I2Cx, ENABLE);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)) ;
    I2C_Send7bitAddress(I2Cx, HW_address, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) ;
    I2C_SendData(I2Cx, addr);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) ;
    I2C_SendData(I2Cx, data);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) ;
    I2C_GenerateSTOP(I2Cx, ENABLE);
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY)) ;
}
void I2C_burst_write(I2C_TypeDef* I2Cx, uint8_t HW_address, uint8_t addr, uint8_t n_data, uint8_t* data)
{
    I2C_GenerateSTART(I2Cx, ENABLE);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)) ;
    I2C_Send7bitAddress(I2Cx, HW_address, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) ;
    I2C_SendData(I2Cx, addr);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) ;
    while (n_data--)
    {
        I2C_SendData(I2Cx, *data++);
        while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) ;
    }
    I2C_GenerateSTOP(I2Cx, ENABLE);
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY)) ;
}
uint8_t I2C_single_read(I2C_TypeDef* I2Cx, uint8_t HW_address, uint8_t addr)
{
    uint8_t data;
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY)) ;
    I2C_GenerateSTART(I2Cx, ENABLE);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)) ;
    I2C_Send7bitAddress(I2Cx, HW_address, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) ;
    I2C_SendData(I2Cx, addr);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) ;
    I2C_GenerateSTART(I2Cx, ENABLE);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)) ;
    I2C_Send7bitAddress(I2Cx, HW_address, I2C_Direction_Receiver);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)) ;
    data = I2C_ReceiveData(I2Cx);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)) ;
    I2C_AcknowledgeConfig(I2Cx, DISABLE);
    I2C_GenerateSTOP(I2Cx, ENABLE);
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY)) ;
    return data;
}
void I2C_burst_read(I2C_TypeDef* I2Cx, uint8_t HW_address, uint8_t addr, uint8_t n_data, uint8_t* data)
{
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY)) ;
    I2C_GenerateSTART(I2Cx, ENABLE);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)) ;
    I2C_Send7bitAddress(I2Cx, HW_address, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) ;
    I2C_SendData(I2Cx, addr);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) ;
    I2C_GenerateSTOP(I2Cx, ENABLE);
    I2C_GenerateSTART(I2Cx, ENABLE);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)) ;
    I2C_Send7bitAddress(I2Cx, HW_address, I2C_Direction_Receiver);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) ;
    I2C_AcknowledgeConfig(I2Cx, ENABLE);
    while (n_data--)
    {
        if (!n_data) I2C_AcknowledgeConfig(I2Cx, DISABLE);
        while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)) ;
        *data++ = I2C_ReceiveData(I2Cx);
    }
    I2C_AcknowledgeConfig(I2Cx, DISABLE);
    I2C_GenerateSTOP(I2Cx, ENABLE);
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY)) ;
}

/*
ErrorStateEnum BH1750_StartContinuouslyLowResolution(void)
{
    I2C_GenerateSTART(I2C1, ENABLE);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) ;
    I2C_Send7bitAddress(I2C1, BH1750_I2C_ADDRES_L, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) ;
    I2C_SendData(I2C1, BH1750_CONTINUOUSLY_LOW_RESOLUTION_COMAND);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) ;
    I2C_GenerateSTOP(I2C1, ENABLE);
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)) ;
}*/


ErrorStateEnum BH1750_ReadData(uint32_t * dataRec)
{
    uint8_t data;
    uint32_t tempDataBuff;

    I2C1_Enable();

    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)) ;
    I2C_GenerateSTART(I2C1, ENABLE);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) ;
    I2C_Send7bitAddress(I2C1, BH1750_I2C_ADDRES_L, I2C_Direction_Receiver);
    I2C_AcknowledgeConfig(I2C1, ENABLE);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)) ;
    data = I2C_ReceiveData(I2C1);
    tempDataBuff = (data << 8);
    I2C_AcknowledgeConfig(I2C1, DISABLE);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)) ;
    data = I2C_ReceiveData(I2C1);
    tempDataBuff |= data;
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)) ;
//    I2C_AcknowledgeConfig(I2C1, DISABLE);
    I2C_GenerateSTOP(I2C1, ENABLE);
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)) ;
    *dataRec = tempDataBuff;

    I2C1_Disable();
}


