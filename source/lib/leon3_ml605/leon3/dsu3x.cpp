#include "lheaders.h"

//#define hindex AHB_SAVE_DSU// : integer := 0;
//#define haddr uiMemaddr// : integer := 16#900#;
//#define hmask uiAddrmask// : integer := 16#f00#;
//#define ncpu CFG_NCPU//   : integer := 1;
//#define tbits   CFG_DSU_TBITS//: integer := 30; -- timer bits (instruction trace time tag)
//#define tech CFG_MEMTECH//   : integer := DEFMEMTECH; 
//#define irq     0//: integer := 0; 
//#define kbytes  CFG_ATBSZ//: integer := 0;
//#define clk2x   CFG_CLK2X//: integer range 0 to 1 := 0;
//#define testen  0//: integer := 0

#define TBUFABITS (log2[CFG_ATBSZ]+6)//: integer := log2(kbytes) + 6;
#define NBITS     (log2x[CFG_NCPU])//: integer := log2x(ncpu);
#define PROC_H    (24+NBITS-1)//: integer := 24+NBITS-1;
#define PROC_L    (24)//: integer := 24;
#define AREA_H    (23)//: integer := 23;
#define AREA_L    (20)//: integer := 20;
#define HBITS     (28)//: integer := 28;

#define DSU3_VERSION 1//: integer := 1;

#define TRACEN (CFG_ATBSZ!=0)

//****************************************************************************
dsu3x::dsu3x(uint32 ind_, uint32 uiMemaddr_, uint32 uiAddrmask_)
{
  hindex = ind_;
  uiMemaddr  = uiMemaddr_;
  uiAddrmask = uiAddrmask_;
  
  for(int32 i=0; i<8; i++) hconfig.arr[i] = 0;
  ((ahb_device_reg*)(&hconfig.arr[0]))->vendor  = VENDOR_GAISLER;
  ((ahb_device_reg*)(&hconfig.arr[0]))->device  = GAISLER_LEON3DSU;
  ((ahb_device_reg*)(&hconfig.arr[0]))->version = DSU3_VERSION;
  ((ahb_membar_type*)(&hconfig.arr[4]))->memaddr  = uiMemaddr;
  ((ahb_membar_type*)(&hconfig.arr[4]))->addrmask = uiAddrmask;
  ((ahb_membar_type*)(&hconfig.arr[4]))->area_id  = CFGAREA_TYPE_AHB_MEM;
  ((ahb_membar_type*)(&hconfig.arr[4]))->prefetch = 0;
  ((ahb_membar_type*)(&hconfig.arr[4]))->cache    = 0;

#if (TRACEN)
    pclMem0 = new tbufmem(CFG_ATBSZ);
#endif
}

dsu3x::~dsu3x()
{
#if (TRACEN)
    free(pclMem0);
#endif
}

