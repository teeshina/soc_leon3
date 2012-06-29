#include "ahbjtag.h"

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
                  jcomout_dmai,   //DMA input:
                  ahbmstout_dmao, // DMA output:
                  in_ahbi,        // AHB response:
                  out_ahbo );     // Request to AHB
  
}

