#include "lheaders.h"

extern leon3mp  topLeon3mp;

extern void ResetPutStr();
extern void PrintIndexStr();

#ifdef DBG_regfile_3p

//****************************************************************************
void dbg::regfile_3p_tb(SystemOnChipIO &io)
{
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

  // output file writting:
  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';

    ResetPutStr();
  
    // inputs:
    pStr = PutToStr(pStr, io.inNRst, 1, "inNRst");
    pStr = PutToStr(pStr, in_waddr,CFG_IRFBITS,"in_waddr");//  : in  std_logic_vector((abits -1) downto 0);
    pStr = PutToStr(pStr, in_wdata,32,"in_wdata");//  : in  std_logic_vector((dbits -1) downto 0);
    pStr = PutToStr(pStr, in_we,1,"in_we");//     : in  std_ulogic;
    pStr = PutToStr(pStr, in_raddr1,CFG_IRFBITS,"in_raddr1");// : in  std_logic_vector((abits -1) downto 0);
    pStr = PutToStr(pStr, in_re1,1,"in_re1");//    : in  std_ulogic;
    pStr = PutToStr(pStr, in_raddr2,CFG_IRFBITS,"in_raddr2");// : in  std_logic_vector((abits -1) downto 0);
    pStr = PutToStr(pStr, in_re2,1,"in_re2");//    : in  std_ulogic;
    pStr = PutToStr(pStr, in_testin,4,"in_testin");//   : in std_logic_vector(3 downto 0) := "0000");
    // outputs:
    pStr = PutToStr(pStr, ch_rdata1,32,"ch_rdata1");// : out std_logic_vector((dbits -1) downto 0);
    pStr = PutToStr(pStr, ch_rdata2,32,"ch_rdata2");// : out std_logic_vector((dbits -1) downto 0);
    // internal:
                        
    PrintIndexStr();

    *posBench[TB_regfile_3p] << chStr << "\n";
  }

  // Clock update:
  tst_regfile_3p.ClkUpdate();
}
#endif

