#include <stdio.h>
#include "uart.h"
#include "irqctrl.h"

extern "C" void *catch_interrupt(void func(), int irq);
extern void tick_handler();


void IrqControl::Init()
{
  irqmp = (irqmp_fields*)0x80000200;

  irqmp->irqlevel = 0;       /* clear level reg */
  irqmp->irqmask = 0x0;      /* mask all interrupts */
  irqmp->irqclear = -1;      /* clear all pending interrupts */

  catch_interrupt(tick_handler, 4);


  irqmp->irqmask = 0x010;
}
