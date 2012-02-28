//****************************************************************************
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "headers.h"

// Fclk/(8*(1+rate))=speed  => Fclk/speed/8 - 1
//
// baudrate values for SysClk = 66 MHz:
//    1 = 4125 kBaud
//    2 = 2750 kBaud
//    4 = 1650 kBaud
//   71 = 114583 Baud
#define UART_BAUDRATE_VAL     4
#define UART_BAUDRATE_CLKMAX (8*(1+UART_BAUDRATE_VAL))

//****************************************************************************
uart_port::uart_port()
{
  iClkDivider = 0;
  bEmpty     = true;
  bRdDataRdy = false;
  uiSymbCnt  = 0;
  uchString[0] = '\0';
}
uart_port::~uart_port()
{
  free(pclkGen);
}


//****************************************************************************
void uart_port::Update(uint32 inRst,
                       SClock inSysClk,
                       uint32 inTD,
                       uint32 inRTS,
                       uint32 &outRD,
                       uint32 &outCTS )
{
  if(inRst)
  {
    bEmpty     = true;
    bCharReady = false;
    bRdDataRdy = false;
    outRD      = 0;
    outCTS     = 1;    
    eState     = UART_IDLE;
    uchString[0] = '\0';
    uiSymbCnt  = 0;
    return;
  }
  
  if(inSysClk.eClock_z!=SClock::CLK_POSEDGE) return;

  // In IDLE state pass to reset ClkDivider counter.
  if( (eState!=UART_IDLE) && ((++iClkDivider)<UART_BAUDRATE_CLKMAX) ) return;
  iClkDivider = 0;

  outCTS = 0;
  
  switch(eState)
  {
    case UART_IDLE:
      if(!inTD) eState = UART_SHIFT;
      uiShiftCnt = 0;
      iClkDivider = 0;                  // !!! some kind of edge tracking system
      uchString[uiSymbCnt+1] = '\0';
    break;
    case UART_SHIFT:
      uchString[uiSymbCnt]>>=1; 
      uchString[uiSymbCnt] |= ((inTD&0x1)<<7);
      if((++uiShiftCnt)==8)
      {
        eState = UART_STOPBIT;
        uiSymbCnt++;
      }
    break;
    case UART_STOPBIT:
      if(inTD) bCharReady = true;
      else     uiSymbCnt = 0;
      eState = UART_IDLE;
    break;
    default:;
  }

  if(bCharReady&&(uchString[uiSymbCnt-1]=='\n'))
  {
#if 1// This made only to align console output!!! May be removed at any time.
    uchString[uiSymbCnt-1] = '\0';  // 
#endif
    uiSymbCnt  = 0;
    bRdDataRdy = true;
  }
}

