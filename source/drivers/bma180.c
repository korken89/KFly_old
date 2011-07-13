#include "bma180.h"
#include "uart.h"

void BMA180_Init(uint8_t bw, uint8_t range)
{
	uint8_t temp;
	BMA180_Write(SOFT_RESET, 0xB6);
	
	for (volatile int i = 0; i<100000; i++);
	
	// Set ee_w bit
	temp = BMA180_Read(CTRLREG0) | 0x10;
	BMA180_Write(CTRLREG0, temp);					// Have to set ee_w to write any other registers
	
	// Set BW
	bw <<= 4;
	temp = (BMA180_Read(BWTCS) & (~BW_MASK)) | bw;
	
	// Set Range
	temp = (BMA180_Read(OLSB1) & (~RANGE_MASK)) | (range << RANGE_SHIFT);
	BMA180_Write(OLSB1, temp); 						//Write new range data, keep other bits the same
}

void BMA180_BurstRead(int16_t *buffer)
{
	while (!(BMA180_Read(ACCXLSB) & 0x01));	
	uint8_t data;
	/**
	 * Procedure:(RA = Register Address)
	 * Master:	START	ADDRESS+W		  RA		| START	  ADDRESS+R				 ACK  		ACK		  ...  NACK	 STOP
	 * Slave:						ACK		   ACK	|					 ACK   DATA		  DATA		DATA  ...	   
	 **/
	I2C_ClearSI(LPC_I2C0);
	I2C_ClearStart(LPC_I2C0);
	I2C_ClearStop(LPC_I2C0);
	 
	/** Start **/
	I2C_SendStart(LPC_I2C0);
	I2C_WaitForSI(LPC_I2C0);

	//UART0_SendChar(I2C_ReadStatus(LPC_I2C0));		//For debugg purpose. Sends the status of the I2C.

	I2C_ClearSI(LPC_I2C0);
	I2C_Write(LPC_I2C0, BMA180_W);
	I2C_ClearStart(LPC_I2C0);
	I2C_WaitForSI(LPC_I2C0);
	
	//UART0_SendChar(I2C_ReadStatus(LPC_I2C0));
	
	I2C_ClearSI(LPC_I2C0);
	I2C_Write(LPC_I2C0, ACCXLSB);
	I2C_WaitForSI(LPC_I2C0);
	
	//UART0_SendChar(I2C_ReadStatus(LPC_I2C0));
	
	/** Here starts the recieve part **/
	I2C_ClearSI(LPC_I2C0);
	I2C_SendStart(LPC_I2C0);
	I2C_WaitForSI(LPC_I2C0);
	
	//UART0_SendChar(I2C_ReadStatus(LPC_I2C0));
	
	I2C_ClearSI(LPC_I2C0);
	I2C_Write(LPC_I2C0, BMA180_R);
	I2C_ClearStart(LPC_I2C0);
	I2C_WaitForSI(LPC_I2C0);
	
	//UART0_SendChar(I2C_ReadStatus(LPC_I2C0));
	/**
	 * Burst-read X, Y and Z registers and put them in the specified buffer.
	**/	
	for (uint8_t i = 0; i < 6; i++)
	{
		if (i > 4)
			I2C_SendNACK(LPC_I2C0);
		else
			I2C_SendACK(LPC_I2C0);
			
		I2C_ClearSI(LPC_I2C0);
		
		/** The I2C is reciveing the data here... **/

		/** Read **/
		I2C_WaitForSI(LPC_I2C0);
		
		if (i & 0x01)
			*(buffer + i/2) |= ((int16_t)I2C_Read(LPC_I2C0)<<8);	
		else
			*(buffer + i/2) = (int16_t)I2C_Read(LPC_I2C0);			// By not using an OR it will automatically clear the buffer
			
		*(buffer + i/2) >>= 2;
		//UART0_SendChar(I2C_ReadStatus(LPC_I2C0));
	}
	
	I2C_SendStop(LPC_I2C0);
	I2C_ClearSI(LPC_I2C0);
}

