/******************************************************************************
 * @file :   	itg3200.c
 * @brief : 	ITG-3200 Gyro functions
 * @version : 	V1.0
 * @author :	Emil Fresk
 ******************************************************************************/

#include "itg3200.h"
#include "uart.h"

volatile int pos = 0;
volatile int stat = 0;

void ITG3200_Init(void)
{
	ITG3200_Write(PWR_M, 0x80);				// Reset to defaults
	ITG3200_Write(SMPL, 9);				// SMLPRT_DIV = 10
	ITG3200_Write(DLPF, (3<<3)|(3<<0));		// DLPF_CFG = 0, FS_SEL = 3
	ITG3200_Write(INT_C, INT_CONFIG);		// Generate interrupt when device is ready or raw data ready
	ITG3200_Write(PWR_M, 0x00);				// Normal operating conditions
}

/**
 * @brief 	Reads the X, Y and Z register and wites it to a uint16_t buffer of size 3.
 * 			Example: 	uint16_t buff[3];
 * 						ITG3200_BurstRead(buffer);  <-- buffer will now contain X, Y and Z.
 * 
 */
void ITG3200_BurstRead(int16_t *buffer)
{
	while (!(ITG3200_Read(INT_S) & 0x01));
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
	pos = 1;
	I2C_WaitForSI(LPC_I2C0);
	stat = I2C_ReadStatus(LPC_I2C0);
	//UART0_SendChar(I2C_ReadStatus(LPC_I2C0));		//For debugg purpose. Sends the status of the I2C.

	I2C_ClearSI(LPC_I2C0);
	I2C_Write(LPC_I2C0, ITG3200_W);
	I2C_ClearStart(LPC_I2C0);
	pos = 2;
	I2C_WaitForSI(LPC_I2C0);
	stat = I2C_ReadStatus(LPC_I2C0);
	//UART0_SendChar(I2C_ReadStatus(LPC_I2C0));
	
	I2C_ClearSI(LPC_I2C0);
	I2C_Write(LPC_I2C0, GX_H);
	pos = 3;
	I2C_WaitForSI(LPC_I2C0);
	stat = I2C_ReadStatus(LPC_I2C0);
	//UART0_SendChar(I2C_ReadStatus(LPC_I2C0));
	
	/** Here starts the recieve part **/
	I2C_ClearSI(LPC_I2C0);
	I2C_SendStart(LPC_I2C0);
	pos = 4;
	I2C_WaitForSI(LPC_I2C0);
	stat = I2C_ReadStatus(LPC_I2C0);
	//UART0_SendChar(I2C_ReadStatus(LPC_I2C0));
	
	I2C_ClearSI(LPC_I2C0);
	I2C_Write(LPC_I2C0, ITG3200_R);
	I2C_ClearStart(LPC_I2C0);
	pos = 5;
	I2C_WaitForSI(LPC_I2C0);
	stat = I2C_ReadStatus(LPC_I2C0);
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
		pos = 6 + i;
		I2C_WaitForSI(LPC_I2C0);
		stat = I2C_ReadStatus(LPC_I2C0);
		if (i & 0x01)
			*(buffer + i/2) |= (int16_t)I2C_Read(LPC_I2C0);
		else
			*(buffer + i/2) = ((int16_t)I2C_Read(LPC_I2C0)<<8);		// By not using an OR it will automatically clear the buffer
			
		//UART0_SendChar(I2C_ReadStatus(LPC_I2C0));
	}
	
	I2C_SendStop(LPC_I2C0);
	I2C_ClearSI(LPC_I2C0);
}

uint8_t ITG3200_Read(uint8_t address)
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
	I2C_Write(LPC_I2C0, ITG3200_W);
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
	I2C_Write(LPC_I2C0, ITG3200_R);
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

void ITG3200_Write(uint8_t address, uint8_t data)
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
	I2C_Write(LPC_I2C0, ITG3200_W);
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
