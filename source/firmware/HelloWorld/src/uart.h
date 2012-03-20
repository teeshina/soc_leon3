#ifndef __UART_H__
#define __UART_H__

#include <stdarg.h>
#include "stdtypes.h"

static const uint32 UART_STATUS_DREADY        = (1 << 0);
static const uint32 UART_STATUS_TSEMPTY       = (1 << 1);
static const uint32 UART_STATUS_THEMPTY       = (1 << 2);
static const uint32 UART_STATUS_BREAK         = (1 << 3);
static const uint32 UART_STATUS_OVERFLOW      = (1 << 4);
static const uint32 UART_STATUS_PARERR        = (1 << 5);
static const uint32 UART_STATUS_FRAME         = (1 << 6);
static const uint32 UART_STATUS_THALFFULL     = (1 << 7); // half of fifo is full
static const uint32 UART_STATUS_RHALFFULL     = (1 << 8);
static const uint32 UART_STATUS_TFULL         = (1 << 9);
static const uint32 UART_STATUS_RFULL         = (1 << 10);

static const uint32 UART_CTRL_DISABLE       = 0x0;
static const uint32 UART_CTRL_ENABLE_RX     = (1 << 0);
static const uint32 UART_CTRL_ENABLE_TX     = (1 << 1);
static const uint32 UART_CTRL_RX_INT        = (1 << 2);
static const uint32 UART_CTRL_TX_INT        = (1 << 3);
static const uint32 UART_CTRL_PARSEL        = (1 << 4);
//static const uint32 UART_CTRL_EVEN_PARITY   = (1 << 5);
static const uint32 UART_CTRL_PAREN         = (1 << 5);
//static const uint32 UART_CTRL_ODD_PARITY    = (1 << 6);
static const uint32 UART_CTRL_FLOW_CONTROL  = (1 << 6);
static const uint32 UART_CTRL_LOOP_BACK     = (1 << 7);
static const uint32 UART_CTRL_EXTCLKENA     = (1 << 8);
static const uint32 UART_CTRL_FIFO_TX_INT   = (1 << 9);
static const uint32 UART_CTRL_FIFO_RX_INT   = (1 << 10);
static const uint32 UART_CTRL_DEBUG         = (1 << 11);
static const uint32 UART_CTRL_BREAK_IRQEN   = (1 << 12);
static const uint32 UART_CTRL_DELAY_IRQEN   = (1 << 13);
static const uint32 UART_CTRL_TSEMPTY_IRQEN = (1 << 14);


struct uart_fields
{
   volatile int32 data;
   volatile int32 status;
   volatile int32 control;
   volatile int32 scaler;
};

class ApbUart
{
  friend class dbg;
  private:
    static const int32 BUF_LENGTH = 256;
  private:
    uart_fields *uart;
    char chBuf[2*BUF_LENGTH];
    char *pchBuf;
    int32 iBufCnt;
  public:
    void Init();
    void Print(char *str, int32 len);

    void Send();
};

extern int32 printf_uart(const char *_format, ... );

#endif
