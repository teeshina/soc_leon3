#include "lheaders.h"

extern leon3mp  topLeon3mp;

extern void ResetPutStr();
extern void PrintIndexStr();

#ifdef DBG_tbufmem

//****************************************************************************
void dbg::tbufmem_tb(SystemOnChipIO &io)
{
  if(io.inClk.eClock_z==SClock::CLK_POSEDGE)
  {
    //
    in_tbi.addr = rand()&0xFFF;//             : std_logic_vector(11 downto 0);
    in_tbi.data[2] = (uint64(rand())<<49) | (uint64(rand())<<33) | (uint64(rand())<<17) |uint64(rand());//             : std_logic_vector(127 downto 0);
    in_tbi.data[1] = (uint64(rand())<<49) | (uint64(rand())<<33) | (uint64(rand())<<17) |uint64(rand());//             : std_logic_vector(127 downto 0);
    in_tbi.enable = rand()&0x1;//           : std_logic;
    in_tbi.write = rand()&0xf;//            : std_logic_vector(3 downto 0);
    in_tbi.diag = rand()&0xf;//             : std_logic_vector(3 downto 0);
  }

  // 
  tst_tbufmem.Update( io.inClk,
                      in_tbi,
                      ch_tbo );

  // output file writting:
  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';

    ResetPutStr();
  
    // inputs:
    pStr = PutToStr(pStr, io.inNRst, 1, "inNRst");
    pStr = PutToStr(pStr, in_tbi.addr, 12, "in_tbi.addr");
    pStr = PutToStr(pStr, in_tbi.data[0], 64, "in_tbi.data(63 downto 0)");
    pStr = PutToStr(pStr, in_tbi.data[1], 64, "in_tbi.data(127 downto 64)");
    pStr = PutToStr(pStr, in_tbi.enable, 1, "in_tbi.enable");
    pStr = PutToStr(pStr, in_tbi.write, 4, "in_tbi.write");
    pStr = PutToStr(pStr, in_tbi.diag, 4, "in_tbi.diag");
    
    // outputs:
    pStr = PutToStr(pStr, ch_tbo.data[0], 64, "ch_tbo.data(63 downto 0)");
    pStr = PutToStr(pStr, ch_tbo.data[1], 64, "ch_tbo.data(127 downto 64)");

    // internal:
                        
    PrintIndexStr();

    *posBench[TB_tbufmem] << chStr << "\n";
  }

  // Clock update:
  tst_tbufmem.ClkUpdate();
}
#endif

