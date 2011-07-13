#ifndef __I2C_H_
#define __I2C_H_

#include "LPC17xx.h"
#include "bitvals.h"

#define AA				2
#define SI				3
#define STO				4
#define STA				5
#define I2EN			6

void I2C0_Init(void);
void I2C_SendStart(LPC_I2C_TypeDef *);
void I2C_SendStop(LPC_I2C_TypeDef *);
void I2C_SendACK(LPC_I2C_TypeDef *);
void I2C_SendNACK(LPC_I2C_TypeDef *);
void I2C_ClearStart(LPC_I2C_TypeDef *);
void I2C_ClearStop(LPC_I2C_TypeDef *);
void I2C_ClearSI(LPC_I2C_TypeDef *);
void I2C_WaitForSI(LPC_I2C_TypeDef *);
void I2C_Write(LPC_I2C_TypeDef *, uint8_t);
uint8_t I2C_Read(LPC_I2C_TypeDef *);
uint8_t I2C_ReadStatus(LPC_I2C_TypeDef *);
void I2C0_IRQHandler(void);

#endif
