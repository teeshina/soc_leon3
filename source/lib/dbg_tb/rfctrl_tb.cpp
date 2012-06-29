//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GPL
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "ldbg.h"
#include "leon3_ml605\leon3mp.h"
extern leon3mp topLeon3mp;

//#define DBG_RfControl

#ifdef DBG_RfControl
  apb_slv_in_type  in_apbi;//   : in  apb_slv_in_type;
  apb_slv_out_type ch_apbo;//   : out apb_slv_out_type;
  uint32 in_LD[SystemOnChipIO::TOTAL_MAXIM2769];
  uint32 ch_SCLK;
  uint32 ch_SDATA;
  uint32 ch_CSn[SystemOnChipIO::TOTAL_MAXIM2769];
  uint32 in_ExtAntStat;
  uint32 in_ExtAntDetect;
  uint32 ch_ExtAntEna;
  uint32 ch_IntAntContr;
  RfControl tst_RfControl;
#endif


#ifdef DBG_RfControl
struct STst{ uint32 iClkNum, adr; uint32 val : 28; };
STst stst[] = 
{
  {40,  0x80000400, 0x11111111},
  {42,  0x80000404, 0x0fffffff},
  {44,  0x80000408, 0x08fffffe},
  {46,  0x8000040C, 0x08fffffe},
  {48,  0x80000410, 0x08fffffe},
  {50,  0x80000414, 0x08fffff1},
  {52,  0x80000418, 0x08fffff1},
  {54,  0x8000041c, 0x08fffff1},
  {56,  0x80000420, 0x08fffff1},
  {58,  0x80000424, 0x08fffff1},
  {70,  0x80000428, 0x00000005},
  {100, 0x8000042c, 0x00000000},// run loading Max GPS L1
  {3040, 0x80000400, 0x11111111},
  {3042, 0x80000404, 0x0fffffff},
  {3044, 0x80000408, 0x08fffffe},
  {3046, 0x8000040C, 0x08fffffe},
  {3048, 0x80000410, 0x08fffffe},
  {3050, 0x80000414, 0x08fffff1},
  {3052, 0x80000418, 0x08fffff1},
  {3054, 0x8000041c, 0x08fffff1},
  {3056, 0x80000420, 0x08fffff1},
  {3058, 0x80000424, 0x08fffff1},
  {3070, 0x80000428, 0x00000005},
  {3100, 0x8000042c, 0x00000001},// run loading MAX GLO L1
  {6500, 0x8000043c, 0x00000001},
  {6600, 0x8000043c, 0x00000000},
  {6700, 0x8000043c, 0x00000001},
};
#endif

//****************************************************************************
void dbg::rfctrl_tb(SystemOnChipIO &io)
{
  apb_slv_in_type   *pin_apbi  = &topLeon3mp.apbi;
  apb_slv_out_type  *pch_apbo  = &topLeon3mp.apbo.arr[APB_RF_CONTROL];//    : in  apb_slv_out_vector
  uint32 *pin_LD      = io.spimax2769.LD;
  uint32 *pch_SCLK    = &io.spimax2769.SCLK;
  uint32 *pch_SDATA   = &io.spimax2769.nSDATA;
  uint32 *pch_CSn     = io.spimax2769.nCS;
  uint32 *pin_ExtAntStat   = &io.antctrl.ExtAntStat;
  uint32 *pin_ExtAntDetect = &io.antctrl.ExtAntDetect;
  uint32 *pch_ExtAntEna    = &io.antctrl.ExtAntEna;
  uint32 *pch_IntAntContr  = &io.antctrl.IntAntContr;

  RfControl        *p_RfControl = topLeon3mp.pclRfControl;


#ifdef DBG_RfControl
  if(io.inClk.eClock_z==SClock::CLK_POSEDGE)
  {
    //
    in_apbi.psel = 0x1<<APB_RF_CONTROL;//[0:15]  : (0 to APB_SLAVES_MAX-1); -- slave select
    in_apbi.penable = 0;// ;                       -- strobe
    in_apbi.pwrite = 0;
    for (int32 i=0; i<sizeof(stst)/sizeof(STst); i++)
    {
      if(iClkCnt==stst[i].iClkNum)
      {
        in_apbi.pwrite = 1;
        in_apbi.penable = 1;
        in_apbi.paddr = stst[i].adr;
        in_apbi.pwdata = stst[i].val;
      }
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
  }

  tst_RfControl.Update(topLeon3mp.wNRst,
                       io.inClk,
                       in_apbi,
                       ch_apbo,
                       in_LD,
                       ch_SCLK,
                       ch_SDATA,
                       ch_CSn,
                       in_ExtAntStat,
                       in_ExtAntDetect,
                       ch_ExtAntEna,
                       ch_IntAntContr );
  
  pin_apbi = &in_apbi;//    : out apb_slv_in_type;
  pch_apbo = &ch_apbo;//    : in  apb_slv_out_vector
  pin_LD   = in_LD;
  pch_SCLK = &ch_SCLK;
  pch_SDATA = &ch_SDATA;
  pch_CSn  = ch_CSn;
  pin_ExtAntStat = &in_ExtAntStat;
  pin_ExtAntDetect = &in_ExtAntDetect;
  pch_ExtAntEna = &ch_ExtAntEna;
  pch_IntAntContr = &ch_IntAntContr;
  
  p_RfControl = &tst_RfControl;
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
    pStr = PutToStr(pStr, ((pin_LD[1]<<1)|pin_LD[0]), 2, "in_LD");
    pStr = PutToStr(pStr, *pin_ExtAntStat, 1,"in_ExtAntStat");
    pStr = PutToStr(pStr, *pin_ExtAntDetect, 1,"in_ExtAntDetect");


    // Output:
    pStr = PutToStr(pStr, pch_apbo->prdata,32,"ch_apbo.prdata");//                           -- transfer done
    pStr = PutToStr(pStr, pch_apbo->pirq,32,"ch_apbo.pirq");
    pStr = PutToStr(pStr, pch_apbo->pconfig.arr[0],32,"ch_apbo.pconfig(0)");
    pStr = PutToStr(pStr, pch_apbo->pconfig.arr[1],32,"ch_apbo.pconfig(1)");
    pStr = PutToStr(pStr, pch_apbo->pindex,4,"conv_integer:ch_apbo.pindex");
    //
    pStr = PutToStr(pStr, *pch_SCLK, 1, "ch_SCLK");
    pStr = PutToStr(pStr, *pch_SDATA, 1, "ch_SDATA");
    pStr = PutToStr(pStr, ((pch_CSn[1]<<1)|pch_CSn[0]), 2, "ch_CSn");
    pStr = PutToStr(pStr, *pch_ExtAntEna, 1, "ch_ExtAntEna");

    // Internal:
    pStr = PutToStr(pStr, p_RfControl->r.Q.BitCnt,6,"t_BitCnt");
    pStr = PutToStr(pStr, p_RfControl->r.Q.SendWord,32,"t_SendWord");
    pStr = PutToStr(pStr, p_RfControl->r.Q.SClkNegedge,1,"t_SClkNegedge");
    pStr = PutToStr(pStr, p_RfControl->r.Q.loading,1,"t_loading");

    PrintIndexStr();

    *posBench[TB_rfctrl] << chStr << "\n";
  }

#ifdef DBG_RfControl
  tst_RfControl.ClkUpdate();
#endif
}



