//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GPL
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#pragma once

class ChannelsPack
{
  friend class dbg;
  private:
    struct regs
    {
      uint32 dbgIncr;

      uint32 accI1;
      uint32 accQ1;
      uint32 accI2;
      uint32 accQ2;

      uint32 latchI1;
      uint32 latchQ1;
      uint32 latchI2;
      uint32 latchQ2;
    };
    
    regs v;
    TDFF<regs> r;
    
    uint64 rdata;
  public:
  
    void Update(uint32 inNRst,
                SClock inAdcClk,
                uint32 inGpsI,
                uint32 inGpsQ,
                uint32 inGloI,
                uint32 inGloQ,
                uint32 inMsPulse,
                GnssMuxBus &inMuxBus,
                uint64 &outRdData );
  
    void ClkUpdate()
    {
      r.ClkUpdate();
    }
};
