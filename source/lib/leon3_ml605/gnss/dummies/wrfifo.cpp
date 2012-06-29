//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GPL
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "wrfifo.h"

void WrFifo::Update( uint32 inNRst,
                      SClock inBusClk,
                      SClock inAdcClk,
                      uint32 inWrEna,
                      uint32 inWrAdr,
                      uint32 inWrData,
                      uint32 &outDataRdy,
                      uint32 &outAdr,
                      uint32 &outData
                    )
{
  bv = br.Q;
  av = ar.Q;
  
  if(ar.Q.rd_adr == ar.Q.wr_adr2) empty = 1;
  else                            empty = 0;
  
  //
  // TODO: Code Gray counter implementation:
  //
  if(inWrEna) bv.wr_adr = br.Q.wr_adr+1;
  else        bv.wr_adr = br.Q.wr_adr;

  // clock transition BusClk to AdcClk:
  av.wr_adr1 = br.Q.wr_adr;
  av.wr_adr2 = ar.Q.wr_adr1;
  
  // reading address:
  if(!empty) av.rd_adr = ar.Q.rd_adr+1;
  else       av.rd_adr = ar.Q.rd_adr;

  if(!inNRst)  
  {
    av.rd_adr = 0;
    av.wr_adr1 = 0;
    av.wr_adr2 = 0;
    bv.wr_adr = 0;
  }
  
  br.CLK = inBusClk;
  br.D = bv;
  
  ar.CLK = inAdcClk;
  ar.D = av;
  
  outDataRdy = !empty;
  outAdr = (uint32)(DpMem[ar.Q.rd_adr]>>32);
  outData = (uint32)DpMem[ar.Q.rd_adr];
  
                   
  if((inBusClk.eClock==SClock::CLK_POSEDGE)&&inWrEna)
     DpMem[br.Q.wr_adr] = (uint64(inWrAdr)<<32)|uint64(inWrData);

}
