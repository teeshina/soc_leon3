#include "lheaders.h"

extern leon3mp  topLeon3mp;

extern void ResetPutStr();
extern void PrintIndexStr();

#ifdef DBG_mmu

//****************************************************************************
void dbg::mmu_tb(SystemOnChipIO &io)
{
  if(io.inClk.eClock_z==SClock::CLK_POSEDGE)
  {
    //
    in_mmudci.trans_op = rand()&0x1;//std_logic; 
    in_mmudci.transdata.data = rand()&0x1;//             : std_logic_vector(31 downto 0);
    in_mmudci.transdata.su = rand()&0x1;//            : std_logic;
    in_mmudci.transdata.read = rand()&0x1;//            : std_logic;
    in_mmudci.transdata.isid = (mmu_idcache)(rand()&0x1);//             : mmu_idcache;
    in_mmudci.transdata.wb_data = rand()&0x1;//          : std_logic_vector(31 downto 0);
    in_mmudci.flush_op = rand()&0x1;//std_logic;
    in_mmudci.diag_op = rand()&0x1;//std_logic;
    in_mmudci.wb_op = rand()&0x1;//std_logic;
    in_mmudci.fsread = rand()&0x1;//std_logic;
    in_mmudci.mmctrl1.e = rand()&0x1;//    : std_logic;                -- enable
    in_mmudci.mmctrl1.nf = rand()&0x1;//    : std_logic;                -- no fault
    in_mmudci.mmctrl1.pso = rand()&0x1;//    : std_logic;                -- partial store order
    in_mmudci.mmctrl1.pagesize = rand()&0x3;// std_logic_vector(1 downto 0);-- page size
    in_mmudci.mmctrl1.ctx = rand()&MSK32(M_CTX_SZ-1,0);//     : std_logic_vector(M_CTX_SZ-1 downto 0);-- context nr
    in_mmudci.mmctrl1.ctxp = ((rand()<<17) + rand())&MSK32(MMCTRL_CTXP_SZ-1,0);//    : std_logic_vector(MMCTRL_CTXP_SZ-1 downto 0);  -- context table pointer
    in_mmudci.mmctrl1.tlbdis = rand()&0x1;// : std_logic;                            -- tlb disabled
    in_mmudci.mmctrl1.bar = rand()&0x3;// : std_logic_vector(1 downto 0);         -- preplace barrier
    //
    in_mmuici.trans_op = rand()&0x1;//        : std_logic; 
    in_mmuici.transdata.data = (rand()<<17) + rand();//             : std_logic_vector(31 downto 0);
    in_mmuici.transdata.su = rand()&0x1;//            : std_logic;
    in_mmuici.transdata.read = rand()&0x1;//            : std_logic;
    in_mmuici.transdata.isid = (mmu_idcache)(rand()&0x1);//             : mmu_idcache;
    in_mmuici.transdata.wb_data = (rand()<<17) + rand();//          : std_logic_vector(31 downto 0);
    //
    in_mcmmo.data = (rand()<<17) + rand();//             : std_logic_vector(31 downto 0); -- memory data
    in_mcmmo.ready = rand()&0x1;//           : std_logic;         -- cycle ready
    in_mcmmo.grant = rand()&0x1;//           : std_logic;         -- 
    in_mcmmo.retry = rand()&0x1;//           : std_logic;         -- 
    in_mcmmo.mexc = rand()&0x1;//           : std_logic;         -- memory exception
    in_mcmmo.werr = rand()&0x1;//           : std_logic;         -- memory write error
    in_mcmmo.cache = rand()&0x1;//           : std_logic;               -- cacheable data
  
  }

  // 
  tst_mmu.Update( io.inNRst,
                  io.inClk,
                  in_mmudci,
                  ch_mmudco,
                  in_mmuici,// : in  mmuic_in_type;
                  ch_mmuico,
                  in_mcmmo,
                  ch_mcmmi);


  // output file writting:
  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';

    ResetPutStr();
  
    // inputs:
    pStr = PutToStr(pStr, io.inNRst, 1, "inNRst");
    //
    pStr = PutToStr(pStr, in_mmudci.trans_op,1,"in_mmudci.trans_op");//std_logic; 
    pStr = PutToStr(pStr, in_mmudci.transdata.data,32,"in_mmudci.transdata.data");//             : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, in_mmudci.transdata.su,1,"in_mmudci.transdata.su");//            : std_logic;
    pStr = PutToStr(pStr, in_mmudci.transdata.read,1,"in_mmudci.transdata.read");//            : std_logic;
    pStr = PutToStr(pStr, (uint32)in_mmudci.transdata.isid,1,"in_mmudci_transdata_isid");//             : mmu_idcache;
    pStr = PutToStr(pStr, in_mmudci.transdata.wb_data,32,"in_mmudci.transdata.wb_data");//          : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, in_mmudci.flush_op,1,"in_mmudci.flush_op");//std_logic;
    pStr = PutToStr(pStr, in_mmudci.diag_op,1,"in_mmudci.diag_op");//std_logic;
    pStr = PutToStr(pStr, in_mmudci.wb_op,1,"in_mmudci.wb_op");//std_logic;
    pStr = PutToStr(pStr, in_mmudci.fsread,1,"in_mmudci.fsread");;//std_logic;
    pStr = PutToStr(pStr, in_mmudci.mmctrl1.e,1,"in_mmudci.mmctrl1.e");//    : std_logic;                -- enable
    pStr = PutToStr(pStr, in_mmudci.mmctrl1.nf,1,"in_mmudci.mmctrl1.nf");//    : std_logic;                -- no fault
    pStr = PutToStr(pStr, in_mmudci.mmctrl1.pso,1,"in_mmudci.mmctrl1.pso");//    : std_logic;                -- partial store order
    pStr = PutToStr(pStr, in_mmudci.mmctrl1.pagesize,2,"in_mmudci.mmctrl1.pagesize");// std_logic_vector(1 downto 0);-- page size
    pStr = PutToStr(pStr, in_mmudci.mmctrl1.ctx,M_CTX_SZ,"in_mmudci.mmctrl1.ctx");//     : std_logic_vector(M_CTX_SZ-1 downto 0);-- context nr
    pStr = PutToStr(pStr, in_mmudci.mmctrl1.ctxp,MMCTRL_CTXP_SZ,"in_mmudci.mmctrl1.ctxp");//    : std_logic_vector(MMCTRL_CTXP_SZ-1 downto 0);  -- context table pointer
    pStr = PutToStr(pStr, in_mmudci.mmctrl1.tlbdis,1,"in_mmudci.mmctrl1.tlbdis");// : std_logic;                            -- tlb disabled
    pStr = PutToStr(pStr, in_mmudci.mmctrl1.bar,2,"in_mmudci.mmctrl1.bar");// : std_logic_vector(1 downto 0);         -- preplace barrier
    //
    pStr = PutToStr(pStr, in_mmuici.trans_op,1,"in_mmuici.trans_op");//        : std_logic; 
    pStr = PutToStr(pStr, in_mmuici.transdata.data,32,"in_mmuici.transdata.data");//             : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, in_mmuici.transdata.su,1,"in_mmuici.transdata.su");//            : std_logic;
    pStr = PutToStr(pStr, in_mmuici.transdata.read,1,"in_mmuici.transdata.read");//            : std_logic;
    pStr = PutToStr(pStr, (uint32)in_mmuici.transdata.isid,1,"in_mmuici_transdata_isid");//             : mmu_idcache;
    pStr = PutToStr(pStr, in_mmuici.transdata.wb_data,32,"in_mmuici.transdata.wb_data");//          : std_logic_vector(31 downto 0);
    //
    pStr = PutToStr(pStr, in_mcmmo.data,32,"in_mcmmo.data");//             : std_logic_vector(31 downto 0); -- memory data
    pStr = PutToStr(pStr, in_mcmmo.ready,1,"in_mcmmo.ready");//           : std_logic;         -- cycle ready
    pStr = PutToStr(pStr, in_mcmmo.grant,1,"in_mcmmo.grant");//           : std_logic;         -- 
    pStr = PutToStr(pStr, in_mcmmo.retry,1,"in_mcmmo.retry");//           : std_logic;         -- 
    pStr = PutToStr(pStr, in_mcmmo.mexc,1,"in_mcmmo.mexc");//           : std_logic;         -- memory exception
    pStr = PutToStr(pStr, in_mcmmo.werr,1,"in_mcmmo.werr");//           : std_logic;         -- memory write error
    pStr = PutToStr(pStr, in_mcmmo.cache,1,"in_mcmmo.cache");//           : std_logic;               -- cacheable data



    // Outputs:
    pStr = PutToStr(pStr, ch_mmudco.grant,1,"ch_mmudco.grant");//         : std_logic;
    pStr = PutToStr(pStr, ch_mmudco.transdata.finish,1,"ch_mmudco.transdata.finish");;//          : std_logic;
    pStr = PutToStr(pStr, ch_mmudco.transdata.data,32,"ch_mmudco.transdata.data");//          : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, ch_mmudco.transdata.cache,1,"ch_mmudco.transdata.cache");//          : std_logic;
    pStr = PutToStr(pStr, ch_mmudco.transdata.accexc,1,"ch_mmudco.transdata.accexc");//          : std_logic;
    pStr = PutToStr(pStr, ch_mmudco.mmctrl2.fs.ow,1,"ch_mmudco.mmctrl2.fs.ow");//std_logic;          
    pStr = PutToStr(pStr, ch_mmudco.mmctrl2.fs.fav,1,"ch_mmudco.mmctrl2.fs.fav");
    pStr = PutToStr(pStr, ch_mmudco.mmctrl2.fs.ft,3,"ch_mmudco.mmctrl2.fs.ft");//std_logic_vector(2 downto 0);           -- fault type
    pStr = PutToStr(pStr, ch_mmudco.mmctrl2.fs.at_ls,1,"ch_mmudco.mmctrl2.fs.at_ls");//std_logic;                              -- access type, load/store
    pStr = PutToStr(pStr, ch_mmudco.mmctrl2.fs.at_id,1,"ch_mmudco.mmctrl2.fs.at_id");//std_logic;                              -- access type, i/dcache
    pStr = PutToStr(pStr, ch_mmudco.mmctrl2.fs.at_su,1,"ch_mmudco.mmctrl2.fs.at_su");//std_logic;                              -- access type, su/user
    pStr = PutToStr(pStr, ch_mmudco.mmctrl2.fs.l,2,"ch_mmudco.mmctrl2.fs.l");//std_logic_vector(1 downto 0);           -- level 
    pStr = PutToStr(pStr, ch_mmudco.mmctrl2.fs.ebe,8,"ch_mmudco.mmctrl2.fs.ebe");//std_logic_vector(7 downto 0);            
    pStr = PutToStr(pStr, ch_mmudco.mmctrl2.valid,1,"ch_mmudco.mmctrl2.valid");// std_logic;
    pStr = PutToStr(pStr, ch_mmudco.mmctrl2.fa,VA_I_SZ,"ch_mmudco.mmctrl2.fa");//    : std_logic_vector(VA_I_SZ-1 downto 0);   -- fault address register
    pStr = PutToStr(pStr, ch_mmudco.wbtransdata.finish,1,"ch_mmudco.wbtransdata.finish");//          : std_logic;
    pStr = PutToStr(pStr, ch_mmudco.wbtransdata.data,32,"ch_mmudco.wbtransdata.data");//          : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, ch_mmudco.wbtransdata.cache,1,"ch_mmudco.wbtransdata.cache");//          : std_logic;
    pStr = PutToStr(pStr, ch_mmudco.wbtransdata.accexc,1,"ch_mmudco.wbtransdata.accexc");//          : std_logic;
    pStr = PutToStr(pStr, ch_mmudco.tlbmiss,1,"ch_mmudco.tlbmiss");//         : std_logic;
    //
    pStr = PutToStr(pStr, ch_mmuico.transdata.data,32,"ch_mmuico.transdata.data");//          : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, ch_mmuico.grant,1,"ch_mmuico.grant");//           : std_logic;
    pStr = PutToStr(pStr, ch_mmuico.transdata.finish,1,"ch_mmuico.transdata.finish");//          : std_logic;
    pStr = PutToStr(pStr, ch_mmuico.transdata.cache,1,"ch_mmuico.transdata.cache");//          : std_logic;
    pStr = PutToStr(pStr, ch_mmuico.transdata.accexc,1,"ch_mmuico.transdata.accexc");//          : std_logic;
    pStr = PutToStr(pStr, ch_mmuico.tlbmiss,1,"ch_mmuico.tlbmiss");//         : std_logic;
    //
    pStr = PutToStr(pStr, ch_mcmmi.address,32,"ch_mcmmi.address");//          : std_logic_vector(31 downto 0); 
    pStr = PutToStr(pStr, ch_mcmmi.data,32,"ch_mcmmi.data");//             : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, ch_mcmmi.size,2,"ch_mcmmi.size");//          : std_logic_vector(1 downto 0);
    pStr = PutToStr(pStr, ch_mcmmi.burst,1,"ch_mcmmi.burst");//          : std_logic;
    pStr = PutToStr(pStr, ch_mcmmi.read,1,"ch_mcmmi.read");//          : std_logic;
    pStr = PutToStr(pStr, ch_mcmmi.req,1,"ch_mcmmi.req");//          : std_logic;
    pStr = PutToStr(pStr, ch_mcmmi.lock,1,"ch_mcmmi.lock");//          : std_logic;

  
    PrintIndexStr();

    *posBench[TB_mmu] << chStr << "\n";
  }

  // Clock update:
  tst_mmu.ClkUpdate();
}
#endif

