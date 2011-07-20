#include "i2c.h"

/* Private functions */
static uint32_t I2C_Start(LPC_I2C_TypeDef *);
static void I2C_Stop(LPC_I2C_TypeDef *);
static int8_t I2C_getNum(LPC_I2C_TypeDef *);
static uint32_t I2C_SendByte (LPC_I2C_TypeDef *, uint8_t);
static uint32_t I2C_GetByte (LPC_I2C_TypeDef *, uint8_t *, Bool);

/* Private struct */
typedef struct
{
  uint32_t	txrx_setup;		 						/* Transmission setup */
  int32_t	dir;									/* Current direction phase, 0 - write, 1 - read */
  void		(*inthandler)(LPC_I2C_TypeDef *I2Cx);   /* Transmission interrupt handler */
} I2C_CFG_T;

static I2C_CFG_T i2cdat[3];

void I2C0_Init(void)   
{ 
	LPC_PINCON->PINSEL1 |= ((1<<22)|(1<<24));			// Set P0.27-28 as SDA0,SCL0
	LPC_PINCON->I2CPADCFG = 0;							// Normal mode
	
	LPC_SC->PCONP |= (1<<PCI2C0);						// Setup power to I2C0
	LPC_SC->PCLKSEL0 |= (1<<PCLK_I2C0);					// PCLK = 100MHz
	
	LPC_I2C0->I2SCLH = 125;								// PCLK/(125+125) = 400kHz
	LPC_I2C0->I2SCLL = 125;
	
	LPC_I2C0->I2CONCLR = (1<<AA)|(1<<STA)|(1<<I2EN);	// Clear I2C controll bits
	I2C_Cmd(LPC_I2C0, ENABLE);							// Enable I2C0	
}

/***********************************************************************
 * @brief		Generate a start condition on I2C bus (in master mode only)
 * @param[in]	I2Cx I2C peripheral selected, should be I2C0, I2C1 or I2C2
 * @return 		I2Cx statuscode
 **********************************************************************/
static uint32_t I2C_Start(LPC_I2C_TypeDef *I2Cx)
{
	I2Cx->I2CONCLR = I2C_I2CONCLR_SIC;
	I2Cx->I2CONSET = I2C_I2CONSET_STA;

	// Wait for complete
	while (!(I2Cx->I2CONSET & I2C_I2CONSET_SI));
	I2Cx->I2CONCLR = I2C_I2CONCLR_STAC;
	return (I2Cx->I2STAT & I2C_STAT_CODE_BITMASK);
}

/***********************************************************************
 * @brief		Generate a stop condition on I2C bus (in master mode only)
 * @param[in]	I2Cx I2C peripheral selected, should be I2C0, I2C1 or I2C2
 * @return 		None
 **********************************************************************/
static void I2C_Stop(LPC_I2C_TypeDef *I2Cx)
{
	/* Make sure start bit is not active */
	if (I2Cx->I2CONSET & I2C_I2CONSET_STA)
		I2Cx->I2CONCLR = I2C_I2CONCLR_STAC;
	
	I2Cx->I2CONSET = I2C_I2CONSET_STO;
	I2Cx->I2CONCLR = I2C_I2CONCLR_SIC;
}

/***********************************************************************
 * @brief		Send a byte to the I2C
 * @param[in]	I2Cx I2C peripheral selected, should be I2C0, I2C1 or I2C2
 * @param[in]	Databyte to be sent
 * @return 		I2Cx statuscode
 **********************************************************************/
static uint32_t I2C_SendByte (LPC_I2C_TypeDef *I2Cx, uint8_t databyte)
{
	/* Make sure start bit is not active */
	if (I2Cx->I2CONSET & I2C_I2CONSET_STA)
		I2Cx->I2CONCLR = I2C_I2CONCLR_STAC;
	
	I2Cx->I2DAT = databyte & I2C_I2DAT_BITMASK;
	I2Cx->I2CONCLR = I2C_I2CONCLR_SIC;

	while (!(I2Cx->I2CONSET & I2C_I2CONSET_SI));
	return (I2Cx->I2STAT & I2C_STAT_CODE_BITMASK);
}