uint8_t BMA180_Read(uint8_t address)
{
	uint8_t data;
	/**
	 * Procedure:(RA = Register Address)
	 * Master:	START	ADDRESS+W		  RA		| START	  ADDRESS+R				 NACK  STOP
	 * Slave:						ACK		   ACK	|					 ACK   DATA
	 **/
	I2C_ClearSI(LPC_I2C0);
	I2C_ClearStart(LPC_I2C0);
	I2C_ClearStop(LPC_I2C0);
	 
	/** Start **/
	I2C_SendStart(LPC_I2C0);
	I2C_WaitForSI(LPC_I2C0);
	
	//UART0_SendChar(I2C_ReadStatus(LPC_I2C0));  //For debugg purpose. Returns the status of the I2C.
	
	I2C_ClearSI(LPC_I2C0);
	I2C_Write(LPC_I2C0, BMA180_W);
	I2C_ClearStart(LPC_I2C0);
	I2C_WaitForSI(LPC_I2C0);
	
	
	//UART0_SendChar(I2C_ReadStatus(LPC_I2C0));
	
	I2C_ClearSI(LPC_I2C0);
	I2C_Write(LPC_I2C0, address);
	I2C_WaitForSI(LPC_I2C0);
	
	//UART0_SendChar(I2C_ReadStatus(LPC_I2C0));
	
	/** Here starts the recieve part **/
	I2C_ClearSI(LPC_I2C0);
	I2C_SendStart(LPC_I2C0);
	I2C_WaitForSI(LPC_I2C0);
	
	//UART0_SendChar(I2C_ReadStatus(LPC_I2C0));
	
	I2C_ClearSI(LPC_I2C0);
	I2C_Write(LPC_I2C0, BMA180_R);
	I2C_ClearStart(LPC_I2C0);
	I2C_WaitForSI(LPC_I2C0);
	
	//UART0_SendChar(I2C_ReadStatus(LPC_I2C0));
	
	I2C_SendNACK(LPC_I2C0);
	I2C_ClearSI(LPC_I2C0);
	
	/** The I2C is reciveing the data here... **/
	/** Read **/
	I2C_WaitForSI(LPC_I2C0);
	data = I2C_Read(LPC_I2C0);
	//UART0_SendChar(I2C_ReadStatus(LPC_I2C0));
	I2C_SendStop(LPC_I2C0);
	I2C_ClearSI(LPC_I2C0);
	 
	return data;
}

void BMA180_Write(uint8_t address, uint8_t data)
{
	/**(RA = Register Address)
	 * Procedure: 
	 * Master:	START	ADDRESS+W		  RA		DATA		STOP
	 * Slave:						ACK		   ACK	 	  ACK
	 **/
	I2C_ClearSI(LPC_I2C0);
	I2C_ClearStart(LPC_I2C0);
	I2C_ClearStop(LPC_I2C0);
	 
	/** Start **/
	I2C_SendStart(LPC_I2C0);
	I2C_WaitForSI(LPC_I2C0);
	
	//UART0_SendChar(I2C_ReadStatus(LPC_I2C0));  //For debugg purpose. Sends the status of the I2C.
	 
	I2C_ClearSI(LPC_I2C0);
	I2C_Write(LPC_I2C0, BMA180_W);
	I2C_ClearStart(LPC_I2C0);
	I2C_WaitForSI(LPC_I2C0);
	
	
	//UART0_SendChar(I2C_ReadStatus(LPC_I2C0));
	
	I2C_ClearSI(LPC_I2C0);
	I2C_Write(LPC_I2C0, address);
	I2C_WaitForSI(LPC_I2C0);
	
	//UART0_SendChar(I2C_ReadStatus(LPC_I2C0));
	
	I2C_ClearSI(LPC_I2C0);
	I2C_Write(LPC_I2C0, data);
	I2C_WaitForSI(LPC_I2C0);
	
	//UART0_SendChar(I2C_ReadStatus(LPC_I2C0));
	
	I2C_SendStop(LPC_I2C0);
	I2C_ClearSI(LPC_I2C0);
}
