#ifndef __TELEMETRY_H__
#define __TELEMETRY_H__

#include "lpc_types.h"
#include "uart.h"

#define ACK		0x7F
#define NACK	0x7E

void startTelemetry(void);
void stopTelemetry(void);
void rxWait(void);
void rxPing(void);
void rxSaveToFlash(void);
void GetDataCount(void);
void GetData(void);
void CheckCRC(void);
uint8_t crc8(uint8_t *, uint8_t);

#endif
