//****************************************************************************
// Info:        www.gnss-sensor.com
// Author:      GNSS Sensor Ltd
// License:     GPL
// Contact:     alex.kosin@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "dumchan.h"

//****************************************************************************
DummyChannel::DummyChannel(uint32 ind_)
{
  mod_id = ind_;
}

//****************************************************************************
void DummyChannel::Update( uint32 inNRst,
                           SClock inAdcClk,
                           uint32 inI,
                           uint32 inQ,
                           uint32 inMsPulse,
                           Ctrl2Module &c2m,
                           Module2Ctrl &m2c )
{
  v = r.Q;
  
  // write control registers:
  if((c2m.wr_module_sel==mod_id)&&c2m.wr_ena)
  {
    switch(c2m.wr_field_sel)
    {
      case 0:
        v.dbgIncr = c2m.wr_data;
      break;
      default:;
    }
  }
  
  // read data:
  v.rdata_rdy=0; 
  v.rdata = 0;
  if((c2m.rd_module_sel==mod_id)&&c2m.rd_ena)
  {
    v.rdata_rdy=1; 
    switch(c2m.rd_field_sel)
    {
      case 0: v.rdata = uint64(r.Q.dbgIncr); break;
      case 1: v.rdata = (uint64(r.Q.latchQ)<<32) | uint64(r.Q.latchI); break;
      case 2: v.rdata = (uint64(r.Q.latchI)<<32) | uint64(r.Q.latchQ); break;
      default:;
    }
  }

  if(inMsPulse) v.accI = r.Q.dbgIncr;
  else          v.accI = r.Q.accI + inI;

  if(inMsPulse) v.accQ = ~r.Q.dbgIncr;
  else          v.accQ = r.Q.accQ - inQ;


  if(inMsPulse)
  {
    v.latchI = r.Q.accI;
    v.latchQ = r.Q.accQ;
  }
  
  if(!inNRst)
  {
    v.accI = 0;
    v.accQ = 0;
    v.latchI = 0;
    v.latchQ = 0;
    v.rdata_rdy = 0;
  }
  
  m2c.rdata = r.Q.rdata;
  m2c.rdata_rdy = r.Q.rdata_rdy;
  
  
  r.CLK = inAdcClk;
  r.D = v;

}


