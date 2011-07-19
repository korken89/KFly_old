#ifndef __I2C_H_
#define __I2C_H_

#include "LPC17xx.h"
#include "bitvals.h"
#include "lpc_types.h"

/* I2C Bitmasks */
#define I2C_STAT_CODE_BITMASK		0xF8
#define I2C_I2DAT_BITMASK			0xFF


/* I2C return status code definitions ----------------------------- */

/** No relevant information */
#define I2C_I2STAT_NO_INF			0xF8

/* Master transmit mode -------------------------------------------- */
/** A start condition has been transmitted */
#define I2C_I2STAT_M_TX_START		0x08
/** A repeat start condition has been transmitted */
#define I2C_I2STAT_M_TX_RESTART		0x10
/** SLA+W has been transmitted, ACK has been received */
#define I2C_I2STAT_M_TX_SLAW_ACK	0x18
/** SLA+W has been transmitted, NACK has been received */
#define I2C_I2STAT_M_TX_SLAW_NACK	0x20
/** Data has been transmitted, ACK has been received */
#define I2C_I2STAT_M_TX_DAT_ACK		0x28
/** Data has been transmitted, NACK has been received */
#define I2C_I2STAT_M_TX_DAT_NACK	0x30
/** Arbitration lost in SLA+R/W or Data bytes */
#define I2C_I2STAT_M_TX_ARB_LOST	0x38

/* Master receive mode -------------------------------------------- */
/** A start condition has been transmitted */
#define I2C_I2STAT_M_RX_START		0x08
/** A repeat start condition has been transmitted */
#define I2C_I2STAT_M_RX_RESTART		0x10
/** Arbitration lost */
#define I2C_I2STAT_M_RX_ARB_LOST	0x38
/** SLA+R has been transmitted, ACK has been received */
#define I2C_I2STAT_M_RX_SLAR_ACK	0x40
/** SLA+R has been transmitted, NACK has been received */
#define I2C_I2STAT_M_RX_SLAR_NACK	0x48
/** Data has been received, ACK has been returned */
#define I2C_I2STAT_M_RX_DAT_ACK		0x50
/** Data has been received, NACK has been return */
#define I2C_I2STAT_M_RX_DAT_NACK	0x58


#define I2C_I2CONSET_AA				0x04 /*!< Assert acknowledge flag */
#define I2C_I2CONSET_SI				0x08 /*!< I2C interrupt flag */
#define I2C_I2CONSET_STO			0x10 /*!< STOP flag */
#define I2C_I2CONSET_STA			0x20 /*!< START flag */
#define I2C_I2CONSET_I2EN			0x40 /*!< I2C interface enable */

/** Assert acknowledge Clear bit */
#define I2C_I2CONCLR_AAC			0x04
/** I2C interrupt Clear bit */
#define I2C_I2CONCLR_SIC			0x08
/** START flag Clear bit */
#define I2C_I2CONCLR_STAC			0x20
/** I2C interface Disable bit */
#define I2C_I2CONCLR_I2ENC			0x40

/** I2C status values */
#define I2C_SETUP_STATUS_ARBF   (1<<8)	/**< Arbitration false */
#define I2C_SETUP_STATUS_NOACKF (1<<9)	/**< No ACK returned */
#define I2C_SETUP_STATUS_DONE   (1<<10)	/**< Status DONE */

/** SET and CLR bits */
#define AA				2
#define SI				3
#define STO				4
#define STA				5
#define I2EN			6

/** Master transfer setup data structure definitions */
typedef struct
{
  uint32_t          sl_addr7bit;				/**< Slave address in 7bit mode */
  uint8_t*          tx_data;					/**< Pointer to Transmit data - NULL if data transmit
													  is not used */
  uint32_t          tx_length;					/**< Transmit data length - 0 if data transmit
													  is not used*/
  uint32_t          tx_count;					/**< Current Transmit data counter */
  uint8_t*          rx_data;					/**< Pointer to Receive data - NULL if data receive
													  is not used */
  uint32_t          rx_length;					/**< Receive data length - 0 if data receive is
													   not used */
  uint32_t          rx_count;					/**< Current Receive data counter */
  uint32_t          retransmissions_max;		/**< Max Re-Transmission value */
  uint32_t          retransmissions_count;		/**< Current Re-Transmission counter */
  uint32_t          status;						/**< Current status of I2C activity */
  void 				(*callback)(void);			/**< Pointer to Call back function when transmission complete
													used in interrupt transfer mode */
} I2C_M_SETUP_Type;

void I2C0_Init(void);
void I2C_SendStart(LPC_I2C_TypeDef *);
void I2C_SendStop(LPC_I2C_TypeDef *);
void I2C_SendACK(LPC_I2C_TypeDef *);
void I2C_SendNACK(LPC_I2C_TypeDef *);
void I2C_ClearStart(LPC_I2C_TypeDef *);
void I2C_ClearStop(LPC_I2C_TypeDef *);
void I2C_ClearSI(LPC_I2C_TypeDef *);
void I2C_WaitForSI(LPC_I2C_TypeDef *);
int I2C_CheckForSI(LPC_I2C_TypeDef *);
void I2C_Write(LPC_I2C_TypeDef *, uint8_t);
uint8_t I2C_Read(LPC_I2C_TypeDef *);
uint8_t I2C_ReadStatus(LPC_I2C_TypeDef *);
static void I2C_Stop (LPC_I2C_TypeDef *);
void I2C_MasterTransferData(LPC_I2C_TypeDef *, I2C_M_SETUP_Type *);
void I2C_IntCmd(LPC_I2C_TypeDef *, uint8_t);
static int8_t I2C_getNum(LPC_I2C_TypeDef *);
void I2C_MasterHandler(LPC_I2C_TypeDef  *);
void I2C0_IRQHandler(void);

#endif
