//------------------------------------------------------------------------------
//-- Description: This unit implements the data cache controller with MMU.
//------------------------------------------------------------------------------  

#include "lheaders.h"

//#define dsu       CFG_DSU//: integer range 0 to 1  := 0;
//#define dcen      : integer range 0 to 1  := 0;
//#define drepl     CFG_DREPL//: integer range 0 to 3  := 0;
//#define dsets     CFG_DSETS//: integer range 1 to 4  := 1;
//#define dlinesize CFG_DLINE//: integer range 4 to 8  := 4;
//#define dsetsize  CFG_DSETSZ//: integer range 1 to 256 := 1;
//#define dsetlock  CFG_DLOCK//: integer range 0 to 1  := 0;
//#define dsnoop    CFG_DSNOOP//: integer range 0 to 6 := 0;
//#define dlram      CFG_DLRAMEN//: integer range 0 to 1 := 0;
//#define dlramsize  CFG_DLRAMSZ//: integer range 1 to 512 := 1;
//#define dlramstart CFG_DLRAMADDR//: integer range 0 to 255 := 16#8f#;
//#define ilram      CFG_ILRAMEN//: integer range 0 to 1 := 0;
//#define ilramstart : integer range 0 to 255 := 16#8e#;
//#define itlbnum   : integer range 2 to 64 := 8;
//#define dtlbnum   : integer range 2 to 64 := 8;
//#define tlb_type  : integer range 0 to 3 := 1;
//#define memtech   : integer range 0 to NTECH := 0;
//#define cached    CFG_DFIXED//: integer := 0;
//#define mmupgsz   CFG_MMU_PAGESIZE//: integer range 0 to 5 := 0;
//#define smp      : integer := 0;
//#define mmuen    CFG_MMUEN//: integer := 0

//#define M_EN CFG_MMUEN//: boolean := (mmuen /= 0);

#define DSNOOP2        (CFG_DSNOOP%4)//: integer := dsnoop mod 4;
#define DSNOOPSEP      (CFG_DSNOOP>3)//: boolean := (dsnoop > 3);

#define M_TLB_TYPE     (CFG_TLB_TYPE&0x1)//: integer range 0 to 1 := conv_integer(conv_std_logic_vector(tlb_type,2) and conv_std_logic_vector(1,2));  -- eather split or combined
#define M_TLB_FASTWRITE (CFG_TLB_TYPE&0x2)//: integer range 0 to 3 := conv_integer(conv_std_logic_vector(tlb_type,2) and conv_std_logic_vector(2,2));   -- fast writebuffer

#define M_ENT_I        CFG_ITLBNUM//: integer range 2 to 64 := itlbnum;   -- icache tlb entries: number
#define M_ENT_ILOG     (log2[CFG_ITLBNUM])//: integer := log2(M_ENT_I);     -- icache tlb entries: address bits

#define M_ENT_D        CFG_DTLBNUM//: integer range 2 to 64 := dtlbnum;   -- dcache tlb entries: number
#define M_ENT_DLOG     (log2[CFG_DTLBNUM])//: integer := log2(M_ENT_D);     -- dcache tlb entries: address bits

#define M_ENT_C        M_ENT_I//: integer range 2 to 64 := M_ENT_I;   -- i/dcache tlb entries: number
#define M_ENT_CLOG     M_ENT_ILOG//: integer := M_ENT_ILOG;     -- i/dcache tlb entries: address bits

#define LRR_BIT      (TAG_HIGH + 1)
#define TAG_LOW      (DOFFSET_BITS + DLINE_BITS + 2)

#define OFFSET_HIGH (TAG_LOW - 1)
#define OFFSET_LOW  (DLINE_BITS + 2)
#define LINE_HIGH   (OFFSET_LOW - 1)
//#define LINE_LOW    2//: integer := 2;
#define LINE_ZERO   0//: std_logic_vector(DLINE_BITS-1 downto 0) := (others => '0');
#define SETBITS     (log2x[CFG_DSETS]) 
#define DLRUBITS    (lru_table[CFG_DSETS])
#define LOCAL_RAM_START (CFG_DLRAMADDR&MSK32(7,0))//): std_logic_vector(7 downto 0) := conv_std_logic_vector(dlramstart, 8);
#define ILRAM_START     (CFG_ILRAMADDR&MSK32(7,0))//: std_logic_vector(7 downto 0) := conv_std_logic_vector(ilramstart, 8);
#define DIR_BITS    (log2x[CFG_DSETS])

#define ctbl (CFG_DFIXED&MSK32(15,0))//: std_logic_vector(15 downto 0) :=  conv_std_logic_vector(cached, 16);

//******************************************************************

