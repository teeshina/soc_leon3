//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      GNSS Sensor Limited
// License:     GPL
// Contact:     alex.kosin@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "lheaders.h"



//****************************************************************************
void GnssControl::Update(uint32 inNRst,
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
                          uint32 &outIrqEna )
{
  v = r.Q;
  
  if(inRdEna) 
    v.RdAdr = inRdAdr; 
  
  if(BIT32(r.Q.RdAdr,2) == 0) hrdata = (uint32)BITS64(dpMEM[BITS32(r.Q.RdAdr, CFG_GNSS_ADDR_WIDTH-1, 3)], 31, 0);
  else                        hrdata = (uint32)BITS64(dpMEM[BITS32(r.Q.RdAdr, CFG_GNSS_ADDR_WIDTH-1, 3)], 63, 32);
  
  v.WrMemEna = inWrEna;
  v.WrMemAdr = BITS32(inWrAdr, CFG_GNSS_ADDR_WIDTH-1, 0);
  v.WrMemVal = inWrData;
  
  if(inMsReady)                                     v.SnapEna = 1;
  else if(r.Q.SnapCnt==(CFG_GNSS_MEMORY_SIZE64-1))  v.SnapEna = 0;
  
  if(inMsReady)                      v.SnapCnt = 0;
  else if(r.Q.SnapEna & inRdDataRdy) v.SnapCnt = r.Q.SnapCnt + 1;

  if(r.Q.SnapCnt==(CFG_GNSS_MEMORY_SIZE64-1) && inRdDataRdy) v.IrqEna = 1;
  else                                                       v.IrqEna = 0;

  if((inBusClk.eClock==SClock::CLK_POSEDGE) && inRdDataRdy)
  {
    dpMEM[r.Q.SnapCnt] = inRdData;
  }

  // reset:
  if(!inNRst)
  {
    v.SnapEna = 0;
    v.SnapCnt = 0;
  }

  outRdData    = hrdata;

  outMuxBus.wWrEna        = r.Q.WrMemEna;
  outMuxBus.wbWrModuleSel = BITS32(r.Q.WrMemAdr, CFG_GNSS_ADDR_WIDTH-1, 6);
  outMuxBus.wbWrFieldSel  = BITS32(r.Q.WrMemAdr, 5, 2);  // 8 x 64 bits words
  outMuxBus.wbWrData      = r.Q.WrMemVal;
  
  outMuxBus.wRdEna        = r.Q.SnapEna;
  outMuxBus.wbRdModuleSel = BITS32(r.Q.SnapCnt, CFG_GNSS_ADDR_WIDTH-1, 3);
  outMuxBus.wbRdFieldSel  = BITS32(r.Q.SnapCnt, 2, 0);
  
  outIrqEna = r.Q.IrqEna;
  
  r.CLK = inBusClk;
  r.D   = v;
}
