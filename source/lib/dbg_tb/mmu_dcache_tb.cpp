#include "sparc.h"
#include "ldbg.h"
#include "leon3_ml605\leon3mp.h"
extern leon3mp topLeon3mp;

//#define DBG_mmu_dcache


#ifdef DBG_mmu_dcache
  dcache_in_type in_dci;// : in  dcache_in_type;
  dcache_out_type ch_dco;// : out dcache_out_type;
  icache_out_type in_ico;// : in  icache_out_type;
  memory_dc_in_type ch_mcdi;// : out memory_dc_in_type;
  memory_dc_out_type in_mcdo;// : in  memory_dc_out_type;
  ahb_slv_in_type in_ahbsi;// : in  ahb_slv_in_type;
  dcram_in_type ch_dcrami;// : out dcram_in_type;
  dcram_out_type in_dcramo;// : in  dcram_out_type;
  uint32 fpuholdn;// : in  std_logic;
  mmudc_in_type ch_mmudci;// : out mmudc_in_type;
  mmudc_out_type in_mmudco;// : in mmudc_out_type;

  mmu_dcache tst_mmu_dcache;
#endif


#ifdef DBG_mmu_dcache
  enum EStateDCache {dcache_reset, dcache_init, dcache_other};
  EStateDCache eStateDCache = dcache_reset;
#endif

