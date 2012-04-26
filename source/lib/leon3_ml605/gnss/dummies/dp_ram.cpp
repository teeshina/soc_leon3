//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      GNSS Sensor Limited
// License:     GPL
// Contact:     alex.kosin@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "lheaders.h"

//****************************************************************************
template <class T>TDpRam<T>::TDpRam(uint32 adr_width_)
{
  adr_width = adr_width_;
  size = 1<<adr_width;
  MEM = (T*)malloc(size*sizeof(T));
}

//****************************************************************************
template <class T>TDpRam<T>::~TDpRam()
{
  free(MEM);
}

//****************************************************************************
template <class T> void TDpRam<T>::Update(SClock clk_wr, 
                                          uint32 we,
                                          uint32 adr_wr,
                                          T idata, 
                                          SClock clk_rd,
                                          uint32 re,
                                          uint32 adr_rd,
                                          T &odata)
{
  rbRdAdr.CLK = clk_rd;
  if(re) rbRdAdr.D = BITS32(adr_rd, adr_width-1, 0);
  odata = MEM[rbRdAdr.Q];

  WrClkPosedge=0;
  if(clk_wr.eClock==SClock::CLK_POSEDGE) WrClkPosedge=1;
  
  WrEna = we;
  WrAdr = BITS32(adr_wr,adr_width-1,0);
  WrData = idata;
}

//****************************************************************************
template <class T> void TDpRam<T>::ClkUpdate()
{
  rbRdAdr.ClkUpdate();
  if(WrClkPosedge & WrEna)  
    MEM[WrAdr] = WrData;
}



//****************************************************************************
dp_ram::dp_ram(uint32 adr_width_)
{
  adr_width = adr_width_;
  size = 1<<adr_width;
  MEM = (uint64*)malloc(size*sizeof(uint64));
}

//****************************************************************************
dp_ram::~dp_ram()
{
  free(MEM);
}

//****************************************************************************
void dp_ram::Update(SClock clk_wr, 
                    uint32 we,
                    uint32 adr_wr,
                    uint64 idata, 
                    SClock clk_rd,
                    uint32 re,
                    uint32 adr_rd,
                    uint64 &odata)
{
  rbRdAdr.CLK = clk_rd;
  if(re) rbRdAdr.D = BITS32(adr_rd, adr_width-1, 0);
  odata = MEM[rbRdAdr.Q];

  WrClkPosedge=0;
  if(clk_wr.eClock==SClock::CLK_POSEDGE) WrClkPosedge=1;
  
  WrEna = we;
  WrAdr = BITS32(adr_wr,adr_width-1,0);
  WrData = idata;
}





