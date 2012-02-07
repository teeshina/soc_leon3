//****************************************************************************
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "lheaders.h"

#define pipe    0
#define abits   (log2[CFG_AHBRAMSZ] + 8 - AHBDW/64)

ahbram::ahbram(uint32 hindex_, uint32 addr_, uint32 hmask_)
{
  hindex = hindex_;
  addr   = addr_;
  hmask  = hmask_;
  ppSyncram = new syncram *[AHBDW/8];
  for (uint32 i=0; i<=(AHBDW/8-1); i++)
    ppSyncram[i] = new syncram( abits, 8) ;
}
ahbram::~ahbram()
{
  for (uint32 i=0; i<=(AHBDW/8-1); i++)
    delete ppSyncram[i];
}


void ahbram::Update( uint32 rst,//     : in  std_ulogic;
                SClock clk,//     : in  std_ulogic;
                ahb_slv_in_type &ahbsi,//   : in  ahb_slv_in_type;
                ahb_slv_out_type &ahbso//   : out ahb_slv_out_type
              )
{
  ((ahb_device_reg*)(&ahbso.hconfig.arr[0]))->vendor  = VENDOR_GAISLER;
  ((ahb_device_reg*)(&ahbso.hconfig.arr[0]))->device  = GAISLER_AHBRAM;
  ((ahb_device_reg*)(&ahbso.hconfig.arr[0]))->version = log2[CFG_AHBRAMSZ]+10;
  ((ahb_device_reg*)(&ahbso.hconfig.arr[0]))->cfgver  = 0;
  ((ahb_device_reg*)(&ahbso.hconfig.arr[0]))->interrupt = 0;
  ((ahb_membar_type*)(&ahbso.hconfig.arr[4]))->memaddr  = addr;
  ((ahb_membar_type*)(&ahbso.hconfig.arr[4]))->addrmask = hmask;
  ((ahb_membar_type*)(&ahbso.hconfig.arr[4]))->area_id  = CFGAREA_TYPE_AHB_MEM;
  ((ahb_membar_type*)(&ahbso.hconfig.arr[4]))->prefetch = 1;
  ((ahb_membar_type*)(&ahbso.hconfig.arr[4]))->cache    = 1;

  v = r.Q;
  v.hready = 1;
  bs = 0;
  if (r.Q.hwrite | !r.Q.hready)
  {
    haddr = BITS32(r.Q.addr,abits-1+log2[AHBDW/8],log2[AHBDW/8]);
  }else
  {
    haddr = BITS32(ahbsi.haddr,abits-1+log2[AHBDW/8], log2[AHBDW/8]);
    bs = 0; 
  }
  raddr = 0;

  if (ahbsi.hready)
  {
    v.hsel = BIT32(ahbsi.hsel,hindex) & BIT32(ahbsi.htrans,1);
    v.hwrite = ahbsi.hwrite & v.hsel;
    v.addr = BITS32(ahbsi.haddr,abits-1+log2[AHBDW/8], 0); 
    v.size = BITS32(ahbsi.hsize,2,0);
    if ((pipe == 1) && (v.hsel == 1) && (ahbsi.hwrite == 0))
      v.hready = 0;
  }

  if (r.Q.hwrite)
  {
    switch(r.Q.size)
    {
      case HSIZE_BYTE:
        bs |= (1<<(AHBDW/8-1-BITS32(r.Q.addr,log2[AHBDW/16],0)));
      break;
      case HSIZE_HWORD:
        for (uint32 i=0; i<=(AHBDW/16-1); i++)
        {
          if (i == BITS32(r.Q.addr, log2[AHBDW/16],1))
            bs |= MSK32(AHBDW/8-1-i*2, AHBDW/8-1-i*2-1);
        }
      break;
      case HSIZE_WORD:
        if (AHBDW == 32) bs = MSK32(AHBDW/8-1, 0);
        else
        {
          for (uint32 i=0; i<=AHBDW/32-1; i++)
          {
            if (i == BITS32(r.Q.addr,log2[AHBDW/8]-1,2))
              bs |= MSK32(AHBDW/8-1-i*4, AHBDW/8-1-i*4-3);
          }
        }
      break;
      case HSIZE_DWORD:
        if (AHBDW == 32){}
        else if (AHBDW == 64) bs = MSK32(AHBDW/8-1, 0);
        else
        {
          for (uint32 i=0; i<=AHBDW/64-1; i++)
          {
            if (i == BIT32(r.Q.addr,3))
              bs |= MSK32(AHBDW/8-1-i*8, AHBDW/8-1-i*8-7);
          }
        }
      break;
      case HSIZE_4WORD:
        if (AHBDW < 128){}
        else if (AHBDW == 128) bs = MSK32(AHBDW/8-1, 0);
        else
        {
          for (uint32 i=0; i<=AHBDW/64-1; i++)
          {
            if (i == BIT32(r.Q.addr,3))
              bs |= MSK32(AHBDW/8-1-i*8, AHBDW/8-1-i*8-7);
          }
        }
      break;
      default:// --HSIZE_8WORD
        if (AHBDW < 256){}
        else bs = MSK32(AHBDW/8-1, 0);
    }
    v.hready = !(v.hsel & !ahbsi.hwrite);
    v.hwrite = v.hwrite & v.hready;
  }

  //-- Duplicate read data on word basis, unless CORE_ACDM is enabled
  if (CORE_ACDM == 0)
  {
    if (AHBDW == 32)
      seldata = ramdata;
    else if (AHBDW == 64)
    {
      if (r.Q.size == HSIZE_DWORD) seldata = ramdata;
      else
      {
        if (BIT32(r.Q.addr,2) == 0)
        {
          seldata &= ~MSK32(AHBDW/2-1,0);
          seldata |= BITS32(ramdata,AHBDW-1,AHBDW/2);
        }else 
        {
          seldata &= ~MSK32(AHBDW/2-1,0);
          seldata |= BITS32(ramdata,AHBDW/2-1,0);
        }
        seldata &= ~MSK32(AHBDW-1,AHBDW/2);
        seldata |= (BITS32(seldata,AHBDW/2-1,0)<< (AHBDW/2));
      }
    }else if(AHBDW == 128)
    {
      if (r.Q.size == HSIZE_4WORD)
        seldata = ramdata;
      else if (r.Q.size == HSIZE_DWORD)
      {
        if (BIT32(r.Q.addr,3) == 0) seldata = BITS32(ramdata,AHBDW-1,AHBDW/2);
        else                        seldata = BITS32(ramdata,AHBDW/2-1,0);
        seldata |= (BITS32(seldata,AHBDW/2-1,0)<<(AHBDW/2));
      }else
      {
        raddr = BITS32(r.Q.addr,3,2);
        switch(raddr)
        {
          case 0x0: seldata = BITS32(ramdata,4*AHBDW/4-1, 3*AHBDW/4);
          case 0x1: seldata = BITS32(ramdata,3*AHBDW/4-1, 2*AHBDW/4);
          case 0x2: seldata = BITS32(ramdata,2*AHBDW/4-1, 1*AHBDW/4);
          default:  seldata = BITS32(ramdata,AHBDW/4-1, 0);
        }
        seldata |= ( (seldata<<(3*AHBDW/4)) |
                      (seldata<<(AHBDW/2)) |
                      (seldata<<(AHBDW/4)) );
      }
    }else
    {
      // WARNING! Bus AHBDW may be only 32.
      seldata = ramdata;//ahbselectdata(ramdata, BITS32(r.Q.addr,4,2), r.Q.size);
    }
  }else
  {
    seldata = ramdata;
  }

  if (pipe == 0)
  {
    v.prdata = 0;
    hrdata = seldata;
  }else
  {
    v.prdata = seldata;
    hrdata = r.Q.prdata;
  }


  if (!rst)
  {
    v.hwrite = 0;
    v.hready = 1;
  }
  write = bs;
  ramsel = v.hsel | r.Q.hwrite; 
  ramaddr = haddr;
  c = v; 

  ahbso.hrdata = hrdata;
  ahbso.hready = r.Q.hready;
    


  ahbso.hresp   = HRESP_OKAY; 
  ahbso.hsplit  = 0; 
  ahbso.hirq    = 0;
  ahbso.hcache  = 1;
  ahbso.hindex  = hindex;

  //-- Select correct write data
  hwdata = ahbsi.hwdata;
//  hwdata <= ahbreaddata(ahbsi.hwdata, r.addr(4 downto 2),
//                        conv_std_logic_vector(log2(AHBDW/8), 3));
  
  uint32 tmp_ramdata[AHBDW/8];
  ramdata = 0;
  for (uint32 i=0; i<=(AHBDW/8-1); i++)
  {
    ppSyncram[i]->Update(	clk, ramaddr, BITS32(hwdata,i*8+7,i*8),
	                      tmp_ramdata[i], ramsel, BIT32(write,i)); 
    ramdata |= (tmp_ramdata[i]<<(i*8));
  }

  r.CLK = clk;
  r.D = c;
  
}
