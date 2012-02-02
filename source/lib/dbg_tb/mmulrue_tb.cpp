#include "lheaders.h"

extern leon3mp  topLeon3mp;

extern void ResetPutStr();
extern void PrintIndexStr();

#ifdef DBG_mmulrue

//****************************************************************************
void dbg::mmulrue_tb(SystemOnChipIO &io)
{
  if(io.inClk.eClock_z==SClock::CLK_POSEDGE)
  {
    //
    in_lruei.touch = rand()&0x1;//   : std_logic;
    in_lruei.pos = rand()&MSK32(M_ENT_MAX_LOG-1,0);//   : std_logic_vector(M_ENT_MAX_LOG-1 downto 0);
    in_lruei.clear = rand()&0x1;//   : std_logic;
    in_lruei.flush = rand()&0x1;//   : std_logic;
    in_lruei.left = rand()&MSK32(M_ENT_MAX_LOG-1,0);//   : std_logic_vector(M_ENT_MAX_LOG-1 downto 0);
    in_lruei.fromleft = rand()&0x1;//   : std_logic;
    in_lruei.right = rand()&MSK32(M_ENT_MAX_LOG-1,0);//   : std_logic_vector(M_ENT_MAX_LOG-1 downto 0);
    in_lruei.fromright = rand()&0x1;//    : std_logic;
  }

  ptst_mmulrue->Update( io.inNRst,//     : in  std_logic;
                        io.inClk,//     : in  std_logic;
                        in_lruei,//  : in mmulrue_in_type;
                        ch_lrueo);//  : out mmulrue_out_type );

  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';

    ResetPutStr();
    
    // inputs:
    pStr = PutToStr(pStr, io.inNRst, 1, "inNRst");
    //
    pStr = PutToStr(pStr, in_lruei.touch,1,"in_lruei.touch");//   : std_logic;
    pStr = PutToStr(pStr, in_lruei.pos,M_ENT_MAX_LOG,"in_lruei.pos");//   : std_logic_vector(M_ENT_MAX_LOG-1 downto 0);
    pStr = PutToStr(pStr, in_lruei.clear,1,"in_lruei.clear");//   : std_logic;
    pStr = PutToStr(pStr, in_lruei.flush,1,"in_lruei.flush");//   : std_logic;
    pStr = PutToStr(pStr, in_lruei.left,M_ENT_MAX_LOG,"in_lruei.left");//   : std_logic_vector(M_ENT_MAX_LOG-1 downto 0);
    pStr = PutToStr(pStr, in_lruei.fromleft,1,"in_lruei.fromleft");//   : std_logic;
    pStr = PutToStr(pStr, in_lruei.right,M_ENT_MAX_LOG,"in_lruei.right");//   : std_logic_vector(M_ENT_MAX_LOG-1 downto 0);
    pStr = PutToStr(pStr, in_lruei.fromright,1,"in_lruei.fromright");//    : std_logic;

    // Outputs:
    pStr = PutToStr(pStr, ch_lrueo.pos,M_ENT_MAX_LOG,"ch_lrueo.pos");//          : std_logic_vector(M_ENT_MAX_LOG-1 downto 0);
    pStr = PutToStr(pStr, ch_lrueo.movetop,1,"ch_lrueo.movetop");//      : std_logic;

    
    PrintIndexStr();

    *posBench[TB_mmulrue] << chStr << "\n";
  }  
  
  ptst_mmulrue->ClkUpdate();
}
#endif
