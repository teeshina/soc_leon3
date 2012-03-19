#include <stdio.h>
#include "stdtypes.h"
#include "uart.h"

char chStr[64];
int32 iIrqCnt=0;

void tick_handler()
{
  sprintf(chStr,"%s: %i\n", "IRQ4", ++iIrqCnt);
  SendStringToUart(chStr);
}


