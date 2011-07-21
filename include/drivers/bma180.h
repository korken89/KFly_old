#ifndef BMA180_H_
#define BMA180_H_

#include "i2c.h"

#define BMA180_R		0b10000011
#define BMA180_W		0b10000010
#define BMA180_ADDR		0b01000001

/**
 *	Address defines for BMA180 
 **/

//ID and Version Registers
#define BMA_ID			0x00
#define VERSION			0x01
#define ACCXLSB 		0x02
#define ACCXMSB 		0x03
#define ACCYLSB 		0x04
#define ACCYMSB 		0x05
#define ACCZLSB 		0x06
#define ACCZMSB 		0x07
#define TEMPERATURE		0x08
#define STATREG1 		0x09
#define STATREG2 		0x0A
#define STATREG3 		0x0B
#define STATREG4 		0x0C
#define CTRLREG0 		0x0D
#define CTRLREG1 		0x0E
#define CTRLREG2 		0x0F
#define SOFT_RESET		0x10

#define BWTCS 			0x20
#define CTRLREG3 		0x21

#define HILOWNFO 		0x25
#define LOWDUR 			0x26

#define LOWTH 			0x29

#define TCO_Y 			0x2F
#define TCO_Z 			0x30

#define OLSB1 			0x35

#define BW_10HZ 		0x00
#define BW_20HZ			0x01
#define BW_40HZ			0x02
#define BW_75HZ			0x03
#define BW_150HZ		0x04		// *** default ***
#define BW_300HZ		0x05
#define BW_600HZ		0x06
#define BW_1200HZ		0x07
#define BW_HP1HZ		0x08		// High-pass, 1Hz
#define BW_BP0_300HZ	0x09		// Band-pass, 0.3Hz-300Hz
#define BW_MASK 		0xF0
#define BW_SHIFT 		4

#define RANGE_1G		0x00          // +/- 1G ADC resolution 0.13 mg/LSB
#define RANGE_1_5G		0x01          // +/- 1.5G ADC resolution 0.19 mg/LSB
#define RANGE_2G		0x02          // +/- 2G ADC resolution 0.25 mg/LSB    *** default ***
#define RANGE_3G		0x03          // +/- 3G ADC resolution 0.38 mg/LSB
#define RANGE_4G		0x04          // +/- 4G ADC resolution 0.50 mg/LSB
#define RANGE_8G		0x05          // +/- 8G ADC resolution 0.99 mg/LSB
#define RANGE_16G		0x06          // +/- 16G ADC resolution 1.98 mg/LSB
#define RANGE_SHIFT	 	1
#define RANGE_MASK 		0x0E

struct _acc_data {
	int16_t x;
	int16_t y;
	int16_t z;
};

typedef struct _acc_data acc_data;

void BMA180_Init(uint8_t, uint8_t);
Status BMA180_BurstRead(uint8_t *);
Status BMA180_Read(uint8_t, uint8_t*);
Status BMA180_Write(uint8_t, uint8_t);

#endif
