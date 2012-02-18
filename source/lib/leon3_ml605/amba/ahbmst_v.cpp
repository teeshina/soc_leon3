//****************************************************************************
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************
//              Verilog styled AHB master controller
//
//              This implementation is analog to the Gaisler one, and it has 
//              disadvantage, it rises "hbusreq" signal on 2 clocks instead of
//              one
//****************************************************************************

#include "lheaders.h"

//****************************************************************************
void AhbMaster::Update( uint32 inNRst,
                        SClock inClk,
                        ahb_dma_in_type &in_dmai,// DMA request:
                        ahb_dma_out_type &out_dmao,// DMA response:
                        ahb_mst_in_type &in_ahbi,// AHB bus response:
                        ahb_mst_out_type  &out_ahbo// Request to AHB:
                      )
{
  // Address control:
  wbAdrInc = (0x1<<in_dmai.size);
  if(ADDR_INC>0)  wbNextAdr = (BITS32(in_dmai.address,9,0) + wbAdrInc) & MSK32(9,0);
  else            wbNextAdr = BITS32(in_dmai.address,9,0);
  
  if(in_dmai.start & in_dmai.burst & rActive.Q & !rRetry.Q)
  {
    wbCurAdr &= ~MSK32(9,0);
    wbCurAdr |= wbNextAdr;
  }else
    wbCurAdr = in_dmai.address;

  // state control:
  if(rRetry.Q | !in_dmai.start)        wbTransferType = HTRANS_IDLE;
  else if(!rActive.Q | !in_dmai.burst) wbTransferType = HTRANS_NONSEQ;
  else if(in_dmai.busy)                wbTransferType = HTRANS_BUSY;
  else                                 wbTransferType = HTRANS_SEQ;

  wRespEnd = ((in_ahbi.hresp==HRESP_OKAY)||(in_ahbi.hresp==HRESP_ERROR)) ? 1: 0;

  // Ready bit
  if(rActive.Q & in_ahbi.hready & wRespEnd) wReady = 1;
  else                                      wReady = 0;

  // Retry bit:
  if(rActive.Q & in_ahbi.hready & !wRespEnd ) wRetry = 1;
  else                                        wRetry = 0;

  // Error state occures:
  if(rActive.Q&&in_ahbi.hready&&(in_ahbi.hresp==HRESP_ERROR) )  wErrResponse = 1;
  else                                                          wErrResponse = 0;

  
  // bus granted to "hindex" master:
  rGrant.CLK = inClk;
  if(in_ahbi.hready) rGrant.D = BIT32(in_ahbi.hgrant,HMST_INDEX);
  
  // Address has been read by AHB then set Active=1:
  rActive.CLK = inClk;
  if(!inNRst)                                            rActive.D = 0;
  else if(in_ahbi.hready&&(wbTransferType!=HTRANS_IDLE)) rActive.D = rGrant.Q;
  else if(in_ahbi.hready)                                rActive.D = 0;

  // master start pulse:
  rStart.CLK = inClk;
  if(in_ahbi.hready&&(wbTransferType!=HTRANS_IDLE)) rStart.D = rGrant.Q;
  else                                              rStart.D = 0;

  // retry strob:
  rRetry.CLK = inClk;
  if(!inNRst)                                       rRetry.D = 0;
  else if(rActive.Q & !in_ahbi.hready & !wRespEnd ) rRetry.D = 1;
  else if(rActive.Q)                                rRetry.D = 0;

  
  out_ahbo.haddr   = wbCurAdr;
  out_ahbo.htrans  = wbTransferType;
  out_ahbo.hbusreq = in_dmai.start;
  out_ahbo.hwdata  = in_dmai.wdata;
  out_ahbo.hconfig = hconfig;
  out_ahbo.hlock   = 0;
  out_ahbo.hwrite  = in_dmai.write;
  out_ahbo.hsize   = in_dmai.size;
  out_ahbo.hburst  = in_dmai.burst ? HBURST_INCR: HBURST_SINGLE; // 3 bit width state value
  out_ahbo.hprot   = BITS32(PROTECT_BITS,3,0);               // non-cached supervise data
  out_ahbo.hirq    = in_dmai.irq<<IRQ_BIT_INDEX;
  out_ahbo.hindex  = HMST_INDEX;

#if 1
  if(iClkCnt>=199)
  bool st = true;
#endif
  out_dmao.start   = rStart.Q;
  out_dmao.active  = rActive.Q;
  out_dmao.ready   = wReady;
  out_dmao.mexc    = wErrResponse;
  out_dmao.retry   = wRetry;
  out_dmao.haddr   = wbNextAdr;
  out_dmao.rdata   = in_ahbi.hrdata;

}


