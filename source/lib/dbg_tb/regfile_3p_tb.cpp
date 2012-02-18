#include "lheaders.h"

extern leon3mp  topLeon3mp;

extern void ResetPutStr();
extern void PrintIndexStr();


//****************************************************************************
void dbg::regfile_3p_tb(SystemOnChipIO &io)
{
  regfile_3p *ptst_regfile_3p = &topLeon3mp.pclLeon3s[0]->clRegfile3Port;

  uint32 *pin_waddr   = &topLeon3mp.pclLeon3s[0]->rfi.waddr;//  : in  std_logic_vector((abits -1) downto 0);
  uint32 *pin_wdata   = &topLeon3mp.pclLeon3s[0]->rfi.wdata;//  : in  std_logic_vector((dbits -1) downto 0);
  uint32 *pin_we      = &topLeon3mp.pclLeon3s[0]->rfi.wren;//     : in  std_ulogic;
  uint32 *pin_raddr1  = &topLeon3mp.pclLeon3s[0]->rfi.raddr1;// : in  std_logic_vector((abits -1) downto 0);
  uint32 *pin_re1     = &topLeon3mp.pclLeon3s[0]->rfi.ren1;//    : in  std_ulogic;
  uint32 *pch_rdata1  = &topLeon3mp.pclLeon3s[0]->rfo.data1;// : out std_logic_vector((dbits -1) downto 0);
  uint32 *pin_raddr2  = &topLeon3mp.pclLeon3s[0]->rfi.raddr2;// : in  std_logic_vector((abits -1) downto 0);
  uint32 *pin_re2     = &topLeon3mp.pclLeon3s[0]->rfi.ren2;//    : in  std_ulogic;
  uint32 *pch_rdata2  = &topLeon3mp.pclLeon3s[0]->rfo.data2;// : out std_logic_vector((dbits -1) downto 0);
  uint32 *pin_testin  = &topLeon3mp.pclLeon3s[0]->rfi.diag;//   : in std_logic_vector(3 downto 0) := "0000");

#ifdef DBG_regfile_3p
  if(io.inClk.eClock_z==SClock::CLK_POSEDGE)
  {
    //
    in_waddr = rand()&MSK32(CFG_IRFBITS-1,0);
    in_wdata = (rand()<<17)|rand();
    in_we = rand()&0x1;
    in_raddr1 = rand()&MSK32(CFG_IRFBITS-1,0);
    in_re1 = rand()&0x1;
    in_raddr2 = rand()&MSK32(CFG_IRFBITS-1,0);
    in_re2 = rand()&0x1;
    in_testin = 0;
  }

  // 
  tst_regfile_3p.Update(io.inClk,
                        in_waddr,//  : in  std_logic_vector((abits -1) downto 0);
                        in_wdata,//  : in  std_logic_vector((dbits -1) downto 0);
                        in_we,//     : in  std_ulogic;
                        io.inClk,//   : in  std_ulogic;
                        in_raddr1,// : in  std_logic_vector((abits -1) downto 0);
                        in_re1,//    : in  std_ulogic;
                        ch_rdata1,// : out std_logic_vector((dbits -1) downto 0);
                        in_raddr2,// : in  std_logic_vector((abits -1) downto 0);
                        in_re2,//    : in  std_ulogic;
                        ch_rdata2,// : out std_logic_vector((dbits -1) downto 0);
                        in_testin//   : in std_logic_vector(3 downto 0) := "0000");
                        );// : in std_ulogic
  
  ptst_regfile_3p = &tst_regfile_3p;
  pin_waddr   = &in_waddr;
  pin_wdata   = &in_wdata;
  pin_we      = &in_we;
  pin_raddr1  = &in_raddr1;
  pin_re1     = &in_re1;
  pch_rdata1  = &ch_rdata1;
  pin_raddr2  = &in_raddr2;
  pin_re2     = &in_re2;
  pch_rdata2  = &ch_rdata2;
  pin_testin  = &in_testin;

#endif

  // output file writting:
  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';

    ResetPutStr();
  
    // inputs:
    pStr = PutToStr(pStr, io.inNRst, 1, "inNRst");
    pStr = PutToStr(pStr, *pin_waddr,CFG_IRFBITS,"in_waddr");//  : in  std_logic_vector((abits -1) downto 0);
    pStr = PutToStr(pStr, *pin_wdata,32,"in_wdata");//  : in  std_logic_vector((dbits -1) downto 0);
    pStr = PutToStr(pStr, *pin_we,1,"in_we");//     : in  std_ulogic;
    pStr = PutToStr(pStr, *pin_raddr1,CFG_IRFBITS,"in_raddr1");// : in  std_logic_vector((abits -1) downto 0);
    pStr = PutToStr(pStr, *pin_re1,1,"in_re1");//    : in  std_ulogic;
    pStr = PutToStr(pStr, *pin_raddr2,CFG_IRFBITS,"in_raddr2");// : in  std_logic_vector((abits -1) downto 0);
    pStr = PutToStr(pStr, *pin_re2,1,"in_re2");//    : in  std_ulogic;
    pStr = PutToStr(pStr, *pin_testin,4,"in_testin");//   : in std_logic_vector(3 downto 0) := "0000");
    // outputs:
    pStr = PutToStr(pStr, *pch_rdata1,32,"ch_rdata1");// : out std_logic_vector((dbits -1) downto 0);
    pStr = PutToStr(pStr, *pch_rdata2,32,"ch_rdata2");// : out std_logic_vector((dbits -1) downto 0);
    // internal:
                        
    PrintIndexStr();

    *posBench[TB_regfile_3p] << chStr << "\n";
  }

#ifdef DBG_regfile_3p
  // Clock update:
  tst_regfile_3p.ClkUpdate();
#endif
}


