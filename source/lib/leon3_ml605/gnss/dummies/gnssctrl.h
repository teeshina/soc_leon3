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
    
    struct regs
    {
      uint32 wr_ena         : 1;
      uint32 wr_module_sel; // : CFG_GNSS_ADDR_WIDTH-1
      uint32 wr_field_sel   : 4;
      uint32 wr_data;
      uint32 SnapEna : 1;
      uint32 SnapFieldCnt;
      uint32 SnapModuleCnt;
      uint32 SnapEna_z : 1;
      uint32 SnapFieldCnt_z;
      uint32 SnapModuleCnt_z;
    };
    
    regs v;
    TDFF<regs> r;//
    Module2Ctrl v_m2c;

    uint32 snap_end : 1;
    
  public:
    
    void Update(uint32 inNRst,
                SClock inAdcClk,
                // AMBA to GNSS via FIFO
                uint32 inWrAdr,
                uint32 inWrEna,
                uint32 inWrData,
                // Control signals:
                m2c_tot &m2c,
                Ctrl2Module &c2m,
                // snaper run:
                uint32 inMsReady,
                uint32 &outIrqPulse,
                // DP memory interface
                uint32 &outMemWrEna,
                uint32 &outMemWrAdr,
                uint64 &outMemWrData );

    void ClkUpdate()
    {
      r.ClkUpdate();
    }
};
