//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GPL
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#pragma once

class GlobalTimer
{
  friend class dbg;
  private:
    struct regs
    {
      uint32 MsLength;
      uint32 SubMsCnt;
      uint32 MsCnt;
    };
    
    regs v;
    TDFF<regs> r;
    
    uint32 MsPulse : 1;
    uint64 rdata;
    
  public:
    void Update(uint32 inNRst,
                SClock inAdcClk,
                GnssMuxBus &inMuxBus,
                uint64 &outRdData,
                uint32 &outMsPulse );
    
    void ClkUpdate()
    {
      r.ClkUpdate();
    }
};