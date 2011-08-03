#ifndef __TELEMETRY_H__
#define __TELEMETRY_H__

#include "lpc_types.h"
#include "uart.h"

#define ACK		0xFF
#define NACK	0x00

void startTelemetry(void);
void stopTelemetry(void);
void rxWait(void);
void GetDataCount(void);
void GetData(void);
void CheckCRC(void);
void rxPing(void);
void rxTest(void);
uint8_t crc8(uint8_t *, uint8_t);

#endif