/***********************************************************************
 * @brief		Get a byte from the I2C
 * @param[in]	I2Cx I2C peripheral selected, should be I2C0, I2C1 or I2C2
 * @param[in]	retdat is the storage buffer
 * @param[in]	ack to TRUE will generate an ACK, FALSE will generate a NACK
 * @return 		I2Cx statuscode
 **********************************************************************/
static uint32_t I2C_GetByte (LPC_I2C_TypeDef *I2Cx, uint8_t *retdat, Bool ack)
{
	if (ack == TRUE)
		I2Cx->I2CONSET = I2C_I2CONSET_AA;
	else
		I2Cx->I2CONCLR = I2C_I2CONCLR_AAC;
	
	I2Cx->I2CONCLR = I2C_I2CONCLR_SIC;

	while (!(I2Cx->I2CONSET & I2C_I2CONSET_SI));
	*retdat = (uint8_t) (I2Cx->I2DAT & I2C_I2DAT_BITMASK);
	return (I2Cx->I2STAT & I2C_STAT_CODE_BITMASK);
}

/***********************************************************************
 * @brief		Enable or disable I2C peripheral's operation
 * @param[in]	I2Cx I2C peripheral selected, should be I2C0, I2C1 or I2C2
 * @param[in]	NewState New State of I2Cx peripheral's operation
 * @return 		none
 **********************************************************************/
void I2C_Cmd(LPC_I2C_TypeDef *I2Cx, FunctionalState NewState)
{
	if (NewState == ENABLE)
		I2Cx->I2CONSET = I2C_I2CONSET_I2EN;
	else
		I2Cx->I2CONCLR = I2C_I2CONCLR_I2ENC;
}

/***********************************************************************
 * @brief 		Transmit and Receive data in master mode
 * @param[in]	TransferCfg		Pointer to a I2C_M_SETUP_Type structure that
 * 								contains specified information about the
 * 								configuration for master transfer.
 * @param[in]	Opt				a I2C_TRANSFER_OPT_Type type that selected for
 * 								interrupt or polling mode.
 * @return 		SUCCESS or ERROR
 *
 * Note:
 * - In case of using I2C to transmit data only, either transmit length set to 0
 * or transmit data pointer set to NULL.
 * - In case of using I2C to receive data only, either receive length set to 0
 * or receive data pointer set to NULL.
 * - In case of using I2C to transmit followed by receive data, transmit length,
 * transmit data pointer, receive length and receive data pointer should be set
 * corresponding.
 **********************************************************************/
