//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      GNSS Sensor Limited
// License:     GPL
// Contact:     alex.kosin@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************
// WARNING:     DUMMY version of the GNSS top level: interrupt generator
//              and simple accumulators.
//****************************************************************************

#include "lheaders.h"


//****************************************************************************
GnssTop::GnssTop()
{
  pDpRam = new dp_ram(CFG_GNSS_ADDR_WIDTH);
}

//****************************************************************************
GnssTop::~GnssTop()
{
  free(pDpRam);
}

//****************************************************************************
void GnssTop::Update( uint32 inNRst,
                      SClock inBusClk,
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
                      uint32 inGloQ )
{
  // multiplexer:
  
  clFifo.Update( inNRst, inBusClk, inAdcClk,
                 inWrEna, inWrAdr, inWrData, FifoOutDataRdy, FifoOutAdr, FifoOutData );

  clGnssControl.Update(inNRst,inAdcClk, FifoOutAdr, FifoOutDataRdy, FifoOutData,
                       m2c, c2m,
                       GlbTmrOutMsReady, CtrlOutIrqPulse,
                       CtrlOutMemWrEna, CtrlOutMemWrAdr, CtrlOutMemWrData);

  clGlobalTimer.Update( inNRst,
                        inAdcClk,
                        c2m,
                        m2c.arr[MODULE_ID_GLB_TIMER],
                        GlbTmrOutMsReady );
                        
  clChannelsPack.Update(inNRst, inAdcClk, inGpsI, inGpsQ, inGloI, inGloQ,
                        GlbTmrOutMsReady, c2m, &m2c.arr[0]);

  // output reading interface via dual-port RAM
  pDpRam->Update(inAdcClk, CtrlOutMemWrEna, CtrlOutMemWrAdr, CtrlOutMemWrData,
                 inBusClk, inRdEna, (inRdAdr>>3), rd_val);

  if(inRdEna) 
    bv.word_sel = BIT32(inRdAdr,2);

  if(br.Q.word_sel==0) outRdData = (uint32)rd_val;
  else                 outRdData = (uint32)(rd_val>>32);

  
  // IRQ pulse clock transition:
  bv.IrqPulse1 = CtrlOutIrqPulse;
  bv.IrqPulse2 = br.Q.IrqPulse1;
  bv.IrqPulse3 = br.Q.IrqPulse2;

  outIrqPulse  = br.Q.IrqPulse3 & !br.Q.IrqPulse2;

  // registers:
  br.CLK = inBusClk;
  br.D = bv;
}
