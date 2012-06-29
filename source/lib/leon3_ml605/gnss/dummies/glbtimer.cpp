//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GPL
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "leon3_ml605\config.h"
#include "glbtimer.h"

void GlobalTimer::Update( uint32 inNRst,
                          SClock inAdcClk,
                          Ctrl2Module &in_c2m,
                          Module2Ctrl &out_m2c,
                          uint32 &outMsPulse )
{
  // write control registers:
  if((in_c2m.wr_module_sel==MODULE_ID_GLB_TIMER)&&in_c2m.wr_ena)
  {
    switch(in_c2m.wr_field_sel)
    {
      case 0:
        v.MsLength = in_c2m.wr_data;
      break;
      default:;
    }
  }
  
  // read data:
  v.rdata_rdy = 0;
  v.rdata = 0;
  if((in_c2m.rd_module_sel==MODULE_ID_GLB_TIMER)&&in_c2m.rd_ena)
  {
    v.rdata_rdy = 1;
    switch(in_c2m.rd_field_sel)
    {
      case 0:
        v.rdata = (uint64(r.Q.MsCnt)<<32)|uint64(r.Q.MsLength);
      break;
      default:;
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
  
  out_m2c.rdata_rdy = r.Q.rdata_rdy;
  out_m2c.rdata     = r.Q.rdata;
  
  r.CLK = inAdcClk;
  r.D = v;
}