Status I2C_MasterTransferData(LPC_I2C_TypeDef *I2Cx, I2C_M_SETUP_Type *TransferCfg, I2C_TRANSFER_OPT_Type Opt)
{
	uint8_t *txdat;
	uint8_t *rxdat;
	uint32_t CodeStatus;
	uint8_t tmp;

	// reset all default state
	txdat = (uint8_t *) TransferCfg->tx_data;
	rxdat = (uint8_t *) TransferCfg->rx_data;
	// Reset I2C setup value to default state
	TransferCfg->tx_count = 0;
	TransferCfg->rx_count = 0;
	TransferCfg->status = 0;

	if (Opt == I2C_TRANSFER_POLLING)
	{
		/* First Start condition -------------------------------------------------------------- */
		TransferCfg->retransmissions_count = 0;
retry:
		// reset all default state
		txdat = (uint8_t *) TransferCfg->tx_data;
		rxdat = (uint8_t *) TransferCfg->rx_data;
		// Reset I2C setup value to default state
		TransferCfg->tx_count = 0;
		TransferCfg->rx_count = 0;
		CodeStatus = 0;

		// Start command
		CodeStatus = I2C_Start(I2Cx);
		if ((CodeStatus != I2C_I2STAT_M_TX_START) && (CodeStatus != I2C_I2STAT_M_TX_RESTART))
		{
			TransferCfg->retransmissions_count++;
			if (TransferCfg->retransmissions_count > TransferCfg->retransmissions_max)
			{
				// save status
				TransferCfg->status = CodeStatus;
				goto error;
			}
			else
				goto retry;
		}

		/* In case of sending data first --------------------------------------------------- */
		if ((TransferCfg->tx_length != 0) && (TransferCfg->tx_data != NULL))
		{
			/* Send slave address + WR direction bit = 0 ----------------------------------- */
			CodeStatus = I2C_SendByte(I2Cx, (TransferCfg->sl_addr7bit << 1));
			if (CodeStatus != I2C_I2STAT_M_TX_SLAW_ACK)
			{
				TransferCfg->retransmissions_count++;
				if (TransferCfg->retransmissions_count > TransferCfg->retransmissions_max)
				{
					// save status
					TransferCfg->status = CodeStatus | I2C_SETUP_STATUS_NOACKF;
					goto error;
				}
				else
					goto retry;
			}

			/* Send a number of data bytes ---------------------------------------- */
			while (TransferCfg->tx_count < TransferCfg->tx_length)
			{
				CodeStatus = I2C_SendByte(I2Cx, *txdat);
				if (CodeStatus != I2C_I2STAT_M_TX_DAT_ACK)
				{
					TransferCfg->retransmissions_count++;
					if (TransferCfg->retransmissions_count > TransferCfg->retransmissions_max)
					{
						// save status
						TransferCfg->status = CodeStatus | I2C_SETUP_STATUS_NOACKF;
						goto error;
					}
					else
						goto retry;
				}

				txdat++;
				TransferCfg->tx_count++;
			}
		}

		/* Second Start condition (Repeat Start) ------------------------------------------- */
		if ((TransferCfg->tx_length != 0) && (TransferCfg->tx_data != NULL) \
				&& (TransferCfg->rx_length != 0) && (TransferCfg->rx_data != NULL))
		{

			CodeStatus = I2C_Start(I2Cx);
			if ((CodeStatus != I2C_I2STAT_M_RX_START) && (CodeStatus != I2C_I2STAT_M_RX_RESTART))
			{
				TransferCfg->retransmissions_count++;
				if (TransferCfg->retransmissions_count > TransferCfg->retransmissions_max)
				{
					// Update status
					TransferCfg->status = CodeStatus;
					goto error;
				}
				else
					goto retry;
			}
		}

		/* Then, start reading after sending data -------------------------------------- */
		if ((TransferCfg->rx_length != 0) && (TransferCfg->rx_data != NULL))
		{
			/* Send slave address + RD direction bit = 1 ----------------------------------- */
			CodeStatus = I2C_SendByte(I2Cx, ((TransferCfg->sl_addr7bit << 1) | 0x01));
			if (CodeStatus != I2C_I2STAT_M_RX_SLAR_ACK)
			{
				TransferCfg->retransmissions_count++;
				if (TransferCfg->retransmissions_count > TransferCfg->retransmissions_max)
				{
					// update status
					TransferCfg->status = CodeStatus | I2C_SETUP_STATUS_NOACKF;
					goto error;
				}
				else
					goto retry;
			}

			/* Receive a number of data bytes ------------------------------------------------- */
			while (TransferCfg->rx_count < TransferCfg->rx_length)
			{
				/*
				 * Note that: if data length is only one, the master should not
				 * issue an ACK signal on bus after reading to avoid of next data frame
				 * on slave side
				 */
				if (TransferCfg->rx_count < (TransferCfg->rx_length - 1))
				{
					// Issue an ACK signal for next data frame
					CodeStatus = I2C_GetByte(I2Cx, &tmp, 1);
					if (CodeStatus != I2C_I2STAT_M_RX_DAT_ACK)
					{
						TransferCfg->retransmissions_count++;
						if (TransferCfg->retransmissions_count > TransferCfg->retransmissions_max)
						{
							// update status
							TransferCfg->status = CodeStatus;
							goto error;
						}
						else
							goto retry;
					}
				}
				else
				{
					// Do not issue an ACK signal
					CodeStatus = I2C_GetByte(I2Cx, &tmp, 0);
					if (CodeStatus != I2C_I2STAT_M_RX_DAT_NACK)
					{
						TransferCfg->retransmissions_count++;
						if (TransferCfg->retransmissions_count > TransferCfg->retransmissions_max)
						{
							// update status
							TransferCfg->status = CodeStatus;
							goto error;
						}
						else
							goto retry;
					}
				}
				*rxdat++ = tmp;
				TransferCfg->rx_count++;
			}
		}

		/* Send STOP condition ------------------------------------------------- */
		I2C_Stop(I2Cx);
		return SUCCESS;

error:
		// Send stop condition
		I2C_Stop(I2Cx);
		return ERROR;
	}

	else if (Opt == I2C_TRANSFER_INTERRUPT)
	{
		// Setup tx_rx data, callback and interrupt handler
		tmp = I2C_getNum(I2Cx);
		i2cdat[tmp].txrx_setup = (uint32_t) TransferCfg;
		i2cdat[tmp].inthandler = I2C_MasterHandler;
		// Set direction phase, write first
		i2cdat[tmp].dir = 0;

		/* First Start condition -------------------------------------------------------------- */
		I2Cx->I2CONCLR = I2C_I2CONCLR_SIC;
		I2Cx->I2CONSET = I2C_I2CONSET_STA;
		I2C_IntCmd(I2Cx, 1);

		return SUCCESS;
	}

	return ERROR;
}

