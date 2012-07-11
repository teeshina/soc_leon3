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

class GnssMisc
{
  friend class dbg;
  private:
    struct regs
    {
      uint32 ID;
      uint32 rdata_rdy;
      uint64 rdata;
    };
    
    regs v;
    TDFF<regs> r;
    
    int32 wbGpsI;// : 3;
    int32 wbGpsQ;// : 3;
    int32 wbGloI;// : 3;
    int32 wbGloQ;// : 3;

    int32 wbGpsI_IF;
    int32 wbGpsQ_IF;
    int32 wbGloI_IF;
    int32 wbGloQ_IF;
    
  public:
    void Update(uint32 inNRst,
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
                int32 &outGloQ );
    
    void ClkUpdate()
    {
      r.ClkUpdate();
    }
};