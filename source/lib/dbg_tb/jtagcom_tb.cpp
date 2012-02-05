#include "lheaders.h"

extern leon3mp  topLeon3mp;

extern void ResetPutStr();
extern void PrintIndexStr();


void dbg::jtagcom_tb(SystemOnChipIO &io)
{
  uint32 *pinTCK = &topLeon3mp.clAhbMasterJtag.wPadOutDRCK;
  uint32 *pinTDI = &topLeon3mp.clAhbMasterJtag.wPadOutTDI;
  uint32 *pinAdrSel = &topLeon3mp.clAhbMasterJtag.wPadOutAdrSel;
  uint32 *pinDataSel = &topLeon3mp.clAhbMasterJtag.wPadOutDataSel;
  uint32 *pinShift = &topLeon3mp.clAhbMasterJtag.wPadOutShift;
  uint32 *pinUpdate = &topLeon3mp.clAhbMasterJtag.wPadOutUpdate;
  uint32 *pinRESET = &topLeon3mp.clAhbMasterJtag.wPadOutReset;
  uint32 *pchTDO = &topLeon3mp.clAhbMasterJtag.wComOutTDO;
  ahb_dma_out_type *pin_dmao = &topLeon3mp.clAhbMasterJtag.ahbmstout_dmao;// : in  ahb_dma_out_type;    
  ahb_dma_in_type  *pch_dmai = &topLeon3mp.clAhbMasterJtag.jcomout_dmai;// : out ahb_dma_in_type
  jtagcom *p_tst = &topLeon3mp.clAhbMasterJtag.jtagcom0;

#ifdef DBG_jtagcom
  if(io.inClk.eClock_z==SClock::CLK_POSEDGE)
  {
    in_dmao.start = rand()&0x1;
    in_dmao.active = rand()&0x1;
    in_dmao.ready = rand()&0x1;
    in_dmao.retry = rand()&0x1;
    in_dmao.mexc = rand()&0x1;
    in_dmao.haddr = rand()&0x3ff;
    in_dmao.rdata = (rand()<<17)|(rand());
  }
  
  tst_jtagcom.Update(io.inNRst, io.inClk, inTCK, inTDI, inAdrSel, inDataSel, 
                     inShift, inUpdate,inRESET, chTDO, in_dmao, ch_dmai);

  pinTCK = &inTCK;
  pinTDI = &inTDI;
  pinAdrSel = &inAdrSel;
  pinDataSel = &inDataSel;
  pinShift = &inShift;
  pinUpdate = &inUpdate;
  pinRESET = &inRESET;
  pchTDO = &chTDO;
  pin_dmao = &in_dmao;// : in  ahb_dma_out_type;    
  pch_dmai = &ch_dmai;// : out ahb_dma_in_type

#endif
  
  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';
    ResetPutStr();
    
    // Inputs:
    pStr = PutToStr(pStr, io.inNRst, 1,"inNRst");
    pStr = PutToStr(pStr, *pinTCK, 1, "in_tapo.tck");
    pStr = PutToStr(pStr, *pinTDI, 1, "in_tapo.tdi");
    pStr = PutToStr(pStr, *pinAdrSel, 1, "in_tapo.asel");
    pStr = PutToStr(pStr, *pinDataSel, 1, "in_tapo.dsel");
    pStr = PutToStr(pStr, *pinRESET, 1, "in_tapo.reset");
    pStr = PutToStr(pStr, *pinShift, 1, "in_tapo.shift");
    pStr = PutToStr(pStr, *pinUpdate, 1, "in_tapo.upd");
    //
    pStr = PutToStr(pStr, pin_dmao->start, 1, "in_dmao.start");
    pStr = PutToStr(pStr, pin_dmao->active, 1, "in_dmao.active");
    pStr = PutToStr(pStr, pin_dmao->ready, 1, "in_dmao.ready");
    pStr = PutToStr(pStr, pin_dmao->retry, 1,"in_dmao.retry");
    pStr = PutToStr(pStr, pin_dmao->mexc, 1, "in_dmao.mexc");
    pStr = PutToStr(pStr, pin_dmao->haddr, 10, "in_dmao.haddr");
    pStr = PutToStr(pStr, pin_dmao->rdata, 32, "in_dmao.rdata");

    pStr = PutToStr(pStr, *pchTDO, 1, "ch_tapi.tdo");
    pStr = PutToStr(pStr, pch_dmai->address, 32, "ch_dmai.address");
    pStr = PutToStr(pStr, pch_dmai->wdata, 32, "ch_dmai.wdata");
    pStr = PutToStr(pStr, pch_dmai->start, 1, "ch_dmai.start");
    pStr = PutToStr(pStr, pch_dmai->burst, 1, "ch_dmai.burst");
    pStr = PutToStr(pStr, pch_dmai->write, 1, "ch_dmai.write");
    pStr = PutToStr(pStr, pch_dmai->busy, 1, "ch_dmai.busy");
    pStr = PutToStr(pStr, pch_dmai->irq, 1, "ch_dmai.irq");
    pStr = PutToStr(pStr, pch_dmai->size, 3, "ch_dmai.size");
    
    pStr = PutToStr(pStr, p_tst->rAdrSel[0].Q, 1,"t_r.asel(1)");
    pStr = PutToStr(pStr, p_tst->rAdrSel[1].Q, 1,"t_r.asel(0)");
    pStr = PutToStr(pStr, p_tst->rDataSel[0].Q, 1,"t_r.dsel(1)");
    pStr = PutToStr(pStr, p_tst->rDataSel[1].Q, 1,"t_r.dsel(0)");
    pStr = PutToStr(pStr, p_tst->rShift[0].Q, 1,"t_r.shift(1)");
    pStr = PutToStr(pStr, p_tst->rShift[1].Q, 1,"t_r.shift(0)");
    pStr = PutToStr(pStr, p_tst->rShift[2].Q, 1,"t_r.shift2");
    pStr = PutToStr(pStr, p_tst->rTCK.Q, 2,"t_r.tck");
    pStr = PutToStr(pStr, BIT32(p_tst->rTCK.Q,2), 1,"t_r.tck2");
    pStr = PutToStr(pStr, p_tst->rTDI.Q, 2,"t_r.tdi");
    pStr = PutToStr(pStr, BIT32(p_tst->rTDI.Q,2), 1,"t_r.tdi2");
    pStr = PutToStr(pStr, p_tst->rUpdate[0].Q, 1,"t_r.upd(1)");
    pStr = PutToStr(pStr, p_tst->rUpdate[1].Q, 1,"t_r.upd(0)");
    pStr = PutToStr(pStr, p_tst->rUpdate[2].Q, 1,"t_r.upd2");
    pStr = PutToStr(pStr, p_tst->rSeq.Q, 1,"t_r.seq");
    pStr = PutToStr(pStr, p_tst->rHoldn.Q, 1,"t_r.holdn");
    pStr = PutToStr(pStr, p_tst->rReset.Q, 2,"t_r.trst");
    pStr = PutToStr(pStr, (uint32)p_tst->rbState.Q, 2,"t_r_state");
    pStr = PutToStr(pStr, p_tst->rbAdr.Q, 35,"t_r.addr");
    pStr = PutToStr(pStr, p_tst->rbData.Q, 33,"t_r.data");
    
    PrintIndexStr();

    *posBench[TB_jtagcom] << chStr << "\n";
  }

#ifdef DBG_jtagcom
  tst_jtagcom.ClkUpdate();
#endif
}