/***********************************************************************
 * @brief 		Enable/Disable interrupt for I2C peripheral
 * @param[in]	I2Cx	I2C peripheral selected, should be I2C0, I2C1 or I2C2
 * @param[in]	NewState	New State of I2C peripheral interrupt in NVIC core
 * 							should be:
 * 							- ENABLE: enable interrupt for this I2C peripheral
 * 							- DISABLE: disable interrupt for this I2C peripheral
 * @return 		None
 **********************************************************************/
void I2C_IntCmd(LPC_I2C_TypeDef *I2Cx, FunctionalState NewState)
{
	if (NewState)
	{
		if(I2Cx == LPC_I2C0)
			NVIC_EnableIRQ(I2C0_IRQn);
		
		else if (I2Cx == LPC_I2C1)
			NVIC_EnableIRQ(I2C1_IRQn);
		
		else if (I2Cx == LPC_I2C2)
			NVIC_EnableIRQ(I2C2_IRQn);
	}
	else
	{
		if(I2Cx == LPC_I2C0)
			NVIC_DisableIRQ(I2C0_IRQn);
			
		else if (I2Cx == LPC_I2C1)
			NVIC_DisableIRQ(I2C1_IRQn);

		else if (I2Cx == LPC_I2C2)
			NVIC_DisableIRQ(I2C2_IRQn);
	}
}

/***********************************************************************
 * @brief 		Convert I2C peripheral to number
 * @param[in]	I2Cx	I2C peripheral selected, should be I2C0, I2C1 or I2C2
 * @return 		I2C0 returns 0 ... I2C2 returns 2
 **********************************************************************/
static int8_t I2C_getNum(LPC_I2C_TypeDef *I2Cx)
{
	if (I2Cx == LPC_I2C0)
		return (0);

	else if (I2Cx == LPC_I2C1)
		return (1);

	else if (I2Cx == LPC_I2C2)
		return (2);

	return (-1);
}

/***********************************************************************
 * @brief 		General Master Interrupt handler for I2C peripheral
 * @param[in]	I2Cx	I2C peripheral selected, should be LPC_I2C0, LPC_I2C1 or LPC_I2C2
 * @return 		None
 **********************************************************************/