//****************************************************************************
void dsu3x::Update(uint32 rst,//    : in  std_ulogic;
                SClock hclk,//   : in  std_ulogic;
                SClock cpuclk,// : in std_ulogic;
                ahb_mst_in_type &ahbmi,//  : in  ahb_mst_in_type;
                ahb_slv_in_type &ahbsi,//  : in  ahb_slv_in_type;
                ahb_slv_out_type &ahbso,//  : out ahb_slv_out_type;
                l3_debug_out_vector &dbgi,//   : in l3_debug_out_vector(0 to NCPU-1);
                l3_debug_in_vector &dbgo,//   : out l3_debug_in_vector(0 to NCPU-1);
                dsu_in_type &dsui,//   : in dsu_in_type;
                dsu_out_type &dsuo,//   : out dsu_out_type;
                uint32 hclken// : in std_ulogic
                )
{
  v = r.Q;
  iuacc = 0; //--v.slv.hready := 0;
  dbgmode = 0; 
  tstop = 1;
  v.dsubre = (BITS32(r.Q.dsubre,1,0)<<1) | dsui.Break;
  v.dsuen = (BITS32(r.Q.dsuen,1,0)<<1) | dsui.enable;
  hrdata = r.Q.slv.hrdata; 
  
  tv = tr.Q;
  vabufi.enable = 0;
  tv.bphit = 0;
  tv.tbwr = 0;
#if (CFG_CLK2X != 0)
  tv.bphit2 = tr.Q.bphit;
#else
  tv.bphit2 = 0;
#endif
  vabufi.data[0] = vabufi.data[1] = 0;
  vabufi.addr  = 0; 
  vabufi.write = 0;
  aindex       = 0;
  hirq         = 0;
  for (int32 i=0; i<CFG_NCPU; i++)  v.reset[i]      = 0;
#if (TRACEN)
    aindex = tr.Q.aindex + 1;
#if (CFG_CLK2X != 0) 
  vh.irq = tr.Q.bphit | tr.Q.bphit2;
  hirq &= ~MSK32(IRQ_DSU,IRQ_DSU);
  hirq |= (tr.Q.bphit<<IRQ_DSU);
#else
  hirq &= ~MSK32(IRQ_DSU,IRQ_DSU);
  hirq |= (tr.Q.bphit<<IRQ_DSU);
#endif
#endif
  if (hclken == 1)
  {
    v.slv.hready = 0;
    v.act        = 0;
  }
    
//-- check for AHB watchpoints
  bphit1 = 0;
  bphit2 = 0;
#if (TRACEN)
  if ((ahbsi2.hready & tr.Q.ahbactive)==1)
  {
    if ( (((tr.Q.tbreg1.addr ^ (tr.Q.haddr&MSK32(31,2))) && tr.Q.tbreg1.mask) == 0x0)
       && ((tr.Q.tbreg1.read & !tr.Q.hwrite) | (tr.Q.tbreg1.write & tr.Q.hwrite)) )
    {
      bphit1 = 1;
    }
    if ( (((tr.Q.tbreg2.addr ^ (tr.Q.haddr&MSK32(31,2))) && tr.Q.tbreg2.mask) == 0x0)
      && ((tr.Q.tbreg2.read & !tr.Q.hwrite) | (tr.Q.tbreg2.write & tr.Q.hwrite)) ) 
    {
      bphit2 = 1;
    }
    if (bphit1 | bphit2)
    {
      if ( (tr.Q.enable & !r.Q.act) && (tr.Q.dcnten == 0) && (tr.Q.delaycnt != 0x0) )
      {
        tv.dcnten = 1; 
      }else
      {
        tv.enable = 0;
        tv.bphit = tr.Q.Break;
      }
    }
  }
#endif

//-- generate AHB buffer inputs

  vabufi.write = 0x0;
#if (TRACEN)
  if ((tr.Q.enable == 1) && (r.Q.act == 0))
  {
    vabufi.addr |= BITS32(tr.Q.aindex,TBUFABITS-1,0);
    vabufi.data[1] |= (uint64(bphit1 | bphit2) << (127-64));
    vabufi.data[1] |= (uint64(BITS32(r.Q.timer,CFG_DSU_TBITS-1,0)) << (96-64)); 
    vabufi.data[1] |= (uint64(BITS32(ahbmi.hirq,15,1)) << (80-64));
    vabufi.data[1] |= (uint64(tr.Q.hwrite) << (79-64));
    vabufi.data[1] |= (uint64(tr.Q.htrans)<<(77-64));
    vabufi.data[1] |= (uint64(tr.Q.hsize)<<(74-64));
    vabufi.data[1] |= (uint64(tr.Q.hburst)<<(71-64));
    vabufi.data[1] |= (uint64(tr.Q.hmaster)<<(67-64));
    vabufi.data[1] |= (uint64(tr.Q.hmastlock)<<(66-64));
    vabufi.data[1] |= (uint64(ahbmi.hresp)<<(64-64));
    if (tr.Q.hwrite) vabufi.data[0] = (uint64(ahbsi2.hwdata)<<32);
    else           vabufi.data[0] = (uint64(ahbmi.hrdata)<<32);
    vabufi.data[0] |= uint64(tr.Q.haddr);
  }else
  {
    vabufi.addr |= BITS32(tr.Q.haddr,TBUFABITS+3,4);
    vabufi.data[0] = (uint64(ahbsi2.hwdata)<<32) | uint64(ahbsi2.hwdata);
    vabufi.data[1] = (uint64(ahbsi2.hwdata)<<32) | uint64(ahbsi2.hwdata);
  }

//-- write trace buffer

  if ((tr.Q.enable & !r.Q.act) == 1)
  {
    if (tr.Q.ahbactive & ahbsi2.hready)
    {
      tv.aindex     = aindex;
      tv.tbwr       = 1;
      vabufi.enable = 1;
      vabufi.write  = 0xF; 
    }
  }

//-- trace buffer delay counter handling

  if (tr.Q.dcnten == 1)
  {
    if (tr.Q.delaycnt == 0x0)
    {
      tv.enable = 0;
      tv.dcnten = 0;
      tv.bphit  = tr.Q.Break;
    }
    if (tr.Q.tbwr == 1) tv.delaycnt = tr.Q.delaycnt - 1;
  }

//-- save AHB transfer parameters

  if (ahbsi2.hready )
  {
    tv.haddr  = ahbsi2.haddr;
    tv.hwrite = ahbsi2.hwrite;
    tv.htrans = ahbsi2.htrans;
    tv.hsize  = ahbsi2.hsize;
    tv.hburst = ahbsi2.hburst;
    tv.hmaster = ahbsi2.hmaster;
    tv.hmastlock = ahbsi2.hmastlock;
  }
  if (tr.Q.hsel == 1) tv.hwdata = ahbsi2.hwdata;
  if (ahbsi2.hready)
  {
    tv.hsel = BIT32(ahbsi2.hsel,hindex);
    tv.ahbactive = BIT32(ahbsi2.htrans,1);
  }
#endif

  if (r.Q.slv.hsel)
  {
#if (CFG_CLK2X == 0)
    v.cnt = r.Q.cnt - 1;
#else
    if ((r.Q.cnt != 0x7) || (hclken == 1)) v.cnt = r.Q.cnt - 1;
#endif
  }
  
  if (r.Q.slv.hready & hclken)
  {
    v.slv.hsel = 0; //--v.slv.act := 0;
  }
  
  for (int32 i=0; i<=CFG_NCPU-1; i++)
  {
    if (dbgi.arr[i].dsumode == 1)
    {
      if (BIT32(r.Q.dmsk,i) == 0)
      {
        dbgmode = 1;
        if (hclken == 1) v.act = 1;
      }
      v.bn |= (1<<i);
    }else
      tstop = 0;
  }

  if (tstop == 0) v.timer = r.Q.timer + 1;
#if (CFG_CLK2X != 0)
  if (hclken == 1) v.tstop = tstop;
  tstop = r.Q.tstop;
#endif

  cpwd = 0;
  for (int32 i=0;i<=CFG_NCPU-1;i++)
  {
    v.bn |= ((BIT32(v.bn,i) | (dbgmode & BIT32(r.Q.bmsk,i)) | (BIT32(r.Q.dsubre,1) & !BIT32(r.Q.dsubre,2))) << i);
#if (TRACEN)
    v.bn |= ((BIT32(v.bn,i) | (tr.Q.bphit & !BIT32(r.Q.ss,i) & !r.Q.act)) << i);
#endif
    v.pwd &= ~MSK32(i,i);
    v.pwd |= ((dbgi.arr[i].idle & !dbgi.arr[i].ipend & !BIT32(v.bn,i)) << i);
  }
  cpwd &= MSK32(CFG_NCPU-1,0);
  cpwd |= r.Q.pwd;  

  if (ahbsi2.hready & BIT32(ahbsi2.hsel,hindex))
  {
    if (BIT32(ahbsi2.htrans,1) == 1)
    {
      v.slv.hsel = 1;      
      v.slv.haddr = BITS32(ahbsi2.haddr,PROC_H,0);
      v.slv.hwrite = ahbsi2.hwrite;
      v.cnt = 0xF;
    }
  }


    
  for (int32 i=0; i<=CFG_NCPU-1; i++)
  {
    v.en[i] = ((BIT32(r.Q.dsuen,2) & dbgi.arr[i].dsu)<<i);
  }

  rawindex = BITS32(r.Q.slv.haddr,PROC_H,PROC_L);
#if (CFG_NCPU == 1) 
  index = 0;
#else
  if (rawindex > CFG_NCPU) index = CFG_NCPU-1;
  else                     index = rawindex;
#endif


  hasel1 = r.Q.slv.haddr & MSK32(AREA_H-1,AREA_L);
  hasel2 = r.Q.slv.haddr & MSK32(6,2);
  if (r.Q.slv.hsel == 1)
  {
    switch(hasel1>>AREA_L)
    {
      case 0x0:  //-- DSU registers
        if (BITS32(r.Q.cnt,2,0) == 0x6)
        {
          if (hclken == 1) v.slv.hready  = 1;
          else             v.slv.hready2 = 1;
        }
        hrdata = 0x0;
        switch(hasel2>>2)
        {
          case 0://when "00000" =>
            if (r.Q.slv.hwrite == 1)
            {
              if (hclken == 1)
              {
                v.te[index] = BIT32(ahbsi2.hwdata,0);
                v.be[index] = BIT32(ahbsi2.hwdata,1);
                v.bw[index] = BIT32(ahbsi2.hwdata,2);
                v.bs[index] = BIT32(ahbsi2.hwdata,3);
                v.bx[index] = BIT32(ahbsi2.hwdata,4);                
                v.bz[index] = BIT32(ahbsi2.hwdata,5);                
                v.reset[index] = BIT32(ahbsi2.hwdata,9);                
                v.halt[index] = BIT32(ahbsi2.hwdata,10);
	            }else
	            {
	              for(int32 i=0; i<CFG_NCPU; i++) v.reset[i] = r.Q.reset[i];
	            }
            }
            hrdata &= ~MSK32(11,0);
            hrdata |= r.Q.te[index];
            hrdata |= (r.Q.be[index]<<1);
            hrdata |= (r.Q.bw[index]<<2);
            hrdata |= (r.Q.bs[index]<<3);
            hrdata |= (r.Q.bx[index]<<4);
            hrdata |= (r.Q.bz[index]<<5);
            hrdata |= (dbgi.arr[index].dsumode<<6);
            hrdata |= (BIT32(r.Q.dsuen,2)<<7);
            hrdata |= (BIT32(r.Q.dsubre,2)<<8);
            hrdata |= ((!dbgi.arr[index].error)<<9);
            hrdata |= (dbgi.arr[index].halt<<10);
            hrdata |= (dbgi.arr[index].pwd<<11);
          break;
          case 0x02://when "00010" =>  -- timer
            if (r.Q.slv.hwrite)
            {
              if (hclken == 1) v.timer = BITS32(ahbsi2.hwdata,CFG_DSU_TBITS-1,0);
              else             v.timer = r.Q.timer;
            }
            hrdata &= ~MSK32(CFG_DSU_TBITS-1,0);
            hrdata |= r.Q.timer;
          break;
          case 0x08://when "01000" =>
            if (r.Q.slv.hwrite == 1)
            {
              if (hclken == 1)
              {
                v.bn = BITS32(ahbsi2.hwdata,CFG_NCPU-1,0);
                v.ss = BITS32(ahbsi2.hwdata,16+CFG_NCPU-1,16);
	            }else
	            {
	              v.bn = r.Q.bn;
	              v.ss = r.Q.ss;
	            }
            }
            hrdata &= ~MSK32(CFG_NCPU-1,0);
            hrdata |= r.Q.bn;
            hrdata &= ~MSK32(16+CFG_NCPU-1,16);
            hrdata |= (r.Q.ss<<16); 
          break;
          case 0x09://when "01001" =>
            if (r.Q.slv.hwrite & hclken)
            {
              v.bmsk &= ~MSK32(CFG_NCPU-1,0);
              v.bmsk |= BITS32(ahbsi2.hwdata,CFG_NCPU-1,0);
              v.dmsk &= ~MSK32(CFG_NCPU-1,0);
              v.dmsk |= BITS32(ahbsi2.hwdata,CFG_NCPU-1+16,16);
            }
            hrdata &= ~MSK32(CFG_NCPU-1,0);
            hrdata |= r.Q.bmsk;
            hrdata &= ~MSK32(CFG_NCPU-1+16,16);
            hrdata |= (r.Q.dmsk<<16);
          break;
          case 0x10://when "10000" =>
#if (TRACEN)
            hrdata &= ~MSK32((TBUFABITS + 15),16);
            hrdata |= (BITS32(tr.Q.delaycnt,TBUFABITS-1,0)<<16);
            hrdata &= ~MSK32(2,0);
            hrdata |= ((tr.Q.Break<<2) | (tr.Q.dcnten<<1) | tr.Q.enable);
            if (r.Q.slv.hwrite == 1)
            {
              if (hclken == 1)
              {
                tv.delaycnt = BITS32(ahbsi2.hwdata,(TBUFABITS+ 15),16);
                tv.Break  = BIT32(ahbsi2.hwdata,2);                  
                tv.dcnten = BIT32(ahbsi2.hwdata,1);
                tv.enable = BIT32(ahbsi2.hwdata,0);
              }else 
              {
                tv.delaycnt = tr.Q.delaycnt;
                tv.Break    = tr.Q.Break;
                tv.dcnten   = tr.Q.dcnten;
                tv.enable   = tr.Q.enable;
              }
            }
#endif
          break;
          case 0x11://when "10001" =>
#if (TRACEN)
            hrdata &= ~MSK32((TBUFABITS - 1 + 4),4);
            hrdata |= (BITS32(tr.Q.aindex,TBUFABITS- 1,0)<<4);
            if (r.Q.slv.hwrite)
            {
              if (hclken == 1) tv.aindex = BITS32(ahbsi2.hwdata,(TBUFABITS - 1 + 4),4);
	            else             tv.aindex = tr.Q.aindex;
	          }
#endif
          break;
          case 0x14://when "10100" =>
#if (TRACEN)
            hrdata &= ~MSK32(31,2);
            hrdata |= tr.Q.tbreg1.addr; 
            if (r.Q.slv.hwrite & hclken)
              tv.tbreg1.addr = (ahbsi2.hwdata&MSK32(31,2)); 
#endif
          break;
          case 0x15://when "10101" =>
#if (TRACEN)
            hrdata = tr.Q.tbreg1.mask | (tr.Q.tbreg1.read<<1) | (tr.Q.tbreg1.write<<0); 
            if (r.Q.slv.hwrite & hclken)
            {
              tv.tbreg1.mask = ahbsi2.hwdata&MSK32(31,2); 
              tv.tbreg1.read = BIT32(ahbsi2.hwdata,1); 
              tv.tbreg1.write = BIT32(ahbsi2.hwdata,0); 
            }
#endif
          break;
          case 0x16://when "10110" =>
#if (TRACEN)
            hrdata &= ~MSK32(31,2);
            hrdata |= tr.Q.tbreg2.addr; 
            if (r.Q.slv.hwrite & hclken)
              tv.tbreg2.addr = ahbsi2.hwdata&MSK32(31,2); 
#endif
          break;
          case 0x17://when "10111" =>
#if (TRACEN)
            hrdata = tr.Q.tbreg2.mask | (tr.Q.tbreg2.read<<1) | tr.Q.tbreg2.write; 
            if (r.Q.slv.hwrite & hclken)
            {
              tv.tbreg2.mask = ahbsi2.hwdata&MSK32(31,2); 
              tv.tbreg2.read = BIT32(ahbsi2.hwdata,1); 
              tv.tbreg2.write = BIT32(ahbsi2.hwdata,0); 
            }
#endif
          break;
          default:;
        }
      break;
      
      case 0x2://when "010"  =>  -- AHB tbuf
#if (TRACEN)
        if (BITS32(r.Q.cnt,2, 0) == 0x5)
        {
          if (hclken) v.slv.hready  = 1;
          else        v.slv.hready2 = 1;
        }
        vabufi.enable = !(tr.Q.enable & !r.Q.act);
        switch(BITS32(tr.Q.haddr,3,2))
        {
          case 0:
            hrdata = (uint32)BITS64(tbo.data[1],127-64, 96-64);
            if (r.Q.slv.hwrite & hclken)
            {
              vabufi.write &= ~MSK32(3,3);
              vabufi.write |= ((vabufi.enable & v.slv.hready)<<3);
            }
          break;
          case 1:
            hrdata = (uint32)BITS64(tbo.data[1],95-64, 64-64);
            if (r.Q.slv.hwrite & hclken)
            {
              vabufi.write &= ~MSK32(2,2);
              vabufi.write |= ((vabufi.enable & v.slv.hready)<<2);
            }
          break;
          case 2:
            hrdata = (uint32)BITS64(tbo.data[0],63,32);
            if (r.Q.slv.hwrite & hclken)
            {
              vabufi.write &= ~MSK32(1,1);
              vabufi.write |= ((vabufi.enable & v.slv.hready)<<1);
            }
          break;
          default:
            hrdata = (uint32)BITS64(tbo.data[0],31,0);
            if (r.Q.slv.hwrite & hclken)
            {
              vabufi.write &= ~0x1;
              vabufi.write |= (vabufi.enable & v.slv.hready);
            }
        }
#else
          if (hclken == 1) v.slv.hready = 1;
          else             v.slv.hready2 = 1;
#endif
      break;
      case 0x3: case 0x1://when "011" | "001"  =>  -- IU reg file, IU tbuf
        iuacc = 1;
        hrdata = dbgi.arr[index].data;
        if (BITS32(r.Q.cnt,2,0) == 0x5)
        {
          if (hclken) v.slv.hready  = 1;
          else        v.slv.hready2 = 1;
        }
      break;
      case 0x4:  //-- IU reg access
        iuacc = 1;
        hrdata = dbgi.arr[index].data;
        if (BITS32(r.Q.cnt,1,0) == 0x3)
        {
          if (hclken) v.slv.hready  = 1;
          else        v.slv.hready2 = 1;
        }
      break;
      case 0x7: //-- DSU ASI
        if (BITS32(r.Q.cnt,2,1) == 0x3) iuacc = 1;
        else                          iuacc = 0;
        if ((dbgi.arr[index].crdy == 1) || (r.Q.cnt == 0x0))
        {
          if (hclken) v.slv.hready  = 1;
          else        v.slv.hready2 = 1;
        }
        hrdata = dbgi.arr[index].data;          
      break;
      default:
        if (hclken) v.slv.hready  = 1;
        else        v.slv.hready2 = 1;
    }
    if (r.Q.slv.hready & hclken & !v.slv.hsel) v.slv.hready = 0;
#if (CFG_CLK2X != 0)
    if (r.Q.slv.hready2 & hclken) v.slv.hready = 1;
#endif
  }

  if (r.Q.slv.hsel == 1)
  {
    if (r.Q.slv.hwrite & hclken) v.slv.hwdata = ahbsi2.hwdata;
#if (CFG_CLK2X == 0)
    v.slv.hrdata = hrdata;
#else
    if((r.Q.slv.hready | r.Q.slv.hready2) == 0)
      v.slv.hrdata = hrdata;
#endif
  }
      
  if (ahbsi2.hready & BIT32(ahbsi2.hsel,hindex) & !BIT32(ahbsi2.htrans,1))
  {
#if (CFG_CLK2X == 0) 
    v.slv.hready = 1;
#else
    if(r.Q.slv.hsel == 0)
      v.slv.hready = 1;
#endif
  }

#if (CFG_CLK2X != 0) 
  if(r.Q.slv.hready == 1) v.slv.hready2 = 0;
#endif
  if (v.slv.hsel == 0) v.slv.hready = 1;
   
  vh.oen = 0;
#if (CFG_CLK2X != 0)
  if (hclken & r.Q.slv.hsel & (r.Q.slv.hready2 | v.slv.hready))
    vh.oen = 1;
  if ((r.Q.slv.hsel == 1) && (r.Q.cnt == 0x7) && (hclken == 0)) iuacc = 0;
#endif

    
  if (!rst)
  {
    v.bn   = BIT32(r.Q.dsubre,2) ? MSK32(CFG_NCPU-1,0): 0;
    v.bmsk = 0;
    v.dmsk = 0;
    v.ss   = 0;
    v.timer = 0;
    v.slv.hsel = 0;
    for (int32 i=0; i<=CFG_NCPU-1; i++)
    {
      v.bw[i] = BIT32(r.Q.dsubre,2);
      v.be[i] = BIT32(r.Q.dsubre,2); 
      v.bx[i] = BIT32(r.Q.dsubre,2);
      v.bz[i] = BIT32(r.Q.dsubre,2); 
      v.bs[i] = 0;
      v.te[i] = 0;
      v.halt[i] = 0;
    }
    tv.ahbactive = 0;
    tv.enable = 0;
    tv.hsel = 0;
    tv.dcnten = 0;
    tv.tbreg1.read = 0;
    tv.tbreg1.write = 0;
    tv.tbreg2.read = 0;
    tv.tbreg2.write = 0;
    v.slv.hready = 1;
    v.act   = 0;
    v.tstop = 0;
  }
  vabufi.enable = vabufi.enable & !ahbsi.scanen;
  vabufi.diag = ahbsi.testen<<3;
  rin = v;
  trin = tv;
  tbi = vabufi;
  
  for (int32 i=0; i<=CFG_NCPU-1; i++)
  {
    dbgo.arr[i].tenable = r.Q.te[i];
    dbgo.arr[i].dsuen = r.Q.en[i];  
    dbgo.arr[i].dbreak = BIT32(r.Q.bn,i); //-- or (dbgmode and r.Q.bmsk(i));
    if (BITS32(r.Q.slv.haddr,PROC_H,PROC_L) == i)
      dbgo.arr[i].denable = iuacc;
    else
      dbgo.arr[i].denable = 0;

    dbgo.arr[i].step = BIT32(r.Q.ss,i);    
    dbgo.arr[i].berror = r.Q.be[i];
    dbgo.arr[i].bsoft = r.Q.bs[i];
    dbgo.arr[i].bwatch = r.Q.bw[i];
    dbgo.arr[i].btrapa = r.Q.bx[i];
    dbgo.arr[i].btrape = r.Q.bz[i];
    dbgo.arr[i].daddr = r.Q.slv.haddr&MSK32(PROC_L-1,2);
    dbgo.arr[i].ddata = r.Q.slv.hwdata;
    dbgo.arr[i].dwrite = r.Q.slv.hwrite;
    dbgo.arr[i].halt = r.Q.halt[i];
    dbgo.arr[i].reset = r.Q.reset[i];
    dbgo.arr[i].timer &= ~MSK32(CFG_DSU_TBITS-1,0);
    dbgo.arr[i].timer = BITS32(r.Q.timer,CFG_DSU_TBITS-1,0); 
    dbgo.arr[i].timer &= ~MSK32(30,CFG_DSU_TBITS);
  }
  
  ahbso.hconfig = hconfig;
  ahbso.hresp = HRESP_OKAY;
  ahbso.hready = r.Q.slv.hready;
#if (CFG_CLK2X == 0)
    ahbso.hrdata = r.Q.slv.hrdata;
#else
    ahbso.hrdata = hrdata2x;
#endif

  ahbso.hsplit = 0;
  ahbso.hcache = 0;
  ahbso.hirq   = hirq;
  ahbso.hindex = hindex;    

  dsuo.active = r.Q.act;
  dsuo.tstop = tstop;
  dsuo.pwd   = cpwd;
  
  rhin = vh;
    

#if (CFG_CLK2X != 0)
  ag0 : clkand generic map (tech => 0, ren => 0) port map (ahbsi.hmastlock, hclken, ahbsi2.hmastlock);
  ag1 : clkand generic map (tech => 0, ren => 0) port map (ahbsi.hwrite, hclken, ahbsi2.hwrite);
  ag2 : clkand generic map (tech => 0, ren => 0) port map (ahbsi.hready, hclken, ahbsi2.hready);
  gen3 : for i in ahbsi.haddr'range generate
    ag3 : clkand generic map (tech => 0, ren => 0) port map (ahbsi.haddr(i), hclken, ahbsi2.haddr(i));
  end generate;
  gen4 : for i in ahbsi.htrans'range generate 
    ag4 : clkand generic map (tech => 0, ren => 0) port map (ahbsi.htrans(i), hclken, ahbsi2.htrans(i));
  end generate;
--    gen5 : for i in ahbsi.hwdata'range generate
  gen5 : for i in 0 to 31 generate
    ag5 : clkand generic map (tech => 0, ren => 0) port map (ahbsi.hwdata(i), hclken, ahbsi2.hwdata(i));
  end generate;
  gen6 : for i in ahbsi.hsize'range generate 
    ag6 : clkand generic map (tech => 0, ren => 0) port map (ahbsi.hsize(i), hclken, ahbsi2.hsize(i));
  end generate;
  gen7 : for i in ahbsi.hburst'range generate
    ag7 : clkand generic map (tech => 0, ren => 0) port map (ahbsi.hburst(i), hclken, ahbsi2.hburst(i));
  end generate;
  gen8 : for i in ahbsi.hmaster'range generate
    ag8 : clkand generic map (tech => 0, ren => 0) port map (ahbsi.hmaster(i), hclken, ahbsi2.hmaster(i));
  end generate;
  gen9 : for i in ahbsi.hsel'range generate
    ag9 : clkand generic map (tech => 0, ren => 0) port map (ahbsi.hsel(i), hclken, ahbsi2.hsel(i));
  end generate;

  gen10 : for i in hrdata2x'range generate
    ag10 : clkand generic map (tech => 0, ren => 0) port map (r.Q.slv.hrdata(i), rh.oen, hrdata2x(i)); 
  end generate;
  
  reg2 : process(hclk)
  begin
    if rising_edge(hclk) then rh <= rhin; end if;
  end process;
#else
  ahbsi2 = ahbsi;
  rh.irq = 0;
  rh.oen = 0;
  hrdata2x = 0;
#endif
    
  r.CLK = cpuclk;
  r.D = rin;

    
#if (TRACEN)
  tr.CLK = cpuclk;
  tr.D = trin;

  pclMem0->Update(cpuclk, tbi, tbo);
#endif
}
