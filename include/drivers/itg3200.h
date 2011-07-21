#ifndef ITG3200_H_
#define ITG3200_H_

#include "i2c.h"
#include "configKFly.h"
#include "lpc_types.h"
#include "uart.h"

// ITG3200 Register Defines
#define ITG3200_R 			0b11010001		// ADD pin is pulled low
#define ITG3200_W 			0b11010000
#define ITG3200_ADDR		0b01101000

#define ACTL				0
#define OPEN				0
#define LATCH_INT_EN		1
#define INT_ANYRD_2CLEAR	0
#define ITG_RDY_EN			1
#define RAW_RDY_EN			1

#define INT_CONFIG			(RAW_RDY_EN<<0)|(ITG_RDY_EN<<2)|(INT_ANYRD_2CLEAR<<4)|(LATCH_INT_EN<<5)|(OPEN<<6)|(ACTL<<7)

#define ITG3200_ID			0x00
#define	SMPL				0x15
#define DLPF				0x16
#define INT_C				0x17
#define INT_S				0x1A
#define	TMP_H				0x1B
#define	TMP_L				0x1C
#define	GX_H				0x1D
#define	GX_L				0x1E
#define	GY_H				0x1F
#define	GY_L				0x20
#define GZ_H				0x21
#define GZ_L				0x22
#define PWR_M				0x3E

#define DLPF_CFG			0
#define FS_SEL				3

#define RATE2DEGPS			1.0f/14.375f	// Datasheet: 14.375LSB per deg/s

typedef struct {
	int16_t x;
	int16_t y;
	int16_t z;
} gyro_data;

void ITG3200_Init(void);
Status ITG3200_BurstRead(uint8_t *);
Status ITG3200_Read(uint8_t, uint8_t*);
Status ITG3200_Write(uint8_t, uint8_t);

#endif
