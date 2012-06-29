//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************
// WARNING!     This module was verified only for eirq=0
//****************************************************************************

#include "id.h"
#include "leon3_ml605\stdlib\stdlib.h"
#include "irqmp.h"

//pindex  : integer := 0;
//paddr   : integer := 0;
//pmask   : integer := 16#fff#;
//#define ncpu CFG_NCPU   : integer := 1;
#define eirq    0//: integer := 0

static const uint32 REVISION = 3;

//****************************************************************************
irqmp::irqmp(uint32 pindex_, uint32 paddr_, uint32 pmask_)
{
  pindex = pindex_;
  paddr = paddr_;
  pmask = pmask_;
}

//****************************************************************************
uint32 irqmp::prioritize(uint32 b)
{
  uint32 a;//std_logic_vector(15 downto 0);
  uint32 irl;// : std_logic_vector(3 downto 0);
  uint32 level;// : integer range 0 to 15;

  irl   = 0;
  level = 0;
  a     = b;
  for (int32 i=15; i>=0; i--)
  {
    level = i;
    if (BIT32(a,i)) break;
  }
  irl = level;
  return(irl);
}

//****************************************************************************
void irqmp::Update( uint32 rst,//    : in  std_ulogic;
                    SClock clk,//    : in  std_ulogic;
                    apb_slv_in_type &apbi,//   : in  apb_slv_in_type;
                    apb_slv_out_type &apbo,//   : out apb_slv_out_type;
                    irq_out_vector &irqi,//   : in  irq_out_vector(0 to ncpu-1);
                    irq_in_vector &irqo)//   : out irq_in_vector(0 to ncpu-1)
{
  ((ahb_device_reg*)(&apbo.pconfig.arr[0]))->vendor  = VENDOR_GAISLER;
  ((ahb_device_reg*)(&apbo.pconfig.arr[0]))->device  = GAISLER_IRQMP;
  ((ahb_device_reg*)(&apbo.pconfig.arr[0]))->version = REVISION;
  ((ahb_device_reg*)(&apbo.pconfig.arr[0]))->cfgver  = 0;
  ((ahb_device_reg*)(&apbo.pconfig.arr[0]))->interrupt = IRQ_IRQ_CONTROL;
  ((apb_iobar*)(&apbo.pconfig.arr[1]))->area_id  = 0x1;
  ((apb_iobar*)(&apbo.pconfig.arr[1]))->addrmask = pmask;
  ((apb_iobar*)(&apbo.pconfig.arr[1]))->zero     = 0x0;
  ((apb_iobar*)(&apbo.pconfig.arr[1]))->memaddr  = paddr;

  v = r.Q;
  v.cpurst = 0;
  cpurun = 0x1;
  tmpvar = 0;
  ipend2 = 0;
  v2 = r2.Q;

  //-- prioritize interrupts

#if (eirq != 0)
    for (int32 i=0; i<=CFG_NCPU-1; i++)
    {
      temp2.arr[i] = r2.Q.ipend & r2.Q.imask.arr[i];
      ipend2 |= (orv(temp2.arr[i])<<i);
    }
#endif

  for (int32 i=0; i<=CFG_NCPU-1; i++)
  {
    temp.arr[i] = ((r.Q.iforce.arr[i] | r.Q.ipend) & r.Q.imask.arr[i]);
#if (eirq != 0) 
    temp.arr[i] &= ~(1<<eirq);
    temp.arr[i] |= ((BIT32(temp.arr[i],eirq) | BIT32(ipend2,i)) << eirq);
#endif
    v.irl.arr[i] = prioritize( ((temp.arr[i] & r.Q.ilevel) & MSK32(15,1)) );
    if (v.irl.arr[i] == 0)
    {
#if (eirq != 0)
      temp.arr[i] &= ~(1<<eirq);
      temp.arr[i] |= ((BIT32(temp.arr[i],eirq) | BIT32(ipend2,i)) << eirq);
#endif
      v.irl.arr[i] = prioritize( ((temp.arr[i] & (~r.Q.ilevel))  & MSK32(15,1)) );
    }
  }

  //-- register read
  prdata = 0;
  switch (BITS32(apbi.paddr,7,6))
  {
    case 0x0:// "00" =>
      switch(BITS32(apbi.paddr,4,2))
      {
        case 0x0: prdata |= r.Q.ilevel; break;
        case 0x1:
	        prdata |= r.Q.ipend;

#if (eirq != 0)
          prdata |= (r2.Q.ipend << 16);
#endif
        break;
        case 0x2: prdata |= r.Q.iforce.arr[0]; break;
        case 0x3: break;
        case 0x4: case 0x5:
          prdata |= ((CFG_NCPU-1)<<28);
          prdata |= (eirq << 16);
          for (int32 i=0; i<=CFG_NCPU-1; i++)
            prdata |= (irqi.arr[i].pwd<<i);
          if (CFG_NCPU > 1)
          {
            prdata |= (1<<27);
            switch(BITS32(apbi.paddr,4,2))
            {
              case 0x5:
                prdata = 0;
                prdata |= r.Q.ibroadcast;
              break;
              default:;
            }
          }
        break;
        default:;
      }
    break;
    case 0x1://"01" =>
      for (int32 i=0; i<=CFG_NCPU-1; i++)
      {
	      if (i == BITS32(apbi.paddr,5,2))
	      {
	        prdata |= r.Q.imask.arr[i];
#if (eirq != 0)
          prdata |= (r2.Q.imask.arr[i]<<16);
#endif
	      }
      }
    break;
    case 0x2://when "10" =>
      for (int32 i=0; i<=CFG_NCPU-1; i++)
      {
	      if (i == BITS32(apbi.paddr,5,2))
	        prdata |= r.Q.iforce.arr[i];
      }
    break;
    case 0x3:// "11" =>
#if (eirq != 0)
      for (int32 i=0; i<=CFG_NCPU-1; i++)
      {
        if (i ==BITS32(apbi.paddr,5,2))
          prdata |= r2.Q.irl.arr[i];
      }
#endif
    break;
    default:;
  }

  //-- register write

  if(BIT32(apbi.psel,pindex) & apbi.penable & apbi.pwrite)
  {
    switch(BITS32(apbi.paddr,7,6))
    {
      case 0x0:
        switch(BITS32(apbi.paddr,4,2))
        {
          case 0x0: v.ilevel = (apbi.pwdata & MSK32(15,1)); break;
          case 0x1: v.ipend  = (apbi.pwdata & MSK32(15,1));
#if (eirq != 0)
            v2.ipend = BITS32(apbi.pwdata,31,16);
#endif
          break;
          case 0x2: v.iforce.arr[0] = (apbi.pwdata & MSK32(15,1)); break;
          case 0x3: v.ipend  = r.Q.ipend & ((~apbi.pwdata)&MSK32(15,1));
#if (eirq != 0)
            v2.ipend = r2.Q.ipend & BITS32(~apbi.pwdata,31,16);
#endif
          break;
          case 0x4:
            for (int32 i=0; i<=CFG_NCPU-1; i++) 
            {
              v.cpurst &= ~(1<<i);
              v.cpurst |= (BIT32(apbi.pwdata,i)<<i);
            }
          break;
          default:
            if (CFG_NCPU > 1)
            {
              switch(BITS32(apbi.paddr,4,2))
              {
                case 0x5:
                  v.ibroadcast = (apbi.pwdata & MSK32(15,1));
                break;
                default:;
              }
            }
        }
      break;
      case 0x1:
        for (int32 i=0; i<=CFG_NCPU-1; i++) 
        {
	        if (i == BITS32(apbi.paddr,5,2))
	        {
	          v.imask.arr[i] = (apbi.pwdata & MSK32(15,1));
#if (eirq != 0)
            v2.imask.arr[i] = BITS32(apbi.pwdata,31,16));
#endif
	        }
        }
      break;
      case 0x2:
        for (int32 i=0; i<=CFG_NCPU-1; i++) 
        {
	        if (i == BITS32(apbi.paddr,5,2))
	        {
	          v.iforce.arr[i] = (r.Q.iforce.arr[i] | (apbi.pwdata & MSK32(15,1)) ) 
	            & (BITS32(~apbi.pwdata,31,17)<<1);
	        }
        }
      break;
      default:;
    }
  }

  //-- register new interrupts

  for (int32 i=1; i<=15; i++)
  {
    if (i > AHB_IRQ_MAX-1) break;

    if (CFG_NCPU == 1)
    {
      v.ipend |= ((BIT32(v.ipend,i) | BIT32(apbi.pirq,i)) << i);
    }else 
    {
      v.ipend |= ((BIT32(v.ipend,i) | (BIT32(apbi.pirq,i) & !BIT32(r.Q.ibroadcast,i))) << i);
      for (int32 j=0; j<=CFG_NCPU-1; j++)
      {
        tmpvar = v.iforce.arr[j];
        tmpvar |= ((BIT32(apbi.pirq,i) & BIT32(r.Q.ibroadcast,i)) << i);
        v.iforce.arr[j] = tmpvar;
      }
    }
  }

