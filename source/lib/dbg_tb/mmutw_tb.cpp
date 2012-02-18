#include "lheaders.h"

extern leon3mp  topLeon3mp;

extern void ResetPutStr();
extern void PrintIndexStr();

//****************************************************************************
void dbg::mmutw_tb(SystemOnChipIO &io)
{
  mmutw *ptst_mmutw = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->clMMU.tw0;

  mmctrl_type1 *pin_mmctrl1     = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->clMMU.mmctrl1;// : in  mmctrl_type1;
  mmutw_in_type *pin_twi        = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->clMMU.twi;//     : in  mmutw_in_type;
  mmutw_out_type *pch_two       = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->clMMU.two;//     : out mmutw_out_type;
  memory_mm_out_type *pin_mcmmo = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->mcmmo;//   : in  memory_mm_out_type;
  memory_mm_in_type *pch_mcmmi  = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->mcmmi;//   : out memory_mm_in_type

#ifdef DBG_mmutw
  if(io.inClk.eClock_z==SClock::CLK_POSEDGE)
  {
    //
    in_mcmmo.data = (rand()<<17) + rand();//             : std_logic_vector(31 downto 0); -- memory data
    in_mcmmo.ready = rand()&0x1;//           : std_logic;         -- cycle ready
    in_mcmmo.grant = rand()&0x1;//           : std_logic;         -- 
    in_mcmmo.retry = rand()&0x1;//           : std_logic;         -- 
    in_mcmmo.mexc = rand()&0x1;//           : std_logic;         -- memory exception
    in_mcmmo.werr = rand()&0x1;//           : std_logic;         -- memory write error
    in_mcmmo.cache = rand()&0x1;//           : std_logic;               -- cacheable data
    //
    in_mmctrl1.e = rand()&0x1;//    : std_logic;                -- enable
    in_mmctrl1.nf = rand()&0x1;//    : std_logic;                -- no fault
    in_mmctrl1.pso = rand()&0x1;//    : std_logic;                -- partial store order
    in_mmctrl1.pagesize = rand()&0x3;// std_logic_vector(1 downto 0);-- page size
    in_mmctrl1.ctx = rand()&MSK32(M_CTX_SZ-1,0);//     : std_logic_vector(M_CTX_SZ-1 downto 0);-- context nr
    in_mmctrl1.ctxp = ((rand()<<17) + rand())&MSK32(MMCTRL_CTXP_SZ-1,0);//    : std_logic_vector(MMCTRL_CTXP_SZ-1 downto 0);  -- context table pointer
    in_mmctrl1.tlbdis = rand()&0x1;// : std_logic;                            -- tlb disabled
    in_mmctrl1.bar = rand()&0x3;// : std_logic_vector(1 downto 0);         -- preplace barrier
    //
    in_twi.walk_op_ur = rand()&0x1;//      : std_logic;
    in_twi.areq_ur = rand()&0x1;//      : std_logic;
    in_twi.tlbmiss = rand()&0x1;//      : std_logic;
    in_twi.data = (rand()<<17) + rand();//             : std_logic_vector(31 downto 0);
    in_twi.adata = (rand()<<17) + rand();//            : std_logic_vector(31 downto 0);
    in_twi.aaddr = (rand()<<17) + rand();//            : std_logic_vector(31 downto 0);
    //
  }

  tst_mmutw.Update( io.inNRst,//     : in  std_logic;
                    io.inClk,//     : in  std_logic;
                    in_mmctrl1,// : in  mmctrl_type1;
                    in_twi,//     : in  mmutw_in_type;
                    ch_two,//     : out mmutw_out_type;
                    in_mcmmo,//   : in  memory_mm_out_type;
                    ch_mcmmi);//   : out memory_mm_in_type

  ptst_mmutw = &tst_mmutw;
  pin_mmctrl1 = &in_mmctrl1;
  pin_twi     = &in_twi;
  pch_two     = &ch_two;
  pin_mcmmo   = &in_mcmmo;
  pch_mcmmi   = &ch_mcmmi;

#endif

  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';

    ResetPutStr();
    
    // inputs:
    pStr = PutToStr(pStr, io.inNRst, 1, "inNRst");
    //
    pStr = PutToStr(pStr, pin_mcmmo->data,32,"in_mcmmo.data");//             : std_logic_vector(31 downto 0); -- memory data
    pStr = PutToStr(pStr, pin_mcmmo->ready,1,"in_mcmmo.ready");//           : std_logic;         -- cycle ready
    pStr = PutToStr(pStr, pin_mcmmo->grant,1,"in_mcmmo.grant");//           : std_logic;         -- 
    pStr = PutToStr(pStr, pin_mcmmo->retry,1,"in_mcmmo.retry");//           : std_logic;         -- 
    pStr = PutToStr(pStr, pin_mcmmo->mexc,1,"in_mcmmo.mexc");//           : std_logic;         -- memory exception
    pStr = PutToStr(pStr, pin_mcmmo->werr,1,"in_mcmmo.werr");//           : std_logic;         -- memory write error
    pStr = PutToStr(pStr, pin_mcmmo->cache,1,"in_mcmmo.cache");//           : std_logic;               -- cacheable data
    //
    pStr = PutToStr(pStr, pin_mmctrl1->e,1,"in_mmctrl1.e");//    : std_logic;                -- enable
    pStr = PutToStr(pStr, pin_mmctrl1->nf,1,"in_mmctrl1.nf");//    : std_logic;                -- no fault
    pStr = PutToStr(pStr, pin_mmctrl1->pso,1,"in_mmctrl1.pso");//    : std_logic;                -- partial store order
    pStr = PutToStr(pStr, pin_mmctrl1->pagesize,2,"in_mmctrl1.pagesize");// std_logic_vector(1 downto 0);-- page size
    pStr = PutToStr(pStr, pin_mmctrl1->ctx,M_CTX_SZ,"in_mmctrl1.ctx");//     : std_logic_vector(M_CTX_SZ-1 downto 0);-- context nr
    pStr = PutToStr(pStr, pin_mmctrl1->ctxp,MMCTRL_CTXP_SZ,"in_mmctrl1.ctxp");//    : std_logic_vector(MMCTRL_CTXP_SZ-1 downto 0);  -- context table pointer
    pStr = PutToStr(pStr, pin_mmctrl1->tlbdis,1,"in_mmctrl1.tlbdis");// : std_logic;                            -- tlb disabled
    pStr = PutToStr(pStr, pin_mmctrl1->bar,2,"in_mmctrl1.bar");// : std_logic_vector(1 downto 0);         -- preplace barrier
    //
    pStr = PutToStr(pStr, pin_twi->walk_op_ur,1,"in_twi.walk_op_ur");//      : std_logic;
    pStr = PutToStr(pStr, pin_twi->areq_ur,1,"in_twi.areq_ur");//      : std_logic;
    pStr = PutToStr(pStr, pin_twi->tlbmiss,1,"in_twi.tlbmiss");//      : std_logic;
    pStr = PutToStr(pStr, pin_twi->data,32,"in_twi.data");//             : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pin_twi->adata,32,"in_twi.adata");//            : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pin_twi->aaddr,32,"in_twi.aaddr");//            : std_logic_vector(31 downto 0);


    //Outputs:
    pStr = PutToStr(pStr, pch_two->finish,1,"ch_two.finish");//     : std_logic;
    pStr = PutToStr(pStr, pch_two->data,32,"ch_two.data");//             : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pch_two->addr,32,"ch_two.addr");//             : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pch_two->lvl,2,"ch_two.lvl");//     : std_logic_vector(1 downto 0);
    pStr = PutToStr(pStr, pch_two->fault_mexc,1,"ch_two.fault_mexc");//     : std_logic;
    pStr = PutToStr(pStr, pch_two->fault_trans,1,"ch_two.fault_trans");//     : std_logic;
    pStr = PutToStr(pStr, pch_two->fault_inv,1,"ch_two.fault_inv");//     : std_logic;
    pStr = PutToStr(pStr, pch_two->fault_lvl,2,"ch_two.fault_lvl");//     : std_logic_vector(1 downto 0);
    //
    pStr = PutToStr(pStr, pch_mcmmi->address,32,"ch_mcmmi.address");//          : std_logic_vector(31 downto 0); 
    pStr = PutToStr(pStr, pch_mcmmi->data,32,"ch_mcmmi.data");//             : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pch_mcmmi->size,2,"ch_mcmmi.size");//          : std_logic_vector(1 downto 0);
    pStr = PutToStr(pStr, pch_mcmmi->burst,1,"ch_mcmmi.burst");//          : std_logic;
    pStr = PutToStr(pStr, pch_mcmmi->read,1,"ch_mcmmi.read");//          : std_logic;
    pStr = PutToStr(pStr, pch_mcmmi->req,1,"ch_mcmmi.req");//          : std_logic;
    pStr = PutToStr(pStr, pch_mcmmi->lock,1,"ch_mcmmi.lock");//          : std_logic;


    PrintIndexStr();

    *posBench[TB_mmutw] << chStr << "\n";
  }
  
#ifdef DBG_mmutw
  tst_mmutw.ClkUpdate();
#endif
}


