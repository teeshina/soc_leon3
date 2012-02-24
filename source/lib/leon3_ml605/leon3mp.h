#pragma once

//#define USE_GNSSLTD_MODULES
#ifdef USE_GNSSLTD_MODULES
  #define USE_GNSSLTD_AHBRAM
  #define USE_GNSSLTD_AHBCTRL
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
#ifdef USE_GNSSLTD_AHBRAM
    AhbSlaveMem   *pclAhbRAM;
#else
    ahbram *pclAhbRAM;    
#endif

#ifdef USE_GNSSLTD_AHBCTRL
    AhbControl clAhbControl;
#else
    ahbctrl    clAhbControl;
#endif
        
    //
    l3_irq_in_type irqi;//   : in  l3_irq_in_type;
    l3_irq_out_type irqo;//   : out l3_irq_out_type;
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

    ahbrom *pclAhbRom;

  public:
    leon3mp();
    ~leon3mp();

    void Update
    (
      uint32 inNRst,
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
#if (CFG_AHBROM_ENA==1)
      pclAhbRom->ClkUpdate();
#endif
    }
};