//******************************************************************
uint32 mmu_dcache::lru_set(lru_type lru,// : lru_type,
                           lock_type lock)// : lock_type) return std_logic_vector is
{
  uint32 xlru = lru;// : std_logic_vector(4 downto 0);
  uint32 set = 0;//  : std_logic_vector(SETBITS-1 downto 0);
  uint32 xset = 0;// : std_logic_vector(1 downto 0);

#if (CFG_DLOCK==1)
  uint32 unlocked;// : integer range 0 to DSETS-1;
  unlocked = CFG_DSETS-1;
  for (uint32 i=CFG_DSETS-1; i>=0; i--)
  {
    if (BIT32((lock,i) == 0) unlocked = i;
  }

  switch(CFG_DSETS)
  {
    case 2:
      if (BIT32(lock,0)) xset |= 0x1; 
      else          {xset &= ~0x1; xset |= BIT32(xlru,0);}
    break;
    case 3:
      xset = lru3_repl_table[xlru][unlocked];
    break;
    case 4:
      xset = lru4_repl_table[xlru][unlocked];
    break;
    default:;
  }
#else
  switch(CFG_DSETS)
  {
    case 2:
      xset &= ~0x1;
      xset |= BIT32(xlru,0);
    break;
    case 3:
      xset = lru3_repl_table[xlru][0];
    break;
    case 4:
      xset = lru4_repl_table[xlru][0];
    break;
    default:;
  }
#endif

  set = BITS32(xset,SETBITS-1,0);
  return(set);
};

//******************************************************************
mmu_dcache::lru_type mmu_dcache::lru_calc(lru_type lru,// : lru_type;
                                          uint32 set)// : integer) return lru_type is
{
  lru_type new_lru;// : lru_type;
  uint32 xnew_lru;//: std_logic_vector(4 downto 0);
  uint32 xlru;// : std_logic_vector(4 downto 0);

  new_lru = 0;
  xnew_lru = 0;
  xlru = lru;
  switch(CFG_DSETS)
  {
    case 2:
      if (set == 0) xnew_lru |= 0x1;
      else          xnew_lru &= ~0x1;
    break;
    case 3:
      xnew_lru &= ~0x7;
      xnew_lru |= lru_3set_table[lru][set]; 
    break;
    case 4:
      xnew_lru &= ~0xF;
      xnew_lru |= lru_4set_table[lru][set];
    break;
    default:;
  };
  new_lru = BITS32(xnew_lru,DLRUBITS-1,0);
  return(new_lru);
};


//******************************************************************
void mmu_dcache::Update(uint32 rst,// : in  std_logic;
                        SClock clk,// : in  std_logic;
                        dcache_in_type &dci,// : in  dcache_in_type;
                        dcache_out_type &dco,// : out dcache_out_type;
                        icache_out_type &ico,// : in  icache_out_type;
                        memory_dc_in_type &mcdi,// : out memory_dc_in_type;
                        memory_dc_out_type &mcdo,// : in  memory_dc_out_type;
                        ahb_slv_in_type &ahbsi,// : in  ahb_slv_in_type;
                        dcram_in_type &dcrami,// : out dcram_in_type;
                        dcram_out_type &dcramo,// : in  dcram_out_type;
                        uint32 fpuholdn,// : in  std_logic;
                        mmudc_in_type &mmudci,// : out mmudc_in_type;
                        mmudc_out_type &mmudco,// : in mmudc_out_type;
                        SClock sclk)// : in  std_ulogic
{
  //-- init local variables
  uint32 x1 = OFFSET_LOW;
  uint32 x2 = LINE_LOW;

  v = r.Q;
  vs = rs.Q;
  vh = rh.Q;
  dcramov = dcramo;
  vl = rl.Q;
  vl.write = 0;
  lramen = 0;
  lramrd = 0;
  lramwr = 0; 
  lramcs = 0;
  laddr = 0;
  v.cctrlwr = 0;
  ilramen = 0;
  v.flush2 = r.Q.flush;
  sidle = 0;

  pagesize = MMU_getpagesize(CFG_MMU_PAGESIZE, r.Q.mmctrl1);

  if( ((dci.eenaddr|dci.enaddr)==1) || (r.Q.dstate != idle) || 
       ((CFG_DSU==1) && (dci.dsuen==1)) || (r.Q.flush==1) ||  (is_fpga[CFG_MEMTECH]==1) )
    enable = 0xF;
  else enable = 0x0;

  v.mmctrl1wr = 0;
  tagclear = 0;
  mmuisdis = 0;
  if ( (CFG_MMUEN==0) || ((r.Q.asi==ASI_MMU_BP) || (r.Q.mmctrl1.e==0)) )
    mmuisdis = 1;
    
  if (mmuisdis == 1) paddress = r.Q.xaddress;
  else               paddress = r.Q.paddress;

  mds = 1;
  dwrite = 0;
  twrite = 0;
  tpwrite = 0; 
  ddiagwrite = 0;
  tdiagwrite = 0;
  v.holdn    = 1;
  mexc       = 0;
  flush      = 0;
  v.icenable = 0;
  iflush     = 0;
  eholdn     = ico.hold & fpuholdn;
  ddset      = 0;
  vs.snoop   = 0;
  snoopwe    = 0;
  snoopaddr  = ahbsi.haddr& MSK32(OFFSET_HIGH,OFFSET_LOW);
  hcache     = 0; 
  validv     = 0;
  validrawv  = 0;
  hitv       = 0;
  vh.clear  = 0;
  if (CFG_DLRAMEN==1) rlramrd = r.Q.lramrd;
  else                rlramrd = 0;

  miscdata = 0;
  pflush   = 0;
  pflushaddr = BITS32(dci.maddress,VA_I_U,VA_I_D);
  pflushtyp  = PFLUSH_PAGE;
  pftag      = 0;  
  for (int32 i=0; i<4; i++) ctx.arr[i] = 0;
  mmudci_fsread = 0;


  for (int32 i=0; i<4; i++)
  {
    ddatainv.arr[i] = 0;
    tag.arr[i]      = 0;
    ptag.arr[i]     = 0;
  }
  newptag = 0;
  
  v.trans_op = r.Q.trans_op & (!mmudco.grant);
  v.flush_op = r.Q.flush_op & (!mmudco.grant);
  v.diag_op  = r.Q.diag_op & (!mmudco.grant);
  mmudci_trans_op = r.Q.trans_op;
  mmudci_flush_op = r.Q.flush_op;
  mmudci_diag_op  = r.Q.diag_op;
  mmudci_wb_op    = 0;
  mmudci_transdata_data = r.Q.vaddr;
  
  mmudci_su = 0;
  mmudci_read = 0;
  su = 0;
  if ((CFG_MMUEN==0) || (r.Q.mmctrl1.e==0)) v.cache = 1;
  
  rdatasel = ddata;  //-- read data from cache as default
  senable  = 0;
  scanen   = mcdo.scanen ? 0xF : 0x0;
    
  set = 0;
  snoopset = 0;
  csnoopwe = 0;
  ctwrite  = 0;
  ctpwrite = 0;
  cdwrite  = 0;
  wlock    = 0;
  for (int32 i=0; i<CFG_DSETS; i++)
    wlock |= (BIT32(dcramov.tag.arr[i],CTAG_LOCKPOS)<<i);
  wlrr = 0;
  for (int32 i=0; i<4; i++)
    wlrr |= (BIT32(dcramov.tag.arr[i],CTAG_LRRPOS)<<i);
  
#if (CFG_DSETS>1)
  setrepl = r.Q.setrepl; 
  
  //-- random replacement counter
  if (r.Q.rndcnt == (CFG_DSETS - 1)) v.rndcnt = 0;
  else                             v.rndcnt = r.Q.rndcnt + 1;
#else
  setrepl = 0;
#endif

  //-- generate lock bits
  lock = 0;
#if (CFG_DLOCK==1)
  for (int32 i=0; i<CFG_DSETS; i++) 
    lock |= (BIT32(dcramov.tag.arr[i],CTAG_LOCKPOS)<<i);
#endif
    
  //-- AHB snoop handling

#if (DSNOOP2 != 0)
  //-- snoop on NONSEQ or SEQ and first word in cache line
  //-- do not snoop during own transfers or during cache flush
  if( (ahbsi.hready & ahbsi.hwrite & !mcdo.bg)  
   && ((ahbsi.htrans==HTRANS_NONSEQ) ||
      ((ahbsi.htrans==HTRANS_SEQ) && (BITS32(ahbsi.haddr,LINE_HIGH,LINE_LOW) == LINE_ZERO))) )
  {
    vs.snoop = r.Q.cctrl.dsnoop;
    vs.addr  = BITS32(ahbsi.haddr,TAG_HIGH,OFFSET_LOW);
  }

  for (int32 i=0; i<CFG_DSETS; i++)
  {
    senable &= ~(1<<i);
    senable |= ((vs.snoop | rs.Q.snoop)<<i);
  }
    
  //-- clear valid bits on snoop hit (or set hit bits)
  for (int32 i=CFG_DSETS-1; i>=0; i--)
  {
    vh.snhit &= ~(0x1<<i);
    if ((rs.Q.snoop & !(r.Q.flush | r.Q.flush2)) == 1)
    {
      if(BITS32(dcramov.stag.arr[i],TAG_HIGH,TAG_LOW)==BITS32(rs.Q.addr,TAG_HIGH,TAG_LOW))
      {
      #if (DSNOOP2 == 1)
        snoopaddr = rs.Q.addr & MSK32(OFFSET_HIGH,OFFSET_LOW);
        snoopwe   = 1;
        snoopset  = i;        
      #elif (DSNOOP2 == 2)
        vh.snhit |= (1<<i);
      #endif
      }
    }
  }
#endif

  //-- generate access parameters during pipeline stall

  if (!r.Q.holdn || ((CFG_DSU==1) && dci.dsuen)) 
    taddr = r.Q.xaddress &MSK32(OFFSET_HIGH,LINE_LOW);
  else if ( (dci.enaddr & !dci.read) || (eholdn==0) )
    taddr = dci.maddress &MSK32(OFFSET_HIGH,LINE_LOW);
  else
    taddr = dci.eaddress &MSK32(OFFSET_HIGH,LINE_LOW);

  if (dci.write | !r.Q.holdn)
  {
    maddress  = r.Q.xaddress;
    read      = r.Q.read;
    size      = r.Q.size;
    edata     = dci.maddress;
    mmudci_su = r.Q.su;
    mmudci_read = r.Q.read & !r.Q.dlock;
  }else
  {
    maddress = dci.maddress;
    read     = dci.read;
    size     = dci.size;
    edata    = dci.edata;
    mmudci_su = dci.msu;
    mmudci_read = dci.read & !dci.lock;
  }

  newtag  = dci.maddress&MSK32(TAG_HIGH,TAG_LOW);
  newptag = dci.maddress&MSK32(TAG_HIGH,TAG_LOW);
  vl.waddr = BITS32(maddress,OFFSET_HIGH,OFFSET_LOW);  //-- lru write address
  if ((CFG_DSNOOP == 6) && (r.Q.cctrl.dsnoop == 0))
  {
    snoopaddr = taddr&MSK32(OFFSET_HIGH,OFFSET_LOW);
    senable   = enable;
  }

  //-- generate cache hit and valid bits

  if ((CFG_DFIXED!=0) && (r.Q.mmctrl1.e==0)) hcache = BIT32(ctbl, BITS32(dci.maddress,31,28) );
  else                                       hcache = 1;

  forcemiss = !BIT32(dci.asi,3) | dci.lock;
  hit = 0;
  set = 0;
  snoophit = 0;
  uint32 tmp;
#if 1
  if(iClkCnt>=771)
  bool st = true;
#endif

  for (int32 i=CFG_DSETS-1; i>=0; i--)
  {
#if (DSNOOP2 == 2)
    snoophit &= ~(1<<i);
    snoophit |= (BIT32(rh.Q.hit[BITS32(rh.Q.taddr,OFFSET_HIGH,OFFSET_LOW)],i) << i);
#endif
    if ( (BITS32(dcramov.tag.arr[i],TAG_HIGH,TAG_LOW)==BITS32(dci.maddress,TAG_HIGH,TAG_LOW))
      && ((dcramov.ctx.arr[i] == r.Q.mmctrl1.ctx) || (r.Q.mmctrl1.e == 0)) )
      

    {
      hitv |= (1<<i);
    }
    validrawv &= ~(1<<i);
    tmp = genmux(BITS32(dci.maddress,LINE_HIGH,LINE_LOW), BITS32(dcramov.tag.arr[i],CFG_DLINE-1,0));
    validrawv |= ((hcache & BIT32(hitv,i) & !r.Q.flush & !r.Q.flush2 & !BIT32(snoophit,i) & tmp) << i);
    
    validv &= ~(1<<i);
    validv |= (BIT32(validrawv,i) << i);
  }

  if (CFG_DREPL == dir)
  {
    hit = BIT32(hitv, BITS32(dci.maddress,OFFSET_HIGH+DIR_BITS,OFFSET_HIGH+1)) & !r.Q.flush & !r.Q.flush2;
    validraw = BIT32(validrawv, BITS32(dci.maddress,OFFSET_HIGH+DIR_BITS,OFFSET_HIGH+1));
    valid = BIT32(validv, BITS32(dci.maddress,OFFSET_HIGH+DIR_BITS,OFFSET_HIGH+1));
  }else
  {
    hit = orv(hitv) & !r.Q.flush & !r.Q.flush2;
    validraw = orv(validrawv);
    valid = orv(validv);
  }

  //-- cache hit disabled if mmu-enabled but off or BYPASS
  if ((CFG_MMUEN) && (BITS32(dci.asi,4,0)==ASI_MMU_BP))//-- or (r.mmctrl1.e = '0')
    hit = 0;
  

#if (CFG_DSETS > 1)
  if (CFG_DREPL == dir) set = BITS32(dci.maddress,OFFSET_HIGH+DIR_BITS,OFFSET_HIGH+1);
  else
  {
    for (int32 i=CFG_DSETS-1; i>=0; i--)
    {
      if (BIT32(hitv,i))
        set = i;
    }
  }
  if (rlramrd == 1) set = 1;
#else 
  set = 0;
#endif

#if 1
  if(iClkCnt>=49)
  bool stop = true;
#endif

  if (dci.dsuen == 1) diagset = BITS32(r.Q.xaddress,TAG_LOW+SETBITS-1,TAG_LOW);                                                
  else                diagset = BITS32(maddress,TAG_LOW+SETBITS-1,TAG_LOW);
  switch(CFG_DSETS)
  {
    case 1: ddset = 0; break;
    case 3: 
      if (diagset < 3) ddset = diagset;
    break;
    default: ddset = diagset; 
  }

  if (((r.Q.holdn & dci.enaddr) == 1)  && (r.Q.dstate == idle))
  {
    v.hit  = hit;
    v.xaddress = dci.maddress;      
    v.read = dci.read;
    v.size = dci.size;
    v.asi  = BITS32(dci.asi,4,0);
    v.su   = dci.msu;
    v.dlock = dci.lock;
  }

  //-- Store buffer
#if 1
  if(iClkCnt>=771)
  bool st = true;
#endif
  if (mcdo.ready == 1)
  {
    v.wb.addr &= ~(1<<2);
    v.wb.addr |= ( (BIT32(r.Q.wb.addr,2) | (BIT32(r.Q.wb.size,0) & BIT32(r.Q.wb.size,1))) << 2);
    if (r.Q.stpend == 1)
    {
      v.stpend  = r.Q.req;
      v.wb.data1 = r.Q.wb.data2; 
      v.wb.lock = r.Q.wb.lock & r.Q.req;
    }
  }
  if (mcdo.grant == 1) 
  {
    v.req   = r.Q.burst;
    v.burst = 0;
  }
  if (mcdo.grant & !r.Q.wb.read & r.Q.req) v.wb.lock = 0;
    
  if (CFG_DLRAMEN == 1)
  {
    if ((r.Q.holdn == 0) || ((CFG_DSU==1) && (dci.dsuen==1))) laddr = r.Q.xaddress;
    else if ((dci.enaddr & !dci.read) | !eholdn)            laddr = dci.maddress;
    else                                                    laddr = dci.eaddress;
    if  ((dci.enaddr==1) && (BITS32(dci.maddress,31,24)==LOCAL_RAM_START))
      lramen = 1;
    if( ((BITS32(laddr,31,24)==LOCAL_RAM_START) && (dci.dsuen==0))
     || ((dci.dsuen==1) && (BITS32(dci.asi,4,1)==0x5)) )
    {
      lramcs = 1;
    }
  }
  
  if (CFG_ILRAMEN == 1)
  {
    if((dci.enaddr==1) && (BITS32(dci.maddress,31,24)==ILRAM_START) && (dci.dsuen==0))
      ilramen = 1;
  }

  //-- cache freeze operation
  
  if ((r.Q.cctrl.ifrz & dci.intack & BIT32(r.Q.cctrl.ics,0)) == 1)
    v.cctrl.ics = 0x1;
  if ((r.Q.cctrl.dfrz & dci.intack & BIT32(r.Q.cctrl.dcs,0)) == 1)
    v.cctrl.dcs = 0x1;

  if ((r.Q.cctrlwr & !dci.nullify) == 1)
  {
    if ((BITS32(r.Q.xaddress,7,2)== 0x0) && (dci.read == 0))
    {
      v.cctrl.dsnoop = BIT32(dci.maddress,23);
      flush        = BIT32(dci.maddress,22);
      iflush       = BIT32(dci.maddress,21);
      v.cctrl.burst= BIT32(dci.maddress,16);
      v.cctrl.dfrz = BIT32(dci.maddress,5);
      v.cctrl.ifrz = BIT32(dci.maddress,4);
      v.cctrl.dcs  = BITS32(dci.maddress,3,2);
      v.cctrl.ics  = BITS32(dci.maddress,1,0);              
    }
  }

  //-- main Dcache state machine
#if 1
  if(iClkCnt>=35)
  bool st = true;
#endif
  switch(r.Q.dstate)
  {
    case idle:      //-- Idle state
      if (M_TLB_FASTWRITE != 0)
        mmudci_transdata_data = dci.maddress;
      sidle = 1;
      v.nomds = r.Q.nomds & !eholdn;
      for (int32 i=0; i<CFG_DSETS; i++)
      {
        
        if ( (BIT32(snoophit,i) == 0) && ((r.Q.flush | r.Q.flush2) == 0) )
        {
          v.valid.arr[i] = BITS32(dcramov.tag.arr[i],CFG_DLINE-1,0);
        }else v.valid.arr[i] = 0;
      }
#if (DSNOOP2 == 2) 
      vh.snmiss = snoophit;
#endif
      v.nomds = r.Q.nomds & !eholdn; 
      if ((r.Q.stpend==0) || ((mcdo.ready & !r.Q.req)==1)) //-- wait for store queue
      {
        v.wb.addr  = dci.maddress; 
        v.wb.size  = dci.size; 
        v.wb.read  = dci.read;
        v.wb.data1 = dci.edata;
        v.wb.lock  = dci.lock & !dci.nullify & ico.hold;
        v.wb.asi   = BITS32(dci.asi,3,0); 
        if( (CFG_MMUEN) && (BITS32(dci.asi,4,0)!=ASI_MMU_BP) && (r.Q.mmctrl1.e==1) 
         && ((M_TLB_FASTWRITE != 0) || ((dci.enaddr & eholdn & dci.lock & !dci.read)==1)) )
        {
          if (dci.enaddr & eholdn & dci.lock & !dci.read) //-- skip address translation on store in LDST
          {
            v.wb.addr = BITS32(dci.maddress,7,0);
            v.wb.addr |= (BITS32(r.Q.wb.addr,31,8) << 8);
            newptag = r.Q.wb.addr&MSK32(TAG_HIGH,TAG_LOW);
          }else 
          {
            v.wb.addr = mmudco.wbtransdata.data; 
            newptag = mmudco.wbtransdata.data&MSK32(TAG_HIGH,TAG_LOW);
          }
        }
      }
      if (eholdn & !r.Q.nomds) //-- avoid false path through nullify
      {
        switch (dci.asi&0x1F)
        {
          case ASI_SYSR: rdatasel = sysr; break;
          case ASI_DTAG: rdatasel = dtag; break;
          case ASI_DDATA: rdatasel = dddata; break;
          case ASI_DCTX: 
            if (CFG_MMUEN) rdatasel = dctx;
          break;
          case ASI_MMUREGS:
            if (CFG_MMUEN) rdatasel = misc;
          break;
          case ASI_MMUSNOOP_DTAG: rdatasel = mmusnoop_dtag; break;
          default:;
        }
      }
      if (dci.enaddr & eholdn & !r.Q.nomds & !dci.nullify)
      {
        switch (BITS32(dci.asi,4,0))
        {
          case ASI_SYSR:    //-- system registers
            v.cctrlwr = !dci.read & !(dci.dsuen & !dci.eenaddr);
          break;
          case ASI_MMUREGS:
            if (CFG_MMUEN)
            {
              if ((CFG_DSU == 0) || (dci.dsuen == 0))
              {
                //-- clean fault valid bit
                if (dci.read == 1)
                {
                  switch(BITS32(dci.maddress,CNR_U,CNR_D))
                  {
                    case CNR_F: mmudci_fsread = 1; break;
                    default:;
                  }
                }
              }
            }
            v.mmctrl1wr = !dci.read & !(r.Q.mmctrl1wr & dci.dsuen);
          break;
          case ASI_ITAG: case ASI_IDATA: case ASI_ICTX:   //-- Read/write Icache tags
            //-- CTX write has to be done through ctxnr & ASI_ITAG
            if ((ico.flush == 1) || BIT32(dci.asi,4)) mexc = 1;
            else
            {
              v.dstate = asi_idtag;
              v.holdn  = dci.dsuen;
            }
          break;
          case ASI_UINST: case ASI_SINST:
            if (CFG_ILRAMEN == 1) 
            {
              v.dstate  = asi_idtag;
              v.ilramen = 1;
            }
          break;
          case ASI_DFLUSH:    //-- flush data cache
            if (dci.read == 0) flush = 1;
          break;
          case ASI_DDATA:   //-- Read/write Dcache data
            if (r.Q.flush == 1) //-- No access on flush
              mexc = 1;
            else if (dci.read == 0)
            {
              dwrite     = 1;
              ddiagwrite = 1;
            }
          break;
          case ASI_DTAG:     //-- Read/write Dcache tags
            if ((dci.size != 0x2) || (r.Q.flush == 1)) //-- allow only word access
              mexc = 1;
            else if (dci.read == 0)
            {
              twrite = 1;
              tdiagwrite = 1;
            }
          break;
          case ASI_MMUSNOOP_DTAG: //-- Read/write MMU physical snoop tags
            if (DSNOOPSEP)
            {
              snoopaddr = taddr & MSK32(OFFSET_HIGH,OFFSET_LOW);
              if ((dci.size != 0x2) || (r.Q.flush == 1)) //-- allow only word access
                mexc = 1;
              else if (dci.read == 0)
              {
                tpwrite = 1;
                tdiagwrite = 1;
              }
            }
          break;
          case ASI_DCTX:
            //-- write has to be done through ctxnr & ASI_DTAG
            if ((dci.size != 0x2) || (r.Q.flush==1) || (dci.read==0))//-- allow only word access
              mexc = 1;
          break;
          case ASI_FLUSH_PAGE: //-- i/dcache flush page
            if (CFG_MMUEN)
            {
              if (dci.read == 0)
              {
                flush = 1;
                iflush = 1; //--pflush := '1'; pflushtyp := PFLUSH_PAGE;
              }
            }
          break;
          case ASI_FLUSH_CTX: //-- i/dcache flush ctx
            if (CFG_MMUEN)
            {
              if (dci.read == 0)
              {
                flush = 1;
                iflush = 1; //--pflush := '1'; pflushtyp := PFLUSH_CTX;
              }
            }
          break;
          case ASI_MMUFLUSHPROBE:
            if (CFG_MMUEN)
            {
              if (dci.read == 0)      //-- flush
              {
                mmudci_flush_op = 1;
                v.flush_op = !mmudco.grant;
                v.dstate = wflush;
                v.vaddr = dci.maddress;
                v.holdn = 0;
                flush = 1;
                iflush = 1;
              }
            }
          break;
          case ASI_MMU_DIAG:
           if (dci.read == 0)     //-- diag access
           {
              mmudci_diag_op = 1;
              v.diag_op = !mmudco.grant;
              v.vaddr = dci.maddress;
            }
          break;
          default:
            if (dci.read == 1)  //-- read access
            {
              if ((CFG_DLRAMEN == 1) && (lramen == 1))
              {
                lramrd = 1;
              }else if ((CFG_ILRAMEN == 1) && (ilramen == 1))
              {
                v.dstate = asi_idtag;
                v.holdn  = dci.dsuen;
                v.ilramen = 1; 
              }else if (dci.dsuen == 0)
              {
                if (!(BIT32(r.Q.cctrl.dcs,0) && ((hit & valid & !forcemiss)==1)))
                {  //-- read miss
                  v.holdn = 0;
                  v.dstate = wread;
                  v.ready = 0;
                  v.cache = 1;
                  if( (!CFG_MMUEN) || ((BITS32(dci.asi,4,0)==ASI_MMU_BP) || (r.Q.mmctrl1.e==0)) )
                  {
                    //-- cache disabled if mmu-enabled but off or BYPASS
                    if (CFG_MMUEN) v.cache = 0;
                    if ((r.Q.stpend == 0) || ((mcdo.ready & !r.Q.req) == 1))
                    {  //-- wait for store queue
                      v.req = 1; 
                      v.burst = BIT32(dci.size,1) & BIT32(dci.size,0) & !BIT32(dci.maddress,2);
                    }

                  }else
                  {
                    //-- ## mmu case >
                    if ((r.Q.stpend == 0) || ((mcdo.ready & !r.Q.req)== 1))
                    {
                      v.wbinit = 1;     //-- wb init in idle
                      v.burst = BIT32(dci.size,1) & BIT32(dci.size,0) & !BIT32(dci.maddress,2);
                    }else
                      v.wbinit = 0;
                  
                    mmudci_trans_op = 1;  //-- start translation
                    v.trans_op = !mmudco.grant;
                    v.vaddr = dci.maddress; 
                    v.dstate = rtrans;
                    //-- ## < mmu case 
                  }
                }else       //-- read hit
                {
                  if ((CFG_DSETS > 1) && (CFG_DREPL == lru))
                    vl.write = 1;
                }
              }
            }else      //-- write access
            {
              if( (CFG_DLRAMEN==1) && (lramen == 1))
              {
                lramwr = 1;
                if (dci.size == 0x3) //-- double store
                {
                  v.dstate = dblwrite;
                  v.xaddress |= (1<<2);
                }
              }else if ((CFG_ILRAMEN == 1) && (ilramen == 1))
              {
                v.dstate = asi_idtag;
                v.holdn = dci.dsuen;
                v.ilramen = 1;
              }else if (dci.dsuen == 0)
              {
                v.ready = 0;
                if (!CFG_MMUEN || ((BITS32(dci.asi,4,0)==ASI_MMU_BP) || (r.Q.mmctrl1.e==0)))
                {
                  if ((r.Q.stpend==0) || ((mcdo.ready & !r.Q.req)==1))  //-- wait for store queue
                  {
                      v.req = 1;
                      v.stpend = 1; 
                      v.burst = BIT32(dci.size,1) & BIT32(dci.size,0);

                    if (dci.size == 0x3) v.dstate = dblwrite; //-- double store
                  }else    //-- wait for store queue
                  {
                    v.dstate = wwrite;
                    v.holdn  = 0;
                  }
                }else
                {
                //-- ## mmu case >  false and
                  if( ((r.Q.stpend==0) || ((mcdo.ready & !r.Q.req)==1)) 
                   && (((mmudco.wbtransdata.accexc==0) && (M_TLB_FASTWRITE!=0)) || (dci.lock==1)) )
                  {
                    v.req = 1;
                    v.stpend = 1; 
                    v.burst = BIT32(dci.size,1) & BIT32(dci.size,0);
                    if (dci.size == 0x3) v.dstate = dblwrite; //-- double store       
                  }else
                  {
                    if ((r.Q.stpend==0) || ((mcdo.ready & !r.Q.req)==1))
                    {
                      v.wbinit = 1;     //-- wb init in idle
                      v.burst  = BIT32(dci.size,1) & BIT32(dci.size,0);          
                    }else
                    {
                      v.wbinit = 0;
                    }
                    mmudci_trans_op = 1;  //-- start translation
                    v.trans_op = !mmudco.grant; 
                    v.vaddr = dci.maddress;
                    v.holdn = 0;
                    v.dstate = wtrans;
                    v.dblwdata = BIT32(dci.size,0) | BIT32(dci.size,1);  //-- "11"
                    //-- ## < mmu case 
                  }
                }
                  
                if( BIT32(r.Q.cctrl.dcs,0) & ((hit & BIT32(dci.size,1) || validraw)==1) )
                {  //-- write hit
                  twrite = 1;
                  dwrite = 1; 
                  if ((CFG_DSETS > 1) && (CFG_DREPL == lru)) vl.write = 1;
                  setrepl = set;
                  if (DSNOOP2 != 0)
                  {
                    if(((dci.enaddr& !dci.read)==1) || (eholdn==0)) v.xaddress = dci.maddress;
                    else                                            v.xaddress = dci.eaddress;
                  }
                }
                if (dci.size == 0x3) v.xaddress |= (1<<2);
              }
            }

            if (CFG_DSETS > 1)
            {
              vl.set = set;
              v.setrepl = set;
              if (!hit & !r.Q.flush)
              {
                switch(CFG_DREPL)
                {
                  case rnd:
                    if (CFG_DLOCK == 1)
                    {
                      if (BIT32(lock,r.Q.rndcnt)== 0) v.setrepl = r.Q.rndcnt;
                      else
                      {
                        v.setrepl = CFG_DSETS-1;
                        for (uint32 i=CFG_DSETS-1; i>=0; i--)
                        {
                          if ((BIT32(lock,i)==0) && (i>r.Q.rndcnt))
                            v.setrepl = i;
                        }
                      }
                    }else
                      v.setrepl = r.Q.rndcnt;
                  break;
                  case dir:
                    v.setrepl = BITS32(dci.maddress,OFFSET_HIGH+log2x[CFG_DSETS],OFFSET_HIGH+1);
                  break;
                  case lru:
                    v.setrepl = lru_set(rl.Q.lru[BITS32(dci.maddress,OFFSET_HIGH,OFFSET_LOW)],
                                        BITS32(lock,CFG_DSETS-1,0) );
                  break;
                  case lrr:
                    v.setrepl = 0;
                    if (CFG_DLOCK == 1)
                    {
                      if (BIT32(lock,0)==1) v.setrepl |= 0x1;
                      else
                        v.setrepl |= (BIT32(dcramov.tag.arr[0],CTAG_LRRPOS) ^ BIT32(dcramov.tag.arr[1],CTAG_LRRPOS));
                    }else
                    {
                      v.setrepl |= (BIT32(dcramov.tag.arr[0],CTAG_LRRPOS) ^ BIT32(dcramov.tag.arr[1],CTAG_LRRPOS));
                    }
                    if (BIT32(v.setrepl,0)==0) v.lrr = !BIT32(dcramov.tag.arr[0],CTAG_LRRPOS);
                    else                       v.lrr = BIT32(dcramov.tag.arr[0],CTAG_LRRPOS);
                  default:;
                }
              }

              if (CFG_DLOCK == 1)
              {
                if (hit & BIT32(lock,set)) v.lock = 1;
                else                       v.lock = 0;
              }
            }
          break;//default
        }//end case;
      }
    break; // "idle" state
    
    case rtrans:
#if (CFG_MMUEN==1)
      if (r.Q.stpend == 1)
      {
        if (mcdo.ready & !r.Q.req)
          v.ready = 1;       //-- buffer store finish
      }
      
      v.holdn = 0;
      if (mmudco.transdata.finish == 1)
      {
        //-- translation error, i.e. page fault
        if (mmudco.transdata.accexc == 1)
        {
          v.holdn = 1;
          v.dstate = idle;
          mds = 0;
          mexc = !r.Q.mmctrl1.nf;
        }else
        {
          v.dstate = wread;
          v.cache = r.Q.cache & mmudco.transdata.cache;
          v.paddress = mmudco.transdata.data;
          if (v.wbinit == 1)
          {
            v.wb.addr = mmudco.transdata.data;
            v.req = 1;
          }
        }
      }
#endif
    break;

    case wread:     //-- read miss, wait for memory data
      taddr    = r.Q.xaddress&MSK32(OFFSET_HIGH,LINE_LOW);
      newtag   = r.Q.xaddress&MSK32(TAG_HIGH,TAG_LOW);
      newptag  = paddress&MSK32(TAG_HIGH,TAG_LOW);
      v.nomds  = r.Q.nomds & !eholdn;
      v.holdn  = v.nomds;
      rdatasel = memory;
      for (int32 i=0; i<CFG_DSETS; i++)
      {
        wlock &= ~(1<<i);
        wlock |= (r.Q.lock<<i);
      }
      for (int32 i=0; i<4; i++)
      {
        wlrr &= ~(1<<i);
        wlrr |= (r.Q.lrr<<i);
      }
      if ((r.Q.stpend == 0) && (r.Q.ready == 0))
      {
        if (mcdo.ready == 1)
        {
          mds = r.Q.holdn | r.Q.nomds;
          v.xaddress |= (1<<2);
          v.holdn = 1;
          if (r.Q.cctrl.dcs == 0x1)
          {
            v.hit  = mcdo.cache & r.Q.hit & r.Q.cache;
            twrite = v.hit;
          }else if (BIT32(r.Q.cctrl.dcs,1) == 1)
          {
            v.hit  = mcdo.cache & (r.Q.hit | (BIT32(r.Q.asi,3) & !BIT32(r.Q.asi,2))) & r.Q.cache;
            twrite = v.hit;
          }
          dwrite   = twrite; 
          rdatasel = memory;
          mexc     = mcdo.mexc;
          tpwrite  = twrite;
          
          if (r.Q.req == 0)
          {
            if( (((dci.enaddr & !mds)==1) || ((dci.eenaddr & mds & eholdn)==1))
             && ((BIT32(r.Q.cctrl.dcs,0)==1) || (CFG_DLRAMEN == 1)) )
            {
              v.dstate = loadpend;
              v.holdn  = 0;
            }else 
              v.dstate = idle;
          }else 
            v.nomds = 1;
        }
        v.mexc = mcdo.mexc;
        v.wb.data2 = mcdo.data;
      }else
      {
        if (r.Q.ready | (mcdo.ready & !r.Q.req))  //-- wait for store queue
        {
          v.burst = BIT32(r.Q.size,1) & BIT32(r.Q.size,0) & !BIT32(r.Q.xaddress,2);
          v.wb.addr = paddress;
          v.wb.size = r.Q.size; 
          v.wb.read = r.Q.read;
          v.wb.data1 = dci.maddress;
          v.req      = 1; 
          v.wb.lock  = dci.lock;
          v.wb.asi   = BITS32(r.Q.asi,3,0);
          v.ready    = 0;
        }
      }
    break; 
     
    case loadpend:    //-- return from read miss with load pending
      taddr = dci.maddress&MSK32(OFFSET_HIGH,LINE_LOW);
      if( (CFG_DLRAMEN == 1) && (dci.dsuen == 0) )
      {
        laddr = dci.maddress;
        if (BITS32(laddr,31,24)==LOCAL_RAM_START) lramcs = 1;
      }
      v.dstate = idle; 
    break;
    
    case dblwrite:    //-- second part of double store cycle
      v.dstate = idle;
      edata    = dci.edata;
      taddr    = r.Q.xaddress&MSK32(OFFSET_HIGH,LINE_LOW); 
      if( (CFG_DLRAMEN == 1) && (rlramrd == 1) )
      {
        laddr  = r.Q.xaddress;
        lramwr = 1;
      }else
      {
        if (BIT32(r.Q.cctrl.dcs,0) & r.Q.hit) dwrite = 1;
        v.wb.data2 = dci.edata; 
      }
    break;
    
    case asi_idtag:   //-- icache diag and inst local ram access 
      rdatasel   = icache;
      v.icenable = 1;
      v.holdn    = dci.dsuen;
      if (ico.diagrdy == 1)
      {
        v.dstate   = loadpend;
        v.icenable = 0;
        v.ilramen  = 0;
        if ((CFG_DSU==0) || ((CFG_DSU==1) && (dci.dsuen==0)))
          mds = !r.Q.read;
      }
    break;

    case wtrans:
      edata  = dci.edata;  //-- needed for STD store hit
      taddr  = r.Q.xaddress&MSK32(OFFSET_HIGH,LINE_LOW); 
      newtag = r.Q.xaddress&MSK32(TAG_HIGH,TAG_LOW);

      if (CFG_MMUEN)
      {
        if (r.Q.stpend == 1)
        {
          if (mcdo.ready & !r.Q.req)
            v.ready = 1;       //-- buffer store finish
        }

        //-- fetch dblwrite data 2, does the same as state dblwrite,
        //-- except that init of data2 is omitted to end of translation or in wwrite
        if ((r.Q.dblwdata==1) && (r.Q.size == 0x3))
          v.dblwdata = 0;
        
        v.holdn = 0;
        if (mmudco.transdata.finish == 1)
        {
          if (mmudco.transdata.accexc == 1)
          {
            v.holdn  = 1;
            v.dstate = idle;
            mds = 0;
            mexc = !r.Q.mmctrl1.nf;
            
            tagclear = BIT32(r.Q.cctrl.dcs,0) & r.Q.hit;
            
            twrite = tagclear;
            
            if( (twrite==1) && (((dci.enaddr & !mds)==1) || ((dci.eenaddr & mds & eholdn)== 1))
             && (BIT32(r.Q.cctrl.dcs,0)==1) )
            {
              v.dstate = loadpend;
              v.holdn = 0;
            }
          }else
          {
            v.dstate  = wwrite;
            v.cache    = mmudco.transdata.cache;
            v.paddress = mmudco.transdata.data;
            
            if (r.Q.wbinit)
            {
              v.wb.data2 = dci.edata; 
              v.wb.addr  = mmudco.transdata.data;
              v.dstate   = idle;
              v.holdn    = 1; 
              v.req      = 1;
              v.stpend   = 1;
              v.burst    = BIT32(r.Q.size,1) & BIT32(r.Q.size,0) & !BIT32(v.wb.addr,2);

              if( BIT32(r.Q.cctrl.dcs,0) && (r.Q.hit==1) && (r.Q.size==0x3) )  //-- write hit
                dwrite = 1;
            }
          }
        }
        //else
          //-- mmudci_trans_op := '1';  -- start translation
      }
    break;
        
    case wwrite:    //-- wait for store buffer to empty (store access)
      edata = dci.edata;  //-- needed for STD store hit
      if (v.ready | (mcdo.ready & !r.Q.req))  //-- store queue emptied
      {
        if( BIT32(r.Q.cctrl.dcs,0) && (r.Q.hit==1) && (r.Q.size==0x3) )//-- write hit
        {
          taddr  = r.Q.xaddress&MSK32(OFFSET_HIGH,LINE_LOW);
          dwrite = 1; 
        }
        v.dstate = idle; 

        v.req    = 1;
        v.burst  = BIT32(r.Q.size,1) & BIT32(r.Q.size,0);
        v.stpend = 1;

        v.wb.addr = paddress;
        v.wb.size = r.Q.size;
        v.wb.read = r.Q.read;
        v.wb.data1 = dci.maddress;
        v.wb.lock  = dci.lock;
        v.wb.data2 = dci.edata;
        v.wb.asi   = BITS32(r.Q.asi,3,0); 
        if (r.Q.size==0x3) 
          v.wb.addr &= ~(1<<2);
      }else  //-- hold cpu until buffer empty
        v.holdn = 0;
    break;

    case wflush:
      v.holdn = 0;
      if (mmudco.transdata.finish==1)
      {
        v.dstate = idle;
        v.holdn  = 1;
      }
    break;
    
    default: 
      v.dstate = idle;
    break;
  }

  if (CFG_DLRAMEN == 1) v.lramrd = lramcs; //-- read local ram data 
    
  //-- select data to return on read access
  //-- align if byte/half word read from cache or memory.

  if ((CFG_DSU == 1) && (dci.dsuen == 1))
  {
    v.dsuset = ddset; 
    switch(BITS32(dci.asi,4,0))
    {
      case ASI_ITAG: case ASI_IDATA:
        v.icenable = !ico.diagrdy;
        rdatasel   = icache;
      break;
      case ASI_DTAG:
        tdiagwrite = !dci.eenaddr & dci.enaddr & dci.write;
        twrite     = !dci.eenaddr & dci.enaddr & dci.write;
        rdatasel   = dtag; 
      break;
      case ASI_MMUSNOOP_DTAG:
        tdiagwrite = !dci.eenaddr & dci.enaddr & dci.write;
        tpwrite    = !dci.eenaddr & dci.enaddr & dci.write;
        rdatasel   = mmusnoop_dtag;
        senable    = 0x1f;//(others => '1');
      break;
      case ASI_DDATA:
#if (CFG_MMUEN==1)
        ddiagwrite = !dci.eenaddr & dci.enaddr & dci.write;
        dwrite     = !dci.eenaddr & dci.enaddr & dci.write;
        rdatasel   = dddata;
#endif
      break;
      case ASI_UDATA: case ASI_SDATA:
        lramwr = !dci.eenaddr & dci.enaddr & dci.write;
      break;
      case ASI_MMUREGS:
        rdatasel = misc;  
      break;
      default:;
    }
  }
    
  //-- read
#if (CFG_MMUEN==1)
  switch(BITS32(dci.maddress,CNR_U,CNR_D))
  {
    case CNR_CTRL:
      miscdata |= (r.Q.mmctrl1.e<<MMCTRL_E); // SH: this miscdata was reset at the begin
      miscdata |= (r.Q.mmctrl1.nf<<MMCTRL_NF); 
      miscdata |= (r.Q.mmctrl1.pso<<MMCTRL_PSO);
      miscdata |= (0x1<<MMCTRL_VER_D);//"0001";
      miscdata |= (0x0<<MMCTRL_IMPL_D);//"0000";
      miscdata |= (M_ENT_ILOG<<21);
      miscdata |= (M_ENT_DLOG<<18);
      if (M_TLB_TYPE==0)
      {
        miscdata |= (1<<MMCTRL_TLBSEP);
      }else
      {
        miscdata &= ~(MSK32(23,18));
        miscdata |= (M_ENT_CLOG<<21);
        //miscdata(20 downto 18) = (others => '0');
      }
      miscdata |= (r.Q.mmctrl1.tlbdis<<MMCTRL_TLBDIS);
      miscdata |= (pagesize<<MMCTRL_PGSZ_D); //-- r.mmctrl1.pagesize;
      //--custom 
    break;
    case CNR_CTXP:
      miscdata |= (r.Q.mmctrl1.ctxp<<MMCTXP_D); 
    break;
    case CNR_CTX:
      miscdata |= (r.Q.mmctrl1.ctx<<MMCTXNR_D); 
    break;
    case CNR_F:
      miscdata |= (mmudco.mmctrl2.fs.ow<<FS_OW);
      miscdata |= (mmudco.mmctrl2.fs.fav<<FS_FAV);
      miscdata |= (mmudco.mmctrl2.fs.ft<<FS_FT_D);
      miscdata |= (mmudco.mmctrl2.fs.at_ls<<FS_AT_LS);
      miscdata |= (mmudco.mmctrl2.fs.at_id<<FS_AT_ID);
      miscdata |= (mmudco.mmctrl2.fs.at_su<<FS_AT_SU);
      miscdata |= (mmudco.mmctrl2.fs.l<<FS_L_D);
      miscdata |= (mmudco.mmctrl2.fs.ebe<<FS_EBE_D);
    break;
    case CNR_FADDR:
      miscdata |= (mmudco.mmctrl2.fa<<VA_I_D); 
    break;
    default:;
  }
#endif


    
  rdata  = 0;
  for (int32 i=0; i<4; i++)   rdatav.arr[i] = 0;

  align_data  = 0;
  for (int32 i=0; i<4; i++) align_datav.arr[i] = 0;
  
  maddrlow &= ~(0x3);
  maddrlow |= (BITS32(maddress,1,0)); //-- stupid Synopsys VSS bug ...

#if 1
  if(iClkCnt>=34)
  bool st = true;
#endif
  switch (rdatasel)
  {
    case misc:
#if (CFG_MMUEN==1)
      set = 0;
      rdatav.arr[0] = miscdata;
#endif
    break;
    case dddata:
      rdatav = dcramov.data;
      if (dci.dsuen == 1) set = r.Q.dsuset;
      else                set = ddset;
    break;
    case dtag:
      rdatav = dcramov.tag;
      if (dci.dsuen == 1) set = r.Q.dsuset;
      else                set = ddset;
    break;
    case mmusnoop_dtag:
      for (int32 i=0; i<CFG_DSETS; i++)
      {
        rdatav.arr[i] |= (dcramov.stag.arr[i]&MSK32(TAG_HIGH,TAG_LOW)); 
        rdatav.arr[i] |= BIT32(snoophit,i);
      }
      if (dci.dsuen == 1) set = r.Q.dsuset;
      else                set = ddset;
    break;
    case dctx:
      for (int32 i=0; i<CFG_DSETS; i++)
      {
        rdatav.arr[i] &= ~MSK32(M_CTX_SZ-1,0);
        rdatav.arr[i] |= (dcramov.ctx.arr[i]&MSK32(M_CTX_SZ-1,0));
      }
      if (dci.dsuen == 1) set = r.Q.dsuset;
      else                set = ddset;
    break;
    case icache:
      rdatav.arr[0] = ico.diagdata;
      set = 0;
    break;
    case ddata: case memory:
      if (rdatasel == memory)
      {
        rdatav.arr[0] = mcdo.data;
        set           = 0; //--FIXME
      }else
      {
        for (int32 i=0; i<CFG_DSETS; i++) 
          rdatav.arr[i] = dcramov.data.arr[i];
      }
    break;
    case sysr:
      set = 0;
      switch(BITS32(dci.maddress,3,2))
      {
        case 0: 
        case 1:
          rdatav.arr[0] &= ~(1<<23);
          rdatav.arr[0] |= (r.Q.cctrl.dsnoop<<23);
#if (CFG_DSNOOP > 4)
          rdatav.arr[0] |= (1<<17);
#endif
          rdatav.arr[0] &= ~(MSK32(16,14));
          rdatav.arr[0] |= (r.Q.cctrl.burst<<16);
          rdatav.arr[0] |= (ico.flush<<15);
          rdatav.arr[0] |= (r.Q.flush<<14);
          
          rdatav.arr[0] &= ~(MSK32(5,0));
          rdatav.arr[0] |= (r.Q.cctrl.dfrz<<5);
          rdatav.arr[0] |= (r.Q.cctrl.ifrz<<4);
          rdatav.arr[0] |= (r.Q.cctrl.dcs<<2);
          rdatav.arr[0] |= (r.Q.cctrl.ics<<0);
        break;
        case 2:
          rdatav.arr[0] = ico.cfg;
        break;
        default:
          (*(cache_cfg*)(&rdatav.arr[0])).cfg_zerobits   = 0;
          (*(cache_cfg*)(&rdatav.arr[0])).cfg_mmuen      = CFG_MMUEN;
          (*(cache_cfg*)(&rdatav.arr[0])).cfg_lramstart  = CFG_DLRAMADDR;
          (*(cache_cfg*)(&rdatav.arr[0])).cfg_lramsize   = log2[CFG_DLRAMSZ];
          (*(cache_cfg*)(&rdatav.arr[0])).cfg_linesize   = log2[CFG_DLINE];
          (*(cache_cfg*)(&rdatav.arr[0])).cfg_lram       = CFG_DLRAMEN;
          (*(cache_cfg*)(&rdatav.arr[0])).cfg_setsize    = log2[CFG_DSETSZ];
          (*(cache_cfg*)(&rdatav.arr[0])).cfg_sets       = CFG_DSETS-1;
          (*(cache_cfg*)(&rdatav.arr[0])).cfg_snoop      = (CFG_DSNOOP!=0 ? 1: 0);
          (*(cache_cfg*)(&rdatav.arr[0])).cfg_repl       = CFG_DREPL+1;
          (*(cache_cfg*)(&rdatav.arr[0])).cfg_lock       = CFG_DLOCK;

        break;
    }
    default:;
  }
  //-- select which data to update the data cache with
  for (int32 i=0; i<CFG_DSETS; i++)
  {
    switch (size)    //-- merge data during partial write
    {
      case 0:
        switch(maddrlow)
        {
          case 0:
            ddatainv.arr[i] = (edata<<24) | BITS32(dcramov.data.arr[i],23,0);
          break;
          case 1:
            ddatainv.arr[i] = (BITS32(dcramov.data.arr[i],31,24)<<24) 
                            | (BITS32(edata,7,0)<<16) | BITS32(dcramov.data.arr[i],15,0);
          break;
          case 2:
            ddatainv.arr[i] = (BITS32(dcramov.data.arr[i],31,16)<<16) 
                            | (BITS32(edata,7,0)<<8) | BITS32(dcramov.data.arr[i],7,0);
          break;
          default:
            ddatainv.arr[i] = (BITS32(dcramov.data.arr[i],31,8)<<8) | BITS32(edata,7,0);
          break;
        }
      break;
      case 1:
        if (BIT32(maddress,1) == 0)
          ddatainv.arr[i] = (BITS32(edata,15,0)<<16) | BITS32(dcramov.data.arr[i],15,0);
        else
          ddatainv.arr[i] = (BITS32(dcramov.data.arr[i],31,16)<<16) | BITS32(edata,15,0);
      break;
      default:
        ddatainv.arr[i] = edata;
      break;
    }
  }

  //-- handle double load with pipeline hold

  if ((r.Q.dstate == idle) && (r.Q.nomds == 1))
  {
    rdatav.arr[0] = r.Q.wb.data2;
    mexc          = r.Q.mexc;
    set           = 0; //--FIXME
  }

  //-- Handle AHB retry. Re-generate bus request and burst

  if (mcdo.retry == 1)
  {
    v.req   = 1;
    v.burst = BIT32(r.Q.wb.size,0) & BIT32(r.Q.wb.size,1) & !BIT32(r.Q.wb.addr,2);
  }

  //-- Generate new valid bits

  vmaskdbl = decode(BITS32(maddress,LINE_HIGH,LINE_LOW+1));
  if ((size == 0x3) && (read == 0))
  {
    for (int32 i=0; i<CFG_DLINE; i++) 
    {
      vmaskraw &= ~(1<<i);
      vmaskraw |= (BIT32(vmaskdbl,(i/2)) << i);
    }
  }else
    vmaskraw = decode(BITS32(maddress,LINE_HIGH,LINE_LOW));

  for (int32 i=0; i<CFG_DLINE; i++) vmask.arr[i] = vmaskraw;
  if (r.Q.hit == 1)
  {
    for (int32 i=0; i<CFG_DSETS; i++)
      vmask.arr[i] = r.Q.valid.arr[i] | vmaskraw;
  }
  if (r.Q.dstate == idle)
  {
    for (int32 i=0; i<CFG_DSETS; i++)
      vmask.arr[i] = BITS32(dcramov.tag.arr[i],CFG_DLINE-1,0) | vmaskraw;
  }else
  {
    for (int32 i=0; i<CFG_DSETS; i++)
      tag.arr[i] |= (vmask.arr[i]&MSK32(CFG_DLINE-1,0));
  }

  if (mcdo.mexc | r.Q.flush) 
  {
    twrite = 0;
    dwrite = 0;
  }
  if (twrite == 1)
  {
    if (tagclear)
    {
      for (int32 i=0; i<CFG_DLINE; i++)
        vmask.arr[i] = 0;
    }
    v.valid = vmask;
    if( (CFG_DSETS>1) && (CFG_DREPL == lru) && (tdiagwrite==0) )
    {
      vl.write = 1;
      vl.set = setrepl;
    }
  }

  if( (CFG_DSETS>1) && (CFG_DREPL == lru) && (rl.Q.write==1))
  {
    vl.lru[rl.Q.waddr] = lru_calc(rl.Q.lru[rl.Q.waddr], rl.Q.set);
  }

  if (tdiagwrite==1) //-- diagnostic tag write
  {
    if( (CFG_DSU==1) && (dci.dsuen==1))
    {
      for (int32 i=0; i<CFG_DLINE; i++)
        vmask.arr[i] = BITS32(dci.maddress,CFG_DLINE - 1,0);
    }else
    {
      for (int32 i=0; i<CFG_DLINE; i++)
        vmask.arr[i] = BITS32(dci.edata,CFG_DLINE - 1,0);
      newtag = (dci.edata&MSK32(TAG_HIGH,TAG_LOW));
      newptag = (dci.edata&MSK32(TAG_HIGH,TAG_LOW));
      for (int32 i=0; i<4; i++)
      {
        wlrr &= ~(1<<i);
        wlrr |= (BIT32(dci.edata,CTAG_LRRPOS) << i);
      }
      for (int32 i=0;i<CFG_DSETS; i++)
      {
        wlock &= ~(1<<i);
        wlock |= (BIT32(dci.edata,CTAG_LOCKPOS) << i);
      }
    }
  }

  //-- mmureg write
  if (r.Q.mmctrl1wr == 1)
  {
    switch(BITS32(r.Q.xaddress,CNR_U,CNR_D))
    {
      case CNR_CTRL:
        v.mmctrl1.e      = BIT32(dci.maddress,MMCTRL_E);
        v.mmctrl1.nf     = BIT32(dci.maddress,MMCTRL_NF);
        v.mmctrl1.pso    = BIT32(dci.maddress,MMCTRL_PSO);
        v.mmctrl1.tlbdis = BIT32(dci.maddress,MMCTRL_TLBDIS);
        v.mmctrl1.pagesize = BITS32(dci.maddress,MMCTRL_PGSZ_U,MMCTRL_PGSZ_D);
        //--custom 
        //-- Note: before tlb disable tlb flush is required !!!  
      break;
      case CNR_CTXP:
        v.mmctrl1.ctxp = BITS32(dci.maddress,MMCTXP_U,MMCTXP_D);
      break;
      case CNR_CTX:
        v.mmctrl1.ctx  = BITS32(dci.maddress,MMCTXNR_U,MMCTXNR_D);
      break;
      case CNR_F:break;
      case CNR_FADDR:break;
      default:;
    }
  }
    
  //-- cache flush

  if (dci.flush | flush)
  {
    v.flush = 1;
    v.faddr = 0;
    v.pflush     = pflush;
    v.pflushr    = 1;
    v.pflushaddr = pflushaddr;
    v.pflushtyp  = pflushtyp;
  }

  if (r.Q.flush)
  {
    twrite = 1;
    for (int32 i=0; i<CFG_DLINE; i++)   vmask.arr[i] = 0;
    v.faddr = r.Q.faddr +1;
    newtag = 0;
    newptag = 0;
    taddr &= ~MSK32(OFFSET_HIGH,OFFSET_LOW);
    taddr |= ((r.Q.faddr<<OFFSET_LOW) & MSK32(OFFSET_HIGH,OFFSET_LOW));
    wlrr  = 0;
    v.lrr = 0;
    if ( BIT32(r.Q.faddr,DOFFSET_BITS-1) & !BIT32(v.faddr,DOFFSET_BITS-1) )
      v.flush = 0;
  }

  //-- AHB snoop handling (2), bypass write data on read/write contention

#if (DSNOOP2 == 2)
#if 1
if(iClkCnt>=768)
bool st=true;
#endif
  vh.hitaddr = BITS32(rs.Q.addr,OFFSET_HIGH,OFFSET_LOW);
  vh.taddr = taddr&MSK32(OFFSET_HIGH,OFFSET_LOW);
  if( (twrite==1) && (r.Q.dstate==wread) && (r.Q.flush==0) )
  {
    vh.clear &= ~(1<<setrepl);
    vh.clear |= (BIT32(rh.Q.snmiss,setrepl)<<setrepl);
  }
  if (r.Q.flush)
  {
    vh.snhit = 0x3;
    vh.hitaddr = r.Q.faddr;
  }
  for (int32 i=0; i<CFG_DSETS; i++)
  {
    if (BIT32(rh.Q.clear,i))  vh.hit[rh.Q.taddr>>OFFSET_LOW] &= ~(1<<i);
    if (BIT32(rh.Q.snhit,i))  vh.hit[rh.Q.hitaddr] |= (1<<i);
  }
#endif

  //-- update cache with memory data during read miss

  if (read == 1)
  {
    for (int32 i=0; i<CFG_DSETS; i++)
      ddatainv.arr[i] = mcdo.data;
  }

  //-- cache write signals

  if (twrite ==1)
  {
    if (tdiagwrite == 1) ctwrite |= (1<<ddset);
    else                 ctwrite |= (1<<setrepl);
  }
  if (DSNOOPSEP)
  {
    if (tpwrite == 1)
    {
      if (tdiagwrite == 1) ctpwrite |= (1<<ddset);
      else                 ctpwrite |= (1<<setrepl);
    }
  }
  if (dwrite == 1)
  {
    if (ddiagwrite == 1) cdwrite |= (1<<ddset);
    else                 cdwrite |= (1<<setrepl);
  }
    
  csnoopwe = 0; 
  if (snoopwe & !mcdo.scanen) csnoopwe |= (1<<snoopset);


  if (r.Q.flush & twrite)//-- flush 
  {
    ctwrite |= MSK32(MAXSETS-1,0);
    wlrr    = 0;
    wlock   = 0;
    if (DSNOOPSEP)
      ctpwrite = MSK32(MAXSETS-1,0);
     
   //-- precise flush, ASI_FLUSH_PAGE & ASI_FLUSH_CTX
#if 0//       if false then                    -- 
  if M_EN then
    if r.pflush = '1' then
      twrite := '0'; ctwrite := (others => '0');
      for i in DSETS-1 downto 0 loop
        wlrr(i) := dcramov.tag(i)(CTAG_LRRPOS); 
        wlock(i) := dcramov.tag(i)(CTAG_LOCKPOS);
      end loop;
      if r.pflushr = '0' then
        for i in DSETS-1 downto 0 loop
          pftag(OFFSET_HIGH downto OFFSET_LOW) := r.faddr;
          pftag(TAG_HIGH downto TAG_LOW) := dcramov.tag(i)(TAG_HIGH downto TAG_LOW);
          if ((pftag(VA_I_U downto VA_I_D) = r.pflushaddr(VA_I_U downto VA_I_D)) or
              (r.pflushtyp = '1')) then
            ctwrite(i) := '1';
            wlrr(i) := '0';
            wlock(i) := '0';
          end if;
        end loop;
      else
        v.faddr := r.faddr;
      end if;
      v.pflushr := not r.pflushr;
    end if;
  end if;
#endif
  }

  if (r.Q.flush2)
  {
    for (int32 i=0 ;i<0x1<<DOFFSET_BITS; i++)
      vl.lru[i] = 0;
  }

  //-- reset

  if (!rst)
  {
    v.dstate  = idle;
    v.stpend  = 0;
    v.req     = 0;
    v.burst   = 0;
    v.read    = 0;
    v.flush   = 0;
    v.nomds   = 0;
    v.holdn   = 1;
    v.rndcnt  = 0;
    v.setrepl = 0;
    v.dsuset  = 0;
    v.flush2  = 1;
    v.lrr     = 0;
    v.lock    = 0;
    v.ilramen = 0;
    v.cctrl.dcs = 0;
    v.cctrl.ics = 0;
    v.cctrl.burst  = 0;
    v.cctrl.dsnoop = 0;
    //--if M_EN then
      v.mmctrl1.e  = 0;
      v.mmctrl1.nf = 0;
      v.mmctrl1.ctx = 0;
      v.mmctrl1.tlbdis = 0;
      v.mmctrl1.pagesize = 0;
      v.mmctrl1.pso = 0;
      v.trans_op = 0; 
      v.flush_op = 0; 
      v.diag_op = 0;
      v.pflush  = 0;
      v.pflushr = 0;
      v.mmctrl1.pagesize = 0;
    //--end if;
    v.mmctrl1.bar = 0;
    v.faddr = 0;
  }

  if (CFG_DSNOOP == 0) v.cctrl.dsnoop = 0;
  if (CFG_MMUEN==0) v.mmctrl1 = mmctrl_type1_none; //-- kill MMU regs if not enabled

//-- Drive signals

  c = v; 
  cs = vs;
  ch = vh; //-- register inputs
  cl = vl;

    
  //-- tag ram inputs
  senable = senable & BITS32(~scanen,3,0);
  enable  = enable & BITS32(~scanen,3,0);
  if (mcdo.scanen) ctpwrite = 0;
  for (int32 i=0; i<CFG_DSETS; i++)
  {
    tag.arr[i] &= ~MSK32(CFG_DLINE-1,0);
    tag.arr[i] |= (vmask.arr[i]);
    tag.arr[i] &= ~MSK32(TAG_HIGH,TAG_LOW);
    tag.arr[i] |= (newtag&MSK32(TAG_HIGH,TAG_LOW));
    tag.arr[i] &= ~(1<<CTAG_LRRPOS);
    tag.arr[i] |= (BIT32(wlrr,i)<<CTAG_LRRPOS);
    tag.arr[i] &= ~(1<<CTAG_LOCKPOS);
    tag.arr[i] |= (BIT32(wlock,i)<<CTAG_LOCKPOS);
    ctx.arr[i] = r.Q.mmctrl1.ctx;
    ptag.arr[i] &= ~MSK32(TAG_HIGH,TAG_LOW);
    ptag.arr[i] |= (newptag&MSK32(TAG_HIGH,TAG_LOW));
  }
  dcrami.tag  = tag;
  dcrami.ptag = ptag;
  for (int32 i=0; i<4; i++) dcrami.ctx.arr[i]  = ctx.arr[i];
  dcrami.tenable   = enable;
  dcrami.twrite    = ctwrite;
  dcrami.tpwrite   = ctpwrite;
  dcrami.flush     = r.Q.flush;
  dcrami.senable  = senable;//--vs.snoop or rs.snoop;
  dcrami.swrite   = csnoopwe;
  dcrami.saddress = BITS32(snoopaddr,OFFSET_HIGH,OFFSET_LOW);
  dcrami.tdiag = (mcdo.testen<<3);
  dcrami.ddiag = (mcdo.testen<<3);
  
  //-- data ram inputs
  dcrami.denable   = enable;
  dcrami.address &= ~MSK32(19,(OFFSET_HIGH - LINE_LOW + 1));
  dcrami.address &= ~MSK32(OFFSET_HIGH - LINE_LOW,0);
  dcrami.address |= BITS32(taddr,OFFSET_HIGH,LINE_LOW);
  dcrami.data    = ddatainv;
  dcrami.dwrite  = cdwrite;
  dcrami.ldramin.address &= ~MSK32(23,2);
  dcrami.ldramin.address |= (laddr&MSK32(23,2));
  dcrami.ldramin.enable = (lramcs | lramwr) & !mcdo.scanen;
  dcrami.ldramin.read   = rlramrd;
  dcrami.ldramin.write  = lramwr;

  dcrami.spar = 0;
  dcrami.faddress = 0;
  for (int32 i=0; i<4; i++)
  {
    dcrami.dpar.arr[i] = 0;
    dcrami.tpar.arr[i] = 0;
  }
  dcrami.tdiag = (mcdo.testen<<3) | (mcdo.scanen<<2);
  dcrami.sdiag = (mcdo.testen<<3) | (mcdo.scanen<<2);
  dcrami.ddiag = (mcdo.testen<<3) | (mcdo.scanen<<2);

  //-- memory controller inputs
  mcdi.address  = r.Q.wb.addr;
  mcdi.data     = r.Q.wb.data1;
  mcdi.burst    = r.Q.burst;
  mcdi.size     = r.Q.wb.size;
  mcdi.read     = r.Q.wb.read;
  mcdi.asi      = r.Q.wb.asi;
  mcdi.lock     = r.Q.wb.lock;
  mcdi.req      = r.Q.req;
  mcdi.cache    = orv(r.Q.cctrl.dcs);

  //-- diagnostic instruction cache access
  dco.icdiag.flush  = iflush;
  dco.icdiag.pflush = pflush;
  dco.icdiag.pflushaddr = pflushaddr;
  dco.icdiag.pflushtyp = pflushtyp;
  dco.icdiag.read   = read;
  dco.icdiag.tag    = !BIT32(r.Q.asi,0);
  dco.icdiag.ctx    = BIT32(r.Q.asi,4); //--ASI_ICTX "10101"
  dco.icdiag.addr   = r.Q.xaddress;
  dco.icdiag.enable = r.Q.icenable;
  dco.icdiag.ilramen = r.Q.ilramen;    
  dco.icdiag.cctrl   = r.Q.cctrl;
  dco.icdiag.scanen  = mcdo.scanen;

  //-- IU data cache inputs
  dco.data = rdatav;
  dco.mexc = mexc;
  dco.set  = set;
  dco.hold = r.Q.holdn;
  dco.mds  = mds;
  dco.werr = mcdo.werr;
  dco.cache = 0;
  dco.idle  = sidle & !r.Q.stpend;
  dco.scanen  = mcdo.scanen;
  dco.testen  = mcdo.testen;
  
  //-- MMU
  mmudci.trans_op = mmudci_trans_op;    
  mmudci.transdata.data = mmudci_transdata_data; //--r.vaddr;
  mmudci.transdata.su   = mmudci_su;
  mmudci.transdata.read = mmudci_read;
  mmudci.transdata.isid = id_dcache;
  mmudci.transdata.wb_data = dci.maddress;
  
  mmudci.flush_op = mmudci_flush_op;
  mmudci.wb_op    = mmudci_wb_op;
  mmudci.diag_op  = mmudci_diag_op;
  mmudci.fsread   = mmudci_fsread;
  mmudci.mmctrl1  = r.Q.mmctrl1;

                       

  //-- Local registers
  r.CLK = clk;
  r.D = c;

#if (DSNOOP2 != 0)
  rs.CLK = sclk;
  rs.D = cs;
#else
  rs.D.snoop = rs.Q.snoop = 0;
  rs.D.addr = rs.Q.addr = 0; 
#endif

#if (DSNOOP2 == 2)
  rh.CLK = sclk;
  rh.D = ch;
#else
  rhhit <=  (others => (others => '0')); rh.taddr <=  (others => '0');
  rh.clear <=  (others => '0'); rh.snhit <=  (others => '0');
#endif

  if ((CFG_DSETS>1) && (CFG_DREPL==lru))
  {
    rl.CLK = clk;
    rl.D = cl;
  }else if ((CFG_DSETS == 1) || (CFG_DREPL != lru))
  {
      rl.D.write = rl.Q.write = 0;
      rl.D.waddr = rl.Q.waddr = 0;
      rl.D.set = rl.Q.set = 0;
      for (int32 i=0; i<0x1<<DOFFSET_BITS; i++)
        rl.D.lru[i] = rl.Q.lru[i] = 0;
  }
    
};


