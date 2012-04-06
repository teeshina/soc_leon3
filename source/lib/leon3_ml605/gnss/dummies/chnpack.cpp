//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GPL
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "lheaders.h"

#define DUMMY_CHAN_NUM 16

void ChannelsPack::Update( uint32 inNRst,
                           SClock inAdcClk,
                           uint32 inGpsI,
                           uint32 inGpsQ,
                           uint32 inGloI,
                           uint32 inGloQ,
                           uint32 inMsPulse,
                           GnssMuxBus &inMuxBus,
                           uint64 &outRdData )
{
  // write control registers:
  if((inMuxBus.wbWrModuleSel==DUMMY_CHAN_NUM)&&inMuxBus.wWrEna)
  {
    switch(inMuxBus.wbWrFieldSel)
    {
      case 0:
        v.dbgIncr = inMuxBus.wbWrData;
      break;
      default:;
    }
  }
  
  // read data:
  if((inMuxBus.wbRdModuleSel==DUMMY_CHAN_NUM)&&inMuxBus.wRdEna)
  {
    switch(inMuxBus.wbRdFieldSel)
    {
      case 0: rdata = uint64(r.Q.dbgIncr); break;
      case 1: rdata = (uint64(r.Q.latchQ1)<<32) | uint64(r.Q.latchI1); break;
      case 2: rdata = (uint64(r.Q.latchQ2)<<32) | uint64(r.Q.latchI2); break;
      default: rdata = 0;
    }
  }
  
  if(inMsPulse) v.accI1 = r.Q.dbgIncr;
  else          v.accI1 = r.Q.accI1 + inGpsI;

  if(inMsPulse) v.accQ1 = ~r.Q.dbgIncr;
  else          v.accQ1 = r.Q.accQ1 - inGpsQ;

  if(inMsPulse) v.accI2 = ~r.Q.dbgIncr;
  else          v.accI2 = r.Q.accI2 - inGloI;

  if(inMsPulse) v.accQ2 = r.Q.dbgIncr;
  else          v.accQ2 = r.Q.accQ2 + inGloQ;


  if(inMsPulse)
  {
    v.latchI1 = r.Q.accI1;
    v.latchQ1 = r.Q.accQ1;
    v.latchI2 = r.Q.accI2;
    v.latchQ2 = r.Q.accQ2;
  }
  
  if(!inNRst)
  {
    v.accI1 = 0;
    v.accQ1 = 0;
    v.accI2 = 0;
    v.accQ2 = 0;
    v.latchI1 = 0;
    v.latchQ1 = 0;
    v.latchI2 = 0;
    v.latchQ2 = 0;
  }
  
  outRdData = rdata;
}
