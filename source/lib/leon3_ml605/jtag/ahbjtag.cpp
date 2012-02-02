#include "lheaders.h"

//****************************************************************************
void AhbMasterJtag::Update( uint32 inNRst,//         : in  std_ulogic;
                            SClock inClk,//         : in  std_ulogic;
                            //JTAG interface:
                            uint32 nTRST,
                            SClock TCK,
                            uint32 TMS,
                            uint32 TDI,
                            uint32 &TDO,
                            ahb_mst_in_type in_ahbi,//        : in  ahb_mst_in_type;
                            ahb_mst_out_type& out_ahbo//        : out ahb_mst_out_type;
                          )
{
  clJTagPad.Update(nTRST,
                   TCK,
                   TMS,
                   TDI,
                   TDO,
                   wPadOutAdrSel,
                   wPadOutDataSel,
                   wPadOutTDI,
                   wPadOutDRCK,
                   wPadOutCapture,
                   wPadOutShift,
                   wPadOutUpdate,
                   wPadOutReset,
                   wComOutTDO);
                
  jtagcom0.Update(inNRst, 
                  inClk, 
                  wPadOutDRCK,
                  wPadOutTDI,
                  wPadOutAdrSel,
                  wPadOutDataSel,
                  wPadOutShift,
                  wPadOutUpdate,
                  wPadOutReset,
                  wComOutTDO,
                  ahbmstout_dmao, 
                  jcomout_dmai);

  ahbmst0->Update( inNRst,
                  inClk,
                  //DMA input:
                  jcomout_dmai.address,
                  jcomout_dmai.wdata,
                  jcomout_dmai.start,
                  jcomout_dmai.burst,
                  jcomout_dmai.write,
                  jcomout_dmai.busy,
                  jcomout_dmai.irq,
                  jcomout_dmai.size,
                  // DMA output:
                  ahbmstout_dmao.start,
                  ahbmstout_dmao.active,
                  ahbmstout_dmao.ready,
                  ahbmstout_dmao.retry,
                  ahbmstout_dmao.mexc,
                  ahbmstout_dmao.haddr,
                  ahbmstout_dmao.rdata,
                  // AHB response:
                  in_ahbi.hgrant,
                  in_ahbi.hready,
                  in_ahbi.hresp,
                  in_ahbi.hrdata,
                  // Request to AHB
                  out_ahbo );
  
}

