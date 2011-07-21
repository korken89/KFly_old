#include "itg3200.h"

Status ITG3200_Init(void)
{
	uint8_t tx_data[1] = {WHO};
	uint8_t rx_data[1] = {0};
	
	I2C_M_SETUP_Type TransferCfg;
	TransferCfg.sl_addr7bit = ITG3200_ADDR;
	TransferCfg.tx_data = tx_data;
	TransferCfg.tx_length = 1;
	TransferCfg.rx_data = rx_data;
	TransferCfg.rx_length = 1;
	TransferCfg.retransmissions_max = 5;
	TransferCfg.callback = NULL;
	
	return I2C_MasterTransferData(LPC_I2C0, &TransferCfg, I2C_TRANSFER_INTERRUPT);
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
