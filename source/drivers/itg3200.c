/******************************************************************************
 * @file :   	itg3200.c
 * @brief : 	ITG-3200 Gyro functions
 * @version : 	V1.0
 * @author :	Emil Fresk
 ******************************************************************************/

#include "itg3200.h"
#include "uart.h"

void ITG3200_Init(void)
{
	uint8_t status[7] = {0};
	uint8_t tx_data[3] = {WHO, 0, 0};
	uint8_t rx_data[3] = {0, 0, 0};
	//NVIC_EnableIRQ(I2C0_IRQn);
	I2C_M_SETUP_Type TransferCfg;
	TransferCfg.sl_addr7bit = ITG3200_ADDR;
	TransferCfg.tx_data = tx_data;
	TransferCfg.tx_length = 1;
	TransferCfg.rx_data = rx_data;
	TransferCfg.rx_length = 1;
	TransferCfg.retransmissions_max = 5;
	TransferCfg.callback = NULL;
	
	if (I2C_MasterTransferData(LPC_I2C0, &TransferCfg, I2C_TRANSFER_INTERRUPT) == SUCCESS)
		UART0_SendChar('J');
	else
		UART0_SendChar('N');
	
	
	/*status[0] = I2C_Start(LPC_I2C0);
	status[1] = I2C_SendByte(LPC_I2C0, ITG3200_W);
	status[2] = I2C_SendByte(LPC_I2C0, WHO);
	status[3] = I2C_Start(LPC_I2C0);
	status[4] = I2C_SendByte(LPC_I2C0, ITG3200_R);
	status[5] = I2C_GetByte(LPC_I2C0, &data, FALSE);
	I2C_Stop(LPC_I2C0);
	UART0_SendString(status);*/
	for (volatile int i = 0; i < 1000000; i++);
	
	UART0_SendString(rx_data);
}

void ITG3200_BurstRead(int16_t *buffer)
{
}

uint8_t ITG3200_Read(uint8_t address)
{
}

void ITG3200_Write(uint8_t address, uint8_t data)
{
}
