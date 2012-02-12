//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "lheaders.h"

extern leon3mp  topLeon3mp;

extern void ResetPutStr();
extern void PrintIndexStr();


//****************************************************************************
void dbg::apbuart_tb(SystemOnChipIO &io)
{
  apb_slv_in_type   *pin_apbi = &topLeon3mp.apbi;
  apb_slv_out_type  *pch_apbo = &topLeon3mp.apbo.arr[APB_UART_CFG];//    : in  apb_slv_out_vector
  uart_in_type      *pin_uarti = &topLeon3mp.uarti;//  : in  uart_in_type;
  uart_out_type     *pch_uarto = &topLeon3mp.uarto;//  : out uart_out_type);
  apbuart           *p_apbuart = topLeon3mp.pclApbUartA;


#ifdef DBG_apbuart
  if(io.inClk.eClock_z==SClock::CLK_POSEDGE)
  {
    //
    in_apbi.psel = (rand()&0x1)<<APB_UART_CFG;//[0:15]  : (0 to APB_SLAVES_MAX-1); -- slave select
    in_apbi.penable = rand()&0x1;// ;                       -- strobe
    in_apbi.paddr = (rand()<<17)|rand();// : (31 downto 0);  -- address bus (byte)
    in_apbi.pwrite = rand()&0x1;//  ;                       -- write
    in_apbi.pwdata = (rand()<<17)|rand();//  : (31 downto 0);  -- write data bus
    in_apbi.pirq = (rand()<<17)|rand();//[31:0]  : (AHB_IRQ_MAX-1 downto 0); -- interrupt result bus
    in_apbi.testen = 0;//                          -- scan test enable
    in_apbi.testrst = 0;//                         -- scan test reset
    in_apbi.scanen = 0;//                          -- scan enable
    in_apbi.testoen = 0;//                         -- test output enable
    //
    in_uarti.rxd = rand()&0x1;//   	: std_ulogic;
    in_uarti.ctsn = rand()&0x1;//   	: std_ulogic;
    in_uarti.extclk = 0;//	: std_ulogic;

  }
  
  ptst_apbuart->Update(io.inNRst,
                       io.inClk,
                       in_apbi,
                       ch_apbo,
                       in_uarti,
                       ch_uarto);
  
  pin_apbi = &in_apbi;//    : out apb_slv_in_type;
  pch_apbo = &ch_apbo;//    : in  apb_slv_out_vector
  pin_uarti = &in_uarti;
  pch_uarto = &ch_uarto;
  p_apbuart = ptst_apbuart;
#endif

  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';

    ResetPutStr();
  
    // inputs:
    pStr = PutToStr(pStr, io.inNRst, 1, "inNRst");
    //
    pStr = PutToStr(pStr, pin_apbi->psel,APB_SLAVES_MAX,"in_apbi.psel",true);//[0:15]  : (0 to APB_SLAVES_MAX-1); -- slave select
    pStr = PutToStr(pStr, pin_apbi->penable,1,"in_apbi.penable");// ;                       -- strobe
    pStr = PutToStr(pStr, pin_apbi->paddr,32,"in_apbi.paddr");//[31:0] : (31 downto 0);  -- address bus (byte)
    pStr = PutToStr(pStr, pin_apbi->pwrite,1,"in_apbi.pwrite");//  ;                       -- write
    pStr = PutToStr(pStr, pin_apbi->pwdata,32,"in_apbi.pwdata");//[31:0]  : (31 downto 0);  -- write data bus
    pStr = PutToStr(pStr, pin_apbi->pirq,AHB_IRQ_MAX,"in_apbi.pirq");//[31:0]  : (AHB_IRQ_MAX-1 downto 0); -- interrupt result bus
    pStr = PutToStr(pStr, pin_apbi->testen,1,"in_apbi.testen");//                          -- scan test enable
    pStr = PutToStr(pStr, pin_apbi->testrst,1,"in_apbi.testrst");//                         -- scan test reset
    pStr = PutToStr(pStr, pin_apbi->scanen,1,"in_apbi.scanen");//                          -- scan enable
    pStr = PutToStr(pStr, pin_apbi->testoen,1,"in_apbi.testoen");//                         -- test output enable
    //
    pStr = PutToStr(pStr, pin_uarti->rxd,1,"in_uarti.rxd");//   	: std_ulogic;
    pStr = PutToStr(pStr, pin_uarti->ctsn,1,"in_uarti.ctsn");
    pStr = PutToStr(pStr, pin_uarti->extclk,1,"in_uarti.extclk");//	: std_ulogic;


    // Output:
    pStr = PutToStr(pStr, pch_apbo->prdata,32,"ch_apbo.prdata");//                           -- transfer done
    pStr = PutToStr(pStr, pch_apbo->pirq,32,"ch_apbo.pirq");
    pStr = PutToStr(pStr, pch_apbo->pconfig.arr[0],32,"ch_apbo.pconfig(0)");
    pStr = PutToStr(pStr, pch_apbo->pconfig.arr[1],32,"ch_apbo.pconfig(1)");
    pStr = PutToStr(pStr, pch_apbo->pindex,4,"conv_integer:ch_apbo.pindex");
    //
    pStr = PutToStr(pStr, pch_uarto->rtsn,1,"ch_uarto.rtsn");//   	: std_ulogic;
    pStr = PutToStr(pStr, pch_uarto->txd,1,"ch_uarto.txd");//   	: std_ulogic;
    pStr = PutToStr(pStr, pch_uarto->scaler,32,"ch_uarto.scaler");//	: std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pch_uarto->txen,1,"ch_uarto.txen");//     	: std_ulogic;
    pStr = PutToStr(pStr, pch_uarto->flow,1,"ch_uarto.flow");//   	: std_ulogic;
    pStr = PutToStr(pStr, pch_uarto->rxen,1,"ch_uarto.rxen");//     	: std_ulogic;

    // Internal:
    pStr = PutToStr(pStr, (p_apbuart->paddr>>2), 6, "t_paddr");
    pStr = PutToStr(pStr, (p_apbuart->rdata), 32, "t_rdata");

    PrintIndexStr();

    *posBench[TB_apbuart] << chStr << "\n";
  }

#ifdef DBG_apbuart
  ptst_apbuart->ClkUpdate();
#endif
}