void I2C_MasterHandler(LPC_I2C_TypeDef  *I2Cx)
{
	int32_t tmp;
	uint8_t returnCode;
	I2C_M_SETUP_Type *txrx_setup;

	tmp = I2C_getNum(I2Cx);
	txrx_setup = (I2C_M_SETUP_Type *) i2cdat[tmp].txrx_setup;

	returnCode = (I2Cx->I2STAT & I2C_STAT_CODE_BITMASK);
	// Save current status
	txrx_setup->status = returnCode;
	// there's no relevant information
	if (returnCode == I2C_I2STAT_NO_INF)
	{
		I2Cx->I2CONCLR = I2C_I2CONCLR_SIC;
		return;
	}

	/* ----------------------------- TRANSMIT PHASE --------------------------*/
	if (i2cdat[tmp].dir == 0)
	{
		switch (returnCode)
		{
		/* A start/repeat start condition has been transmitted -------------------*/
		case I2C_I2STAT_M_TX_START:
		case I2C_I2STAT_M_TX_RESTART:
			I2Cx->I2CONCLR = I2C_I2CONCLR_STAC;
			/*
			 * If there's any transmit data, then start to
			 * send SLA+W right now, otherwise check whether if there's
			 * any receive data for next state.
			 */
			if ((txrx_setup->tx_data != NULL) && (txrx_setup->tx_length != 0))
			{
				I2Cx->I2DAT = (txrx_setup->sl_addr7bit << 1);
				I2Cx->I2CONCLR = I2C_I2CONCLR_SIC;
			}
			else
				goto next_stage;

			break;

		/* SLA+W has been transmitted, ACK has been received ----------------------*/
		case I2C_I2STAT_M_TX_SLAW_ACK:
		/* Data has been transmitted, ACK has been received */
		case I2C_I2STAT_M_TX_DAT_ACK:
			/* Send more data */
			if ((txrx_setup->tx_count < txrx_setup->tx_length) && (txrx_setup->tx_data != NULL))
			{
				I2Cx->I2DAT =  *(uint8_t *)(txrx_setup->tx_data + txrx_setup->tx_count);
				txrx_setup->tx_count++;
				I2Cx->I2CONCLR = I2C_I2CONCLR_SIC;
			}
			// No more data, switch to next stage
			else
			{
next_stage:
				// Change direction
				i2cdat[tmp].dir = 1;
				// Check if any data to receive
				if ((txrx_setup->rx_length != 0) && (txrx_setup->rx_data != NULL))
				{
						// Check whether if we need to issue an repeat start
						if ((txrx_setup->tx_length != 0) && (txrx_setup->tx_data != NULL))
						{
							// Send out an repeat start command
							I2Cx->I2CONSET = I2C_I2CONSET_STA;
							I2Cx->I2CONCLR = I2C_I2CONCLR_AAC | I2C_I2CONCLR_SIC;
						}
						// Don't need issue an repeat start, just goto send SLA+R
						else
							goto send_slar;
				}
				// No more data send, the go to end stage now
				else
				{
					// Success, goto end stage
					txrx_setup->status |= I2C_SETUP_STATUS_DONE;
					goto end_stage;
				}
			}
			break;

		/* SLA+W has been transmitted, NACK has been received ----------------------*/
		case I2C_I2STAT_M_TX_SLAW_NACK:
		/* Data has been transmitted, NACK has been received -----------------------*/
		case I2C_I2STAT_M_TX_DAT_NACK:
			// Update status
			txrx_setup->status |= I2C_SETUP_STATUS_NOACKF;
			goto retry;
			break;
		/* Arbitration lost in SLA+R/W or Data bytes -------------------------------*/
		case I2C_I2STAT_M_TX_ARB_LOST:
			// Update status
			txrx_setup->status |= I2C_SETUP_STATUS_ARBF;
		default:
			goto retry;
			break;
		}
	}

	/* ----------------------------- RECEIVE PHASE --------------------------*/
	else if (i2cdat[tmp].dir == 1)
	{
		switch (returnCode)
		{
			/* A start/repeat start condition has been transmitted ---------------------*/
		case I2C_I2STAT_M_RX_START:
		case I2C_I2STAT_M_RX_RESTART:
			I2Cx->I2CONCLR = I2C_I2CONCLR_STAC;
			/*
			 * If there's any receive data, then start to
			 * send SLA+R right now, otherwise check whether if there's
			 * any receive data for end of state.
			 */
			if ((txrx_setup->rx_data != NULL) && (txrx_setup->rx_length != 0))
			{
send_slar:
				I2Cx->I2DAT = (txrx_setup->sl_addr7bit << 1) | 0x01;
				I2Cx->I2CONCLR = I2C_I2CONCLR_SIC;
			}
			else
			{
				// Success, goto end stage
				txrx_setup->status |= I2C_SETUP_STATUS_DONE;
				goto end_stage;
			}
			break;

		/* SLA+R has been transmitted, ACK has been received -----------------*/
		case I2C_I2STAT_M_RX_SLAR_ACK:
			if (txrx_setup->rx_count < (txrx_setup->rx_length - 1))
			{
				/*Data will be received,  ACK will be return*/
				I2Cx->I2CONSET = I2C_I2CONSET_AA;
			}
			else
			{
				/*Last data will be received,  NACK will be return*/
				I2Cx->I2CONCLR = I2C_I2CONSET_AA;
			}
			I2Cx->I2CONCLR = I2C_I2CONCLR_SIC;
			break;

		/* Data has been received, ACK has been returned ----------------------*/
		case I2C_I2STAT_M_RX_DAT_ACK:
			// Note save data and increase counter first, then check later
			/* Save data  */
			if ((txrx_setup->rx_data != NULL) && (txrx_setup->rx_count < txrx_setup->rx_length))
			{
				*(uint8_t *)(txrx_setup->rx_data + txrx_setup->rx_count) = (I2Cx->I2DAT & I2C_I2DAT_BITMASK);
				txrx_setup->rx_count++;
			}
			if (txrx_setup->rx_count < (txrx_setup->rx_length - 1))
			{
				/*Data will be received,  ACK will be return*/
				I2Cx->I2CONSET = I2C_I2CONSET_AA;
			}
			else
			{
				/*Last data will be received,  NACK will be return*/
				I2Cx->I2CONCLR = I2C_I2CONSET_AA;
			}

			I2Cx->I2CONCLR = I2C_I2CONCLR_SIC;
			break;

		/* Data has been received, NACK has been return -------------------------*/
		case I2C_I2STAT_M_RX_DAT_NACK:
			/* Save the last data */
			if ((txrx_setup->rx_data != NULL) && (txrx_setup->rx_count < txrx_setup->rx_length))
			{
				*(uint8_t *)(txrx_setup->rx_data + txrx_setup->rx_count) = (I2Cx->I2DAT & I2C_I2DAT_BITMASK);
				txrx_setup->rx_count++;
			}
			// Success, go to end stage
			txrx_setup->status |= I2C_SETUP_STATUS_DONE;
			goto end_stage;
			break;

		/* SLA+R has been transmitted, NACK has been received ------------------*/
		case I2C_I2STAT_M_RX_SLAR_NACK:
			// Update status
			txrx_setup->status |= I2C_SETUP_STATUS_NOACKF;
			goto retry;
			break;

		/* Arbitration lost ----------------------------------------------------*/
		case I2C_I2STAT_M_RX_ARB_LOST:
			// Update status
			txrx_setup->status |= I2C_SETUP_STATUS_ARBF;
		default:
retry:
			// Check if retransmission is available
			if (txrx_setup->retransmissions_count < txrx_setup->retransmissions_max)
			{
				// Clear tx count
				txrx_setup->tx_count = 0;
				I2Cx->I2CONSET = I2C_I2CONSET_STA;
				I2Cx->I2CONCLR = I2C_I2CONCLR_AAC | I2C_I2CONCLR_SIC;
				txrx_setup->retransmissions_count++;
			}
			// End of stage
			else
			{
end_stage:
				// Disable interrupt
				I2C_IntCmd(I2Cx, 0);
				// Send stop
				I2C_Stop(I2Cx);
				// Call callback if installed
				if (txrx_setup->callback != NULL)
					txrx_setup->callback();
			}
			break;
		}
	}
}

void I2C0_IRQHandler(void)
{
	i2cdat[0].inthandler(LPC_I2C0);
}
