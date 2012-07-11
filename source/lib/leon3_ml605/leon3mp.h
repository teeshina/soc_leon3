#pragma once

#include "stdtypes.h"
#include "dff.h"
#include "leon3_ml605\config.h"
#include "leon3_ml605\amba\amba.h"

#include "leon3_ml605\leon3\leon3.h"
#include "leon3_ml605\uart\uart.h"
#include "leon3_ml605\amba\gptimer.h"

#include "leon3_ml605\amba\ahbctrl.h"
#include "leon3_ml605\amba\ahbctrl_v.h"
#include "leon3_ml605\amba\apbctrl.h"
#include "leon3_ml605\amba\gptimer.h"
#include "leon3_ml605\jtag\ahbjtag.h"
#include "leon3_ml605\mem\ahbram.h"
#include "leon3_ml605\mem\ahbmem_v.h"
#include "leon3_ml605\mem\ahbrom.h"
#include "leon3_ml605\uart\apbuart.h"
#include "leon3_ml605\leon3\leon3s.h"
#include "leon3_ml605\leon3\dsu3x.h"
#include "leon3_ml605\leon3\irqmp.h"

#include "leon3_ml605\gnss\rfctrl.h"
#include "leon3_ml605\gnss\gyrospi.h"
#include "leon3_ml605\gnss\accelspi.h"
#include "leon3_ml605\gnss\gnssengine.h"

#define USE_GNSSLTD_MODULES
#ifdef USE_GNSSLTD_MODULES
  //#define USE_GNSSLTD_AHBCTRL
  #define USE_GNSSLTD_RFCONTROL
  #define USE_GNSSLTD_GYROSCOPE
  #define USE_GNSSLTD_ACCELEROMETER
  #define USE_GNSSLTD_GNSSENGINE
#endif

class leon3mp
{
  friend class dbg;
  private:
    SClock clkZero;
  
    ahb_mst_in_type     stCtrl2Mst;
    ahb_mst_out_vector  stMst2Ctrl;
    ahb_slv_in_type     stCtrl2Slv;
    ahb_slv_out_vector  stSlv2Ctrl;

    AhbMasterJtag clAhbMasterJtag;
    ahbram *pclAhbRAM;    

#ifdef USE_GNSSLTD_AHBCTRL
    AhbControl clAhbControl;
#else
    ahbctrl    clAhbControl;
#endif
        
    //
    irq_in_vector irqi;//   : in  l3_irq_in_type;
    irq_out_vector irqo;//   : out l3_irq_out_type;
    l3_debug_in_vector dbgi;//   : in  l3_debug_in_type;
    l3_debug_out_vector dbgo;
    leon3s* pclLeon3s[CFG_NCPU];
    
    dsu_in_type dsui;//   : in dsu_in_type;
    dsu_out_type dsuo;//   : out dsu_out_type;
    dsu3x *pclDsu3x;
  
    apb_slv_in_type    apbi;//    : out apb_slv_in_type;
    apb_slv_out_vector apbo;
    apbctrl *pApbControl;

    uart_in_type uarti;//  : in  uart_in_type;
    uart_out_type uarto;//  : out uart_out_type);
    apbuart *pclApbUartA;
    
    irqmp *pclIrqControl;

    gptimer_in_type  gpti;
    gptimer_out_type gpto;
    gptimer *pclTimer;

    RfControl *pclRfControl;
    gyrospi *pclGyroscope;
    accelspi *pclAccelerometer;
    GnssEngine *pclGnssEngine;

    ahbrom *pclAhbRom;

    uint32 wSysReset;
    uint32 wNRst;
    TDFF<uint32> rbPllLock;
    TDFF<uint32> rReset;

  public:
    leon3mp();
    ~leon3mp();

    void Update
    (
      uint32 inRst,
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
     );
    
    void ClkUpdate()
    {
      clAhbMasterJtag.ClkUpdate();
      clAhbControl.ClkUpdate();
      pclAhbRAM->ClkUpdate();
      
      for(int32 i=0; i<CFG_NCPU; i++)
        pclLeon3s[i]->ClkUpdate();
      pclDsu3x->ClkUpdate();
      pApbControl->ClkUpdate();
      pclApbUartA->ClkUpdate();
      pclAhbRom->ClkUpdate();
      pclIrqControl->ClkUpdate();
      pclTimer->ClkUpdate();
#ifdef USE_GNSSLTD_RFCONTROL
      pclRfControl->ClkUpdate();
#endif      
#ifdef USE_GNSSLTD_GYROSCOPE
      pclGyroscope->ClkUpdate();
#endif
#ifdef USE_GNSSLTD_ACCELEROMETER
      pclAccelerometer->ClkUpdate();
#endif
#ifdef USE_GNSSLTD_GNSSENGINE
      pclGnssEngine->ClkUpdate();
#endif
      rbPllLock.ClkUpdate();
      rReset.ClkUpdate();
    }
};

