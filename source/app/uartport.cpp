//****************************************************************************
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "headers.h"

//****************************************************************************
uart_port::uart_port()
{
  //This equals to baudrate=1 (uart setting)
  double dClk = SYS_CLOCK_F/8.0;
  double scale = 1.0/SYS_CLOCK_F/4.0;
  pclkGen = new ClockGenerator(dClk,scale);
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
void uart_port::Update(uint32 inNRst,
                       uint32 inTD,
                       uint32 inRTS,
                       uint32 &outRD,
                       uint32 &outCTS )
{
  if(!inNRst)
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
  
  if(!pclkGen->Update(clk))
    return;

  outCTS = 0;
  
  switch(eState)
  {
    case UART_IDLE:
      if(inTD) eState = UART_WAIT;
    break;
    case UART_WAIT:
      if(!inTD) eState = UART_SHIFT;
      uiShiftCnt = 0;
      uchString[uiSymbCnt+1] = '\0';
    break;
    case UART_SHIFT:
      uchString[uiSymbCnt]<<=1; 
      uchString[uiSymbCnt] |= (inTD&0x1);
      if((++uiShiftCnt)==8)
      {
        eState = UART_STOPBIT;
        uiSymbCnt++;
      }
    break;
    case UART_STOPBIT:
      if(!inTD) bCharReady = true;
      else      uiSymbCnt = 0;
      eState = UART_IDLE;
    break;
    default:;
  }

  if(bCharReady&&(uchString[uiSymbCnt]=='\n'))  bRdDataRdy = true;
}

