//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#pragma once


class GnssEngine
{
  friend class dbg;
  private:
    uint32 hindex;
    uint32 haddr;
    uint32 hmask;
  private:
    GnssTop clGnssTop;

    uint32 wbAdr; //(CFG_GNSS_ADDR_WIDTH-1 downto 0)
    uint32 wRdEna;
    uint32 wWrEna;
    uint32 wGnssOutIrqPulse;
    uint32 wbGnssOutRdData;

    
  public:
    GnssEngine(uint32 hindex_=AHB_SLAVE_GNSSENGINE, uint32 haddr_=0xd00, uint32 hmask_=0xfff);
    
    void Update(uint32 rst,
                SClock clk,
                ahb_slv_in_type &ahbsi,
                ahb_slv_out_type &ahbso,
                SClock inAdcClk,
                uint32 inGpsI,
                uint32 inGpsQ,
                uint32 inGloI,
                uint32 inGloQ
               );
    
    void ClkUpdate()
    {
      clGnssTop.ClkUpdate();
    }
};
