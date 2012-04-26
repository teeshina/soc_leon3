//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      GNSS Sensor Limited
// License:     GPL
// Contact:     alex.kosin@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#pragma once

template <class T>class TDpRam
{
  friend class dbg;
  private:
    uint32 adr_width;
    uint32 size;
    T *MEM;
    
    uint32 WrClkPosedge : 1;
    uint32 WrEna : 1;
    uint32 WrAdr;
    T WrData;
    
    TDFF<uint32>rbRdAdr;
  public:
    TDpRam(uint32 adr_width_=8);
    ~TDpRam();
    
    void Update(SClock clk_wr,
                uint32 we,
                uint32 adr_wr,
                T idata,
                SClock clk_rd,
                uint32 re,
                uint32 adr_rd,
                T &odata );
    
    void ClkUpdate();
};

class dp_ram
{
  friend class dbg;
  private:
    uint32 adr_width;
    uint32 size;
    uint64 *MEM;
    
    uint32 WrClkPosedge : 1;
    uint32 WrEna : 1;
    uint32 WrAdr;
    uint64 WrData;
    
    TDFF<uint32>rbRdAdr;
  public:
    dp_ram(uint32 adr_width_=8);
    ~dp_ram();
    
    void Update(SClock clk_wr,
                uint32 we,
                uint32 adr_wr,
                uint64 idata,
                SClock clk_rd,
                uint32 re,
                uint32 adr_rd,
                uint64 &odata );
    
    void ClkUpdate()
    {
      rbRdAdr.ClkUpdate();
      if(WrClkPosedge & WrEna)  
        MEM[WrAdr] = WrData;
    }
};
