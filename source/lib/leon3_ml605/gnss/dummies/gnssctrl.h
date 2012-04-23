//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      GNSS Sensor Limited
// License:     GPL
// Contact:     alex.kosin@gnss-sensor.com
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
      uint32 RdAdr;//(CFG_GNSS_ADDR_WIDTH-1 downto 0)
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
