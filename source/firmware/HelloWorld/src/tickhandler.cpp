#include "stdtypes.h"
#include "uart.h"

int32 iIrqCnt=0;

void tick_handler()
{
  printf_uart("%s: %i\n", "IRQ4", ++iIrqCnt);
}


