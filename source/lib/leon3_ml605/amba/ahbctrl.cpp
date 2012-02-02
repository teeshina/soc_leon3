#include "lheaders.h"

//#define USE_PRIORITY_MASTER_ACCESS  // master with lesser index gets the access first

//****************************************************************************
AhbControl::AhbControl()
{
}

//****************************************************************************
void AhbControl::Update( uint32 inNRst,
                         SClock inClk,
                         ahb_mst_out_type *inMst2Ctrl,
                         ahb_mst_in_type  &outCtrl2Mst,
                         ahb_slv_out_type *inSlv2Ctrl,
                         ahb_slv_in_type  &outCtrl2Slv )
{
  // select master
  ahb_mst_out_type *pRqst = &inMst2Ctrl[AHB_MASTER_JTAG];
  ahb_slv_out_type *pSlv;
  ahb_mst_in_type  *pRspn = &outCtrl2Mst;
  
  // hready should rised into 1:
  //  1. idle state. No master acquired including first hbusreq signal
  //  2. Bus WaitAdr (granted) and slave.hready=1
  //  3. Bus WaitData and slave.hready=1

#ifdef USE_PRIORITY_MASTER_ACCESS
  wbMstSel=-1;
  for (int32 i=0; i<AHB_MASTER_TOTAL; i++)
  {
    if(inMst2Ctrl[i].hbusreq)
    {
      wbMstSel = i;
      break;
    }
  }
#else
  // Next logic has to provide bus access with equal priorities for the all masters.
  wbMstReqAll = 0;
  for (int32 i=0; i<AHB_MASTER_TOTAL; i++)
  {
    if(inMst2Ctrl[i].hbusreq)   wbMstReqAll |= (0x1<<i);
  }
  wbMstReqMask = (MSK32(AHB_MASTER_TOTAL-1,0)<<(rbMstSel[0].Q+1)) & MSK32(AHB_MASTER_TOTAL-1,0);

  wbMstSelHighPriority = -1;
  for (int32 i=0; i<AHB_MASTER_TOTAL; i++)
  {
    if(BIT32(wbMstReqAll,i) & !BIT32(wbMstReqMask,i)) 
    {
      wbMstSelHighPriority = i;
      break;
    }
  }

  wbMstSelLowPriority = -1;
  for (int32 i=0; i<AHB_MASTER_TOTAL; i++)
  {
    if(BIT32(wbMstReqAll,i) & BIT32(wbMstReqMask,i))
    {
      wbMstSelLowPriority = i;
      break;
    }
  }

  // Important:
  //    This value determines the "grant" signal, it doesn't depend of "burst" operation,
  //    it just set who will be the next master after current transaction.
  wbMstSelNext = (wbMstSelLowPriority!=-1) ? wbMstSelLowPriority
               : (wbMstSelHighPriority!=-1) ? wbMstSelHighPriority
               : -1;

#endif


  // Important:
  //      check condition of burst operation,
  //      don't change master index in case of (htrans==SEQ) (see page 66 of "AMBA specification")
  wNewBurst = (inMst2Ctrl[rbMstSel[0].Q].hburst!=HBURST_SINGLE)&&(inMst2Ctrl[rbMstSel[0].Q].htrans==HTRANS_NONSEQ);
  wGuardNewBurst = (rbMstSel[0].Q==rbLastBurstMst.Q)&&wNewBurst;

  if( (inMst2Ctrl[rbMstSel[0].Q].hburst != HBURST_SINGLE)
   && (inMst2Ctrl[rbMstSel[0].Q].hbusreq) && !wGuardNewBurst )
    wbMstSel = rbMstSel[0].Q;
  else
    wbMstSel = wbMstSelNext;

  // GUARD: to avoid several transaction from one master, store last BURST master index:
  rbLastBurstMst.CLK = inClk;
  if(!inNRst)                             rbLastBurstMst.D = -1;
  else if(wNewBurst && !wGuardNewBurst )  rbLastBurstMst.D = wbMstSel;
  else if(wbMstSel==-1)                   rbLastBurstMst.D = -1;



  // Check Slaves address spaces:
  wbSlvSel = -1;
  ahb_membar_type *pBank;
  for(int32 slv=0; slv<AHB_SLAVE_TOTAL; slv++)
  {
    pSlv = &inSlv2Ctrl[slv];
    for(int32 i=0; i<AHB_MEM_ID_WIDTH; i++)
    {
      pBank = (ahb_membar_type*)&pSlv->hconfig.arr[AHB_REG_ID_WIDTH+i];
      // Warning!! This valid only for area_id = AHB memory area. 
      if( (rbMstSel[0].Q!=-1) && pBank->addrmask &&
         ((pBank->memaddr&pBank->addrmask)==(BITS32(inMst2Ctrl[rbMstSel[0].Q].haddr,31,20) & pBank->addrmask)) )
      {
        wbSlvSel = slv;
      }
    }
  }

  // Check what is slave address selected:
  // Control (plug'n'play) memory area
  if( (inMst2Ctrl[rbMstSel[0].Q].haddr>=ADDR_CONFIG_MIN)
    &&(inMst2Ctrl[rbMstSel[0].Q].haddr<=ADDR_CONFIG_MAX))  wCfgSel = 1;
  else                                                     wCfgSel = 0;


  uint32 wHReady;
  uint32 wAdrError = ((rbMstSel[0].Q!=-1)&(wbSlvSel==-1)&!wCfgSel);
  uint32 wAdrReady = wCfgSel ? 1
                   : wAdrError ? 0
                   : ((rbMstSel[0].Q!=-1)&inSlv2Ctrl[wbSlvSel].hready);
  uint32 wDataError = ((rbMstSel[1].Q!=-1)&(rbSlvSel.Q==-1)&!rCfgSel.Q);
  uint32 wDataReady = rCfgSel.Q ? 1
                    : wDataError ? 0
                    : ((rbMstSel[1].Q!=-1)&inSlv2Ctrl[rbSlvSel.Q].hready);

  wHReady = ((rbMstSel[0].Q==-1)&&(rbMstSel[1].Q==-1)||wAdrError||wDataError) ? 1
                                                                              : (wAdrReady|wDataReady);

  rbMstSel[0].CLK = inClk;
  if(!inNRst)      rbMstSel[0].D = rbMstSel[0].Q = -1;  // !!! async reset
  else if(wHReady) rbMstSel[0].D = wbMstSel;
  rbMstSel[1].CLK = inClk;
  if(!inNRst)      rbMstSel[1].D = rbMstSel[1].Q = -1;  // !!! async reset
  else if(wHReady) rbMstSel[1].D = rbMstSel[0].Q;
  
  rbSlvSel.CLK = inClk;
  if(!inNRst)      rbSlvSel.D = rbSlvSel.Q = -1;  // !!! async reset
  else if(wHReady) rbSlvSel.D = wbSlvSel;
  
  rCfgSel.CLK = inClk;
  if(!inNRst)       rCfgSel.D = 0;
  else if(wHReady)  rCfgSel.D = wCfgSel;






#ifdef AHBCTRL_FULLPNP_ENABLE
  uint32 wFullPnp = 1;
#else
  uint32 wFullPnp = (BITS32(inMst2Ctrl[rbMstSel[0].Q].haddr,4,2)==0)? 1: 0;
#endif
  uint32 wLibArea = (rbMstSel[0].Q==-1) ? 0
                  : ((inMst2Ctrl[rbMstSel[0].Q].haddr>=ADDR_BUILD_LIB_MIN)&&(inMst2Ctrl[rbMstSel[0].Q].haddr<=ADDR_BUILD_LIB_MAX));
  uint32 wMstArea = ((rbMstSel[0].Q==-1) | !wFullPnp) ? 0
                  : ((inMst2Ctrl[rbMstSel[0].Q].haddr>=ADDR_MSTCFG_MIN)&&(inMst2Ctrl[rbMstSel[0].Q].haddr<=ADDR_MSTCFG_MAX));
  uint32 wSlvArea = ((rbMstSel[0].Q==-1) | !wFullPnp) ? 0
                  : ((inMst2Ctrl[rbMstSel[0].Q].haddr>=ADDR_SLVCFG_MIN)&&(inMst2Ctrl[rbMstSel[0].Q].haddr<=ADDR_SLVCFG_MAX));

  uint32 wbBusInd = (rbMstSel[0].Q==-1) ? 0: BITS32(inMst2Ctrl[rbMstSel[0].Q].haddr,8,5);
  uint32 wbCfgInd = (rbMstSel[0].Q==-1) ? 0: BITS32(inMst2Ctrl[rbMstSel[0].Q].haddr,4,2);

  // Config data latched:
  rbCfgData.CLK = inClk;
  if(!inNRst)                       rbCfgData.D = 0;
  else if(wHReady&wCfgSel&wLibArea) rbCfgData.D = (XILINX_ML401<<16)|LIBVHDL_BUILD;
  else if(wHReady&wCfgSel&wMstArea) rbCfgData.D = inMst2Ctrl[wbBusInd].hconfig.arr[wbCfgInd];
  else if(wHReady&wCfgSel&wSlvArea) rbCfgData.D = inSlv2Ctrl[wbBusInd].hconfig.arr[wbCfgInd];
  else if(wHReady)                  rbCfgData.D = 0;


  outCtrl2Mst.hready = wHReady;
  outCtrl2Mst.hgrant = (wbMstSel==-1) ? 0 : (0x1<<wbMstSel);
  outCtrl2Mst.hrdata = wDataReady&rCfgSel.Q ? rbCfgData.Q
                     : wDataReady ? inSlv2Ctrl[rbSlvSel.Q].hrdata
                     : 0;
  outCtrl2Mst.hcache = wDataReady&rCfgSel.Q ? 1
                     : wDataReady ? inSlv2Ctrl[rbSlvSel.Q].hcache
                     : 0;
  outCtrl2Mst.hresp  = wDataReady&rCfgSel.Q ? HRESP_OKAY
                     : wDataReady ? inSlv2Ctrl[rbSlvSel.Q].hresp
                     : (wDataError|wAdrError) ? HRESP_ERROR
                     : 0;
  outCtrl2Mst.hirq = 0;



  outCtrl2Slv.hsel = (wbSlvSel==-1) ? 0: (0x1<<wbSlvSel);
  outCtrl2Slv.hready = wHReady;
  outCtrl2Slv.haddr = (wbSlvSel==-1) ? 0 : inMst2Ctrl[rbMstSel[0].Q].haddr;
  outCtrl2Slv.hwdata = (wbSlvSel==-1) ? 0 : inMst2Ctrl[rbMstSel[0].Q].hwdata;
  outCtrl2Slv.hwrite = (wbSlvSel==-1) ? 0 : inMst2Ctrl[rbMstSel[0].Q].hwrite;
  outCtrl2Slv.htrans = (wbSlvSel==-1) ? 0 : inMst2Ctrl[rbMstSel[0].Q].htrans;
  outCtrl2Slv.hsize = (wbSlvSel==-1) ? 0 : inMst2Ctrl[rbMstSel[0].Q].hsize;

}



