#include "uart.h"
#include "sysctrl.h"

extern ApbUart clApbUart;

void SysControl::Init()
{
  // Uart initialization:
  clApbUart.Init();

  // Timer initialization:
  clTimer.Init();

  // Interrupt controller:
  clIrqControl.Init();
}
