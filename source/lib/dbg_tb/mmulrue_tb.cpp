#include "ldbg.h"
#include "leon3_ml605\leon3mp.h"
extern leon3mp topLeon3mp;

//#define DBG_mmulrue

#ifdef DBG_mmulrue
    mmulrue_in_type in_lruei;//  : in mmulrue_in_type;
    mmulrue_out_type ch_lrueo;//  : out mmulrue_out_type );

    mmulrue tst_mmulrue(0,CFG_DTLBNUM);
#endif


//****************************************************************************
void dbg::mmulrue_tb(SystemOnChipIO &io)
{
  mmulrue *ptst_mmulrue = topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->clMMU.dtlb0->lru->l1[0];
  
  mmulrue_in_type *pin_lruei = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->clMMU.dtlb0->lru->lruei->arr[0];//  : in mmulru_in_type;
  mmulrue_out_type *pch_lrueo = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->clMMU.dtlb0->lru->lrueo->arr[0];//  : out mmulru_out_type 
#ifdef DBG_mmulrue
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

  tst_mmulrue.Update( io.inNRst,//     : in  std_logic;
                        io.inClk,//     : in  std_logic;
                        in_lruei,//  : in mmulrue_in_type;
                        ch_lrueo);//  : out mmulrue_out_type );
  
  ptst_mmulrue = &tst_mmulrue;
  pin_lruei = &in_lruei;
  pch_lrueo = &ch_lrueo;
#endif

  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';

    ResetPutStr();
    
    // inputs:
    pStr = PutToStr(pStr, io.inNRst, 1, "inNRst");
    //
    pStr = PutToStr(pStr, pin_lruei->touch,1,"in_lruei.touch");//   : std_logic;
    pStr = PutToStr(pStr, pin_lruei->pos,M_ENT_MAX_LOG,"in_lruei.pos");//   : std_logic_vector(M_ENT_MAX_LOG-1 downto 0);
    pStr = PutToStr(pStr, pin_lruei->clear,1,"in_lruei.clear");//   : std_logic;
    pStr = PutToStr(pStr, pin_lruei->flush,1,"in_lruei.flush");//   : std_logic;
    pStr = PutToStr(pStr, pin_lruei->left,M_ENT_MAX_LOG,"in_lruei.left");//   : std_logic_vector(M_ENT_MAX_LOG-1 downto 0);
    pStr = PutToStr(pStr, pin_lruei->fromleft,1,"in_lruei.fromleft");//   : std_logic;
    pStr = PutToStr(pStr, pin_lruei->right,M_ENT_MAX_LOG,"in_lruei.right");//   : std_logic_vector(M_ENT_MAX_LOG-1 downto 0);
    pStr = PutToStr(pStr, pin_lruei->fromright,1,"in_lruei.fromright");//    : std_logic;

    // Outputs:
    pStr = PutToStr(pStr, pch_lrueo->pos,M_ENT_MAX_LOG,"ch_lrueo.pos");//          : std_logic_vector(M_ENT_MAX_LOG-1 downto 0);
    pStr = PutToStr(pStr, pch_lrueo->movetop,1,"ch_lrueo.movetop");//      : std_logic;

    
    PrintIndexStr();

    *posBench[TB_mmulrue] << chStr << "\n";
  }  
#ifdef DBG_mmulrue
  tst_mmulrue.ClkUpdate();
#endif
}


