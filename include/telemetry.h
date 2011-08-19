#ifndef __TELEMETRY_H__
#define __TELEMETRY_H__

#include "lpc_types.h"
#include "uart.h"
#include "input.h"
#include "pid.h"
#include "fmath.h"

#define ACK		0x7F
#define NACK	0x7E

void InitTelemetry(void);
void startTelemetry(void);
void stopTelemetry(void);
void rxWait(void);
void rxNothing(void);
void rxPing(void);
void rxSaveToFlash(void);
void rxGetRegulatorData(void);
void rxSetRegulatorData(void);
void rxGetChannelMix(void);
void rxSetChannelMix(void);
void GetDataCount(void);
void GetData(void);
void CheckCRC(void);
uint8_t crc8(uint8_t *, uint8_t);

#endif
