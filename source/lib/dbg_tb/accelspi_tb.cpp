//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GPL
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "lheaders.h"

#ifdef DBG_accelspi
class AccelerEmu
{
  friend class dbg;
  private:
    uint32 clk_z;
    uint32 mem[64];
    uint32 wshift;
    uint32 rshift;
    uint32 iBitCnt;
    
    uint32 ModeRW;
    uint32 ModeIncr;
    uint32 addr;
  public:
    AccelerEmu() : clk_z(0),rshift(0)
    {
      memset(mem,0,sizeof(uint64)*sizeof(64));
      mem[0x0f] = 0xD4;//who_am_i default value
      mem[0x20] = 0x07;
    }
    void Update(uint32 nCS, uint32 clk, uint32 idata, uint32 &odata, uint32 &oint1, uint32 &oint2 )
    {
      bool bPosedge = false;
      bool bNegedge = false;
      if(clk&!clk_z) bPosedge = true;
      if(!clk&clk_z) bNegedge = true;
      
      if(bPosedge&!nCS)
      {
        iBitCnt++;
        wshift = (wshift<<1)|idata;
        
        if(iBitCnt==8)
        {
          ModeRW = BIT32(wshift,7);
          ModeIncr = BIT32(wshift,6);
          addr = BITS32(wshift,5,0);
        }
        if((iBitCnt==16)&&(ModeRW==0))
        {
          mem[addr] = BITS32(wshift,7,0);
          iBitCnt=8;
          if(ModeIncr==1)
            addr+=1;
        }
      }
      
      if(bNegedge)
      {
        if(iBitCnt==8) rshift = mem[addr];
        if(iBitCnt>8)  rshift <<= 1;
      }
      
      if(nCS==1) iBitCnt=0;
      odata = BIT32(rshift,7);
      
      clk_z = clk;
      oint1 = 0;
      oint2 = 0;
    }
};
AccelerEmu clAccelerEmu;

struct STst{ uint32 iClkNum; uint32 rw:1; uint32 adr; uint32 val : 8; };
STst stst[] = 
{
  {37,  1, (0x800009<<8)|(0x00<<2), 0x8},// scaler
  {38,  1, (0x800009<<8)|(0x01<<2), 500},// interval of reading counter
#if 1
  {39,  1, (0x800009<<8)|(0x02<<2), 0x8},// enable reading by writting into 0x3C
#else
  {39,  1, (0x800009<<8)|(0x02<<2), 0x1},// enable internal counter of reading
#endif
  // gyro:
  {40,  1, (0x800008<<8)|(0xf<<2), 0x1},
  {42,  1, (0x800008<<8)|(0x20<<2), 0xf},
  {44,  1, (0x800008<<8)|(0x21<<2), 0xe},
  {46,  1, (0x800008<<8)|(0x22<<2), 0x7},
  {48,  0, (0x800008<<8)|(0x21<<2), 0x8},
  {50,  0, (0x800008<<8)|(0x23<<2), 0x9},
  
  {61,  1, (0x800008<<8)|(0x24<<2), 0xa},
  {62,  1, (0x800008<<8)|(0x25<<2), 0xb},
  {63,  1, (0x800008<<8)|(0x2f<<2), 0xc},
  {64,  1, (0x800008<<8)|(0x30<<2), 0xd},
  {65,  1, (0x800008<<8)|(0x31<<2), 0xe},
  {66,  1, (0x800008<<8)|(0x32<<2), 0xf},
  {67,  1, (0x800008<<8)|(0x33<<2), 0x10},
  {68,  1, (0x800008<<8)|(0x34<<2), 0x11},
  {69,  1, (0x800008<<8)|(0x35<<2), 0x12},
  {70,  1, (0x800008<<8)|(0x36<<2), 0x13},
  {71,  1, (0x800008<<8)|(0x37<<2), 0x14},
  {72,  1, (0x800008<<8)|(0x38<<2), 0x15},
  {73,  1, (0x800008<<8)|(0x39<<2), 0x16},
  {74,  1, (0x800008<<8)|(0x3a<<2), 0x17},
  {75,  1, (0x800008<<8)|(0x3b<<2), 0x18},
  {76,  1, (0x800008<<8)|(0x3c<<2), 0x19},
  {77,  1, (0x800008<<8)|(0x3d<<2), 0x1a},
  {78,  1, (0x800008<<8)|(0x3e<<2), 0x1b},
  {79,  1, (0x800008<<8)|(0x3f<<2), 0x1c},
  {80,  1, (0x800008<<8)|(0x3a<<2), 0x1d},
  {81,  1, (0x800008<<8)|(0x3a<<2), 0x1e},
  {82,  1, (0x800008<<8)|(0x3a<<2), 0x1f},
  {83,  1, (0x800008<<8)|(0x3a<<2), 0x20},
  {84,  1, (0x800008<<8)|(0x3a<<2), 0x21},
  {85,  1, (0x800008<<8)|(0x3a<<2), 0x22},
  {86,  1, (0x800008<<8)|(0x3a<<2), 0x23},
  
  
  
  {380,  1, (0x800009<<8)|(0x03<<2), 0x0},//run by writting
  {2500,  1, (0x800008<<8)|(0x3f<<2), 0xcc},
  {7800,  0, (0x800008<<8)|(0x0f<<2), 0x0},
  {10800, 0, (0x800008<<8)|(0x25<<2), 0x0},
};
#endif

