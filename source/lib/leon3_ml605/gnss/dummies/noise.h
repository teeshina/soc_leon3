//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GPL
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#pragma once

class Noise
{
  friend class dbg;
  private:
    struct regs
    {
      uint32 Carrier;
      uint32 CarrierTh;
      uint32 rdata_rdy;
      uint64 rdata;
    };
    
    regs v;
    TDFF<regs> r;
    
  public:
    void Update(uint32 inNRst,
                SClock inAdcClk,
                Ctrl2Module &in_c2m,
                Module2Ctrl &out_m2c );
    
    void ClkUpdate()
    {
      r.ClkUpdate();
    }
};