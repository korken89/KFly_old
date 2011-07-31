#ifndef __TELEMETRY_H__
#define __TELEMETRY_H__

#include "lpc_types.h"
#include "uart.h"

#define ACK		0xFF
#define NACK	0x00

void startTelemetry(void);
void stopTelemetry(void);
void rxWait(void);
void rxPing(void);

#endif
