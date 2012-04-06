//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GPL
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#pragma once

class GnssControl
{
  friend class dbg;
  private:
    uint64 dpMEM[CFG_GNSS_MEMORY_SIZE64];
    
    struct regs
    {
      uint32 RdMemVal;
      uint32 WrMemEna;
      uint32 WrMemAdr;
      uint32 WrMemVal;
      
      uint32 SnapEna : 1;
      uint32 SnapCnt;
      uint32 IrqEna  : 1;
    };
    
    regs v;
    TDFF<regs> r;//

    uint32 hrdata;

    
  public:
    
    void Update(uint32 inNRst,
                SClock inBusClk,
                uint32 inRdAdr,
                uint32 inRdEna,
                uint32 &outRdData,
                uint32 inWrAdr,
                uint32 inWrEna,
                uint32 inWrData,
                GnssMuxBus &outMuxBus,
                uint64 inRdData,
                uint32 inRdDataRdy,
                // snaper run:
                uint32 inMsReady,
                uint32 &outIrqEna );

    void ClkUpdate()
    {
      r.ClkUpdate();
    }
};
