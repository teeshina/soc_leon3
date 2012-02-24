//-----------------------------------------------------------------------------   
//-- Description: Interface module between I/D cache controllers and Amba AHB
//------------------------------------------------------------------------------  
#include "lheaders.h"

//#define hindex    AHB_MASTER_LEON3: integer range 0 to NAHBMST-1  := 0;
//#define ilinesize CFG_ILINE//: integer range 4 to 8 := 4;
//#define cached    CFG_DFIXED//: integer := 0;
//#define clk2x     CFG_CLK2X//: integer := 0;
//#define scantest  : integer := 0);

#define ctbl (CFG_DFIXED&MSK32(15,0))

//******************************************************************
uint32 ahb_slv_dec_cache(uint32 haddr,// : std_logic_vector(31 downto 0);
                         ahb_slv_out_vector &ahbso,// : ahb_slv_out_vector;
                         uint32 cached)// : integer)  return std_ulogic is
{
  uint32 hcache = 0;// : std_ulogic;
  uint32 uiCtbl = 0;// : std_logic_vector(15 downto 0);

  if (cached == 0)
  {
    for (uint32 i=0; i<AHB_SLAVES_MAX; i++)
    {
      for (uint32 j=AHB_MEM_ID_WIDTH; j<AHB_CFG_WORDS; j++)
      {
        if ( BIT32(ahbso.arr[i].hconfig.arr[j],16) && (BITS32(ahbso.arr[i].hconfig.arr[j],15,4) != 0) )
        {
          if( (BITS32(haddr,31,20) & BITS32(ahbso.arr[i].hconfig.arr[j],15,4)) 
           == (BITS32(ahbso.arr[i].hconfig.arr[j],31,20) & BITS32(ahbso.arr[i].hconfig.arr[j],15,4)) )
          {
            hcache = 1;
          }
        }
      }
    }
  }else
  {
    uiCtbl = MSK32(15,0)&cached;
    hcache = BIT32(uiCtbl, BITS32(haddr,31,28));
  }
  return(hcache);
}

//******************************************************************
uint32 ahbdrivedata(uint32 hdata, uint32 width)// : std_logic_vector)   return std_logic_vector is
{
  uint32 data=0;// : std_logic_vector(AHBDW-1 downto 0);
  if (CFG_AHBDW < width)
    data = hdata;
  else
  {
    for (uint32 i=0; i<CFG_AHBDW/width; i++)
      data |= (hdata<<(i*width)); 
  }
  return data;
}


//******************************************************************
uint32 mmu_acache::dec_fixed(uint32 haddr,// : std_logic_vector(3 downto 0);
                             uint32 cached)// : integer) return std_ulogic is
{
  if (cached != 0) return BIT32(ctbl, BITS32(haddr,3,0));
  else             return(1);
}

