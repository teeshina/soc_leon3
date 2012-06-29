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
                          Module2Ctrl &out_m2c)
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
  
  
  out_m2c.rdata_rdy = r.Q.rdata_rdy;
  out_m2c.rdata     = r.Q.rdata;
  
  r.CLK = inAdcClk;
  r.D = v;
}
