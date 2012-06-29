//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      GNSS Sensor Limited
// License:     GPL
// Contact:     alex.kosin@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "ldbg.h"
#include "leon3_ml605\leon3mp.h"
extern leon3mp topLeon3mp;

//#define DBG_GnssEngine

#ifdef DBG_GnssEngine
  ahb_slv_in_type in_ahbsi;
  ahb_slv_out_type ch_ahbso;
  uint32 inGpsI;
  uint32 inGpsQ;
  uint32 inGloI;
  uint32 inGloQ;
  GnssEngine tst_GnssEngine;
#endif


#ifdef DBG_GnssEngine
struct STst{ uint32 iClkNum; uint32 wr; uint32 adr; uint32 val; };
STst stst[] = 
{
  {40,  1, 0xD0000000+(32<<6), 0x1000}, // ms length
  {42,  0, 0xD0000000, 0xffffffff},
  {50,  1, 0xD0000000, 0xf00ff000},// set dbgIncr of channel 0
  {51,  1, 0xD0000000+((1*16)<<2), 0xf00ff001},// set dbgIncr of channel 1
  {52,  1, 0xD0000000+((2*16)<<2), 0xf00ff002},// set dbgIncr of channel 2
  {53,  1, 0xD0000000+((3*16)<<2), 0xf00ff003},// set dbgIncr of channel 2
  {54,  1, 0xD0000000+((4*16)<<2), 0xf00ff004},// set dbgIncr of channel 2
  {55,  1, 0xD0000000+((5*16)<<2), 0xf00ff005},// set dbgIncr of channel 2
  {56,  1, 0xD0000000+((6*16)<<2), 0xf00ff006},// set dbgIncr of channel 2
  {57,  1, 0xD0000000+((7*16)<<2), 0xf00ff007},// set dbgIncr of channel 2
  {58,  1, 0xD0000000+((8*16)<<2), 0xf00ff008},// set dbgIncr of channel 2
  {59,  1, 0xD0000000+((9*16)<<2), 0xf00ff009},// set dbgIncr of channel 2
  {60,  1, 0xD0000000+((10*16)<<2), 0xf00ff010},// set dbgIncr of channel 2
  {61,  1, 0xD0000000+((11*16)<<2), 0xf00ff011},// set dbgIncr of channel 2
  {62,  1, 0xD0000000+((12*16)<<2), 0xf00ff012},// set dbgIncr of channel 2
  {63,  1, 0xD0000000+((13*16)<<2), 0xf00ff013},// set dbgIncr of channel 2
  {64,  1, 0xD0000000+((14*16)<<2), 0xf00ff014},// set dbgIncr of channel 2
  {65,  1, 0xD0000000+((15*16)<<2), 0xf00ff015},// set dbgIncr of channel 2
  {66,  1, 0xD0000000+((16*16)<<2), 0xf00ff016},// set dbgIncr of channel 2
  {67,  1, 0xD0000000+((17*16)<<2), 0xf00ff017},// set dbgIncr of channel 2
  {68,  1, 0xD0000000+((18*16)<<2), 0xf00ff018},// set dbgIncr of channel 2
  {69,  1, 0xD0000000+((19*16)<<2), 0xf00ff019},// set dbgIncr of channel 2
  {70,  1, 0xD0000000+((20*16)<<2), 0xf00ff020},// set dbgIncr of channel 2
  {71,  1, 0xD0000000+((21*16)<<2), 0xf00ff021},// set dbgIncr of channel 2
  {72,  1, 0xD0000000+((22*16)<<2), 0xf00ff022},// set dbgIncr of channel 2
  {73,  1, 0xD0000000+((23*16)<<2), 0xf00ff023},// set dbgIncr of channel 2
  {74,  1, 0xD0000000+((24*16)<<2), 0xf00ff024},// set dbgIncr of channel 2
  {75,  1, 0xD0000000+((25*16)<<2), 0xf00ff025},// set dbgIncr of channel 2
  {76,  1, 0xD0000000+((26*16)<<2), 0xf00ff026},// set dbgIncr of channel 2
  {77,  1, 0xD0000000+((27*16)<<2), 0xf00ff027},// set dbgIncr of channel 2
  {78,  1, 0xD0000000+((28*16)<<2), 0xf00ff028},// set dbgIncr of channel 2
  {79,  1, 0xD0000000+((29*16)<<2), 0xf00ff029},// set dbgIncr of channel 2
  {80,  1, 0xD0000000+((30*16)<<2), 0xf00ff030},// set dbgIncr of channel 2
  {81,  1, 0xD0000000+((31*16)<<2), 0xf00ff031},// set dbgIncr of channel 31
  {4500,  0, 0xD0000000+(32<<6), 0x0},
  {4501,  0, 0xD0000000+(32<<6)+0x4, 0x0},
};
#endif

