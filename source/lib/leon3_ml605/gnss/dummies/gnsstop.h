//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GPL
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#pragma once

class GnssTop
{
  friend class dbg;
  private:
    uint64 dpMEM[CFG_GNSS_MEMORY_SIZE64];
    
    struct regs
    {
      uint32 MsLength;
      uint32 MsCnt;
      uint32 irq_ena : 1;
    };
    
    regs v;
    TDFF<regs> r;//

    uint64 wbReclkOutSelData;
    uint32 wReclkOutSelDataRdy;
    uint32 wReclkOutMsReady;
    GnssMuxBus stReclkOutMuxBus;
    Reclock clReclock;

    GnssMuxBus stCtrlOutMuxBus;
    GnssControl clGnssControl;

    uint64 wbGlbTimerOutData;
    uint32 wGlbTmrOutMsReady;
    GlobalTimer clGlobalTimer;
    
    uint64 wbChnOutData;
    ChannelsPack clChannelsPack;


    uint64 wbSelData; // multiplexer on top level
    
  public:
    
    void Update(uint32 inNRst,
                SClock inClk,
                uint32 inRdAdr,
                uint32 inRdEna,
                uint32 &outRdData,
                uint32 inWrAdr,
                uint32 inWrEna,
                uint32 inWrData,
                uint32 &outIrqPulse,
                
                SClock inAdcClk,
                uint32 inGpsI,
                uint32 inGpsQ,
                uint32 inGloI,
                uint32 inGloQ );
    
    void ClkUpdate()
    {
      r.ClkUpdate();
      clReclock.ClkUpdate();
      clGnssControl.ClkUpdate();
      clGlobalTimer.ClkUpdate();
      clChannelsPack.ClkUpdate();
    }
};
