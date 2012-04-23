#include <stdio.h>
#include <stdarg.h>
#include "soconfig.h"
#include "uart.h"
#include "sysctrl.h"

extern ApbUart    clApbUart;

static const int32 BUF_LENGTH = 256;
char chBuf[2*BUF_LENGTH];
char *pchBuf;
int32 iBufCnt=0;


char tmpUartStr[256];

//****************************************************************************
int32 printf_uart(const char *_format, ... )
{
  int32 ret = 0;
  va_list arg;
  va_start( arg, _format );
  
  ret = vsprintf( tmpUartStr, _format, arg );
  va_end( arg );

  clApbUart.Print(tmpUartStr, ret);
  return ret;
}


//****************************************************************************
void uart_empty_irqhandler()
{
  clApbUart.Send();
}

//****************************************************************************
void ApbUart::Send()
{
  if(iBufCnt==0) 
  {
    uart->control &= ~UART_CTRL_TSEMPTY_IRQEN;
    return;
  }
  while((uart->status&UART_STATUS_TFULL)==0)
  {
    uart->data = (unsigned int)(*(pchBuf-iBufCnt));
    if((--iBufCnt)==0)
    {
      uart->control &= ~UART_CTRL_TSEMPTY_IRQEN;
      break;
    }
  }
}

//****************************************************************************
void ApbUart::Print(char *str, int32 len)
{
  char *tmp = str;

  for (int32 i=0; i<len; ++i)
  {
    *pchBuf = *(pchBuf-BUF_LENGTH) = str[i];
    if((++pchBuf)>=&chBuf[2*BUF_LENGTH]) pchBuf = &chBuf[BUF_LENGTH];
    if((++iBufCnt)>BUF_LENGTH) iBufCnt = BUF_LENGTH;   // Overflow buffer

  }
  if(iBufCnt) uart->control |= UART_CTRL_TSEMPTY_IRQEN;
  Send();

}

//****************************************************************************
void ApbUart::Init()
{
  uart = (uart_fields*)ADR_APBUART_BASE;

  // scaler = 66MHz/(8*(1+rate)) = 115200 = 71
#ifdef PC_SIMULATION   // 
  uart->scaler = 4;	   // for the fast simulation. It is the same as in file "settings.txt"
#else
  uart->scaler = 71;       // 115200 baudrate
#endif

  pchBuf  = &chBuf[BUF_LENGTH];
  iBufCnt = 0;

  // Init port for the transmition:
  uart->control = UART_CTRL_ENABLE_TX|UART_CTRL_TSEMPTY_IRQEN;
}
