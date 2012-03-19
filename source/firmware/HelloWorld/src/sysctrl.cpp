#include "uart.h"
#include "sysctrl.h"

void SysControl::Init()
{
  // Uart initialization:
	UartInit();

	// Timer initialization:
	clTimer.Init();

	// Interrupt controller:
	clIrqControl.Init();
}
