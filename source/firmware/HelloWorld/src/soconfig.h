#ifndef __SOCONFIG_H__
#define __SOCONFIG_H__

#include "stdtypes.h"

static const uint32 ADR_APBUART_BASE = 0x80000100;
static const uint32 ADR_IRQMP_BASE   = 0x80000200;
static const uint32 ADR_GPTIMER_BASE = 0x80000300;

static const uint32 ADR_GNSS_ENGINE_BASE = 0xD0000000;


static const int32 TIMERS_TOTAL    = 2;

// IRQ numbers:
static const int32 IRQ_IRQ_CONTROL = 0;// irqmp, has to be 0
static const int32 IRQ_DSU         = IRQ_IRQ_CONTROL+1;
static const int32 IRQ_GNSS_ENGINE = IRQ_DSU+1;
static const int32 IRQ_UART_CFG    = IRQ_GNSS_ENGINE+1;
static const int32 IRQ_TIMER       = IRQ_UART_CFG+1;
static const int32 IRQ_TOTAL       = IRQ_TIMER+1+(TIMERS_TOTAL-1);



#endif
