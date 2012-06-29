#include "ldbg.h"
#include "leon3_ml605\leon3mp.h"
extern leon3mp topLeon3mp;

//#define DBG_mmu_icache


#ifdef DBG_mmu_icache
  icache_in_type      ici;
  icache_out_type     ico;
  dcache_in_type      dci;
  dcache_out_type     dco;
  memory_ic_in_type   mcii;
  memory_ic_out_type  mcio;
  mmudc_in_type       mmudci;
  mmuic_in_type       mmuici;
  mmuic_out_type      mmuico;
  icram_in_type       icramin;
  icram_out_type      icramo;
  uint32 fpuholdn;

  mmu_icache tst_mmu_icache;
#endif


//****************************************************************************
void dbg::mmu_icache_tb(SystemOnChipIO &io)
{
  // WARNING! different defines with the same name in mmu_icache and mmu_dcache
  #define ITAG_LOW       (IOFFSET_BITS + ILINE_BITS + 2)//: integer := IOFFSET_BITS + ILINE_BITS + 2;

  mmu_icache *ptst_mmu_icache = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->clICache;  
  
  uint32 *pin_fpuholdn          = &topLeon3mp.pclLeon3s[0]->pclProc3->pholdn;
  icache_in_type *pin_ici       = &topLeon3mp.pclLeon3s[0]->pclProc3->ici;
  icache_out_type *pch_ico      = &topLeon3mp.pclLeon3s[0]->pclProc3->ico;
  dcache_in_type *pin_dci       = &topLeon3mp.pclLeon3s[0]->pclProc3->dci;
  dcache_out_type *pin_dco      = &topLeon3mp.pclLeon3s[0]->pclProc3->dco;
  memory_ic_in_type *pch_mcii   = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->mcii;
  memory_ic_out_type *pin_mcio  = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->mcio;
  icram_in_type *pch_icrami     = &topLeon3mp.pclLeon3s[0]->crami.icramin;
  icram_out_type *pin_icramo    = &topLeon3mp.pclLeon3s[0]->cramo.icramo;
  mmudc_in_type *pin_mmudci     = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->mmudci;
  mmuic_in_type *pch_mmuici     = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->mmuici;
  mmuic_out_type *pin_mmuico    = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->mmuico;
#ifdef DBG_mmu_icache
  if(io.inClk.eClock_z==SClock::CLK_POSEDGE)
  {
    fpuholdn = rand()&0x1;//1;

    uint32 tmp;
    //icache_in_type *pIci = &ici;
    ici.rpc = (rand()<<17) + rand();//              : std_logic_vector(31 downto 0); -- raw address (npc)
    ici.fpc = (rand()<<17) + rand();//              : std_logic_vector(31 downto 0); -- latched address (fpc)
    ici.dpc = (rand()<<17) + rand();//              : std_logic_vector(31 downto 0); -- latched address (dpc)
    tmp = rand();
    ici.rbranch = tmp&0x1;//          : std_ulogic;      -- Instruction branch
    ici.fbranch = (tmp>>1)&0x1;//          : std_ulogic;      -- Instruction branch
    ici.inull   = (tmp>>2)&0x1;//            : std_ulogic;      -- instruction nullify
    ici.su      = (tmp>>3)&0x1;//               : std_ulogic;      -- super-user
    ici.flush   = (tmp>>4)&0x1;//            : std_ulogic;      -- flush icache
    ici.flushl  = (tmp>>5)&0x1;//           : std_ulogic;                        -- flush line
    ici.fline   = ((rand()<<17) + rand())&0xFFFFFFFC;//            : std_logic_vector(31 downto 3);     -- flush line offset
    ici.pnull   = (tmp>>6)&0x1;//            : std_ulogic;

    //dcache_in_type *pDci = &dci;
    dci.asi = rand()&0xFF;//              : std_logic_vector(7 downto 0); 
    dci.maddress = (rand()<<17) + rand();//         : std_logic_vector(31 downto 0); 
    dci.eaddress = (rand()<<17) + rand();//         : std_logic_vector(31 downto 0); 
    dci.edata = (rand()<<17) + rand();//            : std_logic_vector(31 downto 0); 
    tmp = rand();
    dci.size = tmp&0x3;//             : std_logic_vector(1 downto 0);
    dci.enaddr = (tmp>>2)&0x1;//           : std_ulogic;
    dci.eenaddr = (tmp>>3)&0x1;//          : std_ulogic;
    dci.nullify = (tmp>>4)&0x1;//          : std_ulogic;
    dci.lock = (tmp>>5)&0x1;//             : std_ulogic;
    dci.read = (tmp>>6)&0x1;//             : std_ulogic;
    dci.write = (tmp>>7)&0x1;//            : std_ulogic;
    dci.flush = (tmp>>8)&0x1;//            : std_ulogic;
    dci.flushl = (tmp>>9)&0x1;//           : std_ulogic;                        -- flush line  
    dci.dsuen = (tmp>>10)&0x1;//            : std_ulogic;
    dci.msu = (tmp>>11)&0x1;//              : std_ulogic;                   -- memory stage supervisor
    dci.esu = (tmp>>12)&0x1;//              : std_ulogic;                   -- execution stage supervisor
    dci.intack = (tmp>>13)&0x1;//           : std_ulogic;

    //dcache_out_type *pDco = &dco;  
    dco.data.arr[0] = (rand()<<17) + rand();//             : cdatatype;
    dco.data.arr[1] = (rand()<<17) + rand();
    dco.data.arr[2] = (rand()<<17) + rand();
    dco.data.arr[3] = (rand()<<17) + rand();
    tmp = rand();
    dco.set = tmp&0x3;//              : std_logic_vector(1 downto 0);
    dco.mexc = (tmp>>2)&0x1;//             : std_ulogic;
    dco.hold = (tmp>>3)&0x1;//             : std_ulogic;
    dco.mds = (tmp>>4)&0x1;//              : std_ulogic;
    dco.werr = (tmp>>5)&0x1;//             : std_ulogic;	
    dco.icdiag.addr = (rand()<<17) + rand();//             : std_logic_vector(31 downto 0); -- memory stage address
    dco.icdiag.enable = (tmp>>6)&0x1;//           : std_ulogic;
    dco.icdiag.read = (tmp>>7)&0x1;//             : std_ulogic;
    dco.icdiag.tag = (tmp>>8)&0x1;//              : std_ulogic;
    dco.icdiag.ctx = (tmp>>9)&0x1;//              : std_ulogic;
    dco.icdiag.flush = (tmp>>10)&0x1;//            : std_ulogic;
    dco.icdiag.ilramen = (tmp>>11)&0x1;//          : std_ulogic;
    dco.icdiag.cctrl.burst = (tmp>>12)&0x1;//  : std_ulogic;				-- icache burst enable
    dco.icdiag.cctrl.dfrz = (tmp>>13)&0x1;//   : std_ulogic;				-- dcache freeze enable
    dco.icdiag.cctrl.ifrz = (tmp>>14)&0x1;//   : std_ulogic;				-- icache freeze enable
    tmp = rand();
    dco.icdiag.cctrl.dsnoop = (tmp>>0)&0x1;// : std_ulogic;				-- data cache snooping
    dco.icdiag.cctrl.dcs = (tmp>>1)&0x3;//    : std_logic_vector(1 downto 0);	-- dcache state
    dco.icdiag.cctrl.ics = (tmp>>3)&0x3;//    : std_logic_vector(1 downto 0);	-- icache state
    dco.icdiag.pflush = (tmp>>4)&0x1;//           : std_ulogic;
    dco.icdiag.pflushaddr = tmp&MSK32(VA_I_U,VA_I_D);//       : std_logic_vector(VA_I_U downto VA_I_D); 
    tmp = rand();
    dco.icdiag.pflushtyp = (tmp>>0)&0x1;//        : std_ulogic;
    //dco.icdiag.ilock = (tmp>>1)&0xf;//            : std_logic_vector(0 to 3); 
    dco.icdiag.scanen = 0;//           : std_ulogic;
    dco.cache = (tmp>>5)&0x1;//            : std_ulogic;
    dco.idle = 0;//             : std_ulogic;			-- idle mode
    dco.scanen = 0;//           : std_ulogic;
    dco.testen = 0;//           : std_ulogic;

    //memory_ic_out_type *pMco = &mcio;
    mcio.data = (rand()<<17) + rand();//             : std_logic_vector(31 downto 0); // memory data
    tmp = rand();
    mcio.ready = (tmp>>0)&0x1;//            : std_ulogic;          // cycle ready
    mcio.grant = (tmp>>1)&0x1;//            : std_ulogic;          // 
    mcio.retry = (tmp>>2)&0x1;//            : std_ulogic;          // 
    mcio.mexc = (tmp>>3)&0x1;//             : std_ulogic;          // memory exception
    mcio.cache = (tmp>>4)&0x1;//            : std_ulogic;    // cacheable data
    mcio.par = (tmp>>5)&0xf;//              : std_logic_vector(3 downto 0);  // parity
    mcio.scanen = 0;//           : std_ulogic;

    //icram_out_type *pRco = &icramo;
    icramo.tag.arr[0] = (rand()<<17) + rand();//     : cdatatype;
    icramo.tag.arr[1] = (rand()<<17) + rand();
    icramo.tag.arr[2] = (rand()<<17) + rand();
    icramo.tag.arr[3] = (rand()<<17) + rand();
    icramo.data.arr[0] = (rand()<<17) + rand();//      : cdatatype;
    icramo.data.arr[1] = (rand()<<17) + rand();
    icramo.data.arr[2] = (rand()<<17) + rand();
    icramo.data.arr[3] = (rand()<<17) + rand();
    icramo.ctx.arr[0] = (rand()<<17) + rand();//           : ctxdatatype;
    icramo.ctx.arr[1] = (rand()<<17) + rand();
    icramo.ctx.arr[2] = (rand()<<17) + rand();
    icramo.ctx.arr[3] = (rand()<<17) + rand();
    icramo.tpar.arr[0] = (rand()<<17) + rand();//          : cpartype;
    icramo.tpar.arr[1] = (rand()<<17) + rand();
    icramo.tpar.arr[2] = (rand()<<17) + rand();
    icramo.tpar.arr[3] = (rand()<<17) + rand();
    icramo.dpar.arr[0] = (rand()<<17) + rand();//          : cpartype;
    icramo.dpar.arr[1] = (rand()<<17) + rand();
    icramo.dpar.arr[2] = (rand()<<17) + rand();
    icramo.dpar.arr[3] = (rand()<<17) + rand();

    //mmudc_in_type *pMdci = &mmudci;
    tmp = rand();
    mmudci.trans_op         = (tmp>>0)&0x1;//std_logic; 
    mmudci.transdata.data   = (rand()<<17) + rand();//             : std_logic_vector(31 downto 0);
    mmudci.transdata.su     = (tmp>>1)&0x1;//            : std_logic;
    mmudci.transdata.read   = (tmp>>2)&0x1;//            : std_logic;
    mmudci.transdata.isid   = (mmu_idcache)((tmp>>3)&0x1);//id_icache;//             : mmu_idcache;
    mmudci.transdata.wb_data = (rand()<<17) + rand();//          : std_logic_vector(31 downto 0);
    mmudci.flush_op         = (tmp>>4)&0x1;//std_logic;
    mmudci.diag_op          = (tmp>>5)&0x1;//std_logic;
    mmudci.wb_op            = (tmp>>6)&0x1;//std_logic;
    mmudci.fsread           = (tmp>>7)&0x1;//std_logic;
    mmudci.mmctrl1.e        = (tmp>>8)&0x1;//    : std_logic;                -- enable
    mmudci.mmctrl1.nf       = (tmp>>9)&0x1;//    : std_logic;                -- no fault
    mmudci.mmctrl1.pso      = (tmp>>10)&0x1;//    : std_logic;                -- partial store order
    mmudci.mmctrl1.pagesize = (tmp>>11)&0x3;// std_logic_vector(1 downto 0);-- page size
    mmudci.mmctrl1.ctx      = rand()&MSK32(M_CTX_SZ-1,0);//     : std_logic_vector(M_CTX_SZ-1 downto 0);-- context nr
    mmudci.mmctrl1.ctxp     = rand()&MSK32(MMCTRL_CTXP_SZ-1,0);//    : std_logic_vector(MMCTRL_CTXP_SZ-1 downto 0);  -- context table pointer
    tmp = rand();
    mmudci.mmctrl1.tlbdis   = tmp&0x1;// : std_logic;                            -- tlb disabled
    mmudci.mmctrl1.bar      = (tmp>>1)&0x3;// : std_logic_vector(1 downto 0);         -- preplace barrier

    //mmuic_out_type
    mmuico.transdata.data   = (rand()<<17) + rand();//          : std_logic_vector(31 downto 0);
    tmp = rand();
    mmuico.grant            = tmp&0x1;//           : std_logic;
    mmuico.transdata.finish = (tmp>>1)&0x1;//          : std_logic;
    mmuico.transdata.cache  = (tmp>>2)&0x1;//          : std_logic;
    mmuico.transdata.accexc = (tmp>>3)&0x1;//          : std_logic;
    mmuico.tlbmiss          = (tmp>>4)&0x1;//         : std_logic;
  }

  tst_mmu_icache.Update(io.inNRst, io.inClk, ici, ico, dci, dco, mcii, mcio, 
       icramin, icramo, fpuholdn, mmudci, mmuici, mmuico);

  ptst_mmu_icache = &tst_mmu_icache;
  pin_fpuholdn = &fpuholdn;
  pin_ici = &ici;
  pch_ico = &ico;
  pin_dci = &dci;
  pin_dco = &dco;
  pch_mcii = &mcii;
  pin_mcio = &mcio;
  pch_icrami = &icramin;
  pin_icramo = &icramo;
  pin_mmudci = &mmudci;
  pch_mmuici = &mmuici;
  pin_mmuico = &mmuico;
#endif

  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';

    ResetPutStr();
  
    // inputs:
    pStr = PutToStr(pStr, io.inNRst, 1, "inNRst");
    pStr = PutToStr(pStr, *pin_fpuholdn, 1, "fpuholdn");
    pStr = PutToStr(pStr, pin_ici->rpc,32,"ici.rpc");//              : std_logic_vector(31 downto 0); -- raw address (npc)
    pStr = PutToStr(pStr, pin_ici->fpc,32,"ici.fpc");//              : std_logic_vector(31 downto 0); -- latched address (fpc)
    pStr = PutToStr(pStr, pin_ici->dpc,32,"ici.dpc");//              : std_logic_vector(31 downto 0); -- latched address (dpc)
    pStr = PutToStr(pStr, pin_ici->rbranch,1,"ici.rbranch");//          : std_ulogic;      -- Instruction branch
    pStr = PutToStr(pStr, pin_ici->fbranch,1,"ici.fbranch");//          : std_ulogic;      -- Instruction branch
    pStr = PutToStr(pStr, pin_ici->inull,1,"ici.inull");//            : std_ulogic;      -- instruction nullify
    pStr = PutToStr(pStr, pin_ici->su,1,"ici.su");//               : std_ulogic;      -- super-user
    pStr = PutToStr(pStr, pin_ici->flush,1,"ici.flush");//            : std_ulogic;      -- flush icache
    pStr = PutToStr(pStr, pin_ici->flushl,1,"ici.flushl");//           : std_ulogic;                        -- flush line
    pStr = PutToStr(pStr, (pin_ici->fline>>3),29,"ici.fline");//            : std_logic_vector(31 downto 3);     -- flush line offset
    pStr = PutToStr(pStr, pin_ici->pnull,1,"ici.pnull");//            : std_ulogic;
    //dcache_in_type
    pStr = PutToStr(pStr, pin_dci->asi,8,"dci.asi");//              : std_logic_vector(7 downto 0); 
    pStr = PutToStr(pStr, pin_dci->maddress,32,"dci.maddress");//         : std_logic_vector(31 downto 0); 
    pStr = PutToStr(pStr, pin_dci->eaddress,32,"dci.eaddress");//         : std_logic_vector(31 downto 0); 
    pStr = PutToStr(pStr, pin_dci->edata,32,"dci.edata");//            : std_logic_vector(31 downto 0); 
    pStr = PutToStr(pStr, pin_dci->size,2,"dci.size");//             : std_logic_vector(1 downto 0);
    pStr = PutToStr(pStr, pin_dci->enaddr,1,"dci.enaddr");//           : std_ulogic;
    pStr = PutToStr(pStr, pin_dci->eenaddr,1,"dci.eenaddr");//          : std_ulogic;
    pStr = PutToStr(pStr, pin_dci->nullify,1,"dci.nullify");//          : std_ulogic;
    pStr = PutToStr(pStr, pin_dci->lock,1,"dci.lock");//             : std_ulogic;
    pStr = PutToStr(pStr, pin_dci->read,1,"dci.read");//             : std_ulogic;
    pStr = PutToStr(pStr, pin_dci->write,1,"dci.write");//            : std_ulogic;
    pStr = PutToStr(pStr, pin_dci->flush,1,"dci.flush");//            : std_ulogic;
    pStr = PutToStr(pStr, pin_dci->flushl,1,"dci.flushl");//           : std_ulogic;                        -- flush line  
    pStr = PutToStr(pStr, pin_dci->dsuen,1,"dci.dsuen");//            : std_ulogic;
    pStr = PutToStr(pStr, pin_dci->msu,1,"dci.msu");//              : std_ulogic;                   -- memory stage supervisor
    pStr = PutToStr(pStr, pin_dci->esu,1,"dci.esu");//              : std_ulogic;                   -- execution stage supervisor
    pStr = PutToStr(pStr, pin_dci->intack,1,"dci.intack");//           : std_ulogic;
    //dcache_out_type  
    pStr = PutToStr(pStr, pin_dco->data.arr[0],IDBITS,"dco.data(0)");//             : cdatatype;
    pStr = PutToStr(pStr, pin_dco->data.arr[1],IDBITS,"dco.data(1)");
    pStr = PutToStr(pStr, pin_dco->data.arr[2],IDBITS,"dco.data(2)");
    pStr = PutToStr(pStr, pin_dco->data.arr[3],IDBITS,"dco.data(3)");
    pStr = PutToStr(pStr, pin_dco->set,2,"dco.set");//              : std_logic_vector(1 downto 0);
    pStr = PutToStr(pStr, pin_dco->mexc,1,"dco.mexc");//             : std_ulogic;
    pStr = PutToStr(pStr, pin_dco->hold,1,"dco.hold");//             : std_ulogic;
    pStr = PutToStr(pStr, pin_dco->mds,1,"dco.mds");//              : std_ulogic;
    pStr = PutToStr(pStr, pin_dco->werr,1,"dco.werr");//             : std_ulogic;	
    pStr = PutToStr(pStr, pin_dco->icdiag.addr,32,"dco.icdiag.addr");//             : std_logic_vector(31 downto 0); -- memory stage address
    pStr = PutToStr(pStr, pin_dco->icdiag.enable,1,"dco.icdiag.enable");//           : std_ulogic;
    pStr = PutToStr(pStr, pin_dco->icdiag.read,1,"dco.icdiag.read");//             : std_ulogic;
    pStr = PutToStr(pStr, pin_dco->icdiag.tag,1,"dco.icdiag.tag");//              : std_ulogic;
    pStr = PutToStr(pStr, pin_dco->icdiag.ctx,1,"dco.icdiag.ctx");//              : std_ulogic;
    pStr = PutToStr(pStr, pin_dco->icdiag.flush,1,"dco.icdiag.flush");//            : std_ulogic;
    pStr = PutToStr(pStr, pin_dco->icdiag.ilramen,1,"dco.icdiag.ilramen");//          : std_ulogic;
    pStr = PutToStr(pStr, pin_dco->icdiag.cctrl.burst,1,"dco.icdiag.cctrl.burst");//  : std_ulogic;				-- icache burst enable
    pStr = PutToStr(pStr, pin_dco->icdiag.cctrl.dfrz,1,"dco.icdiag.cctrl.dfrz");//   : std_ulogic;				-- dcache freeze enable
    pStr = PutToStr(pStr, pin_dco->icdiag.cctrl.ifrz,1,"dco.icdiag.cctrl.ifrz");//   : std_ulogic;				-- icache freeze enable
    pStr = PutToStr(pStr, pin_dco->icdiag.cctrl.dsnoop,1,"dco.icdiag.cctrl.dsnoop");// : std_ulogic;				-- data cache snooping
    pStr = PutToStr(pStr, pin_dco->icdiag.cctrl.dcs,2,"dco.icdiag.cctrl.dcs");//    : std_logic_vector(1 downto 0);	-- dcache state
    pStr = PutToStr(pStr, pin_dco->icdiag.cctrl.ics,2,"dco.icdiag.cctrl.ics");//    : std_logic_vector(1 downto 0);	-- icache state
    pStr = PutToStr(pStr, pin_dco->icdiag.pflush,1,"dco.icdiag.pflush");//           : std_ulogic;
    pStr = PutToStr(pStr, pin_dco->icdiag.pflushaddr,VA_I_U-VA_I_D+1,"dco.icdiag.pflushaddr");//       : std_logic_vector(VA_I_U downto VA_I_D); 
    pStr = PutToStr(pStr, pin_dco->icdiag.pflushtyp,1,"dco.icdiag.pflushtyp");//        : std_ulogic;
    pStr = PutToStr(pStr, pin_dco->icdiag.scanen,1,"dco.icdiag.scanen");//           : std_ulogic;
    pStr = PutToStr(pStr, pin_dco->cache,1,"dco.cache");//            : std_ulogic;
    pStr = PutToStr(pStr, pin_dco->idle,1,"dco.idle");//             : std_ulogic;			-- idle mode
    pStr = PutToStr(pStr, pin_dco->scanen,1,"dco.scanen");//           : std_ulogic;
    pStr = PutToStr(pStr, pin_dco->testen,1,"dco.testen");//           : std_ulogic;
    //memory_ic_out_type
    pStr = PutToStr(pStr, pin_mcio->data,32,"mcio.data");//             : std_logic_vector(31 downto 0); // memory data
    pStr = PutToStr(pStr, pin_mcio->ready,1,"mcio.ready");//            : std_ulogic;          // cycle ready
    pStr = PutToStr(pStr, pin_mcio->grant,1,"mcio.grant");//            : std_ulogic;          // 
    pStr = PutToStr(pStr, pin_mcio->retry,1,"mcio.retry");//            : std_ulogic;          // 
    pStr = PutToStr(pStr, pin_mcio->mexc,1,"mcio.mexc");//             : std_ulogic;          // memory exception
    pStr = PutToStr(pStr, pin_mcio->cache,1,"mcio.cache");//            : std_ulogic;    // cacheable data
    pStr = PutToStr(pStr, pin_mcio->par,4,"mcio.par");//              : std_logic_vector(3 downto 0);  // parity
    pStr = PutToStr(pStr, pin_mcio->scanen,1,"mcio.scanen");//           : std_ulogic;
    //icram_out_type 
    pStr = PutToStr(pStr, pin_icramo->tag.arr[0],32,"icramo.tag(0)");//     : cdatatype;
    pStr = PutToStr(pStr, pin_icramo->tag.arr[1],32,"icramo.tag(1)");
    pStr = PutToStr(pStr, pin_icramo->tag.arr[2],32,"icramo.tag(2)");
    pStr = PutToStr(pStr, pin_icramo->tag.arr[3],32,"icramo.tag(3)");
    pStr = PutToStr(pStr, pin_icramo->data.arr[0],32,"icramo.data(0)");//      : cdatatype;
    pStr = PutToStr(pStr, pin_icramo->data.arr[1],32,"icramo.data(1)");
    pStr = PutToStr(pStr, pin_icramo->data.arr[2],32,"icramo.data(2)");
    pStr = PutToStr(pStr, pin_icramo->data.arr[3],32,"icramo.data(3)");
    pStr = PutToStr(pStr, pin_icramo->ctx.arr[0],M_CTX_SZ,"icramo.ctx(0)");//           : ctxdatatype;
    pStr = PutToStr(pStr, pin_icramo->ctx.arr[1],M_CTX_SZ,"icramo.ctx(1)");
    pStr = PutToStr(pStr, pin_icramo->ctx.arr[2],M_CTX_SZ,"icramo.ctx(2)");
    pStr = PutToStr(pStr, pin_icramo->ctx.arr[3],M_CTX_SZ,"icramo.ctx(3)");
    pStr = PutToStr(pStr, pin_icramo->tpar.arr[0],4,"icramo.tpar(0)");//          : cpartype;
    pStr = PutToStr(pStr, pin_icramo->tpar.arr[1],4,"icramo.tpar(1)");
    pStr = PutToStr(pStr, pin_icramo->tpar.arr[2],4,"icramo.tpar(2)");
    pStr = PutToStr(pStr, pin_icramo->tpar.arr[3],4,"icramo.tpar(3)");
    pStr = PutToStr(pStr, pin_icramo->dpar.arr[0],4,"icramo.dpar(0)");//          : cpartype;
    pStr = PutToStr(pStr, pin_icramo->dpar.arr[1],4,"icramo.dpar(0)");
    pStr = PutToStr(pStr, pin_icramo->dpar.arr[2],4,"icramo.dpar(0)");
    pStr = PutToStr(pStr, pin_icramo->dpar.arr[3],4,"icramo.dpar(0)");
    //mmudc_in_type 
    pStr = PutToStr(pStr, pin_mmudci->trans_op,1,"mmudci.trans_op");//std_logic; 
    pStr = PutToStr(pStr, pin_mmudci->transdata.data,32,"mmudci.transdata.data");//             : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pin_mmudci->transdata.su,1,"mmudci.transdata.su");//            : std_logic;
    pStr = PutToStr(pStr, pin_mmudci->transdata.read,1,"mmudci.transdata.read");//            : std_logic;
    pStr = PutToStr(pStr, (uint32)pin_mmudci->transdata.isid,1,"mmudci.transdata.isid");//             : mmu_idcache;
    pStr = PutToStr(pStr, pin_mmudci->transdata.wb_data,32,"mmudci.transdata.wb_data");//          : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pin_mmudci->flush_op,1,"mmudci.flush_op");//std_logic;
    pStr = PutToStr(pStr, pin_mmudci->diag_op,1,"mmudci.diag_op");//std_logic;
    pStr = PutToStr(pStr, pin_mmudci->wb_op,1,"mmudci.wb_op");//std_logic;
    pStr = PutToStr(pStr, pin_mmudci->fsread,1,"mmudci.fsread");//std_logic;
    pStr = PutToStr(pStr, pin_mmudci->mmctrl1.e,1,"mmudci.mmctrl1.e");//    : std_logic;                -- enable
    pStr = PutToStr(pStr, pin_mmudci->mmctrl1.nf,1,"mmudci.mmctrl1.nf");//    : std_logic;                -- no fault
    pStr = PutToStr(pStr, pin_mmudci->mmctrl1.pso,1,"mmudci.mmctrl1.pso");//    : std_logic;                -- partial store order
    pStr = PutToStr(pStr, pin_mmudci->mmctrl1.pagesize,2,"mmudci.mmctrl1.pagesize");// std_logic_vector(1 downto 0);-- page size
    pStr = PutToStr(pStr, pin_mmudci->mmctrl1.ctx,M_CTX_SZ,"mmudci.mmctrl1.ctx");//     : std_logic_vector(M_CTX_SZ-1 downto 0);-- context nr
    pStr = PutToStr(pStr, pin_mmudci->mmctrl1.ctxp,MMCTRL_CTXP_SZ,"mmudci.mmctrl1.ctxp");//    : std_logic_vector(MMCTRL_CTXP_SZ-1 downto 0);  -- context table pointer
    pStr = PutToStr(pStr, pin_mmudci->mmctrl1.tlbdis,1,"mmudci.mmctrl1.tlbdis");// : std_logic;                            -- tlb disabled
    pStr = PutToStr(pStr, pin_mmudci->mmctrl1.bar,2,"mmudci.mmctrl1.bar");// : std_logic_vector(1 downto 0);         -- preplace barrier
    //mmuic_out_type
    pStr = PutToStr(pStr, pin_mmuico->transdata.data,32,"mmuico.transdata.data");//          : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pin_mmuico->grant,1,"mmuico.grant");//           : std_logic;
    pStr = PutToStr(pStr, pin_mmuico->transdata.finish,1,"mmuico.transdata.finish");//          : std_logic;
    pStr = PutToStr(pStr, pin_mmuico->transdata.cache,1,"mmuico.transdata.cache");//          : std_logic;
    pStr = PutToStr(pStr, pin_mmuico->transdata.accexc,1,"mmuico.transdata.accexc");//          : std_logic;
    pStr = PutToStr(pStr, pin_mmuico->tlbmiss,1,"mmuico.tlbmiss");//         : std_logic;

    // outputs:
    pStr = PutToStr(pStr, pch_ico->data.arr[0],IDBITS,"ch_ico.data(0)");
    pStr = PutToStr(pStr, pch_ico->data.arr[1],IDBITS,"ch_ico.data(1)");
    pStr = PutToStr(pStr, pch_ico->data.arr[2],IDBITS,"ch_ico.data(2)");
    pStr = PutToStr(pStr, pch_ico->data.arr[3],IDBITS,"ch_ico.data(3)");
    pStr = PutToStr(pStr, pch_ico->set,2,"ch_ico.set");//              : std_logic_vector(1 downto 0);
    pStr = PutToStr(pStr, pch_ico->mexc,1,"ch_ico.mexc");//             : std_ulogic;
    pStr = PutToStr(pStr, pch_ico->hold,1,"ch_ico.hold");//             : std_ulogic;
    pStr = PutToStr(pStr, pch_ico->flush,1,"ch_ico.flush");//            : std_ulogic;			-- flush in progress
    pStr = PutToStr(pStr, pch_ico->diagrdy,1,"ch_ico.diagrdy");//          : std_ulogic;			-- diagnostic access ready
    pStr = PutToStr(pStr, pch_ico->diagdata,IDBITS,"ch_ico.diagdata");//         : std_logic_vector(IDBITS-1 downto 0);-- diagnostic data
    pStr = PutToStr(pStr, pch_ico->mds,1,"ch_ico.mds");//              : std_ulogic;			-- memory data strobe
    pStr = PutToStr(pStr, pch_ico->cfg,32,"ch_ico.cfg");//              : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pch_ico->idle,1,"ch_ico.idle");//             : std_ulogic;			-- idle mode
    pStr = PutToStr(pStr, pch_ico->cstat.cmiss,1,"ch_ico.cstat.cmiss");//            : l3_cstat_type;
    pStr = PutToStr(pStr, pch_ico->cstat.tmiss,1,"ch_ico.cstat.tmiss");//   : std_ulogic;			-- TLB miss
    pStr = PutToStr(pStr, pch_ico->cstat.chold,1,"ch_ico.cstat.chold");//   : std_ulogic;			-- cache hold
    pStr = PutToStr(pStr, pch_ico->cstat.mhold,1,"ch_ico.cstat.mhold");//   : std_ulogic;			-- cache mmu hold

    pStr = PutToStr(pStr, pch_mcii->address,32,"ch_mcii.address");//          : std_logic_vector(31 downto 0); // memory address
    pStr = PutToStr(pStr, pch_mcii->burst,1,"ch_mcii.burst");//            : std_ulogic;      // burst request
    pStr = PutToStr(pStr, pch_mcii->req,1,"ch_mcii.req");//              : std_ulogic;      // memory cycle request
    pStr = PutToStr(pStr, pch_mcii->su,1,"ch_mcii.su");//               : std_ulogic;      // supervisor address space
    pStr = PutToStr(pStr, pch_mcii->flush,1,"ch_mcii.flush");//            : std_ulogic;      // flush in progress

    pStr = PutToStr(pStr, pch_icrami->address,20,"ch_icramin.address");// : std_logic_vector(19 downto 0);
    pStr = PutToStr(pStr, pch_icrami->tag.arr[0],IDBITS,"ch_icramin.tag(0)");//     : cdatatype;
    pStr = PutToStr(pStr, pch_icrami->tag.arr[1],IDBITS,"ch_icramin.tag(1)");//     : cdatatype;
    pStr = PutToStr(pStr, pch_icrami->tag.arr[2],IDBITS,"ch_icramin.tag(2)");//     : cdatatype;
    pStr = PutToStr(pStr, pch_icrami->tag.arr[3],IDBITS,"ch_icramin.tag(3)");//     : cdatatype;
    pStr = PutToStr(pStr, pch_icrami->twrite,4,"ch_icramin.twrite",true);//  : std_logic_vector(0 to 3);
    pStr = PutToStr(pStr, pch_icrami->tenable,1,"ch_icramin.tenable");// : std_ulogic;
    pStr = PutToStr(pStr, pch_icrami->flush,1,"ch_icramin.flush");//   : std_ulogic;
    pStr = PutToStr(pStr, pch_icrami->data,32,"ch_icramin.data");//    : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pch_icrami->denable,1,"ch_icramin.denable");// : std_ulogic;
    pStr = PutToStr(pStr, pch_icrami->dwrite,4,"ch_icramin.dwrite",true);//  : std_logic_vector(0 to 3);
    pStr = PutToStr(pStr, pch_icrami->ldramin.enable,1,"ch_icramin.ldramin.enable");//        : std_ulogic;                       
    pStr = PutToStr(pStr, pch_icrami->ldramin.read,1,"ch_icramin.ldramin.read");//          : std_ulogic;                         
    pStr = PutToStr(pStr, pch_icrami->ldramin.write,1,"ch_icramin.ldramin.write");//         : std_ulogic;                       
    pStr = PutToStr(pStr, pch_icrami->ctx,M_CTX_SZ,"ch_icramin.ctx");//           : std_logic_vector(M_CTX_SZ-1 downto 0);
    pStr = PutToStr(pStr, pch_icrami->tpar.arr[0],4,"ch_icramin.tpar(0)");//          : cpartype;
    pStr = PutToStr(pStr, pch_icrami->tpar.arr[1],4,"ch_icramin.tpar(1)");
    pStr = PutToStr(pStr, pch_icrami->tpar.arr[2],4,"ch_icramin.tpar(2)");
    pStr = PutToStr(pStr, pch_icrami->tpar.arr[3],4,"ch_icramin.tpar(3)");
    pStr = PutToStr(pStr, pch_icrami->dpar,4,"ch_icramin.dpar");//          : std_logic_vector(3 downto 0);

    pStr = PutToStr(pStr, pch_mmuici->trans_op,1,"ch_mmuici.trans_op");//        : std_logic; 
    pStr = PutToStr(pStr, pch_mmuici->transdata.data,32,"ch_mmuici.transdata.data");//             : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pch_mmuici->transdata.su,1,"ch_mmuici.transdata.su");//            : std_logic;
    pStr = PutToStr(pStr, pch_mmuici->transdata.read,1,"ch_mmuici.transdata.read");//            : std_logic;
    pStr = PutToStr(pStr, (uint32)pch_mmuici->transdata.isid,1,"ch_mmuici.transdata.isid");//             : mmu_idcache;
    pStr = PutToStr(pStr, pch_mmuici->transdata.wb_data,32,"ch_mmuici.transdata.wb_data");//          : std_logic_vector(31 downto 0);

    // internal:
    pStr = PutToStr(pStr, (uint32)ptst_mmu_icache->rdatasel,2,"t_rdatasel");
    pStr = PutToStr(pStr, ptst_mmu_icache->twrite,1,"t_twrite");
    pStr = PutToStr(pStr, ptst_mmu_icache->diagen,1,"t_diagen");
    pStr = PutToStr(pStr, ptst_mmu_icache->dwrite,1,"t_dwire");
    pStr = PutToStr(pStr, ptst_mmu_icache->taddr>>LINE_LOW,TAG_HIGH-LINE_LOW+1,"t_taddr");

    pStr = PutToStr(pStr, (ptst_mmu_icache->wtag>>ITAG_LOW), TAG_HIGH-ITAG_LOW+1,"t_wtag");// : std_logic_vector(TAG_HIGH downto TAG_LOW); -- write tag value
    pStr = PutToStr(pStr, ptst_mmu_icache->ddatain,32,"t_ddatain");// : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, ptst_mmu_icache->diagdata,32,"t_diagdata");// : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, ptst_mmu_icache->vmaskraw, CFG_ILINE,"t_vmaskraw");
    pStr = PutToStr(pStr, ptst_mmu_icache->vmask, CFG_ILINE,"t_vmask");// : std_logic_vector((ILINE_SIZE -1) downto 0);
    pStr = PutToStr(pStr, ptst_mmu_icache->xaddr_inc, ILINE_BITS, "t_xaddr_inc");// : std_logic_vector((ILINE_BITS -1) downto 0);
    pStr = PutToStr(pStr, ptst_mmu_icache->lastline,1,"t_lastline");
    pStr = PutToStr(pStr, ptst_mmu_icache->nlastline,1,"t_nlastline");
    pStr = PutToStr(pStr, ptst_mmu_icache->nnlastline,1,"t_nnlastline");//std_logic;
    pStr = PutToStr(pStr, ptst_mmu_icache->enable,1,"t_enable");//std_logic;
    pStr = PutToStr(pStr, ptst_mmu_icache->error,1,"t_error");//std_logic;
    pStr = PutToStr(pStr, ptst_mmu_icache->whit,1,"t_whit"); 
    pStr = PutToStr(pStr, ptst_mmu_icache->hit,1,"t_hit");
    pStr = PutToStr(pStr, ptst_mmu_icache->valid,1,"t_valid");//std_logic;
    pStr = PutToStr(pStr, ptst_mmu_icache->cacheon,1,"t_cacheon");//std_logic;
    pStr = PutToStr(pStr, ptst_mmu_icache->branch,1,"t_branch");//  : std_logic;
    pStr = PutToStr(pStr, ptst_mmu_icache->eholdn,1,"t_eholdn");//  : std_logic;
    pStr = PutToStr(pStr, ptst_mmu_icache->mds,1,"t_mds");
    pStr = PutToStr(pStr, ptst_mmu_icache->write,1,"t_write");//  : std_logic;
    pStr = PutToStr(pStr, ptst_mmu_icache->tparerr,CFG_ISETS,"t_tparerr");
    pStr = PutToStr(pStr, ptst_mmu_icache->dparerr,CFG_ISETS,"t_dparerr");//  : std_logic_vector(0 to ISETS-1);
    pStr = PutToStr(pStr, ptst_mmu_icache->set,MAXSETS,"t_set");//     : integer range 0 to MAXSETS-1;
    pStr = PutToStr(pStr, ptst_mmu_icache->setrepl,log2x[CFG_ISETS],"t_setrepl");// : std_logic_vector(log2x(ISETS)-1 downto 0); -- set to replace
    pStr = PutToStr(pStr, ptst_mmu_icache->ctwrite,MAXSETS,"t_ctwrite");
    pStr = PutToStr(pStr, ptst_mmu_icache->cdwrite,MAXSETS,"t_cdwrite");
    pStr = PutToStr(pStr, ptst_mmu_icache->validv,MAXSETS,"t_validv");// : std_logic_vector(0 to MAXSETS-1);
    pStr = PutToStr(pStr, ptst_mmu_icache->wlrr,1,"t_wlrr");// : std_logic;
    pStr = PutToStr(pStr, ptst_mmu_icache->vdiagset,CFG_ISETS,"t_vdiagset");
    pStr = PutToStr(pStr, ptst_mmu_icache->rdiagset,CFG_ISETS,"t_rdiagset");// : integer range 0 to ISETS-1;
    pStr = PutToStr(pStr, ptst_mmu_icache->lock,CFG_ISETS,"t_lock");// : std_logic_vector(0 to ISETS-1);
    pStr = PutToStr(pStr, ptst_mmu_icache->wlock,1,"t_wlock");
    pStr = PutToStr(pStr, ptst_mmu_icache->sidle,1,"t_sidle");// : std_logic;
    pStr = PutToStr(pStr, ptst_mmu_icache->lramacc,1,"t_lramacc");
    pStr = PutToStr(pStr, ptst_mmu_icache->ilramwr,1,"t_ilramwr");
    pStr = PutToStr(pStr, ptst_mmu_icache->lramcs,1,"t_lramcs");//  : std_ulogic;
    pStr = PutToStr(pStr, ptst_mmu_icache->pftag>>2,30,"t_pftag");// : std_logic_vector(31 downto 2);
    pStr = PutToStr(pStr, ptst_mmu_icache->mmuici_trans_op,1,"t_mmuici_trans_op");// : std_logic;
    pStr = PutToStr(pStr, ptst_mmu_icache->mmuici_su,1,"t_mmuici_su");// : std_logic;

    pStr = PutToStr(pStr, (uint32)ptst_mmu_icache->v.istate,2,"t_v_state");
    pStr = PutToStr(pStr, ptst_mmu_icache->r.Q.req,1,"t_r_req");  


    PrintIndexStr();

    *posBench[TB_mmu_icache] << chStr << "\n";
  }
#ifdef DBG_mmu_icache
  tst_mmu_icache.ClkUpdate();
#endif
}

