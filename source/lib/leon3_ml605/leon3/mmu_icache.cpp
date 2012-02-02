//-- Description: This unit implements the instruction cache controller.

#include "lheaders.h"

//#define icen      : integer range 0 to 1  := 0;
//#define irepl     CFG_IREPL//: integer range 0 to 3  := 0;
//#define isets     CFG_ISETS//: integer range 1 to 4  := 1;
//#define ilinesize CFG_ILINE//: integer range 4 to 8  := 4;
//#define isetsize  CFG_ISETSZ//: integer range 1 to 256 := 1;
//#define isetlock  CFG_ILOCK//: integer range 0 to 1  := 0;
//#define lram      CFG_ILRAMEN//: integer range 0 to 1 := 0;
//#define lramsize  CFG_ILRAMSZ//: integer range 1 to 512 := 1;
//#define lramstart CFG_ILRAMADDR//: integer range 0 to 255 := 16#8e#;
//#define mmuen     CFG_MMUEN//: integer              := 0

//#define M_EN         CFG_MMUEN//: boolean := (mmuen = 1);
//#define ILINE_BITS    (log2[CFG_ILINE])//: integer := log2(ilinesize);
//#define IOFFSET_BITS  (8+log2[CFG_ISETSZ]-ILINE_BITS)//: integer := 8 +log2(isetsize) - ILINE_BITS;
#define TAG_LOW       (IOFFSET_BITS + ILINE_BITS + 2)//: integer := IOFFSET_BITS + ILINE_BITS + 2;
#define OFFSET_HIGH   (TAG_LOW - 1)//: integer := TAG_LOW - 1;
#define OFFSET_LOW    (ILINE_BITS + 2)//: integer := ILINE_BITS + 2;
#define LINE_HIGH     (OFFSET_LOW - 1)//: integer := OFFSET_LOW - 1;
//#define LINE_LOW      (2)//: integer := 2;
#define LRR_BIT       (TAG_HIGH + 1)//: integer := TAG_HIGH + 1;
//#define PCLOW         (2)//: integer  := 2;
//#define ILINE_SIZE    CFG_ILINE//: integer := ilinesize;
//#define ICLOCK_BIT    CFG_ILOCK//: integer := isetlock;
//#define ICREPLACE     CFG_IREPL//: integer range 0 to 3  := irepl;

#define lline         MSK32((ILINE_BITS-1),0)//: std_logic_vector((ILINE_BITS -1) downto 0) := (others=>'1');

#define LRAM_START    (CFG_ILRAMADDR&0xFF)//: std_logic_vector(7 downto 0) := conv_std_logic_vector(lramstart, 8);
#define LRAM_BITS     (log2[CFG_ILRAMSZ] + 10)//: integer := log2(lramsize) + 10;
#define LRAMCS_EN     false//: boolean := false;

#define SETBITS       (log2x[CFG_ISETS])//: integer := log2x(ISETS); 
#define ILRUBITS      (lru_table[CFG_ISETS])//: integer := lru_table(ISETS);


//******************************************************************

//******************************************************************
uint32 mmu_icache::lru_set ( lru_type lru, 
                             lock_type lock)// return std_logic_vector is    
{
  uint32 xlru;// : std_logic_vector(4 downto 0);
  uint32 set;//  : std_logic_vector(SETBITS-1 downto 0);
  uint32 xset;// : std_logic_vector(1 downto 0);
  //uint32 unlocked;// : integer range 0 to ISETS-1;

  set  = 0;
  xset = 0;
  xlru = 0; 
  xlru = lru; 
  
#if (CFG_ILOCK == 1)
  unlocked = CFG_ISETS-1;
  for (i =CFG_ISETS-1; i>=0; i--)
  {
    if lock(i) = '0' then unlocked := i; end if;
  }
#endif
    
#if (CFG_ISETS==2)
  #if (CFG_ILOCK == 1)
    if ((lock&0x1)== 1) xset |= 0x1;
    else                xset |= (xlru&0x1);
  #else
    xset |= (xlru&0x1);
  #endif
#elif (CFG_ISETS==3)
    if isetlock = 1 then
      xset := conv_std_logic_vector(lru3_repl_table(conv_integer(xlru)) (unlocked), 2);
    else
      xset := conv_std_logic_vector(lru3_repl_table(conv_integer(xlru)) (0), 2);
    end if;
#elif (CFG_ISETS==4)
    if isetlock = 1 then
      xset := conv_std_logic_vector(lru4_repl_table(conv_integer(xlru)) (unlocked), 2);
    else
      xset := conv_std_logic_vector(lru4_repl_table(conv_integer(xlru)) (0), 2);
    end if;
#else
  end case;
  set := xset(SETBITS-1 downto 0);
  
#endif

  return(set);
}

