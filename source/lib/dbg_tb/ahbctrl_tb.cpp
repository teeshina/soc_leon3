#include "lheaders.h"

void dbg::ahbctrl_tb(SystemOnChipIO &io)
{
  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';

    pStr = PutToStr(pStr, io.inNRst, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.wPadOutDRCK, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.wPadOutTDI, 1);
    pStr = PutToStr(pStr, (uint32)0, 8);//inst
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.wPadOutAdrSel, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.wPadOutDataSel, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.wPadOutReset, 1);
    pStr = PutToStr(pStr, (uint32)0, 1);//capt
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.wPadOutShift, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.wPadOutUpdate, 1);

    pStr = PutToStr(pStr, io.jtag.TDO, 1);
    
    
    *posBench[TB_AHBCTRL] << chStr << "\n";
  }
}
