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
  
  for(int32 i=0; i<AHB_MASTERS_MAX; i++) stMst2Ctrl.arr[i] = ahbm_none;
  for(int32 i=0; i<AHB_SLAVES_MAX; i++) stSlv2Ctrl.arr[i] = ahbs_none;

  for(int32 i=0; i<CFG_NCPU; i++)
    pclLeon3s[i] = new leon3s(AHB_MASTER_LEON3+i);
    
  pclDsu3x = new dsu3x(AHB_SLAVE_DSU, 0x900, 0xf00);    
  
#ifdef USE_GNSSLTD_AHBRAM
  pclAhbRAM = new AhbSlaveMem( AHB_SLAVE_RAM, VENDOR_GAISLER, GAISLER_AHBRAM, 0x400, 0xFFF);
#else
  pclAhbRAM = new ahbram(AHB_SLAVE_RAM, 0x400, 0xfff);
#endif

#if (CFG_AHBROM_ENA==1)
  pclAhbRom = new ahbrom(AHB_SLAVE_ROM, 0x000, 0xfff);
#endif

  pApbControl = new apbctrl(AHB_SLAVE_APBBRIDGE, 0x800, 0xfff);
  
  pclApbUartA = new apbuart(APB_UART_CFG, 0x1, 0xfff); // total address 0x800001xx  = 256 bytes
  
  pclIrqControl = new irqmp(APB_IRQ_CONTROL, 0x2, 0xfff);
}

//****************************************************************************
leon3mp::~leon3mp()
{
  free(pclAhbRAM);
  for(int32 i=0; i<CFG_NCPU; i++) free(pclLeon3s[i]);
  free(pclDsu3x);
#if (CFG_AHBROM_ENA==1)
  free(pclAhbRom);
#endif
  free(pApbControl);
  free(pclApbUartA);
  free(pclIrqControl);
}

//****************************************************************************
void leon3mp::Update( uint32 inRst,
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

  // reset logic:
  wSysReset    = inRst;// TODO: change it to positive push button nad add PllLocked
  rbPllLock.CLK = inClk;
  if(wSysReset) rbPllLock.D = 0;
  else          rbPllLock.D = BITS32( ((rbPllLock.Q<<1)|1), 4, 0);
  
  rReset.CLK = inClk;
  rReset.D  =  BIT32(rbPllLock.Q,4) & BIT32(rbPllLock.Q,3) & BIT32(rbPllLock.Q,2);
  wNRst = rReset.Q;

  // AHB controller:
  clAhbControl.Update(wNRst,
                      inClk,
                      stCtrl2Mst,
                      stMst2Ctrl,
                      stCtrl2Slv,
                      stSlv2Ctrl );

  // JTAG unit:
  clAhbMasterJtag.Update( wNRst,
                          inClk,
                          // JTAG interface:
                          nTRST, TCK, TMS, TDI, TDO,
                          // AMBA intefrace:
                          stCtrl2Mst,
                          stMst2Ctrl.arr[AHB_MASTER_JTAG]);


  
  // Core and Dunbug support units
  for(int32 i=0; i<CFG_NCPU; i++)
  {
    irqi.arr[i].irl = 0;
    
    pclLeon3s[i]->Update(inClk, 
                        wNRst,
                        stCtrl2Mst,
                        stMst2Ctrl.arr[AHB_MASTER_LEON3+i],
                        stCtrl2Slv,
                        stSlv2Ctrl,
                        irqi.arr[i],
                        irqo.arr[i],
                        dbgi.arr[i],
                        dbgo.arr[i]);
  }

  dsui.enable = 1;
  dsui.Break  = 0;  // push button emulation
  pclDsu3x->Update(wNRst, 
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

#if (CFG_AHBROM_ENA==1)
  // Internal RAM:
  pclAhbRom->Update(wNRst, inClk, stCtrl2Slv, stSlv2Ctrl.arr[AHB_SLAVE_ROM]);
#endif

  // Internal RAM:
  pclAhbRAM->Update( wNRst, inClk, stCtrl2Slv, stSlv2Ctrl.arr[AHB_SLAVE_RAM] );


  // AHB/APB bridge
  pApbControl->Update(wNRst, inClk, stCtrl2Slv, stSlv2Ctrl.arr[AHB_SLAVE_APBBRIDGE], apbi, apbo);
  
  // APB UART 1 (config)
  uarti.rxd    = inRX;
  uarti.ctsn   = inCTS;
  uarti.extclk = 0;
  pclApbUartA->Update(wNRst, inClk, apbi, apbo.arr[APB_UART_CFG], uarti, uarto);
  outTX  = uarto.txd;
  outRTS = uarto.rtsn;
  
  // IRQ control:
  pclIrqControl->Update(wNRst, inClk, apbi, apbo.arr[APB_IRQ_CONTROL], irqo, irqi);
}