#if (eirq != 0)
    for (int32 i=16; i<=31; i++)
    {
      if (i > AHB_IRQ_MAX-1) break;
      v2.ipend &= ~(1<<(i-16));
      v2.ipend |= ((BIT32(v2.ipend,i-16) | BIT32(apbi.pirq,i)) << (i-16));
    }
#endif

  //-- interrupt acknowledge
  for ( int32 i=0; i<=CFG_NCPU-1; i++)
  {
    if (irqi.arr[i].intack == 1)
    {
      tmpirq = decode(irqi.arr[i].irl);
      temp.arr[i] = tmpirq & MSK32(15,1);
      v.iforce.arr[i] = v.iforce.arr[i] & (~temp.arr[i] & MSK32(15,1));
      v.ipend  = v.ipend & ( (~(((~r.Q.iforce.arr[i])&MSK32(15,1)) & temp.arr[i])) & MSK32(15,1) );
#if (eirq != 0)
      if (eirq == irqi.arr[i].irl)
      {
        v2.irl.arr[i] = (orv(temp2.arr[i])<<4) | prioritize(temp2.arr[i]);
        if (BIT32(v2.irl.arr[i],4) == 1)
        {
         v2.ipend &= ~(1 << (BITS32(v2.irl.arr[i],3,0)));
        }
      }
#endif
    }
  }

  //-- reset

  if (!rst)
  {
    for(int32 i=0; i<CFG_NCPU; i++)
    {
      v.imask.arr[i] = 0;
      v.iforce.arr[i] = 0;
      v2.imask.arr[i] = 0;
      v2.irl.arr[i] = 0;
    }
    v.ipend = 0;
    if (CFG_NCPU > 1)
      v.ibroadcast = 0;
    v2.ipend = 0;
  }

  apbo.prdata = prdata;
  for (int32 i=0; i<=CFG_NCPU-1; i++)
  {
    irqo.arr[i].irl = r.Q.irl.arr[i];
    irqo.arr[i].rst = BIT32(r.Q.cpurst,i);
    irqo.arr[i].run = BIT32(cpurun,i);
    irqo.arr[i].rstvec = 0;  //-- Alternate reset vector
    irqo.arr[i].iact = 0;
    irqo.arr[i].index = i;
    irqo.arr[i].hrdrst = 0;
  }

  rin  = v;
  r2in = v2;
    

  apbo.pirq = 0;
  //apbo.pconfig = pconfig;
  apbo.pindex = pindex;

  r.CLK = clk;
  r.D = rin;

  
#if (eirq != 0)
  r2.CLK = clk;
  r2.D = r2in;
#else
  r2.D.ipend = 0;
  for (int32 i=0; i<=(CFG_NCPU-1); i++)
  {
    r2.D.imask.arr[i] = 0;
    r2.D.irl.arr[i]   = 0;
  }
#endif
}
