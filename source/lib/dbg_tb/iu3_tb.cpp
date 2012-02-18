#include "lheaders.h"

extern leon3mp  topLeon3mp;

extern void ResetPutStr();
extern void PrintIndexStr();

//****************************************************************************
void dbg::iu3_tb(SystemOnChipIO &io)
{
  iu3 *ptst_iu3 = &topLeon3mp.pclLeon3s[0]->pclProc3->clIU3;
  
  uint32 *pin_holdn = &topLeon3mp.pclLeon3s[0]->pclProc3->holdnx;// : in  std_ulogic;
  uint32 *pinNRst             = &topLeon3mp.pclLeon3s[0]->rst.Q;
  icache_in_type *pch_ici     = &topLeon3mp.pclLeon3s[0]->pclProc3->ici;//   : out icache_in_type;
  icache_out_type *pin_ico    = &topLeon3mp.pclLeon3s[0]->pclProc3->ico;//   : in  icache_out_type;
  dcache_in_type *pch_dci     = &topLeon3mp.pclLeon3s[0]->pclProc3->dci;//   : out dcache_in_type;
  dcache_out_type *pin_dco    = &topLeon3mp.pclLeon3s[0]->pclProc3->dco;//   : in  dcache_out_type;
  iregfile_in_type *pch_rfi   = &topLeon3mp.pclLeon3s[0]->rfi;//   : out iregfile_in_type;
  iregfile_out_type *pin_rfo  = &topLeon3mp.pclLeon3s[0]->rfo;//   : in  iregfile_out_type;
  l3_irq_in_type *pin_irqi    = &topLeon3mp.irqi;//  : in  l3_irq_in_type;
  l3_irq_out_type *pch_irqo   = &topLeon3mp.irqo;//  : out l3_irq_out_type;
  l3_debug_in_type *pin_dbgi  = &topLeon3mp.dbgi.arr[0];//  : in  l3_debug_in_type;
  l3_debug_out_type *pch_dbgo = &topLeon3mp.dbgo.arr[0];//  : out l3_debug_out_type;
  mul32_in_type *pch_muli     = &topLeon3mp.pclLeon3s[0]->pclProc3->muli;//  : out mul32_in_type;
  mul32_out_type *pin_mulo    = &topLeon3mp.pclLeon3s[0]->pclProc3->mulo;//  : in  mul32_out_type;
  div32_in_type *pch_divi     = &topLeon3mp.pclLeon3s[0]->pclProc3->divi;//  : out div32_in_type;
  div32_out_type *pin_divo    = &topLeon3mp.pclLeon3s[0]->pclProc3->divo;//  : in  div32_out_type;
  fpc_out_type *pin_fpo       = &topLeon3mp.pclLeon3s[0]->fpo;//   : in  fpc_out_type;
  fpc_in_type *pch_fpi        = &topLeon3mp.pclLeon3s[0]->fpi;//   : out fpc_in_type;
  fpc_out_type *pin_cpo       = &topLeon3mp.pclLeon3s[0]->cpo;//   : in  fpc_out_type;
  fpc_in_type *pch_cpi        = &topLeon3mp.pclLeon3s[0]->cpi;//   : out fpc_in_type;
  tracebuf_out_type *pin_tbo  = &topLeon3mp.pclLeon3s[0]->tbo;//   : in  tracebuf_out_type;
  tracebuf_in_type *pch_tbi   = &topLeon3mp.pclLeon3s[0]->tbi;//   : out tracebuf_in_type;
  uint32 t_inClk;
  if((io.inClk.eClock==SClock::CLK_POSEDGE)||(io.inClk.eClock==SClock::CLK_POSITIVE)) 
    t_inClk = 1;
  else                                                                                t_inClk = 0;

#ifdef DBG_iu3
  if(io.inClk.eClock_z==SClock::CLK_POSEDGE)
  {
    holdnx = rand()&0x1;//1;

    uint32 tmp;
    //icache_out_type:
    ico.data.arr[0] = (rand()<<17) + rand();
    ico.data.arr[1] = (rand()<<17) + rand();
    ico.data.arr[2] = (rand()<<17) + rand();
    ico.data.arr[3] = (rand()<<17) + rand();
    tmp = rand();
    ico.set = (tmp>>0)&0x3;//              : std_logic_vector(1 downto 0);
    if(ico.set>CFG_ISETS-1) ico.set = CFG_ISETS-1;
    ico.mexc = (tmp>>2)&0x1;//             : std_ulogic;
    ico.hold = (tmp>>3)&0x1;//             : std_ulogic;
    ico.flush = (tmp>>4)&0x1;//            : std_ulogic;			-- flush in progress
    ico.diagrdy = (tmp>>5)&0x1;//          : std_ulogic;			-- diagnostic access ready
    ico.diagdata = (rand()<<17) + rand();//         : std_logic_vector(IDBITS-1 downto 0);-- diagnostic data
    ico.mds = (tmp>>6)&0x1;//              : std_ulogic;			-- memory data strobe
    ico.cfg = (rand()<<17) + rand();//              : std_logic_vector(31 downto 0);
    ico.idle = (tmp>>7)&0x1;//             : std_ulogic;			-- idle mode
    ico.cstat.cmiss = (tmp>>8)&0x1;//   : std_ulogic;			-- cache miss
    ico.cstat.tmiss = (tmp>>9)&0x1;//   : std_ulogic;			-- TLB miss
    ico.cstat.chold = (tmp>>10)&0x1;//   : std_ulogic;			-- cache hold
    ico.cstat.mhold = (tmp>>11)&0x1;//   : std_ulogic;			-- cache mmu hold

    //dcache_out_type:
    dco.data.arr[0] = (rand()<<17) + rand();//             : cdatatype;
    dco.data.arr[1] = (rand()<<17) + rand();
    dco.data.arr[2] = (rand()<<17) + rand();
    dco.data.arr[3] = (rand()<<17) + rand();
    tmp = rand();
    dco.set = tmp&0x3;//              : std_logic_vector(1 downto 0);
    if(dco.set>CFG_DSETS-1) dco.set = CFG_DSETS-1;
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
    dco.icdiag.ilock = (tmp>>1)&0xf;//            : std_logic_vector(0 to 3); 
    dco.icdiag.scanen = 0;//           : std_ulogic;
    dco.cache = (tmp>>5)&0x1;//            : std_ulogic;
    dco.idle = 0;//             : std_ulogic;			-- idle mode
    dco.scanen = rand()&0x1;//0;//           : std_ulogic;
    dco.testen = rand()&0x1;//0;//           : std_ulogic;
    
    //iregfile_out_type
    rfo.data1 = (rand()<<17) + rand();//     : std_logic_vector(RDBITS-1 downto 0); -- read data 1
    rfo.data2 = (rand()<<17) + rand();//     : std_logic_vector(RDBITS-1 downto 0); -- read data 2

    //l3_irq_in_type
    tmp = rand();
    irqi.irl = tmp&0xf;//     : std_logic_vector(3 downto 0);
    irqi.rst = (tmp>>4)&0x1;//     : std_ulogic;
    irqi.run = (tmp>>5)&0x1;//     : std_ulogic;
    irqi.rstvec = rand()<<12;//  : std_logic_vector(31 downto 12);
    irqi.iact = (tmp>>6)&0x1;//        : std_ulogic;
    irqi.index = 1;//   :

    //l3_debug_in_type
    tmp = rand();
    dbgi.dsuen = 1;//(tmp>>0)&0x1;//   : std_ulogic;  -- DSU enable
    dbgi.denable = (tmp>>1)&0x1;// : std_ulogic;  -- diagnostic register access enable
    dbgi.dbreak = (tmp>>2)&0x1;//  : std_ulogic;  -- debug break-in
    dbgi.step = (tmp>>3)&0x1;//    : std_ulogic;  -- single step    
    dbgi.halt = (tmp>>4)&0x1;//    : std_ulogic;  -- halt processor
    dbgi.reset = (tmp>>5)&0x1;//   : std_ulogic;  -- reset processor
    dbgi.dwrite = (tmp>>6)&0x1;//  : std_ulogic;  -- read/write
    dbgi.daddr = ((rand()<<17) + (rand()<<2))&0xFFFFFC;//   : std_logic_vector(23 downto 2); -- diagnostic address
    dbgi.ddata = (rand()<<17) + rand();//   : std_logic_vector(31 downto 0); -- diagnostic data
    dbgi.btrapa = (tmp>>7)&0x1;//  : std_ulogic;	   -- break on IU trap
    dbgi.btrape = (tmp>>8)&0x1;//  : std_ulogic;	-- break on IU trap
    dbgi.berror = (tmp>>9)&0x1;//  : std_ulogic;	-- break on IU error mode
    dbgi.bwatch = (tmp>>10)&0x1;//  : std_ulogic;	-- break on IU watchpoint
    dbgi.bsoft = (tmp>>11)&0x1;//   : std_ulogic;	-- break on software breakpoint (TA 1)
    dbgi.tenable = (tmp>>12)&0x1;// : std_ulogic;
    dbgi.timer = ((rand()<<17) + (rand()))&0x7FFFFFFF;//   :  std_logic_vector(30 downto 0);
    
    //mul32_out_type
    tmp = rand();
    mulo.ready = (tmp>>0)&0x1;//           : std_logic;
    mulo.nready = (tmp>>1)&0x1;//          : std_logic;
    mulo.icc = (tmp>>2)&0xf;//             : std_logic_vector(3 downto 0); -- ICC
    mulo.result = ((uint64)rand()<<37) + rand();//          : std_logic_vector(63 downto 0); -- mul result

    //div32_out_type
    tmp = rand();
    divo.ready = (tmp>>0)&0x1;//           : std_logic;
    divo.nready = (tmp>>1)&0x1;//          : std_logic;
    divo.icc = (tmp>>2)&0xf;//             : std_logic_vector(3 downto 0); -- ICC
    divo.result = (rand()<<17) + rand();//          : std_logic_vector(31 downto 0); -- div result

    //
    fpo = fpc_out_none;
    fpo.dbg.data  = (rand()<<17) + rand();
    
    //
    cpo.data   = 0;
    cpo.exc    = 0;
    cpo.cc     = 0;
    cpo.ccv    = 0;
    cpo.ldlock = 0;
    cpo.holdn  = 0;
    cpo.dbg.data    = 0;

   
    //tracebuf_out_type
    //             : std_logic_vector(127 downto 0);
    tbo.data[0] = ((uint64)rand()<<49) + ((uint64)rand()<<33) + ((uint64)rand()<<17) + rand();
    tbo.data[1] = ((uint64)rand()<<49) + ((uint64)rand()<<33) + ((uint64)rand()<<17) + rand();

    
    // dbgexc() function:
    tmp = rand();
    tst_iu3.in_r.f.pc  = (rand()<<17) + (rand()<<CFG_PCLOW);
    tst_iu3.in_r.w.s.cwp = rand() & MSK32(NWINLOG2-1,0);
    tst_iu3.in_r.w.s.icc = rand()&0xF;
    tst_iu3.in_r.w.s.tt = rand()&0xFF;
    tst_iu3.in_r.w.s.tba = ((rand()<<15) + rand())&0xFFFFF;
    tst_iu3.in_r.w.s.wim = rand() & MSK32(CFG_NWIN-1,0);
    tst_iu3.in_r.w.s.pil = rand()&0xF;
    tst_iu3.in_r.w.s.ec = tmp&0x1;
    tst_iu3.in_r.w.s.ef = (tmp>>1)&0x1;
    tst_iu3.in_r.w.s.ps = (tmp>>2)&0x1;
    tst_iu3.in_r.w.s.s = (tmp>>3)&0x1;
    tst_iu3.in_r.w.s.et = (tmp>>4)&0x1;
    tst_iu3.in_r.w.s.y  = (rand()<<17) + rand();
    tst_iu3.in_r.w.s.asr18  = (rand()<<17) + rand();
    tst_iu3.in_r.w.s.svt = (tmp>>5)&0x1;
    tst_iu3.in_r.w.s.dwt = (tmp>>6)&0x1;
    tst_iu3.in_r.w.s.dbp = (tmp>>7)&0x1;
    tst_iu3.in_r.w.result  = (rand()<<17) + rand();

    tst_iu3.in_r.d.pc= (rand()<<17) + (rand()<<CFG_PCLOW);//    : pctype;
    tst_iu3.in_r.d.inst.arr[0] = (rand()<<17) + rand();//  : icdtype;
    tst_iu3.in_r.d.inst.arr[1] = (rand()<<17) + rand();
    tst_iu3.in_r.d.cwp = rand()&MSK32(NWINLOG2-1,0);//   : cwptype;
    tst_iu3.in_r.d.set = rand()&MSK32(ISETMSB,0);//   : std_logic_vector(ISETMSB downto 0);
    tst_iu3.in_r.d.mexc = rand()&0x1;//  : std_ulogic;
    tst_iu3.in_r.d.cnt = rand()&0x3;//   : std_logic_vector(1 downto 0);
    tst_iu3.in_r.d.pv = rand()&0x1;//    : std_ulogic;
    tst_iu3.in_r.d.annul = rand()&0x1;// : std_ulogic;
    tst_iu3.in_r.d.inull = rand()&0x1;// : std_ulogic;
    tst_iu3.in_r.d.step = rand()&0x1;//  : std_ulogic;        
    tst_iu3.in_r.d.divrdy = rand()&0x1;//: std_ulogic;        

    tst_iu3.in_r.a.ctrl.pc = (rand()<<17) + (rand()<<CFG_PCLOW);
    tst_iu3.in_r.a.ctrl.inst = ((rand()<<17) + rand());
    tst_iu3.in_r.a.ctrl.cnt = rand()&0x3;
    tst_iu3.in_r.a.ctrl.rd = rand()&MSK32(RFBITS-1,0);//    : rfatype;=std_logic_vector(RFBITS-1 downto 0)
    tst_iu3.in_r.a.ctrl.tt = rand()&0x3f;//    : std_logic_vector(5 downto 0);
    tst_iu3.in_r.a.ctrl.trap = rand()&0x1;//  : std_ulogic;
    tst_iu3.in_r.a.ctrl.annul = rand()&0x1;// : std_ulogic;
    tst_iu3.in_r.a.ctrl.wreg = rand()&0x1;//  : std_ulogic;
    tst_iu3.in_r.a.ctrl.wicc = rand()&0x1;//  : std_ulogic;
    tst_iu3.in_r.a.ctrl.wy = rand()&0x1;//    : std_ulogic;
    tst_iu3.in_r.a.ctrl.ld = rand()&0x1;//    : std_ulogic;
    tst_iu3.in_r.a.ctrl.pv = rand()&0x1;//    : std_ulogic;
    tst_iu3.in_r.a.ctrl.rett = rand()&0x1;//  : std_ulogic;
    tst_iu3.in_r.a.rs1 = rand()&0x1f;//   : std_logic_vector(4 downto 0);
    tst_iu3.in_r.a.rfa1 = rand()&MSK32(RFBITS-1,0);
    tst_iu3.in_r.a.rfa2 = rand()&MSK32(RFBITS-1,0);// : rfatype;
    tst_iu3.in_r.a.rsel1 = rand()&0x7;
    tst_iu3.in_r.a.rsel2 = rand()&0x7;// : std_logic_vector(2 downto 0);
    tst_iu3.in_r.a.rfe1 = rand()&0x1;
    tst_iu3.in_r.a.rfe2 = rand()&0x1;// : std_ulogic;
    tst_iu3.in_r.a.cwp = rand()&MSK32(NWINLOG2-1,0);//   : cwptype;
    tst_iu3.in_r.a.imm= (rand()<<17) + rand();//   : word;
    tst_iu3.in_r.a.ldcheck1 = rand()&0x1;// : std_ulogic;
    tst_iu3.in_r.a.ldcheck2 = rand()&0x1;// : std_ulogic;
    tst_iu3.in_r.a.ldchkra = rand()&0x1;// : std_ulogic;
    tst_iu3.in_r.a.ldchkex = rand()&0x1;// : std_ulogic;
    tst_iu3.in_r.a.su = rand()&0x1;// : std_ulogic;
    tst_iu3.in_r.a.et = rand()&0x1;// : std_ulogic;
    tst_iu3.in_r.a.wovf = rand()&0x1;// : std_ulogic;
    tst_iu3.in_r.a.wunf = rand()&0x1;// : std_ulogic;
    tst_iu3.in_r.a.ticc = rand()&0x1;// : std_ulogic;
    tst_iu3.in_r.a.jmpl = rand()&0x1;// : std_ulogic;
    tst_iu3.in_r.a.step = rand()&0x1;//  : std_ulogic;            
    tst_iu3.in_r.a.mulstart = rand()&0x1;// : std_ulogic;            
    tst_iu3.in_r.a.divstart = rand()&0x1;// : std_ulogic;            
    tst_iu3.in_r.a.bp = rand()&0x1;
    tst_iu3.in_r.a.nobp = rand()&0x1;// : std_ulogic;
    
    tst_iu3.in_r.e.ctrl.pc  = (rand()<<17) + (rand()<<CFG_PCLOW);
    tst_iu3.in_r.e.ctrl.inst = (rand()<<17) + rand();
    tst_iu3.in_r.e.ctrl.cnt = rand()&0x3;
    tst_iu3.in_r.e.ctrl.rd = rand()&MSK32(RFBITS-1,0);//    : rfatype;=std_logic_vector(RFBITS-1 downto 0)
    tst_iu3.in_r.e.ctrl.tt = rand()&0x3f;//    : std_logic_vector(5 downto 0);
    tst_iu3.in_r.e.ctrl.trap = rand()&0x1;//  : std_ulogic;
    tst_iu3.in_r.e.ctrl.annul = rand()&0x1;// : std_ulogic;
    tst_iu3.in_r.e.ctrl.wreg = rand()&0x1;//  : std_ulogic;
    tst_iu3.in_r.e.ctrl.wicc = rand()&0x1;//  : std_ulogic;
    tst_iu3.in_r.e.ctrl.wy = rand()&0x1;//    : std_ulogic;
    tst_iu3.in_r.e.ctrl.ld = rand()&0x1;//    : std_ulogic;
    tst_iu3.in_r.e.ctrl.pv = rand()&0x1;//    : std_ulogic;
    tst_iu3.in_r.e.ctrl.rett = rand()&0x1;//  : std_ulogic;
    tst_iu3.in_r.e.op1 = (rand()<<17) + rand();//    : word;
    tst_iu3.in_r.e.op2 = (rand()<<17) + rand();//    : word;
    tst_iu3.in_r.e.aluop = rand()&0x7;;//  : std_logic_vector(2 downto 0);  	-- Alu operation
    tst_iu3.in_r.e.alusel = rand()&0x3;;// : std_logic_vector(1 downto 0);  	-- Alu result select
    tst_iu3.in_r.e.aluadd = rand()&0x1;;// : std_ulogic;
    tst_iu3.in_r.e.alucin = rand()&0x1;;// : std_ulogic;
    tst_iu3.in_r.e.ldbp1 = rand()&0x1;
    tst_iu3.in_r.e.ldbp2 = rand()&0x1;// : std_ulogic;
    tst_iu3.in_r.e.invop2 = rand()&0x1;// : std_ulogic;
    tst_iu3.in_r.e.shcnt = rand()&0x1F;//  : std_logic_vector(4 downto 0);  	-- shift count
    tst_iu3.in_r.e.sari = rand()&0x1;//   : std_ulogic;				-- shift msb
    tst_iu3.in_r.e.shleft = rand()&0x1;// : std_ulogic;				-- shift left/right
    tst_iu3.in_r.e.ymsb;//   : std_ulogic;				-- shift left/right
    tst_iu3.in_r.e.rd = rand()&0x1f;// 	   : std_logic_vector(4 downto 0);
    tst_iu3.in_r.e.jmpl = rand()&0x1;//   : std_ulogic;
    tst_iu3.in_r.e.su = rand()&0x1;//     : std_ulogic;
    tst_iu3.in_r.e.et = rand()&0x1;//     : std_ulogic;
    tst_iu3.in_r.e.cwp = rand()&MSK32(NWINLOG2-1,0);//    : cwptype;
    tst_iu3.in_r.e.icc = rand()&0xf;//    : std_logic_vector(3 downto 0);
    tst_iu3.in_r.e.mulstep = rand()&0x1;//: std_ulogic;            
    tst_iu3.in_r.e.mul = rand()&0x1;//    : std_ulogic;            
    tst_iu3.in_r.e.mac = rand()&0x1;//    : std_ulogic;
    tst_iu3.in_r.e.bp = rand()&0x1;//     : std_ulogic;
    
    tst_iu3.in_r.m.ctrl.pc  = (rand()<<17) + (rand()<<CFG_PCLOW);
    tst_iu3.in_r.m.ctrl.inst = (rand()<<17) + rand();
    tst_iu3.in_r.m.ctrl.cnt = rand()&0x3;
    tst_iu3.in_r.m.ctrl.rd = rand()&MSK32(RFBITS-1,0);//    : rfatype;=std_logic_vector(RFBITS-1 downto 0)
    tst_iu3.in_r.m.ctrl.tt = rand()&0x3f;//    : std_logic_vector(5 downto 0);
    tst_iu3.in_r.m.ctrl.trap = rand()&0x1;//  : std_ulogic;
    tst_iu3.in_r.m.ctrl.annul = rand()&0x1;// : std_ulogic;
    tst_iu3.in_r.m.ctrl.wreg = rand()&0x1;//  : std_ulogic;
    tst_iu3.in_r.m.ctrl.wicc = rand()&0x1;//  : std_ulogic;
    tst_iu3.in_r.m.ctrl.wy = rand()&0x1;//    : std_ulogic;
    tst_iu3.in_r.m.ctrl.ld = rand()&0x1;//    : std_ulogic;
    tst_iu3.in_r.m.ctrl.pv = rand()&0x1;//    : std_ulogic;
    tst_iu3.in_r.m.ctrl.rett = rand()&0x1;//  : std_ulogic;
    tst_iu3.in_r.m.result = (rand()<<17) + rand();// : word;
    tst_iu3.in_r.m.y = (rand()<<17) + rand();//      : word;
    tst_iu3.in_r.m.icc = rand()&0xf;//    : std_logic_vector(3 downto 0);
    tst_iu3.in_r.m.nalign = rand()&0x1;// : std_ulogic;
    tst_iu3.in_r.m.dci.Signed = rand()&0x1;
    tst_iu3.in_r.m.dci.enaddr = rand()&0x1;
    tst_iu3.in_r.m.dci.read = rand()&0x1;
    tst_iu3.in_r.m.dci.write = rand()&0x1;
    tst_iu3.in_r.m.dci.lock = rand()&0x1;
    tst_iu3.in_r.m.dci.dsuen = rand()&0x1;// : std_ulogic;
    tst_iu3.in_r.m.dci.size = rand()&0x3;// : std_logic_vector(1 downto 0);
    tst_iu3.in_r.m.dci.asi = rand()&0xff;//  : std_logic_vector(7 downto 0);    
    tst_iu3.in_r.m.werr = rand()&0x1;//   : std_ulogic;
    tst_iu3.in_r.m.wcwp = rand()&0x1;//   : std_ulogic;
    tst_iu3.in_r.m.irqen = rand()&0x1;//  : std_ulogic;
    tst_iu3.in_r.m.irqen2 = rand()&0x1;// : std_ulogic;
    tst_iu3.in_r.m.mac = rand()&0x1;//    : std_ulogic;
    tst_iu3.in_r.m.divz = rand()&0x1;//   : std_ulogic;
    tst_iu3.in_r.m.su = rand()&0x1;//     : std_ulogic;
    tst_iu3.in_r.m.mul = rand()&0x1;//    : std_ulogic;
    tst_iu3.in_r.m.casa = rand()&0x1;//   : std_ulogic;
    tst_iu3.in_r.m.casaz = rand()&0x1;//  : std_ulogic;
    
    tst_iu3.in_r.x.ctrl.pc  = (rand()<<17) + (rand()<<CFG_PCLOW);
    tst_iu3.in_r.x.ctrl.inst = (2<<30)|((rand()&0x1f)<<25)|((rand()&0x3f)<<19) | rand();//(rand()<<17) + rand();
    tst_iu3.in_r.x.ctrl.cnt = rand()&0x3;
    tst_iu3.in_r.x.ctrl.rd = rand()&MSK32(RFBITS-1,0);//    : rfatype;=std_logic_vector(RFBITS-1 downto 0)
    tst_iu3.in_r.x.ctrl.tt = rand()&0x3f;//    : std_logic_vector(5 downto 0);
    tst_iu3.in_r.x.ctrl.trap = rand()&0x1;//  : std_ulogic;
    tst_iu3.in_r.x.ctrl.annul = rand()&0x1;// : std_ulogic;
    tst_iu3.in_r.x.ctrl.wreg = rand()&0x1;//  : std_ulogic;
    tst_iu3.in_r.x.ctrl.wicc = rand()&0x1;//  : std_ulogic;
    tst_iu3.in_r.x.ctrl.wy = rand()&0x1;//    : std_ulogic;
    tst_iu3.in_r.x.ctrl.ld = rand()&0x1;//    : std_ulogic;
    tst_iu3.in_r.x.ctrl.pv = rand()&0x1;//    : std_ulogic;
    tst_iu3.in_r.x.ctrl.rett = rand()&0x1;//  : std_ulogic;
    tst_iu3.in_r.x.result = (rand()<<17) + rand();// : word;
    tst_iu3.in_r.x.y = (rand()<<17) + rand();//      : word;
    tst_iu3.in_r.x.icc = rand()&0xf;//    : std_logic_vector( 3 downto 0);
    tst_iu3.in_r.x.annul_all = rand()&0x1;
    tst_iu3.in_r.x.data.arr[0]  = (rand()<<17) + rand();
    tst_iu3.in_r.x.data.arr[1]  = (rand()<<17) + rand();
    tst_iu3.in_r.x.set = rand()&MSK32(DSETMSB,0);//    : std_logic_vector(DSETMSB downto 0);
    tst_iu3.in_r.x.mexc = rand()&0x1;//   : std_ulogic;
    tst_iu3.in_r.x.dci.Signed = rand()&0x1;
    tst_iu3.in_r.x.dci.enaddr = rand()&0x1;
    tst_iu3.in_r.x.dci.read = rand()&0x1;
    tst_iu3.in_r.x.dci.write = rand()&0x1;
    tst_iu3.in_r.x.dci.lock = rand()&0x1;
    tst_iu3.in_r.x.dci.dsuen = rand()&0x1;// : std_ulogic;
    tst_iu3.in_r.x.dci.size = rand()&0x3;// : std_logic_vector(1 downto 0);
    tst_iu3.in_r.x.dci.asi = rand()&0xff;//  : std_logic_vector(7 downto 0);    
    tst_iu3.in_r.x.laddr = rand()&0x3;//  : std_logic_vector(1 downto 0);
    tst_iu3.in_r.x.rstate = exception_state(rand()&0x3);//dsu2;                    // !!!!! WARNING!!!!!!! 
    tst_iu3.in_r.x.npc = rand()&0x7;//    : std_logic_vector(2 downto 0);
    tst_iu3.in_r.x.intack = rand()&0x1;// : std_ulogic;
    tst_iu3.in_r.x.ipend = rand()&0x1;//  : std_ulogic;
    tst_iu3.in_r.x.mac = rand()&0x1;//    : std_ulogic;
    tst_iu3.in_r.x.debug = rand()&0x1;//  : std_ulogic;
    tst_iu3.in_r.x.nerror = rand()&0x1;// : std_ulogic;
    
    tst_iu3.in_wpr.arr[0].addr  = (rand()<<17) + (rand()<<CFG_PCLOW);
    tst_iu3.in_wpr.arr[0].mask  = (rand()<<17) + (rand()<<CFG_PCLOW);
    tst_iu3.in_wpr.arr[0].exec = (tmp>>8)&0x1;
    tst_iu3.in_wpr.arr[0].load = (tmp>>9)&0x1;
    tst_iu3.in_wpr.arr[0].store = (tmp>>10)&0x1;
    tmp = rand();
    tst_iu3.in_wpr.arr[1].addr  = (rand()<<17) + (rand()<<CFG_PCLOW);
    tst_iu3.in_wpr.arr[1].mask  = (rand()<<17) + (rand()<<CFG_PCLOW);
    tst_iu3.in_wpr.arr[1].exec = (tmp>>8)&0x1;
    tst_iu3.in_wpr.arr[1].load = (tmp>>9)&0x1;
    tst_iu3.in_wpr.arr[1].store = (tmp>>10)&0x1;
    tmp = rand();
    tst_iu3.in_wpr.arr[2].addr  = (rand()<<17) + (rand()<<CFG_PCLOW);
    tst_iu3.in_wpr.arr[2].mask  = (rand()<<17) + (rand()<<CFG_PCLOW);
    tst_iu3.in_wpr.arr[2].exec = (tmp>>8)&0x1;
    tst_iu3.in_wpr.arr[2].load = (tmp>>9)&0x1;
    tst_iu3.in_wpr.arr[2].store = (tmp>>10)&0x1;
    tmp = rand();
    tst_iu3.in_wpr.arr[3].addr  = (rand()<<17) + (rand()<<CFG_PCLOW);
    tst_iu3.in_wpr.arr[3].mask  = (rand()<<17) + (rand()<<CFG_PCLOW);
    tst_iu3.in_wpr.arr[3].exec = (tmp>>8)&0x1;
    tst_iu3.in_wpr.arr[3].load = (tmp>>9)&0x1;
    tst_iu3.in_wpr.arr[3].store = (tmp>>10)&0x1;

    tmp = rand();
    tst_iu3.in_ir.addr = (rand()<<17) + (rand()<<CFG_PCLOW);
    tst_iu3.in_ir.pwd = rand()&0x1;
    tst_iu3.in_dsur.asi = tmp&0xff;
    tmp = rand();
    #define TBUFBITS  (10+log2[CFG_ITBSZ]-4)//: integer := 10 + log2(tbuf) - 4;
    tst_iu3.in_dsur.tbufcnt = BITS32(tmp,TBUFBITS-1,0);
    tmp = rand();
    tst_iu3.in_dsur.err = tmp&0x1;
    tst_iu3.in_dsur.tt = (tmp>>1)&0xFF;
    tst_iu3.in_dsur.crdy = rand()&0x6;
  }

  tst_iu3.Update(io.inClk, io.inNRst, holdnx, ici, ico, dci, dco, rfi, rfo, irqi, irqo,
                 dbgi, dbgo, muli, mulo, divi, divo, fpo, fpi, cpo, cpi, tbo, tbi, io.inClk);

  ptst_iu3 = &tst_iu3;
  pinNRst  = &io.inNRst;
  pin_holdn = &holdnx;// : in  std_ulogic;
  pch_ici = &ici;
  pin_ico = &ico;
  pch_dci = &dci;
  pin_dco = &dco;
  pch_rfi = &rfi;
  pin_rfo = &rfo;
  pin_irqi = &irqi;
  pch_irqo = &irqo;
  pin_dbgi = &dbgi;
  pch_dbgo = &dbgo;
  pch_muli = &muli;
  pin_mulo = &mulo;
  pch_divi = &divi;
  pin_divo = &divo;
  pin_fpo = &fpo;
  pch_fpi = &fpi;
  pin_cpo = &cpo;
  pch_cpi = &cpi;
  pin_tbo = &tbo;
  pch_tbi = &tbi;
#else
  ptst_iu3->in_r = ptst_iu3->rin; //single function debug purpose
#endif

  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';

    ResetPutStr();
    
    // inputs:
    pStr = PutToStr(pStr, *pinNRst, 1, "inNRst");
    pStr = PutToStr(pStr, *pin_holdn, 1, "holdnx");
    pStr = PutToStr(pStr, pin_ico->data.arr[0],IDBITS,"ico.data(0)");
    pStr = PutToStr(pStr, pin_ico->data.arr[1],IDBITS,"ico.data(1)");
    pStr = PutToStr(pStr, pin_ico->data.arr[2],IDBITS,"ico.data(2)");
    pStr = PutToStr(pStr, pin_ico->data.arr[3],IDBITS,"ico.data(3)");
    pStr = PutToStr(pStr, pin_ico->set,2,"ico.set");//              : std_logic_vector(1 downto 0);
    pStr = PutToStr(pStr, pin_ico->mexc,1,"ico.mexc");//             : std_ulogic;
    pStr = PutToStr(pStr, pin_ico->hold,1,"ico.hold");//             : std_ulogic;
    pStr = PutToStr(pStr, pin_ico->flush,1,"ico.flush");//            : std_ulogic;			-- flush in progress
    pStr = PutToStr(pStr, pin_ico->diagrdy,1,"ico.diagrdy");//          : std_ulogic;			-- diagnostic access ready
    pStr = PutToStr(pStr, pin_ico->diagdata,IDBITS,"ico.diagdata");//         : std_logic_vector(IDBITS-1 downto 0);-- diagnostic data
    pStr = PutToStr(pStr, pin_ico->mds,1,"ico.mds");//              : std_ulogic;			-- memory data strobe
    pStr = PutToStr(pStr, pin_ico->cfg,32,"ico.cfg");//              : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pin_ico->idle,1,"ico.idle");//             : std_ulogic;			-- idle mode
    pStr = PutToStr(pStr, pin_ico->cstat.cmiss,1,"ico.cstat.cmiss");//            : l3_cstat_type;
    pStr = PutToStr(pStr, pin_ico->cstat.tmiss,1,"ico.cstat.tmiss");//   : std_ulogic;			-- TLB miss
    pStr = PutToStr(pStr, pin_ico->cstat.chold,1,"ico.cstat.chold");//   : std_ulogic;			-- cache hold
    pStr = PutToStr(pStr, pin_ico->cstat.mhold,1,"ico.cstat.mhold");//   : std_ulogic;			-- cache mmu hold
    //dcache_out_type *pDco = &dco;  
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
    pStr = PutToStr(pStr, pin_dco->icdiag.ilock,4,"dco.icdiag.ilock");//            : std_logic_vector(0 to 3); 
    pStr = PutToStr(pStr, pin_dco->icdiag.scanen,1,"dco.icdiag.scanen");//           : std_ulogic;
    pStr = PutToStr(pStr, pin_dco->cache,1,"dco.cache");//            : std_ulogic;
    pStr = PutToStr(pStr, pin_dco->idle,1,"dco.idle");//             : std_ulogic;			-- idle mode
    pStr = PutToStr(pStr, pin_dco->scanen,1,"dco.scanen");//           : std_ulogic;
    pStr = PutToStr(pStr, pin_dco->testen,1,"dco.testen");//           : std_ulogic;
    pStr = PutToStr(pStr, pin_rfo->data1,RDBITS,"rfo.data1");//     : std_logic_vector(RDBITS-1 downto 0); -- read data 1
    pStr = PutToStr(pStr, pin_rfo->data2,RDBITS,"rfo.data2");//     : std_logic_vector(RDBITS-1 downto 0); -- read data 2
    pStr = PutToStr(pStr, pin_irqi->irl,4,"irqi.irl");//     : std_logic_vector(3 downto 0);
    pStr = PutToStr(pStr, pin_irqi->rst,1,"irqi.rst");//     : std_ulogic;
    pStr = PutToStr(pStr, pin_irqi->run,1,"irqi.run");//     : std_ulogic;
    pStr = PutToStr(pStr, pin_irqi->rstvec>>12,20,"irqi.rstvec");//  : std_logic_vector(31 downto 12);
    pStr = PutToStr(pStr, pin_irqi->iact,1,"irqi.iact");//        : std_ulogic;
    pStr = PutToStr(pStr, pin_irqi->index,4,"irqi.index");//   :
    pStr = PutToStr(pStr, pin_dbgi->dsuen,1,"dbgi.dsuen");//   : std_ulogic;  -- DSU enable
    pStr = PutToStr(pStr, pin_dbgi->denable,1,"dbgi.denable");// : std_ulogic;  -- diagnostic register access enable
    pStr = PutToStr(pStr, pin_dbgi->dbreak,1,"dbgi.dbreak");//  : std_ulogic;  -- debug break-in
    pStr = PutToStr(pStr, pin_dbgi->step,1,"dbgi.step");//    : std_ulogic;  -- single step    
    pStr = PutToStr(pStr, pin_dbgi->halt,1,"dbgi.halt");//    : std_ulogic;  -- halt processor
    pStr = PutToStr(pStr, pin_dbgi->reset,1,"dbgi.reset");//   : std_ulogic;  -- reset processor
    pStr = PutToStr(pStr, pin_dbgi->dwrite,1,"dbgi.dwrite");//  : std_ulogic;  -- read/write
    pStr = PutToStr(pStr, pin_dbgi->daddr>>2,22,"dbgi.daddr");//   : std_logic_vector(23 downto 2); -- diagnostic address
    pStr = PutToStr(pStr, pin_dbgi->ddata,32,"dbgi.ddata");//   : std_logic_vector(31 downto 0); -- diagnostic data
    pStr = PutToStr(pStr, pin_dbgi->btrapa,1,"dbgi.btrapa");//  : std_ulogic;	   -- break on IU trap
    pStr = PutToStr(pStr, pin_dbgi->btrape,1,"dbgi.btrape");//  : std_ulogic;	-- break on IU trap
    pStr = PutToStr(pStr, pin_dbgi->berror,1,"dbgi.berror");//  : std_ulogic;	-- break on IU error mode
    pStr = PutToStr(pStr, pin_dbgi->bwatch,1,"dbgi.bwatch");//  : std_ulogic;	-- break on IU watchpoint
    pStr = PutToStr(pStr, pin_dbgi->bsoft,1,"dbgi.bsoft");//   : std_ulogic;	-- break on software breakpoint (TA 1)
    pStr = PutToStr(pStr, pin_dbgi->tenable,1,"dbgi.tenable");// : std_ulogic;
    pStr = PutToStr(pStr, pin_dbgi->timer,31,"dbgi.timer");//   :  std_logic_vector(30 downto 0);
    pStr = PutToStr(pStr, pin_mulo->ready,1,"mulo.ready");//           : std_logic;
    pStr = PutToStr(pStr, pin_mulo->nready,1,"mulo.nready");//          : std_logic;
    pStr = PutToStr(pStr, pin_mulo->icc,4,"mulo.icc");//             : std_logic_vector(3 downto 0); -- ICC
    pStr = PutToStr(pStr, pin_mulo->result,64,"mulo.result");//          : std_logic_vector(63 downto 0); -- mul result
    pStr = PutToStr(pStr, pin_divo->ready,1,"divo.ready");//           : std_logic;
    pStr = PutToStr(pStr, pin_divo->nready,1,"divo.nready");//          : std_logic;
    pStr = PutToStr(pStr, pin_divo->icc,4,"divo.icc");//             : std_logic_vector(3 downto 0); -- ICC
    pStr = PutToStr(pStr, pin_divo->result,32,"divo.result");//          : std_logic_vector(31 downto 0); -- div result
    pStr = PutToStr(pStr, pin_tbo->data[0],64,"tbo.data(63 downto 0)");
    pStr = PutToStr(pStr, pin_tbo->data[1],64,"tbo.data(127 downto 64)");
    // outputs:
    pStr = PutToStr(pStr, pch_ici->rpc,32,"ch_ici.rpc");//              : std_logic_vector(31 downto 0); -- raw address (npc)
    pStr = PutToStr(pStr, pch_ici->fpc,32,"ch_ici.fpc");//              : std_logic_vector(31 downto 0); -- latched address (fpc)
    pStr = PutToStr(pStr, pch_ici->dpc,32,"ch_ici.dpc");//              : std_logic_vector(31 downto 0); -- latched address (dpc)
    pStr = PutToStr(pStr, pch_ici->rbranch,1,"ch_ici.rbranch");//          : std_ulogic;      -- Instruction branch
    pStr = PutToStr(pStr, pch_ici->fbranch,1,"ch_ici.fbranch");//          : std_ulogic;      -- Instruction branch
    pStr = PutToStr(pStr, pch_ici->inull,1,"ch_ici.inull");//            : std_ulogic;      -- instruction nullify
    pStr = PutToStr(pStr, pch_ici->su,1,"ch_ici.su");//               : std_ulogic;      -- super-user
    pStr = PutToStr(pStr, pch_ici->flush,1,"ch_ici.flush");//            : std_ulogic;      -- flush icache
    pStr = PutToStr(pStr, pch_ici->flushl,1,"ch_ici.flushl");//           : std_ulogic;                        -- flush line
    pStr = PutToStr(pStr, (pch_ici->fline>>3),29,"ch_ici.fline");//            : std_logic_vector(31 downto 3);     -- flush line offset
    pStr = PutToStr(pStr, pch_ici->pnull,1,"--ch_ici.pnull");//            : std_ulogic;
    //dcache_in_type *pDci = &dci;
    pStr = PutToStr(pStr, pch_dci->asi,8,"ch_dci.asi");//              : std_logic_vector(7 downto 0); 
    pStr = PutToStr(pStr, pch_dci->maddress,32,"ch_dci.maddress");//         : std_logic_vector(31 downto 0); 
    pStr = PutToStr(pStr, pch_dci->eaddress,32,"ch_dci.eaddress");//         : std_logic_vector(31 downto 0); 
    pStr = PutToStr(pStr, pch_dci->edata,32,"ch_dci.edata");//            : std_logic_vector(31 downto 0); 
    pStr = PutToStr(pStr, pch_dci->size,2,"ch_dci.size");//             : std_logic_vector(1 downto 0);
    pStr = PutToStr(pStr, pch_dci->enaddr,1,"ch_dci.enaddr");//           : std_ulogic;
    pStr = PutToStr(pStr, pch_dci->eenaddr,1,"ch_dci.eenaddr");//          : std_ulogic;
    pStr = PutToStr(pStr, pch_dci->nullify,1,"ch_dci.nullify");//          : std_ulogic;
    pStr = PutToStr(pStr, pch_dci->lock,1,"ch_dci.lock");//             : std_ulogic;
    pStr = PutToStr(pStr, pch_dci->read,1,"ch_dci.read");//             : std_ulogic;
    pStr = PutToStr(pStr, pch_dci->write,1,"ch_dci.write");//            : std_ulogic;
    pStr = PutToStr(pStr, pch_dci->flush,1,"ch_dci.flush");//            : std_ulogic;
    pStr = PutToStr(pStr, pch_dci->flushl,1,"--ch_dci.flushl");//           : std_ulogic;                        -- flush line  
    pStr = PutToStr(pStr, pch_dci->dsuen,1,"ch_dci.dsuen");//            : std_ulogic;
    pStr = PutToStr(pStr, pch_dci->msu,1,"ch_dci.msu");//              : std_ulogic;                   -- memory stage supervisor
    pStr = PutToStr(pStr, pch_dci->esu,1,"ch_dci.esu");//              : std_ulogic;                   -- execution stage supervisor
    pStr = PutToStr(pStr, pch_dci->intack,1,"ch_dci.intack");//           : std_ulogic;
    //iregfile_in_type &rfi,//   : out iregfile_in_type;
    pStr = PutToStr(pStr, pch_rfi->raddr1,10,"ch_rfi.raddr1");//  : std_logic_vector(9 downto 0); -- read address 1
    pStr = PutToStr(pStr, pch_rfi->raddr2,10,"ch_rfi.raddr2");//  : std_logic_vector(9 downto 0); -- read address 2
    pStr = PutToStr(pStr, pch_rfi->waddr,10,"ch_rfi.waddr");//   : std_logic_vector(9 downto 0); -- write address
    pStr = PutToStr(pStr, pch_rfi->wdata,32,"ch_rfi.wdata");//   : std_logic_vector(31 downto 0); -- write data
    pStr = PutToStr(pStr, pch_rfi->ren1,1,"ch_rfi.ren1");//          : std_ulogic;      -- read 1 enable
    pStr = PutToStr(pStr, pch_rfi->ren2,1,"ch_rfi.ren2");//          : std_ulogic;      -- read 2 enable
    pStr = PutToStr(pStr, pch_rfi->wren,1,"ch_rfi.wren");//          : std_ulogic;      -- write enable
    pStr = PutToStr(pStr, pch_rfi->diag,4,"ch_rfi.diag");//    : std_logic_vector(3 downto 0); -- write data
    //l3_irq_out_type &irqo,//  : out l3_irq_out_type;
    pStr = PutToStr(pStr, pch_irqo->intack,1,"ch_irqo.intack");//  : std_ulogic;
    pStr = PutToStr(pStr, pch_irqo->irl,4,"ch_irqo.irl");//   : std_logic_vector(3 downto 0);
    pStr = PutToStr(pStr, pch_irqo->pwd,1,"ch_irqo.pwd");//         : std_ulogic;
    pStr = PutToStr(pStr, pch_irqo->fpen,1,"--ch_irqo.fpen");//        : std_ulogic;
    //l3_debug_out_type &dbgo,//  : out l3_debug_out_type;
    pStr = PutToStr(pStr, pch_dbgo->data,32,"ch_dbgo.data");//    : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pch_dbgo->crdy,1,"ch_dbgo.crdy");//    : std_ulogic;
    pStr = PutToStr(pStr, pch_dbgo->dsu,1,"ch_dbgo.dsu");//     : std_ulogic;
    pStr = PutToStr(pStr, pch_dbgo->dsumode,1,"ch_dbgo.dsumode");// : std_ulogic;
    pStr = PutToStr(pStr, pch_dbgo->error,1,"ch_dbgo.error");//   : std_ulogic;
    pStr = PutToStr(pStr, pch_dbgo->halt,1,"ch_dbgo.halt");//    : std_ulogic;
    pStr = PutToStr(pStr, pch_dbgo->pwd,1,"ch_dbgo.pwd");//     : std_ulogic;
    pStr = PutToStr(pStr, pch_dbgo->idle,1,"ch_dbgo.idle");//    : std_ulogic;
    pStr = PutToStr(pStr, pch_dbgo->ipend,1,"ch_dbgo.ipend");//   : std_ulogic;
    pStr = PutToStr(pStr, pch_dbgo->icnt,1,"ch_dbgo.icnt");//    : std_ulogic;
    pStr = PutToStr(pStr, pch_dbgo->fcnt,1,"--ch_dbgo.fcnt");//    : std_ulogic;
    pStr = PutToStr(pStr, pch_dbgo->optype,6,"--ch_dbgo.optype");//  : std_logic_vector(5 downto 0); -- instruction type
    pStr = PutToStr(pStr, pch_dbgo->bpmiss,1,"--ch_dbgo.bpmiss");//  : std_ulogic;     -- branch predict miss
    pStr = PutToStr(pStr, pch_dbgo->istat.cmiss,1,"--ch_dbgo.istat.cmiss");//   : std_ulogic;			-- cache miss
    pStr = PutToStr(pStr, pch_dbgo->istat.tmiss,1,"--ch_dbgo.istat.tmiss");//   : std_ulogic;			-- TLB miss
    pStr = PutToStr(pStr, pch_dbgo->istat.chold,1,"--ch_dbgo.istat.chold");//   : std_ulogic;			-- cache hold
    pStr = PutToStr(pStr, pch_dbgo->istat.mhold,1,"--ch_dbgo.istat.mhold");//   : std_ulogic;			-- cache mmu hold
    pStr = PutToStr(pStr, pch_dbgo->dstat.cmiss,1,"--ch_dbgo.dstat.cmiss");//   : std_ulogic;			-- cache miss
    pStr = PutToStr(pStr, pch_dbgo->dstat.tmiss,1,"--ch_dbgo.dstat.tmiss");//   : std_ulogic;			-- TLB miss
    pStr = PutToStr(pStr, pch_dbgo->dstat.chold,1,"--ch_dbgo.dstat.chold");//   : std_ulogic;			-- cache hold
    pStr = PutToStr(pStr, pch_dbgo->dstat.mhold,1,"--ch_dbgo.dstat.mhold");//   : std_ulogic;			-- cache mmu hold
    pStr = PutToStr(pStr, pch_dbgo->wbhold,1,"--ch_dbgo.wbhold");//  : std_ulogic;     -- write buffer hold
    pStr = PutToStr(pStr, pch_dbgo->su,1,"--ch_dbgo.su");//      : std_ulogic;     -- supervisor state
    //mul32_in_type &muli,//  : out mul32_in_type;
    pStr = PutToStr(pStr, pch_muli->op1, 33,"ch_muli.op1");
    pStr = PutToStr(pStr, pch_muli->op2, 33,"ch_muli.op2");
    pStr = PutToStr(pStr, pch_muli->flush, 1,"ch_muli.flush");
    pStr = PutToStr(pStr, pch_muli->Signed, 1,"ch_muli.signed");
    pStr = PutToStr(pStr, pch_muli->start, 1,"ch_muli.start");
    pStr = PutToStr(pStr, pch_muli->mac, 1,"ch_muli.mac");
    pStr = PutToStr(pStr, pch_muli->acc, 40,"ch_muli.acc");
    //div32_in_type &divi,//  : out div32_in_type;
    pStr = PutToStr(pStr, pch_divi->y, 33,"ch_divi.y");
    pStr = PutToStr(pStr, pch_divi->op1, 33,"ch_divi.op1");
    pStr = PutToStr(pStr, pch_divi->op2, 33,"ch_divi.op2");
    pStr = PutToStr(pStr, pch_divi->flush, 1,"ch_divi.flush");
    pStr = PutToStr(pStr, pch_divi->Signed, 1,"ch_divi.Signed");
    pStr = PutToStr(pStr, pch_divi->start, 1,"ch_divi.start");
    //fpc_in_type &fpi,//   : out fpc_in_type;
    //fpc_in_type &cpi,//   : out fpc_in_type;
    //tracebuf_in_type &tbi,//   : out tracebuf_in_type;
    pStr = PutToStr(pStr, pch_tbi->addr,12,"ch_tbi.addr");//             : std_logic_vector(11 downto 0);
    pStr = PutToStr(pStr, pch_tbi->data[0],64,"ch_tbi.data(63 downto 0)");//             : std_logic_vector(127 downto 0);
    pStr = PutToStr(pStr, pch_tbi->data[1],64,"ch_tbi.data(127 downto 64)");//             : std_logic_vector(127 downto 0);
    pStr = PutToStr(pStr, pch_tbi->enable,1,"ch_tbi.enable");//           : std_logic;
    pStr = PutToStr(pStr, pch_tbi->write,4,"ch_tbi.write");//            : std_logic_vector(3 downto 0);
    pStr = PutToStr(pStr, pch_tbi->diag,4,"ch_tbi.diag");//             : std_logic_vector(3 downto 0);

    
    // internal:
    pStr = PutToStr(pStr, ptst_iu3->in_r.d.pc>>CFG_PCLOW,32-CFG_PCLOW,"in_r.d.pc");
    pStr = PutToStr(pStr, ptst_iu3->in_r.d.inst.arr[0],32,"in_r.d.inst(0)");
    pStr = PutToStr(pStr, ptst_iu3->in_r.d.inst.arr[1],32,"in_r.d.inst(1)");
    pStr = PutToStr(pStr, ptst_iu3->in_r.d.cwp,NWINLOG2,"in_r.d.cwp");
    pStr = PutToStr(pStr, ptst_iu3->in_r.d.set,ISETMSB+1,"in_r.d.set");
    pStr = PutToStr(pStr, ptst_iu3->in_r.d.mexc,1,"in_r.d.mexc");
    pStr = PutToStr(pStr, ptst_iu3->in_r.d.cnt,2,"in_r.d.cnt");
    pStr = PutToStr(pStr, ptst_iu3->in_r.d.pv,1,"in_r.d.pv");
    pStr = PutToStr(pStr, ptst_iu3->in_r.d.annul,1,"in_r.d.annul");
    pStr = PutToStr(pStr, ptst_iu3->in_r.d.inull,1,"in_r.d.inull");
    pStr = PutToStr(pStr, ptst_iu3->in_r.d.step,1,"in_r.d.step");
    pStr = PutToStr(pStr, ptst_iu3->in_r.d.divrdy,1,"in_r.d.divrdy");

    pStr = PutToStr(pStr, ptst_iu3->in_r.a.ctrl.pc>>CFG_PCLOW,32-CFG_PCLOW,"in_r.a.ctrl.pc");//    : pctype;
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.ctrl.inst,32,"in_r.a.ctrl.inst");//  : word;
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.ctrl.cnt,2,"in_r.a.ctrl.cnt");//   : std_logic_vector(1 downto 0);
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.ctrl.rd,RFBITS,"in_r.a.ctrl.rd");//    : rfatype;
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.ctrl.tt,6,"in_r.a.ctrl.tt");//    : std_logic_vector(5 downto 0);
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.ctrl.trap,1,"in_r.a.ctrl.trap");//  : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.ctrl.annul,1,"in_r.a.ctrl.annul");// : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.ctrl.wreg,1,"in_r.a.ctrl.wreg");//  : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.ctrl.wicc,1,"in_r.a.ctrl.wicc");//  : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.ctrl.wy,1,"in_r.a.ctrl.wy");//    : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.ctrl.ld,1,"in_r.a.ctrl.ld");//    : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.ctrl.pv,1,"in_r.a.ctrl.pv");//    : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.ctrl.rett,1,"in_r.a.ctrl.rett");//  : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.rs1,5,"in_r.a.rs1");
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.rfa1,RFBITS,"in_r.a.rfa1");
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.rfa2,RFBITS,"in_r.a.rfa2");
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.rsel1,3,"in_r.a.rsel1");
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.rsel2,3,"in_r.a.rsel2");
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.rfe1,1,"in_r.a.rfe1");
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.rfe2,1,"in_r.a.rfe2");
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.cwp,NWINLOG2,"in_r.a.cwp");
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.imm,32,"in_r.a.imm");
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.ldcheck1,1,"in_r.a.ldcheck1");
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.ldcheck2,1,"in_r.a.ldcheck2");
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.ldchkra,1,"in_r.a.ldchkra");
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.ldchkex,1,"in_r.a.ldchkex");
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.su,1,"in_r.a.su");
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.et,1,"in_r.a.et");
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.wovf,1,"in_r.a.wovf");
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.wunf,1,"in_r.a.wunf");
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.ticc,1,"in_r.a.ticc");
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.jmpl,1,"in_r.a.jmpl");
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.step,1,"in_r.a.step");
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.mulstart,1,"in_r.a.mulstart");
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.divstart,1,"in_r.a.divstart");
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.bp,1,"in_r.a.bp");
    pStr = PutToStr(pStr, ptst_iu3->in_r.a.nobp,1,"in_r.a.nobp");

    pStr = PutToStr(pStr, ptst_iu3->in_r.e.ctrl.pc>>CFG_PCLOW,32-CFG_PCLOW,"in_r.e.ctrl.pc");//    : pctype;
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.ctrl.inst,32,"in_r.e.ctrl.inst");//  : word;
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.ctrl.cnt,2,"in_r.e.ctrl.cnt");//   : std_logic_vector(1 downto 0);
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.ctrl.rd,RFBITS,"in_r.e.ctrl.rd");//    : rfatype;
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.ctrl.tt,6,"in_r.e.ctrl.tt");//    : std_logic_vector(5 downto 0);
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.ctrl.trap,1,"in_r.e.ctrl.trap");//  : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.ctrl.annul,1,"in_r.e.ctrl.annul");// : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.ctrl.wreg,1,"in_r.e.ctrl.wreg");//  : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.ctrl.wicc,1,"in_r.e.ctrl.wicc");//  : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.ctrl.wy,1,"in_r.e.ctrl.wy");//    : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.ctrl.ld,1,"in_r.e.ctrl.ld");//    : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.ctrl.pv,1,"in_r.e.ctrl.pv");//    : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.ctrl.rett,1,"in_r.e.ctrl.rett");//  : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.op1,32,"in_r.e.op1");//    : word;
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.op2,32,"in_r.e.op2");//    : word;
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.aluop,3,"in_r.e.aluop");//  : std_logic_vector(2 downto 0);  	-- Alu operation
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.alusel,2,"in_r.e.alusel");// : std_logic_vector(1 downto 0);  	-- Alu result select
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.aluadd,1,"in_r.e.aluadd");// : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.alucin,1,"in_r.e.alucin");// : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.ldbp1,1,"in_r.e.ldbp1");
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.ldbp2,1,"in_r.e.ldbp2");// : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.invop2,1,"in_r.e.invop2");// : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.shcnt,5,"in_r.e.shcnt");//  : std_logic_vector(4 downto 0);  	-- shift count
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.sari,1,"in_r.e.sari");//   : std_ulogic;				-- shift msb
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.shleft,1,"in_r.e.shleft");// : std_ulogic;				-- shift left/right
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.ymsb,1,"in_r.e.ymsb");//   : std_ulogic;				-- shift left/right
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.rd,5,"in_r.e.rd");// 	   : std_logic_vector(4 downto 0);
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.jmpl,1,"in_r.e.jmpl");//   : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.su,1,"in_r.e.su");//     : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.et,1,"in_r.e.et");//     : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.cwp,NWINLOG2,"in_r.e.cwp");//    : cwptype;
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.icc,4,"in_r.e.icc");//    : std_logic_vector(3 downto 0);
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.mulstep,1,"in_r.e.mulstep");//: std_ulogic;            
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.mul,1,"in_r.e.mul");//    : std_ulogic;            
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.mac,1,"in_r.e.mac");//    : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.e.bp,1,"in_r.e.bp");//     : std_ulogic;

    pStr = PutToStr(pStr, ptst_iu3->in_r.m.ctrl.pc>>CFG_PCLOW,32-CFG_PCLOW,"in_r.m.ctrl.pc");//    : pctype;
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.ctrl.inst,32,"in_r.m.ctrl.inst");//  : word;
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.ctrl.cnt,2,"in_r.m.ctrl.cnt");//   : std_logic_vector(1 downto 0);
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.ctrl.rd,RFBITS,"in_r.m.ctrl.rd");//    : rfatype;
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.ctrl.tt,6,"in_r.m.ctrl.tt");//    : std_logic_vector(5 downto 0);
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.ctrl.trap,1,"in_r.m.ctrl.trap");//  : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.ctrl.annul,1,"in_r.m.ctrl.annul");// : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.ctrl.wreg,1,"in_r.m.ctrl.wreg");//  : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.ctrl.wicc,1,"in_r.m.ctrl.wicc");//  : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.ctrl.wy,1,"in_r.m.ctrl.wy");//    : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.ctrl.ld,1,"in_r.m.ctrl.ld");//    : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.ctrl.pv,1,"in_r.m.ctrl.pv");//    : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.ctrl.rett,1,"in_r.m.ctrl.rett");//  : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.result,32,"in_r.m.result");
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.y,32,"in_r.m.y");
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.icc,4,"in_r.m.icc");
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.nalign,1,"in_r.m.nalign");
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.dci.Signed,1,"in_r.m.dci.signed");
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.dci.enaddr,1,"in_r.m.dci.enaddr");
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.dci.read,1,"in_r.m.dci.read");
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.dci.write,1,"in_r.m.dci.write");
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.dci.lock,1,"in_r.m.dci.lock");
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.dci.dsuen,1,"in_r.m.dci.dsuen");
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.dci.size,2,"in_r.m.dci.size");
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.dci.asi,8,"in_r.m.dci.asi");
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.werr,1,"in_r.m.werr");
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.wcwp,1,"in_r.m.wcwp");
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.irqen,1,"in_r.m.irqen");
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.irqen2,1,"in_r.m.irqen2");
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.mac,1,"in_r.m.mac");
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.divz,1,"in_r.m.divz");
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.su,1,"in_r.m.su");
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.mul,1,"in_r.m.mul");
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.casa,1,"in_r.m.casa");
    pStr = PutToStr(pStr, ptst_iu3->in_r.m.casaz,1,"in_r.m.casaz");

    pStr = PutToStr(pStr, ptst_iu3->in_r.x.ctrl.pc>>CFG_PCLOW,32-CFG_PCLOW,"in_r.x.ctrl.pc");//    : pctype;
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.ctrl.inst,32,"in_r.x.ctrl.inst");//  : word;
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.ctrl.cnt,2,"in_r.x.ctrl.cnt");//   : std_logic_vector(1 downto 0);
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.ctrl.rd,RFBITS,"in_r.x.ctrl.rd");//    : rfatype;
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.ctrl.tt,6,"in_r.x.ctrl.tt");//    : std_logic_vector(5 downto 0);
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.ctrl.trap,1,"in_r.x.ctrl.trap");//  : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.ctrl.annul,1,"in_r.x.ctrl.annul");// : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.ctrl.wreg,1,"in_r.x.ctrl.wreg");//  : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.ctrl.wicc,1,"in_r.x.ctrl.wicc");//  : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.ctrl.wy,1,"in_r.x.ctrl.wy");//    : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.ctrl.ld,1,"in_r.x.ctrl.ld");//    : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.ctrl.pv,1,"in_r.x.ctrl.pv");//    : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.ctrl.rett,1,"in_r.x.ctrl.rett");//  : std_ulogic;
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.result,32,"in_r.x.result");
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.y,32,"in_r.x.y");
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.icc,4,"in_r.x.icc");
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.annul_all,1,"in_r.x.annul_all");
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.data.arr[0],32,"in_r.x.data(0)");
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.data.arr[1],32,"in_r.x.data(1)");
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.set,DSETMSB+1,"in_r.x.set");
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.mexc,1,"in_r.x.mexc");
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.dci.Signed,1,"in_r.x.dci.signed");
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.dci.enaddr,1,"in_r.x.dci.enaddr");
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.dci.read,1,"in_r.x.dci.read");
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.dci.write,1,"in_r.x.dci.write");
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.dci.lock,1,"in_r.x.dci.lock");
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.dci.dsuen,1,"in_r.x.dci.dsuen");
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.dci.size,2,"in_r.x.dci.size");
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.dci.asi,8,"in_r.x.dci.asi");
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.laddr,2,"in_r.x.laddr");
    pStr = PutToStr(pStr, (uint32)ptst_iu3->in_r.x.rstate,2,"--  in_r.x.rstate");
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.npc,3,"in_r.x.npc");
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.intack,1,"in_r.x.intack");
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.ipend,1,"in_r.x.ipend");
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.mac,1,"in_r.x.mac");
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.debug,1,"in_r.x.debug");
    pStr = PutToStr(pStr, ptst_iu3->in_r.x.nerror,1,"in_r.x.nerror");

      pStr = PutToStr(pStr, pin_fpo->dbg.data,32,"--fpo.dbg.data");
      pStr = PutToStr(pStr, ptst_iu3->in_r.f.pc>>CFG_PCLOW,32-CFG_PCLOW,"in_r.f.pc");
      pStr = PutToStr(pStr, ptst_iu3->in_r.w.s.cwp,NWINLOG2,"in_r.w.s.cwp");//    : cwptype;                             -- current window pointer
      pStr = PutToStr(pStr, ptst_iu3->in_r.w.s.icc,4,"in_r.w.s.icc");//    : std_logic_vector(3 downto 0);	  -- integer condition codes
      pStr = PutToStr(pStr, ptst_iu3->in_r.w.s.tt,8,"in_r.w.s.tt");//     : std_logic_vector(7 downto 0);	  -- trap type
      pStr = PutToStr(pStr, ptst_iu3->in_r.w.s.tba,20,"in_r.w.s.tba");//    : std_logic_vector(19 downto 0);	  -- trap base address
      pStr = PutToStr(pStr, ptst_iu3->in_r.w.s.wim,CFG_NWIN,"in_r.w.s.wim");//    : std_logic_vector(NWIN-1 downto 0);   -- window invalid mask
      pStr = PutToStr(pStr, ptst_iu3->in_r.w.s.pil,4,"in_r.w.s.pil");//    : std_logic_vector(3 downto 0);	  -- processor interrupt level
      pStr = PutToStr(pStr, ptst_iu3->in_r.w.s.ec,1,"in_r.w.s.ec");//   : std_ulogic;
      pStr = PutToStr(pStr, ptst_iu3->in_r.w.s.ef,1,"in_r.w.s.ef");//   : std_ulogic;
      pStr = PutToStr(pStr, ptst_iu3->in_r.w.s.ps,1,"in_r.w.s.ps");//   : std_ulogic;
      pStr = PutToStr(pStr, ptst_iu3->in_r.w.s.s,1,"in_r.w.s.s");//   : std_ulogic;
      pStr = PutToStr(pStr, ptst_iu3->in_r.w.s.et,1,"in_r.w.s.et");//   : std_ulogic;
      pStr = PutToStr(pStr, ptst_iu3->in_r.w.s.y,32,"in_r.w.s.y");//      : word;
      pStr = PutToStr(pStr, ptst_iu3->in_r.w.s.asr18,32,"in_r.w.s.asr18");//  : word;
      pStr = PutToStr(pStr, ptst_iu3->in_r.w.s.svt,1,"in_r.w.s.svt");//   : std_ulogic;
      pStr = PutToStr(pStr, ptst_iu3->in_r.w.s.dwt,1,"in_r.w.s.dwt");//   : std_ulogic;
      pStr = PutToStr(pStr, ptst_iu3->in_r.w.s.dbp,1,"in_r.w.s.dbp");//   : std_ulogic;
      pStr = PutToStr(pStr, ptst_iu3->in_r.w.result,32,"in_r.w.result");
      
    ptst_iu3->in_wpr = ptst_iu3->vwpr;

    pStr = PutToStr(pStr, ptst_iu3->in_wpr.arr[0].addr>>2,30,"in_wpr(0).addr");//    : std_logic_vector(31 downto 2);  -- watchpoint address
    pStr = PutToStr(pStr, ptst_iu3->in_wpr.arr[0].mask>>2,30,"in_wpr(0).mask");//    : std_logic_vector(31 downto 2);  -- watchpoint mask
    pStr = PutToStr(pStr, ptst_iu3->in_wpr.arr[0].exec,1,"in_wpr(0).exec");//    : std_ulogic;			    -- trap on instruction
    pStr = PutToStr(pStr, ptst_iu3->in_wpr.arr[0].load,1,"in_wpr(0).load");//    : std_ulogic;			    -- trap on load
    pStr = PutToStr(pStr, ptst_iu3->in_wpr.arr[0].store,1,"in_wpr(0).store");//   : std_ulogic;			    -- trap on store
    pStr = PutToStr(pStr, ptst_iu3->in_wpr.arr[1].addr>>2,30,"in_wpr(1).addr");//    : std_logic_vector(31 downto 2);  -- watchpoint address
    pStr = PutToStr(pStr, ptst_iu3->in_wpr.arr[1].mask>>2,30,"in_wpr(1).mask");//    : std_logic_vector(31 downto 2);  -- watchpoint mask
    pStr = PutToStr(pStr, ptst_iu3->in_wpr.arr[1].exec,1,"in_wpr(1).exec");//    : std_ulogic;			    -- trap on instruction
    pStr = PutToStr(pStr, ptst_iu3->in_wpr.arr[1].load,1,"in_wpr(1).load");//    : std_ulogic;			    -- trap on load
    pStr = PutToStr(pStr, ptst_iu3->in_wpr.arr[1].store,1,"in_wpr(1).store");//   : std_ulogic;			    -- trap on store
    pStr = PutToStr(pStr, ptst_iu3->in_wpr.arr[2].addr>>2,30,"in_wpr(2).addr");//    : std_logic_vector(31 downto 2);  -- watchpoint address
    pStr = PutToStr(pStr, ptst_iu3->in_wpr.arr[2].mask>>2,30,"in_wpr(2).mask");//    : std_logic_vector(31 downto 2);  -- watchpoint mask
    pStr = PutToStr(pStr, ptst_iu3->in_wpr.arr[2].exec,1,"in_wpr(2).exec");//    : std_ulogic;			    -- trap on instruction
    pStr = PutToStr(pStr, ptst_iu3->in_wpr.arr[2].load,1,"in_wpr(2).load");//    : std_ulogic;			    -- trap on load
    pStr = PutToStr(pStr, ptst_iu3->in_wpr.arr[2].store,1,"in_wpr(2).store");//   : std_ulogic;			    -- trap on store
    pStr = PutToStr(pStr, ptst_iu3->in_wpr.arr[3].addr>>2,30,"in_wpr(3).addr");//    : std_logic_vector(31 downto 2);  -- watchpoint address
    pStr = PutToStr(pStr, ptst_iu3->in_wpr.arr[3].mask>>2,30,"in_wpr(3).mask");//    : std_logic_vector(31 downto 2);  -- watchpoint mask
    pStr = PutToStr(pStr, ptst_iu3->in_wpr.arr[3].exec,1,"in_wpr(3).exec");//    : std_ulogic;			    -- trap on instruction
    pStr = PutToStr(pStr, ptst_iu3->in_wpr.arr[3].load,1,"in_wpr(3).load");//    : std_ulogic;			    -- trap on load
    pStr = PutToStr(pStr, ptst_iu3->in_wpr.arr[3].store,1,"in_wpr(3).store");//   : std_ulogic;			    -- trap on store

    ptst_iu3->in_ir = ptst_iu3->irin;
    ptst_iu3->in_dsur = ptst_iu3->dsuin;

    pStr = PutToStr(pStr, ptst_iu3->in_ir.addr>>CFG_PCLOW,32-CFG_PCLOW,"in_ir.addr");
    pStr = PutToStr(pStr, ptst_iu3->in_ir.pwd,1,"in_ir.pwd");
    pStr = PutToStr(pStr, ptst_iu3->in_dsur.asi,8,"in_dsur.asi");
    #define TBUFBITS  (10+log2[CFG_ITBSZ]-4)
    pStr = PutToStr(pStr, ptst_iu3->in_dsur.tbufcnt,TBUFBITS,"in_dsur.tbufcnt");
    pStr = PutToStr(pStr, ptst_iu3->in_dsur.err,1,"in_dsur.err");
    pStr = PutToStr(pStr, ptst_iu3->in_dsur.tt,8,"in_dsur.tt");
    pStr = PutToStr(pStr, ptst_iu3->in_dsur.crdy>>1,2,"in_dsur.crdy");
    
    pStr = PutToStr(pStr, t_inClk,1,"t_inClk");
    pStr = PutToStr(pStr, topLeon3mp.pclLeon3s[0]->cramo.icramo.data.arr[0], 32, "t_cramo_data0");
    pStr = PutToStr(pStr, topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->mcio.data, 32, "t_mcio_data");

    PrintIndexStr();

    *posBench[TB_iu3] << chStr << "\n";
  }
#ifdef DBG_iu3
  tst_iu3.ClkUpdate();
#endif
}


