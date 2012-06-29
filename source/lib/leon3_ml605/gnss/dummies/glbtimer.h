//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GPL
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#pragma once

#include "stdtypes.h"
#include "dff.h"
#include "leon3_ml605\gnss\buses.h"

class GlobalTimer
{
  friend class dbg;
  private:
    struct regs
    {
      uint32 MsLength;
      uint32 SubMsCnt;
      uint32 MsCnt;
      uint64 rdata;
      uint32 rdata_rdy;
    };
    
    regs v;
    TDFF<regs> r;
    
    uint32 MsPulse : 1;
    
  public:
    void Update(uint32 inNRst,
                SClock inAdcClk,
                Ctrl2Module &in_c2m,
                Module2Ctrl &out_m2c,
                uint32 &outMsPulse );
    
    void ClkUpdate()
    {
      r.ClkUpdate();
    }
};