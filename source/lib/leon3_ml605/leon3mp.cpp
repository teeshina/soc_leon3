//****************************************************************************
// Property:    GNSS Sensor Limited
// License:     GNU2
// Contact:     chief@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "leon3mp.h"


//****************************************************************************
leon3mp::leon3mp()
{
  clkZero.eClock = clkZero.eClock_z = SClock::CLK_NEGATIVE;
  
  for(int32 i=0; i<AHB_MASTERS_MAX; i++) stMst2Ctrl.arr[i] = ahbm_none;
  for(int32 i=0; i<AHB_SLAVES_MAX; i++) stSlv2Ctrl.arr[i] = ahbs_none;

  for(int32 i=0; i<CFG_NCPU; i++)
    pclLeon3s[i] = new leon3s(AHB_MASTER_LEON3+i);
    
  pclDsu3x = new dsu3x(AHB_SLAVE_DSU, 0x900, 0xf00);    
  
  pclAhbRAM = new ahbram(AHB_SLAVE_RAM, 0x400, 0xfff);

  pclAhbRom = new ahbrom(AHB_SLAVE_ROM, 0x000, 0xfff);

#ifdef USE_GNSSLTD_GNSSENGINE
  pclGnssEngine = new GnssEngine(AHB_SLAVE_GNSSENGINE, 0xd00, 0xfff);
#endif

  pApbControl = new apbctrl(AHB_SLAVE_APBBRIDGE, 0x800, 0xfff);
  
  pclApbUartA = new apbuart(APB_UART_CFG, 0x1, 0xfff); // total address 0x800001xx  = 256 bytes
  
  pclIrqControl = new irqmp(APB_IRQ_CONTROL, 0x2, 0xfff);// total address 0x800002xx  = 256 bytes
  
  pclTimer = new gptimer(APB_TIMER, 0x3, 0xfff);// total address 0x800003xx  = 256 bytes

#ifdef USE_GNSSLTD_RFCONTROL
  pclRfControl = new RfControl(APB_RF_CONTROL, 0x4, 0xfff);// total address 0x800004xx  = 256 bytes
#endif
#ifdef USE_GNSSLTD_GYROSCOPE
  pclGyroscope = new gyrospi(APB_GYROSCOPE, 0x6, 0xffe);// total address 0x80000600..0x800007ff = 512 bytes
#endif
#ifdef USE_GNSSLTD_ACCELEROMETER
  pclAccelerometer = new accelspi(APB_ACCELEROMETER, 0x8, 0xffe);// total address 0x80000800..0x800009ff = 512 bytes
#endif
}

//****************************************************************************
leon3mp::~leon3mp()
{
  free(pclAhbRAM);
  for(int32 i=0; i<CFG_NCPU; i++) free(pclLeon3s[i]);
  free(pclDsu3x);
  free(pclAhbRom);
#ifdef USE_GNSSLTD_GNSSENGINE
  free(pclGnssEngine);
#endif
  free(pApbControl);
  free(pclApbUartA);
  free(pclIrqControl);
  free(pclTimer);
#ifdef USE_GNSSLTD_RFCONTROL
  free(pclRfControl);
#endif
#ifdef USE_GNSSLTD_GYROSCOPE
  free(pclGyroscope);
#endif
#ifdef USE_GNSSLTD_ACCELEROMETER
  free(pclAccelerometer);
#endif
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
                      uint32 &outTX,
                      // SPI MAX2769 GPS L1/GLO L1
                      uint32 inLD[2],
                      uint32 &outSCLK,
                      uint32 &outSDATA,
                      uint32 *outCSn,
                      // Antenna control
                      uint32 inExtAntStat,
                      uint32 inExtAntDetect,
                      uint32 &outExtAntEna,
                      uint32 &outIntAntContr,
                      // GNSS RF inputs:
                      SClock inAdcClk,
                      uint32 inIa,
                      uint32 inQa,
                      uint32 inIb,
                      uint32 inQb,
                      // Gyroscope SPI interface
                      uint32 inGyroSDI,
                      uint32 inGyroInt1, // interrupt 1
                      uint32 inGyroInt2, // interrupt 2
                      uint32 &outGyroSDO,
                      uint32 &outGyroCSn,
                      uint32 &outGyroSPC,
                      // Accelerometer SPI interface
                      uint32 inAccelerSDI,
                      uint32 inAccelerInt1, // interrupt 1
                      uint32 inAccelerInt2, // interrupt 2
                      uint32 &outAccelerSDO,
                      uint32 &outAccelerCSn,
                      uint32 &outAccelerSPC
                    )
{

  // reset logic:
  wSysReset    = inRst;
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

  // Internal RAM:
  pclAhbRom->Update(wNRst, inClk, stCtrl2Slv, stSlv2Ctrl.arr[AHB_SLAVE_ROM]);

  // Internal RAM:
  pclAhbRAM->Update( wNRst, inClk, stCtrl2Slv, stSlv2Ctrl.arr[AHB_SLAVE_RAM] );

#ifdef USE_GNSSLTD_GNSSENGINE
  pclGnssEngine->Update(wNRst, inClk, stCtrl2Slv, stSlv2Ctrl.arr[AHB_SLAVE_GNSSENGINE],
                        inAdcClk, inIa, inQa, inIb, inQb);
#endif

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

  // Timer Unit
  gpti.dhalt  = dsuo.tstop;
  gpti.extclk = 0;
  pclTimer->Update(wNRst, inClk, apbi, apbo.arr[APB_TIMER], gpti, gpto);

  // Antenna control and MAX2769 GPS/GLO-L1 SPIs
#ifdef USE_GNSSLTD_RFCONTROL
  pclRfControl->Update(wNRst, inClk, apbi, apbo.arr[APB_RF_CONTROL],
                      inLD, outSCLK, outSDATA, outCSn,
                      inExtAntStat, inExtAntDetect, outExtAntEna, outIntAntContr );
#endif

  // 3-axis Gyroscope with SPI interface
#ifdef USE_GNSSLTD_GYROSCOPE
    pclGyroscope->Update(wNRst, inClk, apbi, apbo.arr[APB_GYROSCOPE],
                         inGyroInt1, inGyroInt2, inGyroSDI,
                         outGyroSPC, outGyroSDO, outGyroCSn);
#endif

  // 3-axis Accelerometer with SPI interface
#ifdef USE_GNSSLTD_ACCELEROMETER
    pclAccelerometer->Update(wNRst, inClk, apbi, apbo.arr[APB_ACCELEROMETER],
                             inAccelerInt1, inAccelerInt2, inAccelerSDI,
                             outAccelerSPC, outAccelerSDO, outAccelerCSn);
#endif

}

