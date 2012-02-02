#include "lheaders.h"

extern leon3mp  topLeon3mp;

extern void ResetPutStr();
extern void PrintIndexStr();

#ifdef DBG_mmulru

//****************************************************************************
void dbg::mmulru_tb(SystemOnChipIO &io)
{
  if(io.inClk.eClock_z==SClock::CLK_POSEDGE)
  {
    //
    in_lrui.touch = rand()&0x1;//   : std_logic;
    in_lrui.touchmin = rand()&0x1;//   : std_logic;
    in_lrui.flush = rand()&0x1;//   : std_logic;
    in_lrui.pos = rand()&MSK32(M_ENT_MAX_LOG-1,0);//   : std_logic_vector(M_ENT_MAX_LOG-1 downto 0);
    in_lrui.mmctrl1.e= rand()&0x1;//    : std_logic;                -- enable
    in_lrui.mmctrl1.nf= rand()&0x1;//    : std_logic;                -- no fault
    in_lrui.mmctrl1.pso= rand()&0x1;//    : std_logic;                -- partial store order
    in_lrui.mmctrl1.pagesize= rand()&0x3;// std_logic_vector(1 downto 0);-- page size
    in_lrui.mmctrl1.ctx = rand()&MSK32(M_CTX_SZ-1,0);//     : std_logic_vector(M_CTX_SZ-1 downto 0);-- context nr
    in_lrui.mmctrl1.ctxp = ((rand()<<15)|rand())&MSK32(MMCTRL_CTXP_SZ-1,0);//    : std_logic_vector(MMCTRL_CTXP_SZ-1 downto 0);  -- context table pointer
    in_lrui.mmctrl1.tlbdis= rand()&0x1;// : std_logic;                            -- tlb disabled
    in_lrui.mmctrl1.bar= rand()&0x3;// : std_logic_vector(1 downto 0);         -- preplace barrier
  }

  ptst_mmulru->Update( io.inNRst,//     : in  std_logic;
                        io.inClk,//     : in  std_logic;
                        in_lrui,//  : in mmulrue_in_type;
                        ch_lruo);//  : out mmulrue_out_type );


  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';

    ResetPutStr();
    
    // inputs:
    pStr = PutToStr(pStr, io.inNRst, 1, "inNRst");
    //
    pStr = PutToStr(pStr, in_lrui.touch,1,"in_lrui.touch");//   : std_logic;
    pStr = PutToStr(pStr, in_lrui.touchmin,1,"in_lrui.touchmin");//   : std_logic;
    pStr = PutToStr(pStr, in_lrui.flush,1,"in_lrui.flush");//   : std_logic;
    pStr = PutToStr(pStr, in_lrui.pos,M_ENT_MAX_LOG,"in_lrui.pos");//   : std_logic_vector(M_ENT_MAX_LOG-1 downto 0);
    pStr = PutToStr(pStr, in_lrui.mmctrl1.e,1,"in_lrui.mmctrl1.e");//    : std_logic;                -- enable
    pStr = PutToStr(pStr, in_lrui.mmctrl1.nf,1,"in_lrui.mmctrl1.nf");//    : std_logic;                -- no fault
    pStr = PutToStr(pStr, in_lrui.mmctrl1.pso,1,"in_lrui.mmctrl1.pso");//    : std_logic;                -- partial store order
    pStr = PutToStr(pStr, in_lrui.mmctrl1.pagesize,2,"in_lrui.mmctrl1.pagesize");// std_logic_vector(1 downto 0);-- page size
    pStr = PutToStr(pStr, in_lrui.mmctrl1.ctx,M_CTX_SZ,"in_lrui.mmctrl1.ctx");//     : std_logic_vector(M_CTX_SZ-1 downto 0);-- context nr
    pStr = PutToStr(pStr, in_lrui.mmctrl1.ctxp,MMCTRL_CTXP_SZ,"in_lrui.mmctrl1.ctxp");//    : std_logic_vector(MMCTRL_CTXP_SZ-1 downto 0);  -- context table pointer
    pStr = PutToStr(pStr, in_lrui.mmctrl1.tlbdis,1,"in_lrui.mmctrl1.tlbdis");// : std_logic;                            -- tlb disabled
    pStr = PutToStr(pStr, in_lrui.mmctrl1.bar,2,"in_lrui.mmctrl1.bar");// : std_logic_vector(1 downto 0);         -- preplace barrier

    // Outputs:
    pStr = PutToStr(pStr, ch_lruo.pos,M_ENT_MAX_LOG,"ch_lruo.pos");//          : std_logic_vector(M_ENT_MAX_LOG-1 downto 0);
    
    PrintIndexStr();

    *posBench[TB_mmulru] << chStr << "\n";
  }

  ptst_mmulru->ClkUpdate();
}
#endif

