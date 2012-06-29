#include "ldbg.h"
#include "leon3_ml605\leon3mp.h"
extern leon3mp topLeon3mp;


void dbg::ahbjtag_tb(SystemOnChipIO &io)
{
  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';

    pStr = PutToStr(pStr, io.inNRst, 1);
    //pStr = PutToStr(pStr, io.TCK, 1);
    //pStr = PutToStr(pStr, io.TDI, 1);
    //pStr = PutToStr(pStr, io.TMS, 1);
    //pStr = PutToStr(pStr, io.nTRST, 1);
    
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Mst.hgrant, 1);
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Mst.hready, 1);
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Mst.hresp, 2);
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Mst.hrdata, 32);

    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.ahbmstout_dmao.start, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.ahbmstout_dmao.active, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.ahbmstout_dmao.ready, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.ahbmstout_dmao.retry, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.ahbmstout_dmao.mexc, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.ahbmstout_dmao.haddr, 10);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.ahbmstout_dmao.rdata, 32);
    

    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jcomout_dmai.address, 32);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jcomout_dmai.wdata, 32);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jcomout_dmai.start, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jcomout_dmai.burst, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jcomout_dmai.write, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jcomout_dmai.busy, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jcomout_dmai.irq, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jcomout_dmai.size, 3);

    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jtagcom0.rbState.Q, 2);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jtagcom0.rbAdr.Q, 35);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jtagcom0.rbData.Q, 33);
    
    *posBench[TB_ahbjtag] << chStr << "\n";
  }
}
