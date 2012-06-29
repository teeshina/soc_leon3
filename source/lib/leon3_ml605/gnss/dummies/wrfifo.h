//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GPL
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#pragma once

#include "stdtypes.h"
#include "dff.h"

class WrFifo
{
  friend class dbg;
  private:
    static const int32 FIFO_BUS_SIZE = 4;
    static const int32 FIFO_SIZE = 1<<FIFO_BUS_SIZE;
    uint64 DpMem[FIFO_SIZE];
    
    struct bus_regs
    {
      uint32 wr_adr : FIFO_BUS_SIZE;
    };
    struct adc_regs
    {
      uint32 wr_adr1 : FIFO_BUS_SIZE;
      uint32 wr_adr2 : FIFO_BUS_SIZE;
      uint32 rd_adr  : FIFO_BUS_SIZE;
    };

    uint32 empty : 1;
   
    bus_regs bv;
    TDFF<bus_regs> br;
    adc_regs av;
    TDFF<adc_regs> ar;
    
    
  public:
    void Update(uint32 inNRst,
                SClock inClkBus,
                SClock inClkAdc,
                uint32 inWrEna,
                uint32 inWrAdr,
                uint32 inWrData,
                uint32 &outDataRdy,
                uint32 &outAdr,
                uint32 &outData );
    
    void ClkUpdate()
    {
      br.ClkUpdate();
      ar.ClkUpdate();
    }
};
