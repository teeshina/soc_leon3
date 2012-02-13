//****************************************************************************
// Property:    GNSS Sensor Limited
// License:     GNU2
// Contact:     chief@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "lheaders.h"

//****************************************************************************
leon3mp::leon3mp()
{
  clkZero.eClock = clkZero.eClock_z = SClock::CLK_NEGATIVE;

  for(int32 i=0; i<CFG_NCPU; i++)
    pclLeon3s[i] = new leon3s(AHB_MASTER_LEON3+i);
    
  pclDsu3x = new dsu3x(AHB_SLAVE_DSU, 0x900, 0xf00);    
  
#ifdef USE_PURE_GAISLER
  pclAhbRAM = new ahbram(AHB_SLAVE_RAM, 0x400, 0xfff);
#else
  pclAhbRAM = new AhbSlaveMem( AHB_SLAVE_RAM, VENDOR_GAISLER, GAISLER_AHBRAM, 0x400, 0xFFF);
#endif

  pApbControl = new apbctrl(AHB_SLAVE_APBBRIDGE, 0x800, 0xfff);
  pclApbUartA = new apbuart(APB_UART_CFG, 0x1, 0xfff); // total address 0x800001xx  = 256 bytes
}

//****************************************************************************
leon3mp::~leon3mp()
{
  free(pclAhbRAM);
  for(int32 i=0; i<CFG_NCPU; i++) free(pclLeon3s[i]);
  free(pclDsu3x);
  free(pApbControl);
  free(pclApbUartA);
}

//****************************************************************************
void leon3mp::Update( uint32 inNRst,
                      SClock inClk,
                      // JTAG interface:
                      uint32 nTRST,
                      SClock TCK,
                      uint32 TMS,
                      uint32 TDI,
                      uint32 &TDO,
                      // UART 1 (cfg) interface:
                      uint32 inCTS, // if CTS=0. then may send data via TX, otherwise wait sending
                      uint32 inRX,
                      uint32 &outRTS,// If this uart can't recieve data RX, then rise this to "1"
                      uint32 &outTX
                    )
{
  // AHB controller:
  clAhbControl.Update(inNRst,
                      inClk,
                      stMst2Ctrl,
                      stCtrl2Mst,
                      stSlv2Ctrl,
                      stCtrl2Slv );

  // JTAG unit:
  clAhbMasterJtag.Update( inNRst,
                          inClk,
                          // JTAG interface:
                          nTRST, TCK, TMS, TDI, TDO,
                          // AMBA intefrace:
                          stCtrl2Mst,
                          stMst2Ctrl.arr[AHB_MASTER_JTAG]);

  // Internal RAM:
  pclAhbRAM->Update( inNRst, inClk, stCtrl2Slv, stSlv2Ctrl.arr[AHB_SLAVE_RAM] );

  
  // Core and Dunbug support units
  for(int32 i=0; i<CFG_NCPU; i++)
  {
    pclLeon3s[i]->Update(inClk, 
                        inNRst,
                        stCtrl2Mst,
                        stMst2Ctrl.arr[AHB_MASTER_LEON3+i],
                        stCtrl2Slv,
                        stSlv2Ctrl,
                        irqi,
                        irqo,
                        dbgi.arr[i],
                        dbgo.arr[i]);
  }

  pclDsu3x->Update(inNRst, 
                   clkZero,
                   inClk,
                   stCtrl2Mst,  // only to make AHB trace
                   stCtrl2Slv,
                   stSlv2Ctrl.arr[AHB_SLAVE_DSU],
                   dbgo,
                   dbgi,
                   dsui,
                   dsuo,
                   1);  

  // AHB/APB bridge
  pApbControl->Update(inNRst, inClk, stCtrl2Slv, stSlv2Ctrl.arr[AHB_SLAVE_APBBRIDGE], apbi, apbo);
  
  // APB UART 1 (config)
  uarti.rxd    = inRX;
  uarti.ctsn   = inCTS;
  uarti.extclk = 0;
  pclApbUartA->Update(inNRst, inClk, apbi, apbo.arr[APB_UART_CFG], uarti, uarto);
  outTX  = uarto.txd;
  outRTS = uarto.rtsn;
  if(!outTX) 
  bool st = true;
}

