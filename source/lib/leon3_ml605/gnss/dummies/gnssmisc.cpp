//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GPL
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "leon3_ml605\config.h"
#include "gnssmisc.h"

void GnssMisc::Update( uint32 inNRst,
                          SClock inAdcClk,
                          Ctrl2Module &in_c2m,
                          Module2Ctrl &out_m2c,
                          uint32 inGpsI,
                          uint32 inGpsQ,
                          uint32 inGloI,
                          uint32 inGloQ,
                          int32 &outGpsI,
                          int32 &outGpsQ,
                          int32 &outGloI,
                          int32 &outGloQ)
{
  // write control registers:
  if((in_c2m.wr_module_sel==MODULE_ID_MISC)&&in_c2m.wr_ena)
  {
    switch(in_c2m.wr_field_sel)
    {
      case 0:
        v.ID = in_c2m.wr_data;
      break;
      default:;
    }
  }
  
  // read data:
  v.rdata_rdy = 0;
  v.rdata = 0;
  if((in_c2m.rd_module_sel==MODULE_ID_MISC)&&in_c2m.rd_ena)
  {
    v.rdata_rdy = 1;
    switch(in_c2m.rd_field_sel)
    {
      case 0:
        v.rdata = uint64(r.Q.ID);
      break;
      default:;
    }
  }
  
  
  if(inGpsI==0)     wbGpsI = -3;
  else if(inGpsI==1)wbGpsI = -1;
  else if(inGpsI==2)wbGpsI = +1;
  else              wbGpsI = +3;

  if(inGpsQ==0)     wbGpsQ = -3;
  else if(inGpsQ==1)wbGpsQ = -1;
  else if(inGpsQ==2)wbGpsQ = +1;
  else              wbGpsQ = +3;


  if(inGloI==0)     wbGloI = -3;
  else if(inGloI==1)wbGloI = -1;
  else if(inGloI==2)wbGloI = +1;
  else              wbGloI = +3;

  if(inGloQ==0)     wbGloQ = -3;
  else if(inGloQ==1)wbGloQ = -1;
  else if(inGloQ==2)wbGloQ = +1;
  else              wbGloQ = +3;
  
  
  // TODO: implemet carrier NCO IF:
  wbGpsI_IF = wbGpsI;
  wbGpsQ_IF = wbGpsQ;
  wbGloI_IF = wbGloI;
  wbGloQ_IF = wbGloQ;
  
  out_m2c.rdata_rdy = r.Q.rdata_rdy;
  out_m2c.rdata     = r.Q.rdata;
  
  outGpsI = wbGpsI_IF;
  outGpsQ = wbGpsQ_IF;
  outGloI = wbGloI_IF;
  outGloQ = wbGloQ_IF;
  
  r.CLK = inAdcClk;
  r.D = v;
}