//****************************************************************************
void dbg::mmu_dcache_tb(SystemOnChipIO &io)
{
  mmu_dcache *ptst_mmu_dcache = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->clDCache;  
  
  uint32 *pin_fpuholdn          = &topLeon3mp.pclLeon3s[0]->pclProc3->pholdn;
  dcache_in_type *pin_dci       = &topLeon3mp.pclLeon3s[0]->pclProc3->dci;
  dcache_out_type *pch_dco      = &topLeon3mp.pclLeon3s[0]->pclProc3->dco;
  icache_out_type *pin_ico      = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->icol;
  memory_dc_in_type *pch_mcdi   = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->mcdi;
  memory_dc_out_type *pin_mcdo  = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->mcdo;
  ahb_slv_in_type *pin_ahbsi    = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->ahbsi2;
  dcram_in_type *pch_dcrami     = &topLeon3mp.pclLeon3s[0]->crami.dcramin;
  dcram_out_type *pin_dcramo    = &topLeon3mp.pclLeon3s[0]->cramo.dcramo;
  mmudc_in_type *pch_mmudci     = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->mmudci;
  mmudc_out_type *pin_mmudco    = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->mmudco;
  
#ifdef DBG_mmu_dcache
  if(io.inClk.eClock_z==SClock::CLK_POSEDGE)
  {
    fpuholdn = 1;
    
    switch(eStateDCache)
    {
      case dcache_reset:
        if(iClkCnt>10) eStateDCache = dcache_init;
      break;
      case dcache_init:
        in_ico.hold = 1;
        in_dci.enaddr = 1;
        in_dci.dsuen = !in_dci.dsuen;
        in_dci.asi = ASI_MMUREGS;
        if(in_dci.dsuen)
        {
          in_dci.maddress = (BITS32(in_dci.maddress,CNR_U,CNR_D)+1)<<CNR_D;
        }
        if(iClkCnt>30)
          eStateDCache = dcache_other;
      break;
      default:;
    }
    if(eStateDCache>=dcache_other)
    {
      in_dci.asi = rand()&0xff;//              : std_logic_vector(7 downto 0); 
      in_dci.maddress = (rand()<<17) + rand();//         : std_logic_vector(31 downto 0); 
      in_dci.eaddress = (rand()<<17) + rand();//         : std_logic_vector(31 downto 0); 
      in_dci.edata = (rand()<<17) + rand();//            : std_logic_vector(31 downto 0); 
      in_dci.size = rand()&0x3;//             : std_logic_vector(1 downto 0);
      in_dci.enaddr = rand()&0x1;//           : std_ulogic;
      in_dci.eenaddr = rand()&0x1;//          : std_ulogic;
      in_dci.nullify = rand()&0x1;//          : std_ulogic;
      in_dci.lock = rand()&0x1;//             : std_ulogic;
      in_dci.read = rand()&0x1;//             : std_ulogic;
      in_dci.write = rand()&0x1;//            : std_ulogic;
      in_dci.flush = rand()&0x1;//            : std_ulogic;
      in_dci.flushl = rand()&0x1;//           : std_ulogic;                        -- flush line  
      in_dci.dsuen = rand()&0x1;//            : std_ulogic;
      in_dci.msu = rand()&0x1;//              : std_ulogic;                   -- memory stage supervisor
      in_dci.esu = rand()&0x1;//              : std_ulogic;                   -- execution stage supervisor
      in_dci.intack = rand()&0x1;//           : std_ulogic;
      uint32 tmp;
      //icache_out_type:
      in_ico.data.arr[0] = (rand()<<17) + rand();
      in_ico.data.arr[1] = (rand()<<17) + rand();
      in_ico.data.arr[2] = (rand()<<17) + rand();
      in_ico.data.arr[3] = (rand()<<17) + rand();
      tmp = rand();
      in_ico.set = (tmp>>0)&0x3;//              : std_logic_vector(1 downto 0);
      if(in_ico.set>CFG_ISETS-1) in_ico.set = CFG_ISETS-1;
      in_ico.mexc = (tmp>>2)&0x1;//             : std_ulogic;
      in_ico.hold = (tmp>>3)&0x1;//             : std_ulogic;
      in_ico.flush = (tmp>>4)&0x1;//            : std_ulogic;			-- flush in progress
      in_ico.diagrdy = (tmp>>5)&0x1;//          : std_ulogic;			-- diagnostic access ready
      in_ico.diagdata = (rand()<<17) + rand();//         : std_logic_vector(IDBITS-1 downto 0);-- diagnostic data
      in_ico.mds = (tmp>>6)&0x1;//              : std_ulogic;			-- memory data strobe
      in_ico.cfg = (rand()<<17) + rand();//              : std_logic_vector(31 downto 0);
      in_ico.idle = (tmp>>7)&0x1;//             : std_ulogic;			-- idle mode
      in_ico.cstat.cmiss = (tmp>>8)&0x1;//   : std_ulogic;			-- cache miss
      in_ico.cstat.tmiss = (tmp>>9)&0x1;//   : std_ulogic;			-- TLB miss
      in_ico.cstat.chold = (tmp>>10)&0x1;//   : std_ulogic;			-- cache hold
      in_ico.cstat.mhold = (tmp>>11)&0x1;//   : std_ulogic;			-- cache mmu hold
      //
      in_mcdo.data = (rand()<<17) + rand();//             : std_logic_vector(31 downto 0); // memory data
      in_mcdo.ready = rand()&0x1;//            : std_ulogic;    // cycle ready
      in_mcdo.grant = rand()&0x1;//            : std_ulogic;
      in_mcdo.retry = rand()&0x1;//            : std_ulogic;
      in_mcdo.mexc = rand()&0x1;//             : std_ulogic;    // memory exception
      in_mcdo.werr = rand()&0x1;//             : std_ulogic;    // memory write error
      in_mcdo.cache = rand()&0x1;//            : std_ulogic;    // cacheable data
      in_mcdo.ba = rand()&0x1;//               : std_ulogic;    // bus active (used for snooping)
      in_mcdo.bg = rand()&0x1;//               : std_ulogic;    // bus grant  (used for snooping)
      in_mcdo.par = rand()&0xf;//              : std_logic_vector(3 downto 0);  // parity
      in_mcdo.scanen = 0;//           : std_ulogic;
      in_mcdo.testen = 0;//           : std_ulogic;
      //
      in_ahbsi.hsel = rand()&MSK32(AHB_SLAVES_MAX-1,0);//  : (0 to AHB_SLAVES_MAX-1);     -- slave select
      in_ahbsi.haddr = (rand()<<17) + rand();// : (31 downto 0);  -- address bus (byte)
      in_ahbsi.hwrite = rand()&0x1;//  ;                           -- read/write
      in_ahbsi.htrans = rand()&0x3;//  : (1 downto 0);   -- transfer type
      in_ahbsi.hsize = rand()&0x7;// : (2 downto 0);   -- transfer size
      in_ahbsi.hburst = rand()&0x7;//  : (2 downto 0);   -- burst type
      in_ahbsi.hwdata = ((rand()<<17) + rand())&MSK32(CFG_AHBDW-1,0);//  : (AHBDW-1 downto 0);   -- write data bus
      in_ahbsi.hprot = rand()&0xf;// : (3 downto 0);   -- protection control
      in_ahbsi.hready = rand()&0x1;//  ;                -- transfer done
      in_ahbsi.hmaster = rand()&0xf;// : (3 downto 0);  -- current master
      in_ahbsi.hmastlock = rand()&0x1;// ;              -- locked access
      in_ahbsi.hmbsel = rand()&MSK32(AHB_MEM_ID_WIDTH-1,0);//  : (0 to AHB_MEM_ID_WIDTH-1); -- memory bank select
      in_ahbsi.hcache = rand()&0x1;//  ;                -- cacheable
      in_ahbsi.hirq = rand()&MSK32(AHB_IRQ_MAX-1,0);//  : (AHB_IRQ_MAX-1 downto 0); -- interrupt result bus
      in_ahbsi.testen = 0;//                        -- scan test enable
      in_ahbsi.testrst = 0;//                       -- scan test reset
      in_ahbsi.scanen = 0;//                        -- scan enable
      in_ahbsi.testoen = 0;//                       -- test output enable 
      //
      in_dcramo.tag.arr[0] = ((rand()<<17) + rand())&MSK32(IDBITS-1,0);//     : cdatatype;
      in_dcramo.tag.arr[1] = (rand()<<17) + rand();
      in_dcramo.tag.arr[2] = (rand()<<17) + rand();
      in_dcramo.tag.arr[3] = (rand()<<17) + rand();
      in_dcramo.data.arr[0] = (rand()<<17) + rand();;//    : cdatatype;
      in_dcramo.data.arr[1] = (rand()<<17) + rand();
      in_dcramo.data.arr[2] = (rand()<<17) + rand();
      in_dcramo.data.arr[3] = (rand()<<17) + rand();
      in_dcramo.stag.arr[0] = (rand()<<17) + rand();//      : cdatatype;
      in_dcramo.stag.arr[1] = (rand()<<17) + rand();
      in_dcramo.stag.arr[2] = (rand()<<17) + rand();
      in_dcramo.stag.arr[3] = (rand()<<17) + rand();
      in_dcramo.ctx.arr[0] = rand()&MSK32(M_CTX_SZ-1,0);//           : ctxdatatype;
      in_dcramo.ctx.arr[1] = rand()&MSK32(M_CTX_SZ-1,0);
      in_dcramo.ctx.arr[2] = rand()&MSK32(M_CTX_SZ-1,0);
      in_dcramo.ctx.arr[3] = rand()&MSK32(M_CTX_SZ-1,0);
      in_dcramo.tpar.arr[0] = rand()&0xf;//          : cpartype; // tag parity
      in_dcramo.tpar.arr[1] = rand()&0xf;
      in_dcramo.tpar.arr[2] = rand()&0xf;
      in_dcramo.tpar.arr[3] = rand()&0xf;
      in_dcramo.dpar.arr[0] = rand()&0xf;//          : cpartype;   // data parity
      in_dcramo.dpar.arr[1] = rand()&0xf;
      in_dcramo.dpar.arr[2] = rand()&0xf;
      in_dcramo.dpar.arr[3] = rand()&0xf;
      in_dcramo.spar = rand()&0xf;//          : std_logic_vector(3 downto 0);   // snoop tag parity
      //
      in_mmudco.grant = rand()&0x1;//         : std_logic;
      in_mmudco.transdata.finish = rand()&0x1;//          : std_logic;
      in_mmudco.transdata.data = (rand()<<17) + rand();//          : std_logic_vector(31 downto 0);
      in_mmudco.transdata.cache = rand()&0x1;//          : std_logic;
      in_mmudco.transdata.accexc = rand()&0x1;//          : std_logic;
      in_mmudco.mmctrl2.fs.ow = rand();//&0x1;//: 1;//std_logic;          
      in_mmudco.mmctrl2.fs.fav = rand()&0x1;
      in_mmudco.mmctrl2.fs.ft = rand()&0x7;//std_logic_vector(2 downto 0);           -- fault type
      in_mmudco.mmctrl2.fs.at_ls = rand()&0x1;//std_logic;                              -- access type, load/store
      in_mmudco.mmctrl2.fs.at_id = rand()&0x1;//std_logic;                              -- access type, i/dcache
      in_mmudco.mmctrl2.fs.at_su = rand()&0x1;//std_logic;                              -- access type, su/user
      in_mmudco.mmctrl2.fs.l = rand()&0x3;//std_logic_vector(1 downto 0);           -- level 
      in_mmudco.mmctrl2.fs.ebe = rand()&0xff;//std_logic_vector(7 downto 0);            
      in_mmudco.mmctrl2.valid = rand()&0x1;// std_logic;
      in_mmudco.mmctrl2.fa = rand()&MSK32(VA_I_SZ-1,0);//    : std_logic_vector(VA_I_SZ-1 downto 0);   -- fault address register
      in_mmudco.wbtransdata.finish = rand()&0x1;//          : std_logic;
      in_mmudco.wbtransdata.data = (rand()<<17) + rand();//          : std_logic_vector(31 downto 0);
      in_mmudco.wbtransdata.cache = rand()&0x1;//          : std_logic;
      in_mmudco.wbtransdata.accexc = rand()&0x1;//          : std_logic;
      in_mmudco.tlbmiss = rand()&0x1;//         : std_logic;
    }
  }

  tst_mmu_dcache.Update(io.inNRst,
                        io.inClk, 
                        in_dci,// : in  dcache_in_type;
                        ch_dco,// : out dcache_out_type;
                        in_ico,// : in  icache_out_type;
                        ch_mcdi,// : out memory_dc_in_type;
                        in_mcdo,// : in  memory_dc_out_type;
                        in_ahbsi,// : in  ahb_slv_in_type;
                        ch_dcrami,// : out dcram_in_type;
                        in_dcramo,// : in  dcram_out_type;
                        fpuholdn,// : in  std_logic;
                        ch_mmudci,// : out mmudc_in_type;
                        in_mmudco,
                        io.inClk);// : in mmudc_out_type;
                        
  ptst_mmu_dcache = &tst_mmu_dcache;  
  pin_fpuholdn  = &fpuholdn;
  pin_dci       = &in_dci;
  pch_dco       = &ch_dco;
  pin_ico       = &in_ico;
  pch_mcdi      = &ch_mcdi;
  pin_mcdo      = &in_mcdo;
  pin_ahbsi     = &in_ahbsi;
  pch_dcrami    = &ch_dcrami;
  pin_dcramo    = &in_dcramo;
  pch_mmudci    = &ch_mmudci;
  pin_mmudco    = &in_mmudco;

#endif


  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';

    ResetPutStr();
  
    // inputs:
    pStr = PutToStr(pStr, io.inNRst, 1, "inNRst");
    pStr = PutToStr(pStr, *pin_fpuholdn, 1, "fpuholdn");
    //
    pStr = PutToStr(pStr, pin_dci->asi,8,"in_dci.asi");//              : std_logic_vector(7 downto 0); 
    pStr = PutToStr(pStr, pin_dci->maddress,32,"in_dci.maddress");//         : std_logic_vector(31 downto 0); 
    pStr = PutToStr(pStr, pin_dci->eaddress,32,"in_dci.eaddress");//         : std_logic_vector(31 downto 0); 
    pStr = PutToStr(pStr, pin_dci->edata,32,"in_dci.edata");//            : std_logic_vector(31 downto 0); 
    pStr = PutToStr(pStr, pin_dci->size,2,"in_dci.size");//             : std_logic_vector(1 downto 0);
    pStr = PutToStr(pStr, pin_dci->enaddr,1,"in_dci.enaddr");//           : std_ulogic;
    pStr = PutToStr(pStr, pin_dci->eenaddr,1,"in_dci.eenaddr");//          : std_ulogic;
    pStr = PutToStr(pStr, pin_dci->nullify,1,"in_dci.nullify");//          : std_ulogic;
    pStr = PutToStr(pStr, pin_dci->lock,1,"in_dci.lock");//             : std_ulogic;
    pStr = PutToStr(pStr, pin_dci->read,1,"in_dci.read");//             : std_ulogic;
    pStr = PutToStr(pStr, pin_dci->write,1,"in_dci.write");//            : std_ulogic;
    pStr = PutToStr(pStr, pin_dci->flush,1,"in_dci.flush");//            : std_ulogic;
    pStr = PutToStr(pStr, pin_dci->flushl,1,"in_dci.flushl");//           : std_ulogic;                        -- flush line  
    pStr = PutToStr(pStr, pin_dci->dsuen,1,"in_dci.dsuen");//            : std_ulogic;
    pStr = PutToStr(pStr, pin_dci->msu,1,"in_dci.msu");//              : std_ulogic;                   -- memory stage supervisor
    pStr = PutToStr(pStr, pin_dci->esu,1,"in_dci.esu");//              : std_ulogic;                   -- execution stage supervisor
    pStr = PutToStr(pStr, pin_dci->intack,1,"in_dci.intack");//           : std_ulogic;
    //
    pStr = PutToStr(pStr, pin_ico->data.arr[0],IDBITS,"in_ico.data(0)");
    pStr = PutToStr(pStr, pin_ico->data.arr[1],IDBITS,"in_ico.data(1)");
    pStr = PutToStr(pStr, pin_ico->data.arr[2],IDBITS,"in_ico.data(2)");
    pStr = PutToStr(pStr, pin_ico->data.arr[3],IDBITS,"in_ico.data(3)");
    pStr = PutToStr(pStr, pin_ico->set,2,"in_ico.set");//              : std_logic_vector(1 downto 0);
    pStr = PutToStr(pStr, pin_ico->mexc,1,"in_ico.mexc");//             : std_ulogic;
    pStr = PutToStr(pStr, pin_ico->hold,1,"in_ico.hold");//             : std_ulogic;
    pStr = PutToStr(pStr, pin_ico->flush,1,"in_ico.flush");//            : std_ulogic;			-- flush in progress
    pStr = PutToStr(pStr, pin_ico->diagrdy,1,"in_ico.diagrdy");//          : std_ulogic;			-- diagnostic access ready
    pStr = PutToStr(pStr, pin_ico->diagdata,IDBITS,"in_ico.diagdata");//         : std_logic_vector(IDBITS-1 downto 0);-- diagnostic data
    pStr = PutToStr(pStr, pin_ico->mds,1,"in_ico.mds");//              : std_ulogic;			-- memory data strobe
    pStr = PutToStr(pStr, pin_ico->cfg,32,"in_ico.cfg");//              : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pin_ico->idle,1,"in_ico.idle");//             : std_ulogic;			-- idle mode
    pStr = PutToStr(pStr, pin_ico->cstat.cmiss,1,"in_ico.cstat.cmiss");//            : l3_cstat_type;
    pStr = PutToStr(pStr, pin_ico->cstat.tmiss,1,"in_ico.cstat.tmiss");//   : std_ulogic;			-- TLB miss
    pStr = PutToStr(pStr, pin_ico->cstat.chold,1,"in_ico.cstat.chold");//   : std_ulogic;			-- cache hold
    pStr = PutToStr(pStr, pin_ico->cstat.mhold,1,"in_ico.cstat.mhold");//   : std_ulogic;			-- cache mmu hold
    //
    pStr = PutToStr(pStr, pin_mcdo->data,32,"in_mcdo.data");//             : std_logic_vector(31 downto 0); // memory data
    pStr = PutToStr(pStr, pin_mcdo->ready,1,"in_mcdo.ready");//            : std_ulogic;    // cycle ready
    pStr = PutToStr(pStr, pin_mcdo->grant,1,"in_mcdo.grant");//            : std_ulogic;
    pStr = PutToStr(pStr, pin_mcdo->retry,1,"in_mcdo.retry");//            : std_ulogic;
    pStr = PutToStr(pStr, pin_mcdo->mexc,1,"in_mcdo.mexc");//             : std_ulogic;    // memory exception
    pStr = PutToStr(pStr, pin_mcdo->werr,1,"in_mcdo.werr");//             : std_ulogic;    // memory write error
    pStr = PutToStr(pStr, pin_mcdo->cache,1,"in_mcdo.cache");//            : std_ulogic;    // cacheable data
    pStr = PutToStr(pStr, pin_mcdo->ba,1,"in_mcdo.ba");//               : std_ulogic;    // bus active (used for snooping)
    pStr = PutToStr(pStr, pin_mcdo->bg,1,"in_mcdo.bg");//               : std_ulogic;    // bus grant  (used for snooping)
    pStr = PutToStr(pStr, pin_mcdo->par,4,"in_mcdo.par");//              : std_logic_vector(3 downto 0);  // parity
    pStr = PutToStr(pStr, pin_mcdo->scanen,1,"in_mcdo.scanen");//           : std_ulogic;
    pStr = PutToStr(pStr, pin_mcdo->testen,1,"in_mcdo.testen");//           : std_ulogic;
    //
    pStr = PutToStr(pStr, pin_ahbsi->hsel,AHB_SLAVES_MAX,"in_ahbsi.hsel");//  : (0 to AHB_SLAVES_MAX-1);     -- slave select
    pStr = PutToStr(pStr, pin_ahbsi->haddr,32,"in_ahbsi.haddr");// : (31 downto 0);  -- address bus (byte)
    pStr = PutToStr(pStr, pin_ahbsi->hwrite,1,"in_ahbsi.hwrite");//  ;                           -- read/write
    pStr = PutToStr(pStr, pin_ahbsi->htrans,2,"in_ahbsi.htrans");//  : (1 downto 0);   -- transfer type
    pStr = PutToStr(pStr, pin_ahbsi->hsize,3,"in_ahbsi.hsize");// : (2 downto 0);   -- transfer size
    pStr = PutToStr(pStr, pin_ahbsi->hburst,3,"in_ahbsi.hburst");//  : (2 downto 0);   -- burst type
    pStr = PutToStr(pStr, pin_ahbsi->hwdata,CFG_AHBDW,"in_ahbsi.hwdata");//  : (AHBDW-1 downto 0);   -- write data bus
    pStr = PutToStr(pStr, pin_ahbsi->hprot,4,"in_ahbsi.hprot");// : (3 downto 0);   -- protection control
    pStr = PutToStr(pStr, pin_ahbsi->hready,1,"in_ahbsi.hready");//  ;                -- transfer done
    pStr = PutToStr(pStr, pin_ahbsi->hmaster,4,"in_ahbsi.hmaster");// : (3 downto 0);  -- current master
    pStr = PutToStr(pStr, pin_ahbsi->hmastlock,1,"in_ahbsi.hmastlock");// ;              -- locked access
    pStr = PutToStr(pStr, pin_ahbsi->hmbsel,AHB_MEM_ID_WIDTH,"in_ahbsi.hmbsel");//  : (0 to AHB_MEM_ID_WIDTH-1); -- memory bank select
    pStr = PutToStr(pStr, pin_ahbsi->hcache,1,"in_ahbsi.hcache");//  ;                -- cacheable
    pStr = PutToStr(pStr, pin_ahbsi->hirq,AHB_IRQ_MAX,"in_ahbsi.hirq");//  : (AHB_IRQ_MAX-1 downto 0); -- interrupt result bus
    pStr = PutToStr(pStr, pin_ahbsi->testen,1,"in_ahbsi.testen");//                        -- scan test enable
    pStr = PutToStr(pStr, pin_ahbsi->testrst,1,"in_ahbsi.testrst");//                       -- scan test reset
    pStr = PutToStr(pStr, pin_ahbsi->scanen,1,"in_ahbsi.scanen");//                        -- scan enable
    pStr = PutToStr(pStr, pin_ahbsi->testoen,1,"in_ahbsi.testoen");;//                       -- test output enable 
    //
    pStr = PutToStr(pStr, pin_dcramo->tag.arr[0],IDBITS,"in_dcramo.tag(0)");
    pStr = PutToStr(pStr, pin_dcramo->tag.arr[1],IDBITS,"in_dcramo.tag(1)");
    pStr = PutToStr(pStr, pin_dcramo->tag.arr[2],IDBITS,"in_dcramo.tag(2)");
    pStr = PutToStr(pStr, pin_dcramo->tag.arr[3],IDBITS,"in_dcramo.tag(3)");
    pStr = PutToStr(pStr, pin_dcramo->data.arr[0],IDBITS,"in_dcramo.data(0)");
    pStr = PutToStr(pStr, pin_dcramo->data.arr[1],IDBITS,"in_dcramo.data(1)");
    pStr = PutToStr(pStr, pin_dcramo->data.arr[2],IDBITS,"in_dcramo.data(2)");
    pStr = PutToStr(pStr, pin_dcramo->data.arr[3],IDBITS,"in_dcramo.data(3)");
    pStr = PutToStr(pStr, pin_dcramo->stag.arr[0],IDBITS,"in_dcramo.stag(0)");
    pStr = PutToStr(pStr, pin_dcramo->stag.arr[1],IDBITS,"in_dcramo.stag(1)");
    pStr = PutToStr(pStr, pin_dcramo->stag.arr[2],IDBITS,"in_dcramo.stag(2)");
    pStr = PutToStr(pStr, pin_dcramo->stag.arr[3],IDBITS,"in_dcramo.stag(3)");
    pStr = PutToStr(pStr, pin_dcramo->ctx.arr[0],M_CTX_SZ,"in_dcramo.ctx(0)");
    pStr = PutToStr(pStr, pin_dcramo->ctx.arr[1],M_CTX_SZ,"in_dcramo.ctx(1)");
    pStr = PutToStr(pStr, pin_dcramo->ctx.arr[2],M_CTX_SZ,"in_dcramo.ctx(2)");
    pStr = PutToStr(pStr, pin_dcramo->ctx.arr[3],M_CTX_SZ,"in_dcramo.ctx(3)");
    pStr = PutToStr(pStr, pin_dcramo->tpar.arr[0],4,"in_dcramo.tpar(0)");
    pStr = PutToStr(pStr, pin_dcramo->tpar.arr[1],4,"in_dcramo.tpar(1)");
    pStr = PutToStr(pStr, pin_dcramo->tpar.arr[2],4,"in_dcramo.tpar(2)");
    pStr = PutToStr(pStr, pin_dcramo->tpar.arr[3],4,"in_dcramo.tpar(3)");
    pStr = PutToStr(pStr, pin_dcramo->dpar.arr[0],4,"in_dcramo.dpar(0)");
    pStr = PutToStr(pStr, pin_dcramo->dpar.arr[1],4,"in_dcramo.dpar(0)");
    pStr = PutToStr(pStr, pin_dcramo->dpar.arr[2],4,"in_dcramo.dpar(0)");
    pStr = PutToStr(pStr, pin_dcramo->dpar.arr[3],4,"in_dcramo.dpar(0)");
    pStr = PutToStr(pStr, pin_dcramo->spar,4,"in_dcramo.spar");
    //
    pStr = PutToStr(pStr, pin_mmudco->grant,1,"in_mmudco.grant");//         : std_logic;
    pStr = PutToStr(pStr, pin_mmudco->transdata.finish,1,"in_mmudco.transdata.finish");;//          : std_logic;
    pStr = PutToStr(pStr, pin_mmudco->transdata.data,32,"in_mmudco.transdata.data");//          : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pin_mmudco->transdata.cache,1,"in_mmudco.transdata.cache");//          : std_logic;
    pStr = PutToStr(pStr, pin_mmudco->transdata.accexc,1,"in_mmudco.transdata.accexc");//          : std_logic;
    pStr = PutToStr(pStr, pin_mmudco->mmctrl2.fs.ow,1,"in_mmudco.mmctrl2.fs.ow");//std_logic;          
    pStr = PutToStr(pStr, pin_mmudco->mmctrl2.fs.fav,1,"in_mmudco.mmctrl2.fs.fav");
    pStr = PutToStr(pStr, pin_mmudco->mmctrl2.fs.ft,3,"in_mmudco.mmctrl2.fs.ft");//std_logic_vector(2 downto 0);           -- fault type
    pStr = PutToStr(pStr, pin_mmudco->mmctrl2.fs.at_ls,1,"in_mmudco.mmctrl2.fs.at_ls");//std_logic;                              -- access type, load/store
    pStr = PutToStr(pStr, pin_mmudco->mmctrl2.fs.at_id,1,"in_mmudco.mmctrl2.fs.at_id");//std_logic;                              -- access type, i/dcache
    pStr = PutToStr(pStr, pin_mmudco->mmctrl2.fs.at_su,1,"in_mmudco.mmctrl2.fs.at_su");//std_logic;                              -- access type, su/user
    pStr = PutToStr(pStr, pin_mmudco->mmctrl2.fs.l,2,"in_mmudco.mmctrl2.fs.l");//std_logic_vector(1 downto 0);           -- level 
    pStr = PutToStr(pStr, pin_mmudco->mmctrl2.fs.ebe,8,"in_mmudco.mmctrl2.fs.ebe");//std_logic_vector(7 downto 0);            
    pStr = PutToStr(pStr, pin_mmudco->mmctrl2.valid,1,"in_mmudco.mmctrl2.valid");// std_logic;
    pStr = PutToStr(pStr, pin_mmudco->mmctrl2.fa,VA_I_SZ,"in_mmudco.mmctrl2.fa");//    : std_logic_vector(VA_I_SZ-1 downto 0);   -- fault address register
    pStr = PutToStr(pStr, pin_mmudco->wbtransdata.finish,1,"in_mmudco.wbtransdata.finish");//          : std_logic;
    pStr = PutToStr(pStr, pin_mmudco->wbtransdata.data,32,"in_mmudco.wbtransdata.data");//          : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pin_mmudco->wbtransdata.cache,1,"in_mmudco.wbtransdata.cache");//          : std_logic;
    pStr = PutToStr(pStr, pin_mmudco->wbtransdata.accexc,1,"in_mmudco.wbtransdata.accexc");//          : std_logic;
    pStr = PutToStr(pStr, pin_mmudco->tlbmiss,1,"in_mmudco.tlbmiss");//         : std_logic;

    // outputs:
    pStr = PutToStr(pStr, pch_dco->data.arr[0],IDBITS,"ch_dco.data(0)");//             : cdatatype;
    pStr = PutToStr(pStr, pch_dco->data.arr[1],IDBITS,"ch_dco.data(1)");
    pStr = PutToStr(pStr, pch_dco->data.arr[2],IDBITS,"ch_dco.data(2)");
    pStr = PutToStr(pStr, pch_dco->data.arr[3],IDBITS,"ch_dco.data(3)");
    pStr = PutToStr(pStr, pch_dco->set,2,"ch_dco.set");//              : std_logic_vector(1 downto 0);
    pStr = PutToStr(pStr, pch_dco->mexc,1,"ch_dco.mexc");//             : std_ulogic;
    pStr = PutToStr(pStr, pch_dco->hold,1,"ch_dco.hold");//             : std_ulogic;
    pStr = PutToStr(pStr, pch_dco->mds,1,"ch_dco.mds");//              : std_ulogic;
    pStr = PutToStr(pStr, pch_dco->werr,1,"ch_dco.werr");//             : std_ulogic;	
    pStr = PutToStr(pStr, pch_dco->icdiag.addr,32,"ch_dco.icdiag.addr");//             : std_logic_vector(31 downto 0); -- memory stage address
    pStr = PutToStr(pStr, pch_dco->icdiag.enable,1,"ch_dco.icdiag.enable");//           : std_ulogic;
    pStr = PutToStr(pStr, pch_dco->icdiag.read,1,"ch_dco.icdiag.read");//             : std_ulogic;
    pStr = PutToStr(pStr, pch_dco->icdiag.tag,1,"ch_dco.icdiag.tag");//              : std_ulogic;
    pStr = PutToStr(pStr, pch_dco->icdiag.ctx,1,"ch_dco.icdiag.ctx");//              : std_ulogic;
    pStr = PutToStr(pStr, pch_dco->icdiag.flush,1,"ch_dco.icdiag.flush");//            : std_ulogic;
    pStr = PutToStr(pStr, pch_dco->icdiag.ilramen,1,"ch_dco.icdiag.ilramen");//          : std_ulogic;
    pStr = PutToStr(pStr, pch_dco->icdiag.cctrl.burst,1,"ch_dco.icdiag.cctrl.burst");//  : std_ulogic;				-- icache burst enable
    pStr = PutToStr(pStr, pch_dco->icdiag.cctrl.dfrz,1,"ch_dco.icdiag.cctrl.dfrz");//   : std_ulogic;				-- dcache freeze enable
    pStr = PutToStr(pStr, pch_dco->icdiag.cctrl.ifrz,1,"ch_dco.icdiag.cctrl.ifrz");//   : std_ulogic;				-- icache freeze enable
    pStr = PutToStr(pStr, pch_dco->icdiag.cctrl.dsnoop,1,"ch_dco.icdiag.cctrl.dsnoop");// : std_ulogic;				-- data cache snooping
    pStr = PutToStr(pStr, pch_dco->icdiag.cctrl.dcs,2,"ch_dco.icdiag.cctrl.dcs");//    : std_logic_vector(1 downto 0);	-- dcache state
    pStr = PutToStr(pStr, pch_dco->icdiag.cctrl.ics,2,"ch_dco.icdiag.cctrl.ics");//    : std_logic_vector(1 downto 0);	-- icache state
    pStr = PutToStr(pStr, pch_dco->icdiag.pflush,1,"ch_dco.icdiag.pflush");//           : std_ulogic;
    pStr = PutToStr(pStr, pch_dco->icdiag.pflushaddr,VA_I_U-VA_I_D+1,"ch_dco.icdiag.pflushaddr");//       : std_logic_vector(VA_I_U downto VA_I_D); 
    pStr = PutToStr(pStr, pch_dco->icdiag.pflushtyp,1,"ch_dco.icdiag.pflushtyp");//        : std_ulogic;
    pStr = PutToStr(pStr, pch_dco->icdiag.scanen,1,"ch_dco.icdiag.scanen");//           : std_ulogic;
    pStr = PutToStr(pStr, pch_dco->cache,1,"--ch_dco.cache");//            : std_ulogic;
    pStr = PutToStr(pStr, pch_dco->idle,1,"ch_dco.idle");//             : std_ulogic;			-- idle mode
    pStr = PutToStr(pStr, pch_dco->scanen,1,"ch_dco.scanen");//           : std_ulogic;
    pStr = PutToStr(pStr, pch_dco->testen,1,"ch_dco.testen");//           : std_ulogic;
    //
    pStr = PutToStr(pStr, pch_mcdi->address,32,"ch_mcdi.address");//          : std_logic_vector(31 downto 0); 
    pStr = PutToStr(pStr, pch_mcdi->data,32,"ch_mcdi.data");//             : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pch_mcdi->asi,4,"ch_mcdi.asi");//              : std_logic_vector(3 downto 0); // ASI for load/store
    pStr = PutToStr(pStr, pch_mcdi->size,2,"ch_mcdi.size");//             : std_logic_vector(1 downto 0);
    pStr = PutToStr(pStr, pch_mcdi->burst,1,"ch_mcdi.burst");//            : std_ulogic;
    pStr = PutToStr(pStr, pch_mcdi->read,1,"ch_mcdi.read");//             : std_ulogic;
    pStr = PutToStr(pStr, pch_mcdi->req,1,"ch_mcdi.req");//              : std_ulogic;
    pStr = PutToStr(pStr, pch_mcdi->lock,1,"ch_mcdi.lock");//             : std_ulogic;
    pStr = PutToStr(pStr, pch_mcdi->cache,1,"ch_mcdi.cache");//            : std_ulogic;
    //
    pStr = PutToStr(pStr, pch_dcrami->address,20,"ch_dcrami.address");// : std_logic_vector(19 downto 0);
    pStr = PutToStr(pStr, pch_dcrami->tag.arr[0],IDBITS,"ch_dcrami.tag(0)");//     : cdatatype; //std_logic_vector(31  downto 0);
    pStr = PutToStr(pStr, pch_dcrami->tag.arr[1],IDBITS,"ch_dcrami.tag(1)");
    pStr = PutToStr(pStr, pch_dcrami->tag.arr[2],IDBITS,"ch_dcrami.tag(2)");
    pStr = PutToStr(pStr, pch_dcrami->tag.arr[3],IDBITS,"ch_dcrami.tag(3)");
    pStr = PutToStr(pStr, pch_dcrami->ptag.arr[0],IDBITS,"ch_dcrami.ptag(0)");//      : cdatatype; //std_logic_vector(31  downto 0);
    pStr = PutToStr(pStr, pch_dcrami->ptag.arr[1],IDBITS,"ch_dcrami.ptag(1)");
    pStr = PutToStr(pStr, pch_dcrami->ptag.arr[2],IDBITS,"ch_dcrami.ptag(2)");
    pStr = PutToStr(pStr, pch_dcrami->ptag.arr[3],IDBITS,"ch_dcrami.ptag(3)");
    pStr = PutToStr(pStr, pch_dcrami->twrite,4,"ch_dcrami.twrite",true);//  : std_logic_vector(0 to 3);
    pStr = PutToStr(pStr, pch_dcrami->tpwrite,4,"ch_dcrami.tpwrite",true);// : std_logic_vector(0 to 3);
    pStr = PutToStr(pStr, pch_dcrami->tenable,4,"ch_dcrami.tenable",true);// : std_logic_vector(0 to 3);
    pStr = PutToStr(pStr, pch_dcrami->flush,1,"ch_dcrami.flush");//   : std_ulogic;
    pStr = PutToStr(pStr, pch_dcrami->data.arr[0],IDBITS,"ch_dcrami.data(0)");//    : cdatatype;
    pStr = PutToStr(pStr, pch_dcrami->data.arr[1],IDBITS,"ch_dcrami.data(1)");
    pStr = PutToStr(pStr, pch_dcrami->data.arr[2],IDBITS,"ch_dcrami.data(2)");
    pStr = PutToStr(pStr, pch_dcrami->data.arr[3],IDBITS,"ch_dcrami.data(3)");
    pStr = PutToStr(pStr, pch_dcrami->denable,4,"ch_dcrami.denable",true);// : std_logic_vector(0 to 3);
    pStr = PutToStr(pStr, pch_dcrami->dwrite,4,"ch_dcrami.dwrite",true);//  : std_logic_vector(0 to 3);
    pStr = PutToStr(pStr, pch_dcrami->senable,4,"ch_dcrami.senable",true);// : std_logic_vector(0 to 3);
    pStr = PutToStr(pStr, pch_dcrami->swrite,4,"ch_dcrami.swrite",true);//  : std_logic_vector(0 to 3);
    pStr = PutToStr(pStr, pch_dcrami->saddress,20,"ch_dcrami.saddress");//  : std_logic_vector(19 downto 0);
    pStr = PutToStr(pStr, pch_dcrami->faddress,20,"--ch_dcrami.faddress");//  : std_logic_vector(19 downto 0);
    pStr = PutToStr(pStr, pch_dcrami->spar,1,"--ch_dcrami.spar");//          : std_logic;
    pStr = PutToStr(pStr, pch_dcrami->ldramin.address>>2,24-2,"ch_dcrami.ldramin.address");// : std_logic_vector(23 downto 2);
    pStr = PutToStr(pStr, pch_dcrami->ldramin.enable,1,"ch_dcrami.ldramin.enable");//  : std_ulogic;
    pStr = PutToStr(pStr, pch_dcrami->ldramin.read,1,"ch_dcrami.ldramin.read");//    : std_ulogic;
    pStr = PutToStr(pStr, pch_dcrami->ldramin.write,1,"ch_dcrami.ldramin.write");//   : std_ulogic;
    pStr = PutToStr(pStr, pch_dcrami->ctx.arr[0],M_CTX_SZ,"ch_dcrami.ctx(0)");//           : ctxdatatype;
    pStr = PutToStr(pStr, pch_dcrami->ctx.arr[1],M_CTX_SZ,"ch_dcrami.ctx(1)");
    pStr = PutToStr(pStr, pch_dcrami->ctx.arr[2],M_CTX_SZ,"ch_dcrami.ctx(2)");
    pStr = PutToStr(pStr, pch_dcrami->ctx.arr[3],M_CTX_SZ,"ch_dcrami.ctx(3)");
    pStr = PutToStr(pStr, pch_dcrami->tpar.arr[0],4,"--ch_dcrami.tpar(0)");//          : cpartype;
    pStr = PutToStr(pStr, pch_dcrami->tpar.arr[1],4,"--ch_dcrami.tpar(1)");
    pStr = PutToStr(pStr, pch_dcrami->tpar.arr[2],4,"--ch_dcrami.tpar(2)");
    pStr = PutToStr(pStr, pch_dcrami->tpar.arr[3],4,"--ch_dcrami.tpar(3)");
    pStr = PutToStr(pStr, pch_dcrami->dpar.arr[0],4,"--ch_dcrami.dpar(0)");//          : cpartype;
    pStr = PutToStr(pStr, pch_dcrami->dpar.arr[1],4,"--ch_dcrami.dpar(1)");
    pStr = PutToStr(pStr, pch_dcrami->dpar.arr[2],4,"--ch_dcrami.dpar(2)");
    pStr = PutToStr(pStr, pch_dcrami->dpar.arr[3],4,"--ch_dcrami.dpar(3)");
    pStr = PutToStr(pStr, pch_dcrami->tdiag,4,"ch_dcrami.tdiag");//         : std_logic_vector(3 downto 0);
    pStr = PutToStr(pStr, pch_dcrami->ddiag,4,"ch_dcrami.ddiag");//         : std_logic_vector(3 downto 0);
    pStr = PutToStr(pStr, pch_dcrami->sdiag,4,"--ch_dcrami.sdiag");//         : std_logic_vector(3 downto 0);
    //
    pStr = PutToStr(pStr, pch_mmudci->trans_op,1,"ch_mmudci.trans_op");//std_logic; 
    pStr = PutToStr(pStr, pch_mmudci->transdata.data,32,"ch_mmudci.transdata.data");//             : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pch_mmudci->transdata.su,1,"ch_mmudci.transdata.su");//            : std_logic;
    pStr = PutToStr(pStr, pch_mmudci->transdata.read,1,"ch_mmudci.transdata.read");//            : std_logic;
    pStr = PutToStr(pStr, (uint32)pch_mmudci->transdata.isid,1,"mmudci_transdata_isid");//             : mmu_idcache;
    pStr = PutToStr(pStr, pch_mmudci->transdata.wb_data,32,"ch_mmudci.transdata.wb_data");//          : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pch_mmudci->flush_op,1,"ch_mmudci.flush_op");//std_logic;
    pStr = PutToStr(pStr, pch_mmudci->diag_op,1,"ch_mmudci.diag_op");//std_logic;
    pStr = PutToStr(pStr, pch_mmudci->wb_op,1,"ch_mmudci.wb_op");//std_logic;
    pStr = PutToStr(pStr, pch_mmudci->fsread,1,"ch_mmudci.fsread");;//std_logic;
    pStr = PutToStr(pStr, pch_mmudci->mmctrl1.e,1,"ch_mmudci.mmctrl1.e");//    : std_logic;                -- enable
    pStr = PutToStr(pStr, pch_mmudci->mmctrl1.nf,1,"ch_mmudci.mmctrl1.nf");//    : std_logic;                -- no fault
    pStr = PutToStr(pStr, pch_mmudci->mmctrl1.pso,1,"ch_mmudci.mmctrl1.pso");//    : std_logic;                -- partial store order
    pStr = PutToStr(pStr, pch_mmudci->mmctrl1.pagesize,2,"ch_mmudci.mmctrl1.pagesize");// std_logic_vector(1 downto 0);-- page size
    pStr = PutToStr(pStr, pch_mmudci->mmctrl1.ctx,M_CTX_SZ,"ch_mmudci.mmctrl1.ctx");//     : std_logic_vector(M_CTX_SZ-1 downto 0);-- context nr
    pStr = PutToStr(pStr, pch_mmudci->mmctrl1.ctxp,MMCTRL_CTXP_SZ,"ch_mmudci.mmctrl1.ctxp");//    : std_logic_vector(MMCTRL_CTXP_SZ-1 downto 0);  -- context table pointer
    pStr = PutToStr(pStr, pch_mmudci->mmctrl1.tlbdis,1,"ch_mmudci.mmctrl1.tlbdis");// : std_logic;                            -- tlb disabled
    pStr = PutToStr(pStr, pch_mmudci->mmctrl1.bar,2,"ch_mmudci.mmctrl1.bar");// : std_logic_vector(1 downto 0);         -- preplace barrier

    PrintIndexStr();

    *posBench[TB_mmu_dcache] << chStr << "\n";
  }

#ifdef DBG_mmu_dcache
  tst_mmu_dcache.ClkUpdate();
#endif
}