//******************************************************************
void mmu_acache::Update( uint32 rst,//    : in  std_logic;
                        SClock clk,//    : in  std_logic;
                        memory_ic_in_type &mcii,//   : in  memory_ic_in_type;
                        memory_ic_out_type &mcio,//   : out memory_ic_out_type;
                        memory_dc_in_type &mcdi,//   : in  memory_dc_in_type;
                        memory_dc_out_type &mcdo,//   : out memory_dc_out_type;
                        memory_mm_in_type &mcmmi,//  : in  memory_mm_in_type;
                        memory_mm_out_type &mcmmo,//  : out memory_mm_out_type;
                        ahb_mst_in_type &ahbi,//   : in  ahb_mst_in_type;
                        ahb_mst_out_type &ahbo,//   : out ahb_mst_out_type;
                        ahb_slv_out_vector &ahbso,//  : in  ahb_slv_out_vector;
                        uint32 hclken)// : in  std_ulogic );
{

  //-- initialisation

  htrans = HTRANS_IDLE;
  v = r.Q;
  v.werr = 0;
  v2 = r2.Q;
  iready = 0;
  dready = 0;
  mmready = 0;
  igrant = 0;
  dgrant = 0;
  mmgrant = 0; 
  imexc   = 0;
  dmexc = 0;
  mmmexc = 0;
  hlock = 0;
  iretry = 0;
  dretry = 0;
  mmretry = 0;
  ihcache = 0;
  dhcache = 0;
  mmhcache = 0;
  su = 0;
  if (r.Q.bo == 0x0) bo_icache = 1;
  else               bo_icache = 0;
  
  haddr  = 0;
  hwrite = 0;
  hsize  = 0;
  hlock  = 0; 
  hburst = 0; 
  if (ahbi.hready == 1) v.lb = 0;
  if (CFG_SCANTEST_ENA == 1) scanen = ahbi.scanen;
  else                       scanen = 0;
  v.retry2 = (r.Q.retry | r.Q.retry2) & !(r.Q.ba & !r.Q.retry);
  vreqmsk = orv(r2.Q.reqmsk);

  //-- generate AHB signals
#if 1
  if(iClkCnt>35)
  bool st = true;
#endif
  dreq = mcdi.req;
  hwdata = mcdi.data;
  hbusreq = 0;

  if( mcii.req && ((CFG_CLK2X==0) || BIT32(r2.Q.reqmsk,2)) && (r.Q.hlocken==0)
   && !( ((r.Q.ba & dreq) && (r.Q.bo==0x1)) || ((r.Q.ba & mcmmi.req) && (r.Q.bo==0x2)) ) )
 {
    nbo  = 0;
    hbusreq = 1;
    htrans = HTRANS_NONSEQ;
  }else if( dreq && ((CFG_CLK2X==0) || BIT32(r2.Q.reqmsk,1)) 
   && !(( (r.Q.ba & mcii.req) && (r.Q.bo==0)) || ( (r.Q.ba & mcmmi.req) && (r.Q.bo==0x2))) )
  {
    nbo = 0x1;
    hbusreq = 1;
    if (!mcdi.lock | r.Q.hlocken) 
      htrans = HTRANS_NONSEQ;
  }else if( mcmmi.req && ((CFG_CLK2X==0) || BIT32(r2.Q.reqmsk,0)) && (r.Q.hlocken==0)
    && !(( (r.Q.ba & mcii.req) && (r.Q.bo==0x0)) || ( (r.Q.ba & dreq) && (r.Q.bo==0x1))) )
  {
    nbo = 0x2;
    hbusreq = 1;
    htrans = HTRANS_NONSEQ;
  }else
    nbo = 0x3;

  //-- dont change bus master if we have started driving htrans
  if (r.Q.nba == 1)
  {
    nbo = r.Q.nbo;
    hbusreq = 1;
    htrans = HTRANS_NONSEQ;
  }

  //-- dont change bus master on retry
  if (r.Q.retry2 & !r.Q.ba)
  {
    nbo = r.Q.bo;
    hbusreq = 1;
    htrans = HTRANS_NONSEQ;
  }

  dec_hcache = ahb_slv_dec_cache(mcdi.address, ahbso, CFG_DFIXED);
  
  if (nbo == 0x2)
  {
    haddr  = mcmmi.address; 
    hwrite = !mcmmi.read; 
    hsize  = mcmmi.size;
    hlock  = mcmmi.lock;
    htrans = HTRANS_NONSEQ; 
    hburst = HBURST_SINGLE; 
    if (mcmmi.req & r.Q.bg & ahbi.hready & !r.Q.retry)
    {
      mmgrant  = 1;
      v.hcache = dec_fixed(BITS32(haddr,31,28), CFG_DFIXED);
    }
  }else if (nbo == 0x0)
  {
    haddr  = mcii.address;
    hwrite = 0;
    hsize  = HSIZE_WORD;
    hlock  = 0;
    su     = mcii.su;
    if( ((mcii.req & r.Q.ba)==1) && (r.Q.bo==0x0) && (!r.Q.retry) )
    {
      htrans = HTRANS_SEQ;
      uint32 tmp = BITS32(haddr,4,2) + 1;
      haddr &= ~MSK32(4,2);
      haddr |= ((BITS32(tmp,2,0)) << 2);
      if( (((CFG_ILINE==4) && (BITS32(haddr,3,2)==0x2)) || ((CFG_ILINE==8) && (BITS32(haddr,4,2)==0x6))) 
       && (ahbi.hready == 1) )
      {
        v.lb = 1;
      }
    }
    if (mcii.burst==1) hburst = HBURST_INCR; 
    else               hburst = HBURST_SINGLE;
    if (mcii.req & r.Q.bg & ahbi.hready & !r.Q.retry)
    {
      igrant   = 1;
      v.hcache = dec_fixed(BITS32(haddr,31,28), CFG_DFIXED);
    }
  }else if (nbo == 0x1)
  {
    haddr  = mcdi.address;
    hwrite = !mcdi.read;
    hsize  = mcdi.size;
    hlock  = mcdi.lock; 
    if (mcdi.asi != 0xA) su = 1;
    else                 su = 0;  //--ASI_UDATA
    if (mcdi.burst==1) hburst = HBURST_INCR; 
    else               hburst = HBURST_SINGLE;
    if( ((dreq & r.Q.ba)==1) && (r.Q.bo==0x1) && (!r.Q.retry) )
    {
      htrans = HTRANS_SEQ;
      uint32 tmp = BITS32(haddr,4,2) + 1;
      haddr &= ~MSK32(4,2);
      haddr |= (BITS32(tmp,2,0)<<2);
      hburst = HBURST_INCR; 
    }
    if (dreq & r.Q.bg & ahbi.hready & !r.Q.retry)
    {
      dgrant   = (!mcdi.lock | r.Q.hlocken) | r.Q.retry2;
      v.hcache = dec_hcache;
    }
  }

  if ((hclken==1) || (CFG_CLK2X == 0))
  {
    if ((r.Q.ba==1) && ((ahbi.hresp==HRESP_RETRY) || (ahbi.hresp==HRESP_SPLIT)))
      v.retry = !ahbi.hready;
    else 
      v.retry = 0;
  }
    
  if (r.Q.retry) htrans = HTRANS_IDLE;

  if (r.Q.bo == 0x2)
  {
    hwdata = mcmmi.data;
    if (r.Q.ba == 1)
    {
      mmhcache = r.Q.hcache;
      if (ahbi.hready)
      {
        switch(ahbi.hresp)
        {
          case HRESP_OKAY: mmready = 1; break;
          case HRESP_RETRY:
          case HRESP_SPLIT: mmretry = 1; break; 
          default: 
            mmready = 1;
            mmmexc  = 1;
            v.werr  = !mcmmi.read;
        }
      }
    }
  }else if (r.Q.bo == 0x0)
  {
    if (r.Q.ba == 1)
    {
      ihcache = r.Q.hcache;
      if (ahbi.hready)
      {
        switch(ahbi.hresp)
        {
          case HRESP_OKAY: iready = 1;  break;
          case HRESP_RETRY:
          case HRESP_SPLIT: iretry = 1; break;
          default: 
            iready = 1;
            imexc = 1;
        }
      }
    }
  }else if (r.Q.bo == 0x1)
  {
    if (r.Q.ba == 1)
    {
      dhcache = r.Q.hcache;
      if (ahbi.hready)
      {
        switch(ahbi.hresp)
        {
          case HRESP_OKAY: dready = 1; break;
          case HRESP_RETRY:
          case HRESP_SPLIT: dretry = 1; break;
          default:
            dready = 1;
            dmexc  = 1;
            v.werr = !mcdi.read;
        }
      }
    }
    hlock = mcdi.lock | ((r.Q.retry | (r.Q.retry2 & !r.Q.ba)) & r.Q.hlocken);
  }

  if( (nbo==0x1) && ((hsize==0x3) || ((dec_hcache & mcdi.read & mcdi.cache)==1)) )
  {
    hsize = 0x2;
    haddr &= ~MSK32(1,0);
  }

  if( (r.Q.bo==0x1) && (hlock==1)) nbo = 0x1;
  if (ahbi.hready)
  {
    if (r.Q.retry==0) v.bo = nbo;
    v.bg = BIT32(ahbi.hgrant,hindex);
    if( (htrans==HTRANS_NONSEQ) || (htrans==HTRANS_SEQ) ) v.ba = r.Q.bg;
    else                                                  v.ba = 0;
    v.hlocken = hlock & BIT32(ahbi.hgrant,hindex);
    if (CFG_CLK2X != 0)
    {
      igrant  = igrant & vreqmsk;
      dgrant  = dgrant & vreqmsk;
      mmgrant = mmgrant & vreqmsk;
      if (r.Q.bo==nbo) 
        v.ba = v.ba & vreqmsk;
    }
  }

  if (hburst==HBURST_SINGLE)  nb = 1;
  else                        nb = 0;

  v.nbo = nbo;
  v.nba = orv(htrans) & !v.ba;

  if (CFG_CLK2X!= 0)
  {
    v2.hclken2 = hclken;
    if (hclken == 1)
    {
      v2.reqmsk = (mcii.req<<2) | (mcdi.req<<1) | mcmmi.req;
      if((CFG_CLK2X > 8) && (r2.Q.hclken2==1)) v2.reqmsk = 0x7;
    }
  }
                                              
  
  //-- reset operation

  if (!rst)
  {
    v.bg = 0;
    v.bo = 0;
    v.ba = 0;
    v.retry = 0;
    v.werr  = 0;
    v.lb    = 0;
    v.hcache  = 0;
    v.hlocken = 0;
    v.nba     = 0;
    v.nbo     = 0;
    v.retry2  = 0;
  }

  //-- drive ports
  
  ahbo.haddr   = haddr ;
  ahbo.htrans  = htrans;
  ahbo.hbusreq = hbusreq & !r.Q.lb & !((((!bo_icache) & r.Q.ba) | nb) & r.Q.bg);
  ahbo.hwdata  = ahbdrivedata(hwdata,32);
  ahbo.hlock   = hlock;
  ahbo.hwrite  = hwrite;
  ahbo.hsize   = hsize;
  ahbo.hburst  = hburst;
  ahbo.hindex  = hindex;
  if (nbo==0) ahbo.hprot = (0x3<<2) | (su<<1) | 0;
  else        ahbo.hprot = (0x3<<2) | (su<<1) | 1;

  mcio.grant   = igrant;
  mcio.ready   = iready;
  mcio.mexc    = imexc;
  mcio.retry   = iretry;
  mcio.cache   = ihcache;
  mcio.par     = 0;
  mcdo.grant   = dgrant;
  mcdo.ready   = dready;
  mcdo.mexc    = dmexc;
  mcdo.retry   = dretry;
  mcdo.werr    = r.Q.werr;
  mcdo.cache   = dhcache;
  mcdo.ba      = r.Q.ba;
  mcdo.bg      = r.Q.bg & !BIT32(v.bo,1);
  mcdo.par     = 0;


  mcmmo.grant   = mmgrant;
  mcmmo.ready   = mmready;
  mcmmo.mexc    = mmmexc;
  mcmmo.retry   = mmretry;
  mcmmo.werr    = r.Q.werr;
  mcmmo.cache   = mmhcache;

  mcio.scanen  = scanen;
  mcdo.scanen  = scanen;
  mcdo.testen  = ahbi.testen;

  rin  = v;
  r2in = v2;



  mcio.data  = ahbi.hrdata;//ahbreadword(ahbi.hrdata);
  mcdo.data  = ahbi.hrdata;//ahbreadword(ahbi.hrdata);
  mcmmo.data = ahbi.hrdata;//ahbreadword(ahbi.hrdata);
  ahbo.hirq    = 0;
  ahbo.hconfig = hconfig;

  r.CLK = clk;
  r.D = rin;


#if (CFG_CLK2X!=0)
  reg2 : process(clk)
  begin
    if rising_edge(clk) then r2 <= r2in; end if;
  end process;  
#else
  r2.CLK = clk;
  r2.D.reqmsk = r2.Q.reqmsk = 0;
#endif
}
















