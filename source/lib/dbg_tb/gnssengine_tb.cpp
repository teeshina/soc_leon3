//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GPL
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "lheaders.h"

#ifdef DBG_GnssEngine
struct STst{ uint32 iClkNum, adr; uint32 val : 28; };
STst stst[] = 
{
  {40,  0x80000400, 0x11111111},
  {42,  0x80000404, 0x0fffffff},
  {44,  0x80000408, 0x08fffffe},
  {46,  0x8000040C, 0x08fffffe},
  {48,  0x80000410, 0x08fffffe},
  {50,  0x80000414, 0x08fffff1},
  {52,  0x80000418, 0x08fffff1},
  {54,  0x8000041c, 0x08fffff1},
  {56,  0x80000420, 0x08fffff1},
  {58,  0x80000424, 0x08fffff1},
  {70,  0x80000428, 0x00000005},
};
#endif

//****************************************************************************
void dbg::GnssEngine_tb(SystemOnChipIO &io)
{
  ahb_slv_in_type   *pin_ahbsi  = &topLeon3mp.stCtrl2Slv;
  ahb_slv_out_type  *pch_ahbso  = &topLeon3mp.stSlv2Ctrl.arr[AHB_SLAVE_GNSSENGINE];//    : in  apb_slv_out_vector
  uint32 *pinGpsI = &io.gnss.I[0];
  uint32 *pinGpsQ = &io.gnss.Q[0];
  uint32 *pinGloI = &io.gnss.I[1];
  uint32 *pinGloQ = &io.gnss.Q[1];

  GnssEngine        *p_GnssEngine = topLeon3mp.pclGnssEngine;


#ifdef DBG_GnssEngine
  if(io.inClk.eClock_z==SClock::CLK_POSEDGE)
  {
    //
//    in_apbi.psel = 0x1<<APB_RF_CONTROL;//[0:15]  : (0 to APB_SLAVES_MAX-1); -- slave select
//    in_apbi.penable = 0;// ;                       -- strobe
//    in_apbi.pwrite = 0;
    for (int32 i=0; i<sizeof(stst)/sizeof(STst); i++)
    {
      if(iClkCnt==stst[i].iClkNum)
      {
//        in_apbi.pwrite = 1;
//        in_apbi.penable = 1;
//        in_apbi.paddr = stst[i].adr;
//        in_apbi.pwdata = stst[i].val;
      }
    }
  }

  tst_GnssEngine.Update(topLeon3mp.wNRst,
                       io.inClk,
                       in_ahbsi,
                       ch_ahbso,
                       io.gnss.adc_clk,
                       io.gnss.I[0],
                       io.gnss.Q[0],
                       io.gnss.I[1],
                       io.gnss.Q[1] );
  
  pin_ahbsi = &in_ahbsi;
  pch_ahbso = &ch_ahbso;
  pinGpsI = &inGpsI;
  pinGpsQ = &inGpsQ;
  pinGloI = &inGloI;
  pinGloQ = &inGloQ;

  p_GnssEngine = &tst_GnssEngine;
#endif

  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';

    ResetPutStr();
  
    // inputs:
    pStr = PutToStr(pStr, topLeon3mp.wNRst, 1, "inNRst");
    //


    // Output:

    // Internal:

    PrintIndexStr();

    *posBench[TB_GnssEngine] << chStr << "\n";
  }

#ifdef DBG_GnssEngine
  tst_GnssEngine.ClkUpdate();
#endif
}