//****************************************************************************
void dbg::accelspi_tb(SystemOnChipIO &io)
{
  apb_slv_in_type   *pin_apbi  = &topLeon3mp.apbi;
  apb_slv_out_type  *pch_apbo  = &topLeon3mp.apbo.arr[APB_ACCELEROMETER];//    : in  apb_slv_out_vector
  uint32 *pin_Int1 = &io.acceler.Int1;
  uint32 *pin_Int2 = &io.acceler.Int2;
  uint32 *pin_SDI = &io.acceler.SDO;
  uint32 *pch_SPC = &io.acceler.SPC;
  uint32 *pch_SDO = &io.acceler.SDI;
  uint32 *pch_CSn = &io.acceler.nCS;

  accelspi        *p_accelspi = topLeon3mp.pclAccelerometer;


#ifdef DBG_accelspi
  if(io.inClk.eClock_z==SClock::CLK_POSEDGE)
  {
    //
    in_apbi.psel = 0x1<<APB_ACCELEROMETER;//[0:15]  : (0 to APB_SLAVES_MAX-1); -- slave select
    in_apbi.penable = 0;// ;                       -- strobe
    in_apbi.pwrite = 0;
    for (int32 i=0; i<sizeof(stst)/sizeof(STst); i++)
    {
      if(iClkCnt==stst[i].iClkNum)
      {
        in_apbi.pwrite = stst[i].rw;
        in_apbi.penable = 1;
        in_apbi.paddr = stst[i].adr;
        in_apbi.pwdata = stst[i].val;
      }
    }
    in_apbi.testen = 0;//                          -- scan test enable
    in_apbi.testrst = 0;//                         -- scan test reset
    in_apbi.scanen = 0;//                          -- scan enable
    in_apbi.testoen = 0;//                         -- test output enable
    //
  }
  
  clAccelerEmu.Update(ch_CSn, ch_SPC, ch_SDO, in_SDI, in_Int1, in_Int2);

  tst_accelspi.Update(topLeon3mp.wNRst,
                       io.inClk,
                       in_apbi,
                       ch_apbo,
                       in_Int1,
                       in_Int2,
                       in_SDI,
                       ch_SPC,
                       ch_SDO,
                       ch_CSn );
  
  pin_apbi = &in_apbi;//    : out apb_slv_in_type;
  pch_apbo = &ch_apbo;//    : in  apb_slv_out_vector
  pin_Int1 = &in_Int1;
  pin_Int2 = &in_Int2;
  pin_SDI  = &in_SDI;
  pch_SPC  = &ch_SPC;
  pch_SDO  = &ch_SDO;
  pch_CSn  = &ch_CSn;
  
  p_accelspi = &tst_accelspi;
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
    pStr = PutToStr(pStr, *pin_SDI, 1, "in_SDI");
    pStr = PutToStr(pStr, *pin_Int1, 1, "in_Int1");
    pStr = PutToStr(pStr, *pin_Int2, 1, "in_Int2");


    // Output:
    pStr = PutToStr(pStr, pch_apbo->prdata,32,"ch_apbo.prdata");//                           -- transfer done
    pStr = PutToStr(pStr, pch_apbo->pirq,32,"ch_apbo.pirq");
    pStr = PutToStr(pStr, pch_apbo->pconfig.arr[0],32,"ch_apbo.pconfig(0)");
    pStr = PutToStr(pStr, pch_apbo->pconfig.arr[1],32,"ch_apbo.pconfig(1)");
    pStr = PutToStr(pStr, pch_apbo->pindex,4,"conv_integer:ch_apbo.pindex");
    //
    pStr = PutToStr(pStr, *pch_SPC, 1, "ch_SPC");
    pStr = PutToStr(pStr, *pch_SDO, 1, "ch_SDO");
    pStr = PutToStr(pStr, *pch_CSn, 1, "ch_CSn");

    // Internal:
    pStr = PutToStr(pStr, p_accelspi->r.Q.BitCnt,6,"r_BitCnt");
    pStr = PutToStr(pStr, p_accelspi->r.Q.FifoCnt,6,"r_FifoCnt");
    pStr = PutToStr(pStr, p_accelspi->r.Q.ScaleCnt,32,"t_ScaleCnt");
    pStr = PutToStr(pStr, p_accelspi->wRdFifo,1,"t_wRdFifo");
    pStr = PutToStr(pStr, p_accelspi->wWrFifo,1,"t_wWrFifo");
    pStr = PutToStr(pStr, p_accelspi->r.Q.WriteWord,16,"r_WriteWord");
    pStr = PutToStr(pStr, p_accelspi->r.Q.Writting,1,"r_Writting");
    pStr = PutToStr(pStr, p_accelspi->r.Q.Reading,1,"r_Reading");
    pStr = PutToStr(pStr, p_accelspi->rd_possible,1,"rd_possible");
    pStr = PutToStr(pStr, p_accelspi->rd_all_run,1,"rd_all_run");
    pStr = PutToStr(pStr, p_accelspi->rd_word_run,1,"rd_word_run");
    pStr = PutToStr(pStr, p_accelspi->r.Q.RxAdr,8,"r_RxAdr");
    pStr = PutToStr(pStr, p_accelspi->r.Q.rd_wordcnt,5,"r_rd_wordcnt");
    pStr = PutToStr(pStr, p_accelspi->r.Q.RxData,8,"r_RxData");
    pStr = PutToStr(pStr, p_accelspi->rd_word_rdy,1,"rd_word_rdy");

    PrintIndexStr();

    *posBench[TB_accelspi] << chStr << "\n";
  }

#ifdef DBG_accelspi
  tst_accelspi.ClkUpdate();
#endif
}



