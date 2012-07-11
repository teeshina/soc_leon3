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
#include "leon3_ml605\gnss\buses.h"
#include "leon3_ml605\gnss\dummies\dp_ram.h"
#include "leon3_ml605\gnss\dummies\wrfifo.h"
#include "leon3_ml605\gnss\dummies\glbtimer.h"
#include "leon3_ml605\gnss\dummies\gnssctrl.h"
#include "leon3_ml605\gnss\dummies\chnpack.h"
#include "leon3_ml605\gnss\dummies\noise.h"
#include "leon3_ml605\gnss\dummies\gnssmisc.h"

class GnssTop
{
  friend class dbg;
  private:
    //TDpRam<uint64> *pDpRam;
    dp_ram *pDpRam;
    uint64 rd_val; 
    
    struct BusRegs
    {
      uint32 word_sel : 1;
      uint32 IrqPulse1 : 1;
      uint32 IrqPulse2 : 1;
      uint32 IrqPulse3 : 1;
    };
    
    BusRegs bv;
    TDFF<BusRegs> br;//

    uint32 FifoOutDataRdy;
    uint32 FifoOutAdr;
    uint32 FifoOutData;
    WrFifo clFifo;


    m2c_tot m2c;
    Ctrl2Module c2m;
    uint32 CtrlOutIrqPulse;
    uint32 CtrlOutMemWrEna;
    uint32 CtrlOutMemWrAdr;
    uint64 CtrlOutMemWrData;

    GnssControl clGnssControl;

    uint32 GlbTmrOutMsReady;
    GlobalTimer clGlobalTimer;
    
    uint64 wbChnOutData;
    ChannelsPack clChannelsPack;

    int32 mscGpsI;
    int32 mscGpsQ;
    int32 mscGloI;
    int32 mscGloQ;
    GnssMisc clGnssMisc;
    
    Noise    clNoise;

    uint64 wbSelData; // multiplexer on top level
    
  public:
    GnssTop();
    ~GnssTop();
    
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
      br.ClkUpdate();
      pDpRam->ClkUpdate();
      clFifo.ClkUpdate();
      clGnssControl.ClkUpdate();
      clGlobalTimer.ClkUpdate();
      clChannelsPack.ClkUpdate();
      clGnssMisc.ClkUpdate();
      clNoise.ClkUpdate();
    }
};
