#include "lheaders.h"
extern dbg     clDbg;

//****************************************************************************
leon3mp::leon3mp()
{
  for(int32 i=0; i<CFG_NCPU; i++)
    pclLeon3s[i] = new leon3s(AHB_MASTER_LEON3+i);
    
  pclAhbSlaveMem = new AhbSlaveMem( AHB_SLAVE_MEM, VENDOR_GAISLER, GAISLER_AHBRAM, 0x400, 0xFFF);
  pclDsu3x = new dsu3x(AHB_SLAVE_DSU, 0x900, 0xf00);
}

//****************************************************************************
leon3mp::~leon3mp()
{
  free(pclAhbSlaveMem);
  for(int32 i=0; i<CFG_NCPU; i++) free(pclLeon3s[i]);
  free(pclDsu3x);
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
  
               
  for(int32 i=0; i<CFG_NCPU; i++)
  {
    pclLeon3s[i]->Update(inClk, 
                        inNRst,
                        stCtrl2Mst,
                        stMst2Ctrl[AHB_MASTER_LEON3+i],
                        stCtrl2Slv,
                        in_slvo,
                        irqi,
                        irqo,
                        dbgi.arr[i],
                        dbgo.arr[i]);
  }

  SClock clkZero;
  clkZero.eClock = clkZero.eClock_z = SClock::CLK_NEGATIVE;
  pclDsu3x->Update(inNRst, 
                   clkZero,
                   inClk,
                   stCtrl2Mst,  // only to make AHB trace
                   stCtrl2Slv,
                   stSlv2Ctrl[AHB_SLAVE_DSU],
                   dbgo,
                   dbgi,
                   dsui,
                   dsuo,
                   1);  


}

