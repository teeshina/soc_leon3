#include "lheaders.h"


void dbg::jtagcom_tb(SystemOnChipIO &io)
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
    
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.ahbmstout_dmao.start, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.ahbmstout_dmao.active, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.ahbmstout_dmao.ready, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.ahbmstout_dmao.retry, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.ahbmstout_dmao.mexc, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.ahbmstout_dmao.haddr, 10);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.ahbmstout_dmao.rdata, 32);

    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.wComOutTDO, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jcomout_dmai.address, 32);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jcomout_dmai.wdata, 32);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jcomout_dmai.start, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jcomout_dmai.burst, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jcomout_dmai.write, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jcomout_dmai.busy, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jcomout_dmai.irq, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jcomout_dmai.size, 3);
    
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jtagcom0.rAdrSel[0].Q, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jtagcom0.rAdrSel[1].Q, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jtagcom0.rDataSel[0].Q, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jtagcom0.rDataSel[1].Q, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jtagcom0.rShift[0].Q, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jtagcom0.rShift[1].Q, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jtagcom0.rShift[2].Q, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jtagcom0.rTCK[0].Q, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jtagcom0.rTCK[1].Q, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jtagcom0.rTCK[2].Q, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jtagcom0.rTDI[0].Q, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jtagcom0.rTDI[1].Q, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jtagcom0.rTDI[2].Q, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jtagcom0.rUpdate[0].Q, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jtagcom0.rUpdate[1].Q, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jtagcom0.rUpdate[2].Q, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jtagcom0.rSeq.Q, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jtagcom0.rHoldn.Q, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jtagcom0.rReset[0].Q, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jtagcom0.rReset[1].Q, 1);
    pStr = PutToStr(pStr, (uint32)topLeon3mp.clAhbMasterJtag.jtagcom0.rbState.Q, 2);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jtagcom0.rbAdr.Q, 35);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jtagcom0.rbData.Q, 33);
    
    *posBench[TB_JTAGCOM] << chStr << "\n";
  }
}
