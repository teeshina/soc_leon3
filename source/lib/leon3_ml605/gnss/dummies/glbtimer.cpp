//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GPL
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "lheaders.h"

void GlobalTimer::Update( uint32 inNRst,
                          SClock inAdcClk,
                          GnssMuxBus &inMuxBus,
                          uint64 &outRdData,
                          uint32 &outMsPulse )
{
  // write control registers:
  if((inMuxBus.wbWrModuleSel==CFG_GNSS_CHANNELS_TOTAL)&&inMuxBus.wWrEna)
  {
    switch(inMuxBus.wbWrFieldSel)
    {
      case 0:
        v.MsLength = inMuxBus.wbWrData;
      break;
      default:;
    }
  }
  
  // read data:
  if((inMuxBus.wbRdModuleSel==CFG_GNSS_CHANNELS_TOTAL)&&inMuxBus.wRdEna)
  {
    switch(inMuxBus.wbRdFieldSel)
    {
      case 0:
        rdata = (uint64(r.Q.MsCnt)<<32)|uint64(r.Q.MsLength);
      break;
      default: rdata = 0;
    }
  }
  
  // msec counter:
  if((r.Q.SubMsCnt+1)==r.Q.MsLength) MsPulse = 1;
  else                               MsPulse = 0;
  
  if(MsPulse)             v.SubMsCnt = 0;
  else if(v.MsLength!=0)  v.SubMsCnt = r.Q.SubMsCnt + 1;
  
  if(MsPulse) v.MsCnt = r.Q.MsCnt + 1;

  
  // Reset all:
  if(inNRst==0)
  {
    v.MsLength = 0;
    v.SubMsCnt = 0;
    v.MsCnt    = 0;
  }
  
  outMsPulse = MsPulse;
  outRdData  = rdata;
  
  r.CLK = inAdcClk;
  r.D = v;
}
