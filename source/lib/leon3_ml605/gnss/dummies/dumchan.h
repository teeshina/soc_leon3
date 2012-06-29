//****************************************************************************
// Info:        www.gnss-sensor.com
// Author:      GNSS Sensor Ltd
// License:     GPL
// Contact:     alex.kosin@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#pragma once

#include "stdtypes.h"
#include "dff.h"
#include "leon3_ml605\gnss\buses.h"

class DummyChannel
{
  friend class dbg;
  private:
    uint32 mod_id;
  private:    
    struct regs
    {
      uint32 dbgIncr;

      uint32 accI;
      uint32 accQ;

      uint32 latchI;
      uint32 latchQ;

      uint64 rdata;
      uint32 rdata_rdy : 1;
    };
    
    regs v;
    TDFF<regs> r;

  public:
    DummyChannel(uint32 ind_);
    
    void Update( uint32 inNRst,
                 SClock inAdcClk,
                 uint32 inI,
                 uint32 inQ,
                 uint32 inMsPulse,
                 Ctrl2Module &c2m,
                 Module2Ctrl &m2c );
    
    void ClkUpdate()
    {
      r.ClkUpdate();
    }
};