//******************************************************************
mmu_icache::lru_type mmu_icache::lru_calc(lru_type lru,
                              uint32 set)// : integer) return lru_type is
{
  lru_type new_lru;// : lru_type;
  uint32 xnew_lru;//: std_logic_vector(4 downto 0);
  uint32 xlru;// : std_logic_vector(4 downto 0);

  new_lru  = 0;
  xnew_lru = 0;
  xlru     = 0;
  xlru     = lru;
#if (CFG_ISETS==2)
  if (set == 0) xnew_lru |= 0x1;
  else          xnew_lru &= ~0x1;
#elif (CFG_ISETS==3)
  when 3 =>
    xnew_lru(2 downto 0) := lru_3set_table(conv_integer(lru))(set); 
#elif (CFG_ISETS==4)
  when 4 => 
    xnew_lru(4 downto 0) := lru_4set_table(conv_integer(lru))(set);
#endif
  new_lru = BITS32(xnew_lru,ILRUBITS-1,0);
  return(new_lru);
}


//******************************************************************
void mmu_icache::Update(uint32 rst,// : in  std_logic;
                        SClock clk,// : in  std_logic;
                        icache_in_type &ici,// : in  icache_in_type;
                        icache_out_type &ico,// : out icache_out_type;
                        dcache_in_type &dci,// : in  dcache_in_type;
                        dcache_out_type &dco,// : in  dcache_out_type;
                        memory_ic_in_type &mcii,// : out memory_ic_in_type;
                        memory_ic_out_type &mcio,// : in  memory_ic_out_type;
                        icram_in_type &icrami,// : out icram_in_type;
                        icram_out_type &icramo,// : in  icram_out_type;
                        uint32 fpuholdn,// : in  std_logic;
                        mmudc_in_type &mmudci,// : in  mmudc_in_type;
                        mmuic_in_type &mmuici,// : out mmuic_in_type;
                        mmuic_out_type &mmuico)// : in mmuic_out_type)
{
  cache_cfg tmp_icfg;
  tmp_icfg.cfg_zerobits   = 0;
  tmp_icfg.cfg_mmuen      = CFG_MMUEN;
  tmp_icfg.cfg_lramstart  = CFG_ILRAMADDR;
  tmp_icfg.cfg_lramsize   = log2[CFG_ILRAMSZ];
  tmp_icfg.cfg_linesize   = log2[CFG_ILINE];
  tmp_icfg.cfg_lram       = CFG_ILRAMEN;
  tmp_icfg.cfg_setsize    = log2[CFG_ISETSZ];
  tmp_icfg.cfg_sets       = CFG_ISETS - 1;
  tmp_icfg.cfg_snoop      = 0;
  tmp_icfg.cfg_repl       = CFG_IREPL+1;
  tmp_icfg.cfg_lock       = CFG_ILOCK;
  uint32 icfg = *((uint32*)&tmp_icfg);

//-- init local variables
  v  = r.Q;
  vl = rl.Q;
  vl.write = 0;
  vl.set   = r.Q.setrepl;
  vl.waddr = BITS32(r.Q.waddress,OFFSET_HIGH, OFFSET_LOW);

  mds = 1;
  dwrite = 0;
  twrite = 0;
  diagen = 0;
  error  = 0;
  write  = mcio.ready;
  v.diagrdy = 0;
  v.holdn   = 1; 
  v.flush3  = r.Q.flush2;
  sidle     = 0;

  cacheon = (dco.icdiag.cctrl.ics&0x1) & !r.Q.flush;
  enable  = 1;
  branch  = 0;
  eholdn  = dco.hold & fpuholdn;

  rdatasel = idata;  //-- read data from cache as default
  ddatain  = mcio.data; //-- load full word from memory
  //--if M_EN and (mmudci.mmctrl1.e = '1') then wtag := r.vaddress(TAG_HIGH downto TAG_LOW);
  //--else wtag := r.waddress(TAG_HIGH downto TAG_LOW); end if;
  wtag  = r.Q.vaddress & MSK32(TAG_HIGH, TAG_LOW);
  wlrr  = r.Q.lrr;
  wlock = r.Q.lock;
  
  tparerr = 0; 
  dparerr = 0;

  set = 0;
  ctwrite = 0;
  cdwrite = 0;
  vdiagset = 0;
  rdiagset = 0;
  lock    = 0;
  ilramwr = 0;
  lramacc = 0; 
  lramcs  = 0;
  pftag   = 0;

  v.trans_op  = r.Q.trans_op & (!mmuico.grant);
  mmuici_trans_op = r.Q.trans_op;
  
  
  mmuici_su = ici.su;
              
  //-- random replacement counter
#if (CFG_ISETS > 1)
  if (r.Q.rndcnt==(CFG_ISETS-1))  v.rndcnt = 0;
  else                            v.rndcnt = r.Q.rndcnt + 1;
#endif;

  //--local ram access
#if (CFG_ILRAMEN==1) 
  if(ici.fpc(31 downto 24) = LRAM_START) then lramacc := '1'; end if;
#endif

  //-- generate lock bits
  if (CFG_ILOCK == 1)
  {
    for (int32 i=0; i<CFG_ISETS; i++)
    {
      lock &= ~(0x1<<i);
      lock |= (((icramo.tag.arr[i]>>CTAG_LOCKPOS)&0x1) << i);
    }
  }
      
  //-- generate cache hit and valid bits    
  hit = 0;
#if (CFG_IREPL==3)//==dir)
  set := conv_integer(ici.fpc(OFFSET_HIGH + SETBITS downto OFFSET_HIGH+1));
  if (icramo.tag(set)(TAG_HIGH downto TAG_LOW) = ici.fpc(TAG_HIGH downto TAG_LOW))
      and ((icramo.ctx(set) = mmudci.mmctrl1.ctx) or (mmudci.mmctrl1.e = '0') or not M_EN)
  then hit := not r.flush; end if;
  validv(set) := genmux(ici.fpc(LINE_HIGH downto LINE_LOW), 
          icramo.tag(set)(ilinesize -1 downto 0));
#else
  for (int32 i=CFG_ISETS-1; i>=0; i--)
  {
    if ( (BITS32(icramo.tag.arr[i],TAG_HIGH,TAG_LOW) == BITS32(ici.fpc,TAG_HIGH,TAG_LOW))
      && ((icramo.ctx.arr[i]==mmudci.mmctrl1.ctx) || (mmudci.mmctrl1.e==0) || (CFG_MMUEN==0)) )
    {
      hit = !r.Q.flush;
      set = i;
    }
    validv &= ~(0x1<<i);
    validv |= ( genmux(BITS32(ici.fpc,LINE_HIGH,LINE_LOW), BITS32(icramo.tag.arr[i],CFG_ILINE-1,0)) << i) ;
  }
#endif

  if ((lramacc == 1) && (CFG_ISETS > 1)) set = 1;

  if (BITS32(ici.fpc,LINE_HIGH,LINE_LOW) == lline) lastline = 1;
  else                                             lastline = 0;

  if (BITS32(r.Q.waddress,LINE_HIGH,LINE_LOW)==MSK32(ILINE_BITS-1,0)) nlastline = 1;
  else                                                              nlastline = 0;

  if (BITS32(r.Q.waddress,LINE_HIGH,LINE_LOW+1)==MSK32((ILINE_BITS-2),0)) nnlastline = 1;
  else                                                                    nnlastline = 0;

  valid = (validv>>set)&0x1;

  xaddr_inc = (BITS32(r.Q.waddress,LINE_HIGH,LINE_LOW) + 1) & MSK32(ILINE_BITS-1,0);

  if (mcio.ready == 1)
  {
    v.waddress &= ~MSK32(LINE_HIGH,LINE_LOW);
    v.waddress |= (xaddr_inc<<LINE_LOW);
  }


  xaddr_inc = (BITS32(r.Q.vaddress,LINE_HIGH,LINE_LOW) + 1) & MSK32(ILINE_BITS-1,0);

  if (mcio.ready == 1)
  {
    v.vaddress &= ~MSK32(LINE_HIGH,LINE_LOW);
    v.vaddress |= (xaddr_inc<<LINE_LOW);
  }

  
  taddr = ici.rpc & MSK32(TAG_HIGH,LINE_LOW);

//-- main Icache state machine

  switch(r.Q.istate)
  {
    case idle:  //-- main state and cache hit
      v.valid = BITS32(icramo.tag.arr[set],CFG_ILINE-1,0);
      v.hit   = hit;
      v.su    = ici.su; 
      sidle   = 1;

      if (eholdn==0)
        taddr = ici.fpc&MSK32(TAG_HIGH,LINE_LOW);
      else taddr = ici.rpc & MSK32(TAG_HIGH,LINE_LOW);
      v.burst = dco.icdiag.cctrl.burst & !lastline;
      if (eholdn & !(ici.inull | lramacc))
      {
        if (!(cacheon & hit & valid))
        {
          v.istate = streaming;  
          v.holdn  = 0;
          v.overrun = 1;
          
          if (CFG_MMUEN && (mmudci.mmctrl1.e == 1))
          {
            v.istate = trans; 
            mmuici_trans_op = 1;
            v.trans_op = !mmuico.grant;
            v.cache = 0;
            //--v.req := '0';
          }else                          
          {
            v.req   = 1; 
            v.cache = 1;
          }
        }else
        {
          if ((CFG_ISETS > 1) && (CFG_IREPL == lru)) vl.write = 1;
        }
        v.waddress = ici.fpc&MSK32(31,CFG_PCLOW);
        v.vaddress = ici.fpc&MSK32(31,CFG_PCLOW);
      }
      if (dco.icdiag.enable == 1)  diagen = 1;
      ddatain = dci.maddress;
      if (CFG_ISETS > 1)
      {
        if (CFG_IREPL == lru)
        {
          vl.set = set; 
          vl.waddr = BITS32(ici.fpc,OFFSET_HIGH,OFFSET_LOW);
        }
        v.setrepl = set;
        if ( ((!hit && !((dparerr>>set)&0x1)) & !r.Q.flush) == 1)
        {
          switch(CFG_IREPL)
          {
            case rnd:
              if (CFG_ILOCK == 1)
              {
                if (!((lock>>r.Q.rndcnt)&0x1) ) v.setrepl = r.Q.rndcnt;
                else
                {
                  v.setrepl = CFG_ISETS-1;
                  for (int32 i=CFG_ISETS-1; i>=0; i--)
                  {
                    if ((((lock>>i)&0x1) == 0) && (i>(int32)r.Q.rndcnt))
                    {
                      v.setrepl = i;
                    }
                  }
                }
              }else
                v.setrepl = r.Q.rndcnt;
            break;
            case dir:
               v.setrepl = BITS32(ici.fpc,OFFSET_HIGH+SETBITS,OFFSET_HIGH+1);
            break;
            case lru:
              v.setrepl =  lru_set(rl.Q.arr[BITS32(ici.fpc,OFFSET_HIGH,OFFSET_LOW)], lock);
            break;
            case lrr:
              v.setrepl = 0;
              if (CFG_ILOCK == 1)
              {
                v.setrepl &= ~0x1;
                if (lock&0x1) v.setrepl |= 0x1;
                else          v.setrepl |= ( ((icramo.tag.arr[0]>>CTAG_LRRPOS)&0x1) ^ ((icramo.tag.arr[1]>>CTAG_LRRPOS)&0x1) );
              }else
                v.setrepl |= ( ((icramo.tag.arr[0]>>CTAG_LRRPOS)&0x1) ^ ((icramo.tag.arr[1]>>CTAG_LRRPOS)&0x1) );
              
              if (!(v.setrepl&0x1)) v.lrr = !((icramo.tag.arr[0]>>CTAG_LRRPOS)&0x1);
              else                  v.lrr = ((icramo.tag.arr[0]>>CTAG_LRRPOS)&0x1);
          }
        }
        if (CFG_ILOCK == 1)
        {
          if (hit & ((lock>>set)&0x1)) v.lock = 1;
          else                         v.lock = 0;
        }
      }
    break;
    case trans:
#if (CFG_MMUEN==1)
      v.holdn = 0;
      if (mmuico.transdata.finish == 1)
      {
        if (mmuico.transdata.accexc & (!mmudci.mmctrl1.nf | r.Q.su))
        {
          //-- if su then always do mexc
          error    = 1;
          mds      = 0;
          v.holdn  = 0;
          v.istate = stop;
          v.burst  = 0;
        }else
        {
          v.cache    = mmuico.transdata.cache;
          v.waddress = mmuico.transdata.data&MSK32(31,CFG_PCLOW);
          v.istate   = streaming;
          v.req      = 1; 
        }
      }
#endif
    break;
      
    case streaming:   //-- streaming: update cache and send data to IU
      rdatasel = memory;
      taddr &= ~MSK32(TAG_HIGH,LINE_LOW);
      taddr |= (r.Q.vaddress&MSK32(TAG_HIGH,LINE_LOW));
      branch = (ici.fbranch & r.Q.overrun) | (ici.rbranch & !r.Q.overrun);
      v.underrun = r.Q.underrun |
        (write & ((ici.inull | !eholdn) & (mcio.ready & !(r.Q.overrun & !r.Q.underrun))));
      v.overrun = (r.Q.overrun | (eholdn & !ici.inull)) & !(write | r.Q.underrun);
      if (mcio.ready == 1)
      {
        mds = !(r.Q.overrun & !r.Q.underrun);
        v.burst = v.req & !(nnlastline & mcio.ready);
      }
      if (mcio.grant == 1)
      {
        v.req = dco.icdiag.cctrl.burst & r.Q.burst & 
           (!(nnlastline & mcio.ready)) & (dco.icdiag.cctrl.burst | !branch) & !(v.underrun & !cacheon);
        v.burst = v.req & !(nnlastline & mcio.ready);
      }
      v.underrun = (v.underrun | branch) & !v.overrun;
      v.holdn = !(v.overrun | v.underrun);
      if ((mcio.ready==1) && (r.Q.req==0)) //--(v.burst = '0') then
      {
        v.underrun = 0;
        v.overrun  = 0;
        if (BIT32(dco.icdiag.cctrl.ics,0) & !r.Q.flush2)
        {
          v.istate = stop; 
          v.holdn  = 0;
        }else
        {
          v.istate = idle;
          v.flush  = r.Q.flush2;
          v.holdn  = 1;
          if (r.Q.overrun==1) taddr = ici.fpc &MSK32(TAG_HIGH,LINE_LOW);
          else                taddr = ici.rpc &MSK32(TAG_HIGH,LINE_LOW);
        }
      }
    break;
    case stop:    //-- return to main
      taddr = ici.fpc &MSK32(TAG_HIGH,LINE_LOW);
      v.istate = idle;
      v.flush  = r.Q.flush2;
    break;
    default: v.istate = idle;
  }

  if (mcio.retry == 1) v.req = 1;

  if (CFG_ILRAMEN == 1)
  {
    if (LRAMCS_EN)
    {
      if (BITS32(taddr,31,24)==LRAM_START) lramcs = 1;
      else                                 lramcs = 0;
    }else
      lramcs = 1;
  }

//-- Generate new valid bits write strobe
  vmaskraw = decode(BITS32(r.Q.waddress,LINE_HIGH,LINE_LOW));
  twrite = write;
  if (cacheon == 0)
  {
    twrite = 0;
    vmask  = 0;
  }else if (dco.icdiag.cctrl.ics == 0x1)
  {
    twrite = twrite & r.Q.hit;
    vmask  = BITS32(icramo.tag.arr[set],CFG_ILINE-1,0) | vmaskraw;
  }else
  {
    if (r.Q.hit == 1) vmask = r.Q.valid | vmaskraw;
    else              vmask = vmaskraw;
  }
  if (mcio.mexc | (!mcio.cache) | (!r.Q.cache))
  {
    twrite = 0;
    dwrite = 0;
  }else 
    dwrite = twrite;
  if (twrite == 1)
  {
    v.valid = vmask;
    v.hit   = 1;
    if ((CFG_ISETS > 1) && (CFG_IREPL == lru)) vl.write = 1;
  }

  if ((CFG_ISETS > 1) && (CFG_IREPL == lru) && (rl.Q.write == 1))
    vl.arr[rl.Q.waddr] = lru_calc(rl.Q.arr[rl.Q.waddr], rl.Q.set);
  

//-- cache write signals
  if (CFG_ISETS > 1) setrepl = r.Q.setrepl;
  else               setrepl = 0;
  if (twrite == 1) ctwrite |= (1<<setrepl);
  if (dwrite == 1) cdwrite |= (1<<setrepl);
    
//-- diagnostic cache access
  if (diagen == 1)
  {
    if (CFG_ISETS != 1)
    {
     if ((dco.icdiag.ilramen==1) && (CFG_ILRAMEN==1)) v.diagset = 0x1;
     else  v.diagset = BITS32(dco.icdiag.addr, SETBITS-1+TAG_LOW, TAG_LOW);
    }
  }

  if (CFG_ISETS != 1)
  {
    rdiagset = r.Q.diagset;
    vdiagset = v.diagset;
  }

  diagdata = icramo.data.arr[rdiagset];
  if (diagen == 1) //-- diagnostic access
  {
    taddr &= ~MSK32(TAG_HIGH,LINE_LOW);
    taddr |= dco.icdiag.addr&MSK32(TAG_HIGH,LINE_LOW);
    wtag = (dci.maddress&MSK32(TAG_HIGH, TAG_LOW));
    wlrr = (dci.maddress>>CTAG_LRRPOS)&0x1;
    wlock = (dci.maddress>>CTAG_LOCKPOS)&0x1;
    if ((dco.icdiag.ilramen == 1) && (CFG_ILRAMEN == 1))
    {
      ilramwr = !dco.icdiag.read;
    }else if (dco.icdiag.tag == 1)
    {
      twrite  = !dco.icdiag.read;
      dwrite  = 0;
      ctwrite = 0;
      cdwrite = 0;
      ctwrite &= ~(0x1<<vdiagset);
      ctwrite |= ((!dco.icdiag.read)<<vdiagset);
      diagdata = icramo.tag.arr[rdiagset];
    }else
    {
      dwrite  = !dco.icdiag.read;
      twrite  = 0;
      cdwrite = 0;
      cdwrite |= ((!dco.icdiag.read)<<vdiagset);
      ctwrite = 0;
    }
    vmask = BITS32(dci.maddress,CFG_ILINE-1,0);
    v.diagrdy = 1;
    if ((dco.icdiag.ilramen==1) && (CFG_ILRAMEN==1) && (r.Q.flush2==1))
      v.diagrdy = 0;
  }

//-- select data to return on read access

  rdata = icramo.data;
  switch(rdatasel)
  {
    case memory:  rdata.arr[0] = mcio.data; set = 0; break;
    default: ; 
  }

//-- cache flush

  if ((ici.flush | dco.icdiag.flush) == 1)
  {
    v.flush  = 1;
    v.flush2 = 1;
    v.faddr  = 0;
    v.pflush = dco.icdiag.pflush;
    wtag     = 0;
    v.pflushr    = 1;
    v.pflushaddr = dco.icdiag.pflushaddr;
    v.pflushtyp  = dco.icdiag.pflushtyp;
  }

  if (r.Q.flush2 == 1)
  {
    twrite  = 1;
    ctwrite = -1;
    vmask   = 0;
    v.faddr = r.Q.faddr +1; 
    taddr &= ~MSK32(OFFSET_HIGH,OFFSET_LOW);
    taddr |= (r.Q.faddr<<OFFSET_LOW);
    wlrr  = 0;
    wlock = 0;
    if ( BIT32(r.Q.faddr,(IOFFSET_BITS-1)) & !BIT32(v.faddr,(IOFFSET_BITS -1)) )
      v.flush2 = 0;
    v.lrr = 0;
   
    //-- precise flush, ASI_FLUSH_PAGE & ASI_FLUSH_CTX
    if (CFG_MMUEN==1)
    {
      if (r.Q.pflush == 1)
      {
        twrite  = 0;
        ctwrite = 0;
        v.pflushr = !r.Q.pflushr;
        if (r.Q.pflushr == 0)
        {
          for (int32 i=CFG_ISETS-1; i>=0; i--)
          {
            pftag &= ~MSK32(OFFSET_HIGH,OFFSET_LOW);
            pftag |= (r.Q.faddr<<OFFSET_LOW);
            pftag &= ~MSK32(TAG_HIGH, TAG_LOW);
            pftag |= (icramo.tag.arr[i]&MSK32(TAG_HIGH,TAG_LOW)); //--icramo.itramout(i).tag;
            //--if (icramo.itramout(i).ctx = mmudci.mmctrl1.ctx) and
            //--   ((pftag(VA_I_U downto VA_I_D) = r.pflushaddr(VA_I_U downto VA_I_D)) or
            //--    (r.pflushtyp = '1')) then
              ctwrite |= (0x1<<i);
            //--end if;
          }
        }
      }
    }
  }

//-- reset

  if (!rst)
  {
    v.istate = idle;
    v.req    = 0;
    v.burst  = 0;
    v.holdn  = 1;
    v.flush  = 0;
    v.flush2 = 0;
    v.overrun  = 0;
    v.underrun = 0;
    v.rndcnt   = 0;
    v.lrr      = 0;
    v.setrepl  = 0;
    v.diagset  = 0;
    v.lock     = 0;
    v.trans_op = 0;
    v.flush3   = 1;
  }

  if (r.Q.flush3 == 1)
  {
    for (int32 i=0; i<(0x1<<IOFFSET_BITS); i++)
      vl.arr[i] = 0;
  }
    
  //-- Drive signals

  c  = v;     //-- register inputs
  cl = vl;  //-- lru register inputs

  //-- tag ram inputs
  enable = enable & !dco.icdiag.scanen;
  for (int32 i=0; i<CFG_ISETS; i++)
  {
    tag.arr[i] = 0;
    tag.arr[i] |= (vmask&MSK32(CFG_ILINE-1,0));
    tag.arr[i] |= (wtag);
    tag.arr[i] |= (wlrr<<CTAG_LRRPOS);
    tag.arr[i] |= (wlock<<CTAG_LOCKPOS);
  }
  icrami.tag     = tag;
  icrami.tenable = enable;
  icrami.twrite  = ctwrite;
  icrami.flush   = r.Q.flush2;
  icrami.ctx     = mmudci.mmctrl1.ctx;

  //-- data ram inputs
  icrami.denable  = enable;
  icrami.address  = BITS32(taddr,19+LINE_LOW,LINE_LOW);
  icrami.data     = ddatain;
  icrami.dwrite   = cdwrite; 
  
  //-- local ram inputs
  icrami.ldramin.enable = (dco.icdiag.ilramen | lramcs | lramacc) & !dco.icdiag.scanen;
  icrami.ldramin.read  = dco.icdiag.ilramen | lramacc;
  icrami.ldramin.write = ilramwr;
  

  //-- memory controller inputs
  mcii.address = r.Q.waddress&MSK32(31,2);
  mcii.su       = r.Q.su;
  mcii.burst    = r.Q.burst;
  mcii.req      = r.Q.req;
  mcii.flush    = r.Q.flush;

  //-- mmu <-> icache
  mmuici.trans_op = mmuici_trans_op;
  mmuici.transdata.data = r.Q.waddress&MSK32(31,2);
  mmuici.transdata.su   = r.Q.su;
  mmuici.transdata.isid = id_icache;
  mmuici.transdata.read = 1;
  mmuici.transdata.wb_data = 0;

  //-- IU data cache inputs
  ico.data      = rdata;
  ico.mexc      = mcio.mexc | error;
  ico.hold      = r.Q.holdn;
  ico.mds       = mds;
  ico.flush     = r.Q.flush;
  ico.diagdata  = diagdata;
  ico.diagrdy   = r.Q.diagrdy;
  ico.set       = set&0x3;
  ico.cfg       = icfg;
  ico.idle      = sidle;


//-- Local registers
  r.CLK = clk;
  r.D   = c;

  rl.CLK = clk;
  if ((CFG_ISETS > 1) && (CFG_IREPL == lru))
  {
    rl.D = cl;
  }

  if ((CFG_ISETS == 1) || (CFG_IREPL != lru))
  {
    rl.D.write = 0;
    rl.D.waddr = 0;
    rl.D.set = 0;
    for (int32 i=0; i<(0x1<<IOFFSET_BITS); i++)
      rl.D.arr[i] = 0;
  }
};

