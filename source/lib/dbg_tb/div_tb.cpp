#include "lheaders.h"

extern leon3mp  topLeon3mp;

extern void ResetPutStr();
extern void PrintIndexStr();

#ifdef DBG_div32
//****************************************************************************
void dbg::div32_tb(SystemOnChipIO &io)
{
  if(io.inClk.eClock_z==SClock::CLK_POSEDGE)
  {
    uint32 tmp;
    divi.y = ((((uint64)rand())<<18) + ((uint64)rand()))&0x1FFFFFFFF;// Y (MSB divident)
    divi.op1 = ((((uint64)rand())<<18) + ((uint64)rand()))&0x1FFFFFFFF;// operand 1 (LSB divident)
    divi.op2 = ((((uint64)rand())<<18) + ((uint64)rand()))&0x1FFFFFFFF;// operand 2 (divisor)
    tmp = rand();
    divi.Signed = tmp&0x1;
    divi.start = ((tmp>>2)&0x1);
    if((iClkCnt>1500)&&(iClkCnt<2500))
    {
      divi.flush = ((tmp>>3)&0x1);
      holdnx = ((tmp>>4)&0x1);
    }else
    {
      divi.flush = 0;
      holdnx = 1;
    }
  }

  tst_div32.Update( io.inNRst,
                    io.inClk,
                    holdnx,
                    divi,
                    divo);

  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';

    ResetPutStr();
  
    // inputs:
    pStr = PutToStr(pStr, io.inNRst, 1, "inNRst");
    pStr = PutToStr(pStr, holdnx, 1, "holdnx");
    pStr = PutToStr(pStr, divi.y, 33,"divi.y");
    pStr = PutToStr(pStr, divi.op1, 33,"divi.op1");
    pStr = PutToStr(pStr, divi.op2, 33,"divi.op2");
    pStr = PutToStr(pStr, divi.flush, 1,"divi.flush");
    pStr = PutToStr(pStr, divi.Signed, 1,"divi.Signed");
    pStr = PutToStr(pStr, divi.start, 1,"divi.start");

    // outputs:
    pStr = PutToStr(pStr, divo.ready, 1, "ch_divo.ready");
    pStr = PutToStr(pStr, divo.nready, 1, "ch_divo.nready");
    pStr = PutToStr(pStr, divo.icc, 4, "ch_divo.icc");
    pStr = PutToStr(pStr, divo.result, 32, "ch_divo.result");
  
    // internal registers:
    pStr = PutToStr(pStr, tst_div32.vready, 1,"ch_vready");
    pStr = PutToStr(pStr, tst_div32.vnready, 1, "ch_vnready");
    pStr = PutToStr(pStr, tst_div32.vaddin1, 33, "ch_vaddin1");
    pStr = PutToStr(pStr, tst_div32.vaddin2, 33, "ch_vaddin2");
    pStr = PutToStr(pStr, tst_div32.vaddsub, 1,"ch_vaddsub");
    pStr = PutToStr(pStr, tst_div32.ymsb, 1, "ch_ymsd");

    pStr = PutToStr(pStr, tst_div32.v.x[0],64,"ch_v_x[63 downto 0]");
    pStr = PutToStr(pStr, tst_div32.v.x[1],1,"ch_v_x[64]");
    pStr = PutToStr(pStr, tst_div32.v.state, 3, "ch_v_state");
    pStr = PutToStr(pStr, tst_div32.v.zero, 1, "ch_v_zero");
    pStr = PutToStr(pStr, tst_div32.v.zero2, 1, "ch_v_zero2");
    pStr = PutToStr(pStr, tst_div32.v.qcorr, 1, "ch_v_qcorr");
    pStr = PutToStr(pStr, tst_div32.v.zcorr, 1, "ch_v_zcorr");
    pStr = PutToStr(pStr, tst_div32.v.qzero, 1, "ch_v_qzero");
    pStr = PutToStr(pStr, tst_div32.v.qmsb, 1, "ch_v_qmsb");
    pStr = PutToStr(pStr, tst_div32.v.ovf, 1, "ch_v_ovf");
    pStr = PutToStr(pStr, tst_div32.v.neg, 1, "ch_v_neg");
    pStr = PutToStr(pStr, tst_div32.v.cnt, 5, "ch_v_cnt");

    pStr = PutToStr(pStr, tst_div32.addin1, 33, "ch_addin1");
    pStr = PutToStr(pStr, tst_div32.addin2, 33, "ch_addin2");
    pStr = PutToStr(pStr, tst_div32.addout, 33, "ch_addout");

    PrintIndexStr();

    *posBench[TB_div32] << chStr << "\n";
  }

  tst_div32.ClkUpdate();
}
#endif
