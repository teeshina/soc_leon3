//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      GNSS Sensor Limited
// License:     GPL
// Contact:     alex.kosin@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "leon3_ml605\config.h"
#include "gnssctrl.h"

#define last_module (CFG_GNSS_MODULES_TOTAL-1)
#define last_field (CFG_GNSS_DWORD_PER_MODULE-1)

//****************************************************************************
void GnssControl::Update(uint32 inNRst,
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
                          uint64 &outMemWrData
                         )
{
  v = r.Q;
  
  if(inWrEna) v.wr_ena = 1;
  else        v.wr_ena = 0;
  
  if(inWrEna) v.wr_module_sel = BITS32(inWrAdr, CFG_GNSS_ADDR_WIDTH-1, 6);
  if(inWrEna) v.wr_field_sel  = BITS32(inWrAdr, 5, 2);
  if(inWrEna) v.wr_data       = inWrData;
  
  
  snap_end = 0;
  if((r.Q.SnapFieldCnt==last_field)&&(r.Q.SnapModuleCnt==last_module)) snap_end = 1;
  
  // Reading is fully synchronous with all modules:
  if(inMsReady)      v.SnapEna = 1;
  else if(snap_end)  v.SnapEna = 0;
  v.SnapEna_z = r.Q.SnapEna;

  if(inMsReady | !r.Q.SnapEna | snap_end | (r.Q.SnapFieldCnt==last_field)) v.SnapFieldCnt = 0;
  else if(r.Q.SnapEna)                    v.SnapFieldCnt = r.Q.SnapFieldCnt + 1;
  v.SnapFieldCnt_z = r.Q.SnapFieldCnt;

  if(inMsReady| !r.Q.SnapEna | snap_end)              v.SnapModuleCnt = 0;
  else if(r.Q.SnapEna &&(r.Q.SnapFieldCnt==last_field)) v.SnapModuleCnt = r.Q.SnapModuleCnt+1;
  v.SnapModuleCnt_z = r.Q.SnapModuleCnt;

 
  // mux data for DP writting:  
  v_m2c = m2c.arr[r.Q.SnapModuleCnt_z];


  // reset:
  if(!inNRst)
  {
    v.SnapEna = 0;
    v.SnapFieldCnt = 0;
    v.SnapModuleCnt = 0;
    v.SnapEna_z = 0;
    v.SnapFieldCnt_z = 0;
    v.SnapModuleCnt_z = 0;
    v.wr_ena = 0;
  }

  outMemWrEna = r.Q.SnapEna_z;
  outMemWrAdr = (r.Q.SnapModuleCnt<<(log2x[CFG_GNSS_DWORD_PER_MODULE])) | r.Q.SnapFieldCnt_z;
  outMemWrData = v_m2c.rdata;

  c2m.wr_ena        = r.Q.wr_ena;
  c2m.wr_module_sel = r.Q.wr_module_sel;
  c2m.wr_field_sel  = r.Q.wr_field_sel;
  c2m.wr_data       = r.Q.wr_data;

  c2m.rd_ena        = r.Q.SnapEna;
  c2m.rd_module_sel = r.Q.SnapModuleCnt;
  c2m.rd_field_sel  = r.Q.SnapFieldCnt;
  
  outIrqPulse = r.Q.SnapEna_z & !r.Q.SnapEna;
  
  r.CLK = inAdcClk;
  r.D   = v;
}
