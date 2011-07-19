#include "i2c.h"

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
	LPC_I2C0->I2CONSET = (1<<I2EN);						// Enable I2C0	
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

static void I2C_Stop (LPC_I2C_TypeDef *I2Cx)
{
	/* Make sure start bit is not active */
	if (I2Cx->I2CONSET & I2C_I2CONSET_STA)
	{
		I2Cx->I2CONCLR = I2C_I2CONCLR_STAC;
	}
	I2Cx->I2CONSET = I2C_I2CONSET_STO;
	I2Cx->I2CONCLR = I2C_I2CONCLR_SIC;
}

void I2C_MasterTransferData(LPC_I2C_TypeDef *I2Cx, I2C_M_SETUP_Type *TransferCfg)
{
	uint8_t tmp;

	// Reset I2C setup value to default state
	TransferCfg->tx_count = 0;
	TransferCfg->rx_count = 0;
	TransferCfg->status = 0;

	// Setup tx_rx data, callback and interrupt handler
	tmp = I2C_getNum(I2Cx);
	i2cdat[tmp].txrx_setup = (uint32_t) TransferCfg;
	i2cdat[tmp].inthandler = I2C_MasterHandler;
	// Set direction phase, write first
	i2cdat[tmp].dir = 0;

	/* First Start condition */
	I2Cx->I2CONCLR = (1<<SI);
	I2Cx->I2CONSET = (1<<STA);
	I2C_IntCmd(I2Cx, 1);
}

void I2C_IntCmd(LPC_I2C_TypeDef *I2Cx, uint8_t NewState)
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
			// no more data, switch to next stage
			else
			{
next_stage:
				// change direction
				i2cdat[tmp].dir = 1;
				// Check if any data to receive
				if ((txrx_setup->rx_length != 0) && (txrx_setup->rx_data != NULL))
				{
						// check whether if we need to issue an repeat start
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
				// no more data send, the go to end stage now
				else
				{
					// success, goto end stage
					txrx_setup->status |= I2C_SETUP_STATUS_DONE;
					goto end_stage;
				}
			}
			break;

		/* SLA+W has been transmitted, NACK has been received ----------------------*/
		case I2C_I2STAT_M_TX_SLAW_NACK:
		/* Data has been transmitted, NACK has been received -----------------------*/
		case I2C_I2STAT_M_TX_DAT_NACK:
			// update status
			txrx_setup->status |= I2C_SETUP_STATUS_NOACKF;
			goto retry;
			break;
		/* Arbitration lost in SLA+R/W or Data bytes -------------------------------*/
		case I2C_I2STAT_M_TX_ARB_LOST:
			// update status
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
			// success, go to end stage
			txrx_setup->status |= I2C_SETUP_STATUS_DONE;
			goto end_stage;
			break;

		/* SLA+R has been transmitted, NACK has been received ------------------*/
		case I2C_I2STAT_M_RX_SLAR_NACK:
			// update status
			txrx_setup->status |= I2C_SETUP_STATUS_NOACKF;
			goto retry;
			break;

		/* Arbitration lost ----------------------------------------------------*/
		case I2C_I2STAT_M_RX_ARB_LOST:
			// update status
			txrx_setup->status |= I2C_SETUP_STATUS_ARBF;
		default:
retry:
			// check if retransmission is available
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
}
