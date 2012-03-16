//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GPL
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "lheaders.h"

//****************************************************************************
void dbg::irqmp_tb(SystemOnChipIO &io)
{
  char chTmp[128];

  apb_slv_in_type   *pin_apbi = &topLeon3mp.apbi;
  apb_slv_out_type  *pch_apbo = &topLeon3mp.apbo.arr[APB_IRQ_CONTROL];//    : in  apb_slv_out_vector
  irq_out_vector    *pin_irqi = &topLeon3mp.irqo;//   : in  irq_out_vector(0 to ncpu-1);
  irq_in_vector     *pch_irqo = &topLeon3mp.irqi;
  irqmp             *p_irqmp   = topLeon3mp.pclIrqControl;


#ifdef DBG_irqmp
  if(io.inClk.eClock_z==SClock::CLK_POSEDGE)
  {
    //
    in_apbi.psel = 0x1<<APB_IRQ_CONTROL;//[0:15]  : (0 to APB_SLAVES_MAX-1); -- slave select
    in_apbi.penable = rand()&0x1;// ;                       -- strobe
    in_apbi.paddr = 0x80000200 | (rand()&0xff);// : (31 downto 0);  -- address bus (byte)
    in_apbi.pwrite = rand()&0x1;//  ;                       -- write
    in_apbi.pwdata = (rand()<<17) | rand();//  : (31 downto 0);  -- write data bus
    in_apbi.pirq = (rand()<<17)|rand();//[31:0]  : (AHB_IRQ_MAX-1 downto 0); -- interrupt result bus
    in_apbi.testen = 0;//                          -- scan test enable
    in_apbi.testrst = 0;//                         -- scan test reset
    in_apbi.scanen = 0;//                          -- scan enable
    in_apbi.testoen = 0;//                         -- test output enable
    //
    for(int32 i=0; i<CFG_NCPU; i++)
    {
      in_irqi.arr[i].intack = rand()&0x1;//  : std_ulogic;
      in_irqi.arr[i].irl    = rand()&0xf;//   : std_logic_vector(3 downto 0);
      in_irqi.arr[i].pwd    = rand()&0x1;//         : std_ulogic;
      in_irqi.arr[i].fpen   = rand()&0x1;//        : std_ulogic;
      in_irqi.arr[i].idle   = rand()&0x1;//        : std_ulogic;
    }
  }

  ptst_irqmp->Update(topLeon3mp.wNRst,
                       io.inClk,
                       in_apbi,
                       ch_apbo,
                       in_irqi,
                       ch_irqo);
  
  pin_apbi = &in_apbi;//    : out apb_slv_in_type;
  pch_apbo = &ch_apbo;//    : in  apb_slv_out_vector
  pin_irqi = &in_irqi;
  pch_irqo = &ch_irqo;
  p_irqmp = ptst_irqmp;
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
    for(int32 i=0; i<CFG_NCPU; i++)
    {
      sprintf_s(chTmp,"in_irqi(%i).intack",i);
      pStr = PutToStr(pStr, pin_irqi->arr[i].intack,1,chTmp);//  : std_ulogic;
      sprintf_s(chTmp,"in_irqi(%i).irl",i);
      pStr = PutToStr(pStr, pin_irqi->arr[i].irl,4,chTmp);//   : std_logic_vector(3 downto 0);
      sprintf_s(chTmp,"in_irqi(%i).pwd",i);
      pStr = PutToStr(pStr, pin_irqi->arr[i].pwd,1,chTmp);//         : std_ulogic;
      sprintf_s(chTmp,"in_irqi(%i).fpen",i);
      pStr = PutToStr(pStr, pin_irqi->arr[i].fpen,1,chTmp);//        : std_ulogic;
      sprintf_s(chTmp,"in_irqi(%i).idle",i);
      pStr = PutToStr(pStr, pin_irqi->arr[i].idle,1,chTmp);//        : std_ulogic;
    }


    // Output:
    pStr = PutToStr(pStr, pch_apbo->prdata,32,"ch_apbo.prdata");//                           -- transfer done
    pStr = PutToStr(pStr, pch_apbo->pirq,32,"ch_apbo.pirq");
    pStr = PutToStr(pStr, pch_apbo->pconfig.arr[0],32,"ch_apbo.pconfig(0)");
    pStr = PutToStr(pStr, pch_apbo->pconfig.arr[1],32,"ch_apbo.pconfig(1)");
    pStr = PutToStr(pStr, pch_apbo->pindex,4,"conv_integer:ch_apbo.pindex");
    //
    for(int32 i=0; i<CFG_NCPU; i++)
    {
      sprintf_s(chTmp,"ch_irqo(%i).irl",i);
      pStr = PutToStr(pStr, pch_irqo->arr[i].irl,4,chTmp);//     : std_logic_vector(3 downto 0);
      sprintf_s(chTmp,"ch_irqo(%i).rst",i);
      pStr = PutToStr(pStr, pch_irqo->arr[i].rst,1,chTmp);//     : std_ulogic;
      sprintf_s(chTmp,"ch_irqo(%i).run",i);
      pStr = PutToStr(pStr, pch_irqo->arr[i].run,1,chTmp);//     : std_ulogic;
      sprintf_s(chTmp,"ch_irqo(%i).rstvec",i);
      pStr = PutToStr(pStr, pch_irqo->arr[i].rstvec>>12,20,chTmp);//  : std_logic_vector(31 downto 12);
      sprintf_s(chTmp,"ch_irqo(%i).iact",i);
      pStr = PutToStr(pStr, pch_irqo->arr[i].iact,1,chTmp);//        : std_ulogic;
      sprintf_s(chTmp,"ch_irqo(%i).index",i);
      pStr = PutToStr(pStr, pch_irqo->arr[i].index,4,chTmp);//   : std_logic_vector(3 downto 0);
      sprintf_s(chTmp,"ch_irqo(%i).hrdrst",i);
      pStr = PutToStr(pStr, pch_irqo->arr[i].hrdrst,1,chTmp);//      : std_ulogic;
    }

    // Internal:
    pStr = PutToStr(pStr, p_irqmp->v.ipend>>1,15,"t_v_ipend");

    PrintIndexStr();

    *posBench[TB_irqmp] << chStr << "\n";
  }

#ifdef DBG_irqmp
  ptst_irqmp->ClkUpdate();
#endif
}



