#include "itg3200.h"

void ITG3200_Init(void)
{
	ITG3200_Write(PWR_M, 0x80);						// Reset to defaults
	ITG3200_Write(SMPL, 3);							// SMLPRT_DIV = 7 -> 1kHz/8 = 125Hz > UPDATE_RATE
	ITG3200_Write(DLPF, (2<<DLPF_CFG)|(3<<FS_SEL));	// DLPF at 100Hz
	ITG3200_Write(INT_C, INT_CONFIG);				// Generate interrupt when device is ready or raw data ready (not used)
	ITG3200_Write(PWR_M, 0x00);						// Return to normal operation
}

Status ITG3200_BurstRead(uint8_t *buffer, PFV callback)
{
	uint8_t address = GX_H;
	
	I2C_M_SETUP_Type TransferCfg;
	TransferCfg.sl_addr7bit = ITG3200_ADDR;
	TransferCfg.tx_data = &address;
	TransferCfg.tx_length = 1;				// One byte to be sent
	TransferCfg.rx_data = buffer;
	TransferCfg.rx_length = 6;				// Six bytes to be recieved
	TransferCfg.retransmissions_max = 5;
	TransferCfg.callback = callback;
	
	return I2C_MasterTransferData(LPC_I2C0, &TransferCfg, I2C_TRANSFER_POLLING);
}

Status ITG3200_Read(uint8_t address, uint8_t *return_data)
{	
	I2C_M_SETUP_Type TransferCfg;
	TransferCfg.sl_addr7bit = ITG3200_ADDR;
	TransferCfg.tx_data = &address;
	TransferCfg.tx_length = 1;				// One byte to be sent
	TransferCfg.rx_data = return_data;
	TransferCfg.rx_length = 1;				// One byte to be recieved
	TransferCfg.retransmissions_max = 5;
	TransferCfg.callback = NULL;
	
	return I2C_MasterTransferData(LPC_I2C0, &TransferCfg, I2C_TRANSFER_POLLING);
}

Status ITG3200_Write(uint8_t address, uint8_t data)
{
	uint8_t tx_data[2] = {address, data};
	
	I2C_M_SETUP_Type TransferCfg;
	TransferCfg.sl_addr7bit = ITG3200_ADDR;
	TransferCfg.tx_data = tx_data;
	TransferCfg.tx_length = 2;				// Two bytes to be sent
	TransferCfg.rx_data = NULL;
	TransferCfg.rx_length = 0;				// No bytes to be recieved
	TransferCfg.retransmissions_max = 5;
	TransferCfg.callback = NULL;
	
	return I2C_MasterTransferData(LPC_I2C0, &TransferCfg, I2C_TRANSFER_POLLING);
}
