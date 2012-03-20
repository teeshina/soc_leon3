#include <stdio.h>
#include "soconfig.h"
#include "uart.h"
#include "irqctrl.h"

extern "C" void *catch_interrupt(void func(), int irq);
extern void tick_handler();
extern void uart_empty_irqhandler();


void IrqControl::Init()
{
  irqmp = (irqmp_fields*)ADR_IRQMP_BASE;

  irqmp->irqlevel = 0;       // clear level reg
  irqmp->irqmask = 0x0;      // mask all interrupts
  irqmp->irqclear = -1;      // clear all pending interrupts

  catch_interrupt(tick_handler, IRQ_TIMER);
  catch_interrupt(uart_empty_irqhandler, IRQ_UART_CFG);

  irqmp->irqmask |= (1 << IRQ_TIMER);
  irqmp->irqmask |= (1 << IRQ_UART_CFG);
}

void IrqControl::ClearIrq(int32 n)
{
  irqmp->irqclear &= ~(1<<n);
}
