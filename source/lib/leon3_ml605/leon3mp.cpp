#include "lheaders.h"
extern dbg     clDbg;

//****************************************************************************
leon3mp::leon3mp()
{
  pclAhbSlaveMem = new AhbSlaveMem( AHB_SLAVE_MEM,
                                    VENDOR_GAISLER,
                                    GAISLER_AHBRAM,
                                    0x400,
                                    0xFFF);
}

//****************************************************************************
leon3mp::~leon3mp()
{
  free(pclAhbSlaveMem);
}

//****************************************************************************
void leon3mp::Update( uint32 inNRst,
                      SClock inClk,
                      // JTAG interface:
                      uint32 nTRST,
                      SClock TCK,
                      uint32 TMS,
                      uint32 TDI,
                      uint32 &TDO )
{

  clAhbMasterJtag.Update( inNRst,
                          inClk,
                          // JTAG interface:
                          nTRST,
                          TCK,
                          TMS,
                          TDI,
                          TDO,
                          stCtrl2Mst,
                          stMst2Ctrl[AHB_MASTER_JTAG]);
                          
  pclAhbSlaveMem->Update( inNRst,
                          inClk,
                          stCtrl2Slv,
                          stSlv2Ctrl[AHB_SLAVE_MEM] );


  clAhbControl.Update(inNRst,
                      inClk,
                      stMst2Ctrl,
                      stCtrl2Mst,
                      stSlv2Ctrl,
                      stCtrl2Slv );
                      
  memcpy(in_slvo.arr, stSlv2Ctrl, AHB_SLAVE_TOTAL*sizeof(ahb_slv_out_type));
  
               
  clLeon3s.Update(inClk, 
                  inNRst,
                  stCtrl2Mst,
                  stMst2Ctrl[AHB_MASTER_LEON3],
                  stCtrl2Slv,
                  in_slvo,
                  irqi,
                  irqo,
                  dbgi,
                  dbgo);

}

