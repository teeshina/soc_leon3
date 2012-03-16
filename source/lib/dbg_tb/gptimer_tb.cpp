//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GPL
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "lheaders.h"

//****************************************************************************
void dbg::gptimer_tb(SystemOnChipIO &io)
{

  apb_slv_in_type   *pin_apbi  = &topLeon3mp.apbi;
  apb_slv_out_type  *pch_apbo  = &topLeon3mp.apbo.arr[APB_TIMER];//    : in  apb_slv_out_vector
  gptimer_in_type   *pin_gpti  = &topLeon3mp.gpti;//   : in  gptimer_in_type;
  gptimer_out_type  *pch_gpto  = &topLeon3mp.gpto;//   : out gptimer_out_type
  gptimer           *p_gptimer = topLeon3mp.pclTimer;


#ifdef DBG_gptimer
  if(io.inClk.eClock_z==SClock::CLK_POSEDGE)
  {
    //
    in_apbi.psel = 0x1<<APB_TIMER;//[0:15]  : (0 to APB_SLAVES_MAX-1); -- slave select
    in_apbi.penable = 0;// ;                       -- strobe
    in_apbi.pwrite = 0;
    if(iClkCnt==40)
    {
      in_apbi.paddr = 0x80000300; // set scaler
      in_apbi.pwdata = 66;
      in_apbi.pwrite = 1;
      in_apbi.penable = 1;
    }else if(iClkCnt==42)
    {
      in_apbi.paddr = 0x80000304; // set reload value
      in_apbi.pwdata = 66;
      in_apbi.pwrite = 1;
      in_apbi.penable = 1;
    }

    uint32 TimerNum=1;    
    if(iClkCnt==100)
    {
      in_apbi.paddr = 0x80000300 | (TimerNum<<4) | (0<<2); // write value
      in_apbi.pwdata = 10 + TimerNum;
      in_apbi.pwrite = 1;
      in_apbi.penable = 1;
    }else if(iClkCnt==102)
    {
      in_apbi.paddr = 0x80000300 | (TimerNum<<4) | (1<<2); // write reload
      in_apbi.pwdata = 10 + TimerNum;
      in_apbi.pwrite = 1;
      in_apbi.penable = 1;
    }else if(iClkCnt==104)
    {
      in_apbi.paddr = 0x80000300 | (TimerNum<<4) | (2<<2); // write control
      uint32 ena=1,restart=1,load=1,irqen=1,irqpen=0,chain=0;
      in_apbi.pwdata = (chain<<5)|(irqpen<<4)|(irqen<<3)|(load<<2)|(restart<<1)|ena;
      in_apbi.pwrite = 1;
      in_apbi.penable = 1;
    }

    TimerNum=2;        
    if(iClkCnt==100)
    {
      in_apbi.paddr = 0x80000300 | (TimerNum<<4) | (0<<2); // write value
      in_apbi.pwdata = 10 + TimerNum;
      in_apbi.pwrite = 1;
      in_apbi.penable = 1;
    }else if(iClkCnt==152)
    {
      in_apbi.paddr = 0x80000300 | (TimerNum<<4) | (1<<2); // write reload
      in_apbi.pwdata = 10 + TimerNum;
      in_apbi.pwrite = 1;
      in_apbi.penable = 1;
    }else if(iClkCnt==154)
    {
      in_apbi.paddr = 0x80000300 | (TimerNum<<4) | (2<<2); // write control
      uint32 ena=1,restart=1,load=1,irqen=1,irqpen=0,chain=0;
      in_apbi.pwdata = (chain<<5)|(irqpen<<4)|(irqen<<3)|(load<<2)|(restart<<1)|ena;
      in_apbi.pwrite = 1;
      in_apbi.penable = 1;
    }else if(iClkCnt==2090) // CLEAR irq pending bit
    {
      in_apbi.paddr = 0x80000300 | (TimerNum<<4) | (2<<2); // write control
      uint32 ena=1,restart=1,load=1,irqen=1,irqpen=1,chain=0;
      in_apbi.pwdata = (chain<<5)|(irqpen<<4)|(irqen<<3)|(load<<2)|(restart<<1)|ena;
      in_apbi.pwrite = 1;
      in_apbi.penable = 1;
    }
    
    if(iClkCnt>15000)
    {
      in_apbi.paddr = 0x80000300 | (rand()&0xff);// : (31 downto 0);  -- address bus (byte)
      in_apbi.penable = rand()&0x1;
      in_apbi.pwrite = rand()&0x1;//  ;                       -- write
      in_apbi.pwdata = (rand()<<17) | rand();//  : (31 downto 0);  -- write data bus
      in_apbi.pirq = (rand()<<17)|rand();//[31:0]  : (AHB_IRQ_MAX-1 downto 0); -- interrupt result bus
    }
    in_apbi.testen = 0;//                          -- scan test enable
    in_apbi.testrst = 0;//                         -- scan test reset
    in_apbi.scanen = 0;//                          -- scan enable
    in_apbi.testoen = 0;//                         -- test output enable
    //
    in_gpti.dhalt = 0;//rand()&0x1;//    : std_ulogic;
    in_gpti.extclk = rand()&0x1;//   : std_ulogic;
    in_gpti.wdogen = 0;//rand()&0x1;//   : std_ulogic;
  }

  ptst_gptimer->Update(topLeon3mp.wNRst,
                       io.inClk,
                       in_apbi,
                       ch_apbo,
                       in_gpti,
                       ch_gpto);
  
  pin_apbi = &in_apbi;//    : out apb_slv_in_type;
  pch_apbo = &ch_apbo;//    : in  apb_slv_out_vector
  pin_gpti = &in_gpti;
  pch_gpto = &ch_gpto;
  p_gptimer = ptst_gptimer;
#endif

  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';

    ResetPutStr();
  
    // inputs:
    pStr = PutToStr(pStr, topLeon3mp.wNRst, 1, "inNRst");
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
    pStr = PutToStr(pStr, pin_gpti->dhalt,1,"in_gpti.dhalt");//    : std_ulogic;
    pStr = PutToStr(pStr, pin_gpti->extclk,1,"in_gpti.extclk");//   : std_ulogic;
    pStr = PutToStr(pStr, pin_gpti->wdogen,1,"in_gpti.wdogen");//   : std_ulogic;


    // Output:
    pStr = PutToStr(pStr, pch_apbo->prdata,32,"ch_apbo.prdata");//                           -- transfer done
    pStr = PutToStr(pStr, pch_apbo->pirq,32,"ch_apbo.pirq");
    pStr = PutToStr(pStr, pch_apbo->pconfig.arr[0],32,"ch_apbo.pconfig(0)");
    pStr = PutToStr(pStr, pch_apbo->pconfig.arr[1],32,"ch_apbo.pconfig(1)");
    pStr = PutToStr(pStr, pch_apbo->pindex,4,"conv_integer:ch_apbo.pindex");
    //
    pStr = PutToStr(pStr, pch_gpto->tick,8,"ch_gpto.tick",true);//     : std_logic_vector(0 to 7);
    pStr = PutToStr(pStr, pch_gpto->timer1,32,"ch_gpto.timer1");//   : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pch_gpto->wdogn,1,"ch_gpto.wdogn");//    : std_ulogic;
    pStr = PutToStr(pStr, pch_gpto->wdog,1,"ch_gpto.wdog");//    : std_ulogic;

    // Internal:
    pStr = PutToStr(pStr, p_gptimer->scaler,CFG_GPT_SW+1,"t_scaler");
    pStr = PutToStr(pStr, p_gptimer->v.tick,1,"t_v_tick");
    pStr = PutToStr(pStr, p_gptimer->v.tsel,2,"conv_integer:t_v_tsel");

    PrintIndexStr();

    *posBench[TB_gptimer] << chStr << "\n";
  }

#ifdef DBG_gptimer
  ptst_gptimer->ClkUpdate();
#endif
}



