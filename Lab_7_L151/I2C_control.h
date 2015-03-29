#ifndef I2C_CONTROL
#define I2C_CONTROL

#include "stm32l1xx.h"
#include "stm32l1xx_i2c.h"
#include "types_P.h"

#define ALTER_FUNC_4 0x44444444

#define I2C_WRITE	0x0
#define I2C_READ	0x1

#define I2C_FAILURE_TIMEOUT 0xFFFFFFFF

#define BH1750_I2C_ADDRES_L (0x23 << 1)
#define BH1750_I2C_ADDRES_L_NOT_SHIFT (0x23 << 1)
#define BH1750_CONTINUOUSLY_LOW_RESOLUTION_COMAND 0x13



ErrorStateEnum I2C1_Init(void);

void inline I2C1_SendStart(void);

void inline I2C1_SendStop(void);

void inline I2C1_Enable(void);

void inline I2C1_Disable(void);


//high level api functions

ErrorStateEnum BH1750_StartContinuouslyLowResolution(void);

ErrorStateEnum BH1750_ReadData(uint32_t * data);

void I2C_init(I2C_TypeDef* I2Cx, uint32_t speed);
void I2C_single_write(I2C_TypeDef* I2Cx, uint8_t HW_address, uint8_t addr, uint8_t data);
void I2C_burst_write(I2C_TypeDef* I2Cx, uint8_t HW_address, uint8_t addr, uint8_t n_data, uint8_t* data);
uint8_t I2C_single_read(I2C_TypeDef* I2Cx, uint8_t HW_address, uint8_t addr);
void I2C_burst_read(I2C_TypeDef* I2Cx, uint8_t HW_address, uint8_t addr, uint8_t n_data, uint8_t* data);

#endif
