//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GPL
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************
//              Texas Instruments gyroscope SPI simulator
//****************************************************************************

#include "headers.h"

//****************************************************************************
void STMicroelectronics::Update(uint32 nCS, uint32 clk, uint32 idata, uint32 &odata, uint32 &oint1, uint32 &oint2 )
{
  bool bPosedge = false;
  bool bNegedge = false;
  if(clk&!clk_z) bPosedge = true;
  if(!clk&clk_z) bNegedge = true;
  
  if(bPosedge&!nCS)
  {
    iBitCnt++;
    wshift = (wshift<<1)|idata;
    
    if(iBitCnt==8)
    {
      ModeRW = (wshift>>7)&0x1;
      ModeIncr = (wshift>>6)&0x1;
      addr = wshift&0x3F;
    }
    if((iBitCnt==16)&&(ModeRW==0))
    {
      mem[addr] = (wshift&0xFF);
      iBitCnt=8;
      if(ModeIncr==1)
        addr+=1;
    }
  }
  
  if(bNegedge)
  {
    if(iBitCnt==8) rshift = mem[addr];
    if(iBitCnt>8)  rshift <<= 1;
  }
  
  if(nCS==1) iBitCnt=0;
  odata = (rshift>>7)&0x1;
  
  clk_z = clk;
  oint1 = 0;
  oint2 = 0;
}

