#include "i2c.h"

void I2C0_Init(void)   
{ 
	LPC_PINCON->PINSEL1 |= ((1<<22)|(1<<24));			// Set P0.27-28 as SDA0,SCL0
	LPC_PINCON->I2CPADCFG = 0;							// Normal mode
	
	LPC_SC->PCONP |= (1 << PCI2C0);						// Setup power to I2C0
	LPC_SC->PCLKSEL0 |= (1 << PCLK_I2C0);				// PCLK = 100MHz
	
	LPC_I2C0->I2SCLH = 125;								// PCLK/(125+125) = 400kHz
	LPC_I2C0->I2SCLL = 125;
	
	LPC_I2C0->I2CONCLR = (1<<AA)|(1<<STA)|(1<<I2EN);	// Clear I2C controll bits
	LPC_I2C0->I2CONSET = (1<<I2EN);						// Enable I2C0
	//NVIC_EnableIRQ(I2C0_IRQn);							// Enable the interrupt
	
}

void I2C_SendStart(LPC_I2C_TypeDef *I2Cx)
{
	I2Cx->I2CONSET = (1<<STA);
}

void I2C_SendStop(LPC_I2C_TypeDef *I2Cx)
{
	I2Cx->I2CONSET = (1<<STO);
}

void I2C_SendACK(LPC_I2C_TypeDef *I2Cx)
{
	I2Cx->I2CONSET = (1<<AA);
}

void I2C_SendNACK(LPC_I2C_TypeDef *I2Cx)
{
	I2Cx->I2CONCLR = (1<<AA);
}

void I2C_ClearStart(LPC_I2C_TypeDef *I2Cx)
{
	I2Cx->I2CONCLR = (1<<STA);
}

void I2C_ClearStop(LPC_I2C_TypeDef *I2Cx)
{
	I2Cx->I2CONCLR = (1<<STO);
}

void I2C_ClearSI(LPC_I2C_TypeDef *I2Cx)
{
	I2Cx->I2CONCLR = (1<<SI);
}

void I2C_WaitForSI(LPC_I2C_TypeDef *I2Cx)
{
	while (!(I2Cx->I2CONSET & (1<<SI)));
}

int I2C_CheckForSI(LPC_I2C_TypeDef *I2Cx)
{
	return (I2Cx->I2CONSET & (1<<SI));
}

void I2C_Write(LPC_I2C_TypeDef *I2Cx, uint8_t data)
{
	I2Cx->I2DAT = data;
}

uint8_t I2C_Read(LPC_I2C_TypeDef *I2Cx)
{
	return (uint8_t)(I2Cx->I2DAT & 0xFF);
}

uint8_t I2C_ReadStatus(LPC_I2C_TypeDef *I2Cx)
{
	return (uint8_t)(I2Cx->I2STAT & 0xFF);
}

void I2C0_IRQHandler(void)
{
}
