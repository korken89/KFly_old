#include "bma180.h"
#include "uart.h"

void BMA180_Init(uint8_t bw, uint8_t range)
{
	uint8_t temp;
	BMA180_Write(SOFT_RESET, 0xB6);
	
	for (volatile int i = 0; i<100000; i++);
	
	// Set ee_w bit
	BMA180_Read(CTRLREG0, &temp);
	temp = temp | 0x10;
	BMA180_Write(CTRLREG0, temp);					// Have to set ee_w to write any other registers
	
	// Set BW
	bw <<= 4;
	BMA180_Read(BWTCS, &temp);
	temp = (temp & (~BW_MASK)) | bw;
	
	// Set Range
	BMA180_Read(OLSB1, &temp);
	temp = (temp & (~RANGE_MASK)) | (range << RANGE_SHIFT);
	BMA180_Write(OLSB1, temp); 						//Write new range data, keep other bits the same
}

Status BMA180_BurstRead(uint8_t *buffer, PFV callback)
{
	uint8_t address = ACCXLSB;
	
	I2C_M_SETUP_Type TransferCfg;
	TransferCfg.sl_addr7bit = BMA180_ADDR;
	TransferCfg.tx_data = &address;
	TransferCfg.tx_length = 1;				// One byte to be sent
	TransferCfg.rx_data = buffer;
	TransferCfg.rx_length = 6;				// Six bytes to be recieved
	TransferCfg.retransmissions_max = 5;
	TransferCfg.callback = callback;
	
	return I2C_MasterTransferData(LPC_I2C0, &TransferCfg, I2C_TRANSFER_POLLING);
}

Status BMA180_Read(uint8_t address, uint8_t *return_data)
{	
	I2C_M_SETUP_Type TransferCfg;
	TransferCfg.sl_addr7bit = BMA180_ADDR;
	TransferCfg.tx_data = &address;
	TransferCfg.tx_length = 1;				// One byte to be sent
	TransferCfg.rx_data = return_data;
	TransferCfg.rx_length = 1;				// One byte to be recieved
	TransferCfg.retransmissions_max = 5;
	TransferCfg.callback = NULL;
	
	return I2C_MasterTransferData(LPC_I2C0, &TransferCfg, I2C_TRANSFER_POLLING);
}

Status BMA180_Write(uint8_t address, uint8_t data)
{
	/**(RA = Register Address)
	 * Procedure: 
	 * Master:	START	ADDRESS+W		  RA		DATA		STOP
	 * Slave:						ACK		   ACK	 	  ACK
	 **/
	 
	uint8_t tx_data[2] = {address, data};
	
	I2C_M_SETUP_Type TransferCfg;
	TransferCfg.sl_addr7bit = BMA180_ADDR;
	TransferCfg.tx_data = tx_data;
	TransferCfg.tx_length = 2;				// Two bytes to be sent
	TransferCfg.rx_data = NULL;
	TransferCfg.rx_length = 0;				// No bytes to be recieved
	TransferCfg.retransmissions_max = 5;
	TransferCfg.callback = NULL;
	
	return I2C_MasterTransferData(LPC_I2C0, &TransferCfg, I2C_TRANSFER_POLLING);
}