//****************************************************************************
void dbg::GnssEngine_tb(SystemOnChipIO &io)
{
  ahb_slv_in_type   *pin_ahbsi  = &topLeon3mp.stCtrl2Slv;
  ahb_slv_out_type  *pch_ahbso  = &topLeon3mp.stSlv2Ctrl.arr[AHB_SLAVE_GNSSENGINE];//    : in  apb_slv_out_vector
  uint32 *pinGpsI = &io.gnss.I[0];
  uint32 *pinGpsQ = &io.gnss.Q[0];
  uint32 *pinGloI = &io.gnss.I[1];
  uint32 *pinGloQ = &io.gnss.Q[1];

  GnssEngine        *p_GnssEngine = topLeon3mp.pclGnssEngine;


#ifdef DBG_GnssEngine
  if(io.inClk.eClock_z==SClock::CLK_POSEDGE)
  {
    //
    in_ahbsi.hsel = 0x1<<AHB_SLAVE_GNSSENGINE;//rand()&MSK32(AHB_SLAVES_MAX-1,0);//  : (0 to AHB_SLAVES_MAX-1);     -- slave select
    in_ahbsi.haddr = 0;//0xD0000000|((rand()&0xF)<<20)|rand();//(rand()<<17) + rand();// : (31 downto 0);  -- address bus (byte)
    in_ahbsi.hwrite = 0;//rand()&0x1;//  ;                           -- read/write
    in_ahbsi.htrans = HTRANS_NONSEQ;//rand()&0x3;//  : (1 downto 0);   -- transfer type
    in_ahbsi.hsize = HSIZE_WORD;//rand()&0x7;// : (2 downto 0);   -- transfer size
    in_ahbsi.hburst = HBURST_SINGLE;//rand()&0x7;//  : (2 downto 0);   -- burst type
    in_ahbsi.hwdata = 0;//((rand()<<17) + rand())&MSK32(CFG_AHBDW-1,0);//  : (AHBDW-1 downto 0);   -- write data bus
    in_ahbsi.hprot = 0;//rand()&0xf;// : (3 downto 0);   -- protection control
    in_ahbsi.hready = 1;//rand()&0x1;//  ;                -- transfer done
    in_ahbsi.hmaster = rand()&0xf;// : (3 downto 0);  -- current master
    in_ahbsi.hmastlock = rand()&0x1;// ;              -- locked access
    in_ahbsi.hmbsel = rand()&MSK32(AHB_MEM_ID_WIDTH-1,0);//  : (0 to AHB_MEM_ID_WIDTH-1); -- memory bank select
    in_ahbsi.hcache = rand()&0x1;//  ;                -- cacheable
    in_ahbsi.hirq = rand()&MSK32(AHB_IRQ_MAX-1,0);//  : (AHB_IRQ_MAX-1 downto 0); -- interrupt result bus
    in_ahbsi.testen = 0;//                        -- scan test enable
    in_ahbsi.testrst = 0;//                       -- scan test reset
    in_ahbsi.scanen = 0;//                        -- scan enable
    in_ahbsi.testoen = 0;//                       -- test output enable 
    //
    inGpsI = rand()&0x3;
    inGpsQ = rand()&0x3;
    inGloI = rand()&0x3;
    inGloQ = rand()&0x3;
    

    for (int32 i=0; i<sizeof(stst)/sizeof(STst); i++)
    {
      if(iClkCnt==stst[i].iClkNum)
      {
        in_ahbsi.hwrite = stst[i].wr;
        in_ahbsi.haddr  = stst[i].adr;
        in_ahbsi.hwdata = stst[i].val;
      }
    }
  }

  tst_GnssEngine.Update(topLeon3mp.wNRst,
                       io.inClk,
                       in_ahbsi,
                       ch_ahbso,
                       io.gnss.adc_clk,
                       io.gnss.I[0],
                       io.gnss.Q[0],
                       io.gnss.I[1],
                       io.gnss.Q[1] );
  
  pin_ahbsi = &in_ahbsi;
  pch_ahbso = &ch_ahbso;
  pinGpsI = &inGpsI;
  pinGpsQ = &inGpsQ;
  pinGloI = &inGloI;
  pinGloQ = &inGloQ;

  p_GnssEngine = &tst_GnssEngine;
#endif

  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';

    ResetPutStr();
  
    // inputs:
    pStr = PutToStr(pStr, topLeon3mp.wNRst, 1, "inNRst");
    //
    pStr = PutToStr(pStr, pin_ahbsi->hsel,AHB_SLAVES_MAX,"in_ahbsi.hsel",true);//  : (0 to AHB_SLAVES_MAX-1);     -- slave select
    pStr = PutToStr(pStr, pin_ahbsi->haddr,32,"in_ahbsi.haddr");// : (31 downto 0);  -- address bus (byte)
    pStr = PutToStr(pStr, pin_ahbsi->hwrite,1,"in_ahbsi.hwrite");//  ;                           -- read/write
    pStr = PutToStr(pStr, pin_ahbsi->htrans,2,"in_ahbsi.htrans");//  : (1 downto 0);   -- transfer type
    pStr = PutToStr(pStr, pin_ahbsi->hsize,3,"in_ahbsi.hsize");// : (2 downto 0);   -- transfer size
    pStr = PutToStr(pStr, pin_ahbsi->hburst,3,"in_ahbsi.hburst");//  : (2 downto 0);   -- burst type
    pStr = PutToStr(pStr, pin_ahbsi->hwdata,CFG_AHBDW,"in_ahbsi.hwdata");//  : (AHBDW-1 downto 0);   -- write data bus
    pStr = PutToStr(pStr, pin_ahbsi->hprot,4,"in_ahbsi.hprot");// : (3 downto 0);   -- protection control
    pStr = PutToStr(pStr, pin_ahbsi->hready,1,"in_ahbsi.hready");//  ;                -- transfer done
    pStr = PutToStr(pStr, pin_ahbsi->hmaster,4,"in_ahbsi.hmaster");// : (3 downto 0);  -- current master
    pStr = PutToStr(pStr, pin_ahbsi->hmastlock,1,"in_ahbsi.hmastlock");// ;              -- locked access
    pStr = PutToStr(pStr, pin_ahbsi->hmbsel,AHB_MEM_ID_WIDTH,"in_ahbsi.hmbsel",true);//  : (0 to AHB_MEM_ID_WIDTH-1); -- memory bank select
    pStr = PutToStr(pStr, pin_ahbsi->hcache,1,"in_ahbsi.hcache");//  ;                -- cacheable
    pStr = PutToStr(pStr, pin_ahbsi->hirq,AHB_IRQ_MAX,"in_ahbsi.hirq");//  : (AHB_IRQ_MAX-1 downto 0); -- interrupt result bus
    pStr = PutToStr(pStr, pin_ahbsi->testen,1,"in_ahbsi.testen");//                        -- scan test enable
    pStr = PutToStr(pStr, pin_ahbsi->testrst,1,"in_ahbsi.testrst");//                       -- scan test reset
    pStr = PutToStr(pStr, pin_ahbsi->scanen,1,"in_ahbsi.scanen");//                        -- scan enable
    pStr = PutToStr(pStr, pin_ahbsi->testoen,1,"in_ahbsi.testoen");//                       -- test output enable 
    //
    pStr = PutToStr(pStr, *pinGpsI,2,"in_GpsI");
    pStr = PutToStr(pStr, *pinGpsQ,2,"in_GpsQ");
    pStr = PutToStr(pStr, *pinGloI,2,"in_GloI");
    pStr = PutToStr(pStr, *pinGloQ,2,"in_GloQ");


    // Output:
    pStr = PutToStr(pStr, pch_ahbso->hready,1,"ch_ahbso.hready");//                           -- transfer done
    pStr = PutToStr(pStr, pch_ahbso->hresp,2,"ch_ahbso.hresp");//[1:0] : (1 downto 0);   -- response type
    pStr = PutToStr(pStr, pch_ahbso->hrdata,CFG_AHBDW,"ch_ahbso.hrdata");//(AHBDW-1 downto 0);   -- read data bus
    pStr = PutToStr(pStr, pch_ahbso->hsplit,16,"ch_ahbso.hsplit");//  : (15 downto 0);  -- split completion
    pStr = PutToStr(pStr, pch_ahbso->hcache,1,"ch_ahbso.hcache");//                           -- cacheable
    pStr = PutToStr(pStr, pch_ahbso->hirq,AHB_IRQ_MAX,"ch_ahbso.hirq");//    : (AHB_IRQ_MAX-1 downto 0); -- interrupt bus
    pStr = PutToStr(pStr, pch_ahbso->hconfig.arr[0],32,"ch_ahbso.hconfig(0)");//   : ahb_config_type;      -- memory access reg.
    pStr = PutToStr(pStr, pch_ahbso->hconfig.arr[1],32,"ch_ahbso.hconfig(1)");
    pStr = PutToStr(pStr, pch_ahbso->hconfig.arr[2],32,"ch_ahbso.hconfig(2)");
    pStr = PutToStr(pStr, pch_ahbso->hconfig.arr[3],32,"ch_ahbso.hconfig(3)");
    pStr = PutToStr(pStr, pch_ahbso->hconfig.arr[4],32,"ch_ahbso.hconfig(4)");
    pStr = PutToStr(pStr, pch_ahbso->hconfig.arr[5],32,"ch_ahbso.hconfig(5)");
    pStr = PutToStr(pStr, pch_ahbso->hconfig.arr[6],32,"ch_ahbso.hconfig(6)");
    pStr = PutToStr(pStr, pch_ahbso->hconfig.arr[7],32,"ch_ahbso.hconfig(7)");
    pStr = PutToStr(pStr, pch_ahbso->hindex,4,"conv_integer:ch_ahbso.hindex");//    : integer range 0 to AHB_SLAVES_MAX-1;   -- diagnostic use only

    // Internal:

    PrintIndexStr();

    *posBench[TB_GnssEngine] << chStr << "\n";
  }

#ifdef DBG_GnssEngine
  tst_GnssEngine.ClkUpdate();
#endif
}



