//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GPL
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "ldbg.h"
#include "leon3_ml605\leon3mp.h"
extern leon3mp topLeon3mp;

//#define DBG_jtagpad

#ifdef DBG_jtagpad
  uint32 in_nTRST;
  SClock in_TCK;
  uint32 in_TMS;
  uint32 in_TDI;
  uint32 ch_TDO;
  uint32 ch_SelUser1;  //sel=1 when JTAG instruction register holds USER instruction
  uint32 ch_SelUser2;
  uint32 ch_TDI;       //fed through directly from FPGA TDI pin
  uint32 ch_DRCK;      //DRCK is same as TCK. if interface is not selected it remains high
  uint32 ch_Capture;   //Active-high pulse indicating Capture_DR state
  uint32 ch_Shift;     //Active-high. Indicate Shift_DR state
  uint32 ch_Update;    //Active-high indicating the Update_DR state
  uint32 ch_Reset;     //=1 reset output
  uint32 in_tdo1;        //2-nd register doesn't use
  JTagPad tst_jtagpad;
#endif


//****************************************************************************
void dbg::jtagpad_tb(SystemOnChipIO &io)
{
  uint32 *pin_nTRST     = &topLeon3mp.wNRst;
  SClock *pin_TCK       = &io.jtag.TCK;
  uint32 *pin_TMS       = &io.jtag.TMS;
  uint32 *pin_TDI       = &io.jtag.TDI;
  uint32 *pch_TDO       = &io.jtag.TDO;
  uint32 *pch_SelUser1  = &topLeon3mp.clAhbMasterJtag.wPadOutAdrSel;
  uint32 *pch_SelUser2  = &topLeon3mp.clAhbMasterJtag.wPadOutDataSel;
  uint32 *pch_TDI       = &topLeon3mp.clAhbMasterJtag.wPadOutTDI;
  uint32 *pch_DRCK      = &topLeon3mp.clAhbMasterJtag.wPadOutDRCK;
  uint32 *pch_Capture   = &topLeon3mp.clAhbMasterJtag.wPadOutCapture;
  uint32 *pch_Shift     = &topLeon3mp.clAhbMasterJtag.wPadOutShift;
  uint32 *pch_Update    = &topLeon3mp.clAhbMasterJtag.wPadOutUpdate;
  uint32 *pch_Reset     = &topLeon3mp.clAhbMasterJtag.wPadOutReset;
  uint32 *pin_tdo1      = &topLeon3mp.clAhbMasterJtag.wComOutTDO;
  JTagPad *ptst_jtagpad = &topLeon3mp.clAhbMasterJtag.clJTagPad;

#ifdef DBG_jtagpad
  if(io.inClk.eClock_z==SClock::CLK_POSEDGE)
  {
    //
    in_TMS = rand()&0x1;
    in_TDI = rand()&0x1;
    in_tdo1 = rand()&0x1;        //2-nd register doesn't use
  }
  in_TCK = io.inClk;

  ptst_jtagpad->Update( *pin_nTRST,
                        in_TCK,
                        in_TMS,
                        in_TDI,
                        ch_TDO,
                        ch_SelUser1,  //sel=1 when JTAG instruction register holds USER instruction
                        ch_SelUser2,
                        ch_TDI,       //fed through directly from FPGA TDI pin
                        ch_DRCK,      //DRCK is same as TCK. if interface is not selected it remains high
                        ch_Capture,   //Active-high pulse indicating Capture_DR state
                        ch_Shift,     //Active-high. Indicate Shift_DR state
                        ch_Update,    //Active-high indicating the Update_DR state
                        ch_Reset,     //=1 reset output
                        in_tdo1 );    //2-nd register doesn't use
  
  pin_TCK       = &in_TCK;
  pin_TMS       = &in_TMS;
  pin_TDI       = &in_TDI;
  pch_TDO       = &ch_TDO;
  pch_SelUser1  = &ch_SelUser1;
  pch_SelUser2  = &ch_SelUser2;
  pch_TDI       = &ch_TDI;
  pch_DRCK      = &ch_DRCK;
  pch_Capture   = &ch_Capture;
  pch_Shift     = &ch_Shift;
  pch_Update    = &ch_Update;
  pch_Reset     = &ch_Reset;
  pin_tdo1      = &in_tdo1;

  ptst_jtagpad = &tst_jtagpad;
#endif

  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';

    ResetPutStr();
  
    // inputs:
    pStr = PutToStr(pStr, *pin_nTRST, 1, "in_nTRST");
    pStr = PutToStr(pStr, *pin_TMS, 1, "in_TMS");
    pStr = PutToStr(pStr, *pin_TDI, 1, "in_TDI");
    pStr = PutToStr(pStr, *pin_tdo1, 1, "in_tdo1");
    pStr = PutToStr(pStr, *pch_TDO, 1, "ch_TDO");
    pStr = PutToStr(pStr, *pch_SelUser1, 1, "ch_SelUser1");
    pStr = PutToStr(pStr, *pch_SelUser2, 1, "ch_SelUser2");
    pStr = PutToStr(pStr, *pch_TDI, 1, "ch_TDI");
    pStr = PutToStr(pStr, *pch_DRCK, 1, "ch_DRCK");
    pStr = PutToStr(pStr, *pch_Capture, 1, "ch_Capture");
    pStr = PutToStr(pStr, *pch_Shift, 1, "ch_Shift");
    pStr = PutToStr(pStr, *pch_Update, 1, "ch_Update");
    pStr = PutToStr(pStr, *pch_Reset, 1, "ch_Reset");

    //

    PrintIndexStr();

    *posBench[TB_jtagpad] << chStr << "\n";
  }

#ifdef DBG_jtagpad
  ptst_jtagpad->ClkUpdate();
#endif
}



