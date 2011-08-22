#ifndef __TELEMETRY_H__
#define __TELEMETRY_H__

#include "lpc_types.h"
#include "uart.h"
#include "input.h"
#include "pid.h"
#include "fmath.h"
#include "telemetry_commands.h"

#define ACK		0x7F
#define NACK	0x7E

typedef enum {
	PingLength = 0,
	SaveToFlashLength = 0,
	GetRegulatorDataLength = 0,
	SetRegulatorDataLength = 36,
	GetChannelMixLength = 0,
	SetChannelMixLength = 32,
	GetRCCalibrationLength = 0,
	SetRCCalibrationLength = 51,
	GetRCValuesLength = 0
} data_length;

void InitTelemetry(void);
void startTelemetry(void);
void stopTelemetry(void);
void SendACK(void);
void rxWait(void);
void GetDataCount(void);
void GetData(void);
void CheckCRC(void);
uint8_t crc8(uint8_t *, uint8_t);

#endif
