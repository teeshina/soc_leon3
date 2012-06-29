//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************
// WARNING:     vtimer.arr[i].value should be reset during (!rst), otherwise
//              signal "z" always =0, cause value="XXXXXXXX" not equals to zero.
//              Such behavior doesn't according to real situation.
//****************************************************************************

#include "id.h"
#include "gptimer.h"

//pirq     IRQ_TIMER//: integer = 0;
//#define sepirq   CFG_GPT_SEPIRQ//: integer = 0;	-- use separate interrupts for each timer
#define sbits    CFG_GPT_SW//: integer = 16;			-- scaler bits
#define ntimers  CFG_GPT_NTIM//: integer range 1 to 7 = 1; 	-- number of timers
#define nbits    CFG_GPT_TW//: integer = 32;			-- timer bits
//#define gen_wdog CFG_GPT_WDOG//: integer = 0;
//#define ewdogen  CFG_GPT_WDOGEN//: integer = 0

static const uint32 REVISION = 0;//: integer = 0;

//****************************************************************************
gptimer::gptimer(uint32 pindex_, uint32 paddr_, uint32 pmask_)
{
  pindex = pindex_;
  paddr = paddr_;
  pmask = pmask_;
}

//****************************************************************************
void gptimer::Update( uint32 rst,//    : in  std_ulogic;
                 SClock clk,//    : in  std_ulogic;
                 apb_slv_in_type &apbi,//   : in  apb_slv_in_type;
                 apb_slv_out_type &apbo,//   : out apb_slv_out_type;
                 gptimer_in_type &gpti,//   : in  gptimer_in_type;
                 gptimer_out_type &gpto)//   : out gptimer_out_type
{
  ((ahb_device_reg*)(&apbo.pconfig.arr[0]))->vendor  = VENDOR_GAISLER;
  ((ahb_device_reg*)(&apbo.pconfig.arr[0]))->device  = GAISLER_GPTIMER;
  ((ahb_device_reg*)(&apbo.pconfig.arr[0]))->version = REVISION;
  ((ahb_device_reg*)(&apbo.pconfig.arr[0]))->cfgver  = 0;
  ((ahb_device_reg*)(&apbo.pconfig.arr[0]))->interrupt = IRQ_TIMER;
  ((apb_iobar*)(&apbo.pconfig.arr[1]))->area_id  = 0x1;
  ((apb_iobar*)(&apbo.pconfig.arr[1]))->addrmask = pmask;
  ((apb_iobar*)(&apbo.pconfig.arr[1]))->zero     = 0x0;
  ((apb_iobar*)(&apbo.pconfig.arr[1]))->memaddr  = paddr;
  
  v = r.Q;
  v.tick = 0;
  tick = 0;
  memset(&vtimers.arr[0], 0, sizeof(timer_reg));
  for(int32 i=1; i<=ntimers; i++)
  {
    vtimers.arr[i] = r.Q.timers.arr[i];
    v.timers.arr[i].irq = 0;
    v.timers.arr[i].load = 0;
    tick |= (r.Q.timers.arr[i].irq << i);
  }
  xirq = 0;
  v.wdogn = !r.Q.timers.arr[ntimers].irqpen;
  v.wdog = r.Q.timers.arr[ntimers].irqpen;

  //-- scaler operation

  scaler = (r.Q.scaler & MSK32(sbits-1,0)) - 1;	//-- decrement scaler

  if (!gpti.dhalt | r.Q.dishlt) 	//-- halt timers in debug mode
  {
    if (BIT32(scaler,sbits) == 1)
    {
      v.scaler = r.Q.reload;
      v.tick   = 1;               //-- reload scaler
    }else 
      v.scaler = BITS32(scaler,sbits-1,0);
  }

  //-- timer operation

  if ((r.Q.tick == 1) || (r.Q.tsel != 0))
  {
    if (r.Q.tsel == ntimers) v.tsel = 0;
    else                     
    {
      v.tsel = r.Q.tsel + 1;
      if(v.tsel>ntimers) v.tsel = 0;
    }
  }

  res = BITS32(vtimers.arr[r.Q.tsel].value - 1, nbits-1, 0);   //-- decrement selected timer
  if( (BIT32(res,nbits-1) == 1) && ((BIT32(vtimers.arr[r.Q.tsel].value,nbits-1) == 0)) )
    z = 1;
  else 
    z = 0; //-- undeflow detect

  //-- update corresponding register and generate irq

  nirq = 0;
  for (int32 i=1; i<=ntimers-1; i++) nirq |= (r.Q.timers.arr[i].irq << i);
  nirq |= r.Q.timers.arr[ntimers].irq;

  for (int32 i=1; i<=ntimers; i++)
  {
    if (i == r.Q.tsel)
    {
      if( (r.Q.timers.arr[i].enable == 1) && 
          (((r.Q.timers.arr[i].chain & BIT32(nirq,i-1)) |!r.Q.timers.arr[i].chain) == 1) )
      {
        v.timers.arr[i].irq = z & !r.Q.timers.arr[i].load;
        if (v.timers.arr[i].irq & r.Q.timers.arr[i].irqen)
        {
          v.timers.arr[i].irqpen = 1;
        }
        v.timers.arr[i].value = res;
        if (z & !r.Q.timers.arr[i].load)
        {
          v.timers.arr[i].enable = r.Q.timers.arr[i].restart;
          if (r.Q.timers.arr[i].restart)
            v.timers.arr[i].value = r.Q.timers.arr[i].reload;
        }
      }
    }
    if (r.Q.timers.arr[i].load)
      v.timers.arr[i].value = r.Q.timers.arr[i].reload;
  }

#if (CFG_GPT_SEPIRQ != 0)
  for (int32 i=1; i<=ntimers; i++)
  {
    xirq &= ~(1<<(i-1+IRQ_TIMER));
    xirq |= ((r.Q.timers.arr[i].irq & r.Q.timers.arr[i].irqen) << (i-1+IRQ_TIMER));
  }
#else 
  for (int32 i=1; i<=ntimers; i++)
  {
    xirq |= ((BIT32(xirq,IRQ_TIMER) | (r.Q.timers.arr[i].irq & r.Q.timers.arr[i].irqen)) << IRQ_TIMER);
  }
#endif

  //-- read registers

  readdata = 0;
  switch(BITS32(apbi.paddr,6,2))
  {
    case 0x00: readdata  |= r.Q.scaler; break;
    case 0x01: readdata  |= r.Q.reload; break;
    case 0x02:
	    readdata |= ntimers;
	    readdata |= (IRQ_TIMER<<3) ;
#if (CFG_GPT_SEPIRQ != 0) 
      readdata |= (1<<8);
#endif
      readdata |= (r.Q.dishlt<<9);
    break;
    default:
      for (int32 i=1; i<=ntimers; i++)
      {
        if (BITS32(apbi.paddr,6,4) == i)
        {
          switch (BITS32(apbi.paddr,3,2))
          {
            case 0x0: readdata |= r.Q.timers.arr[i].value; break;
            case 0x1: readdata |= r.Q.timers.arr[i].reload; break;
  	        case 0x2: readdata = ( (gpti.dhalt<<6) | (r.Q.timers.arr[i].chain<<5)
	            | (r.Q.timers.arr[i].irqpen<<4) | (r.Q.timers.arr[i].irqen<<3) | (r.Q.timers.arr[i].load<<2)
	            | (r.Q.timers.arr[i].restart<<1) | r.Q.timers.arr[i].enable );
	          break;
            default:;
          }
        }
      }
  }

  //-- write registers

  if (BIT32(apbi.psel,pindex) & apbi.penable & apbi.pwrite)
  {
    switch (BITS32(apbi.paddr,6,2))
    {
      case 0x00: v.scaler = BITS32(apbi.pwdata,sbits-1,0); break;
      case 0x01: 
        v.reload = BITS32(apbi.pwdata,sbits-1,0); 
        v.scaler = BITS32(apbi.pwdata,sbits-1,0);
      break;
      case 0x2: v.dishlt = BIT32(apbi.pwdata,9); break;
      default:
        for (int32 i=1; i<=ntimers; i++)
        {
          if (BITS32(apbi.paddr,6,4) == i)
          {
            switch(BITS32(apbi.paddr,3,2))
            {
              case 0: v.timers.arr[i].value   = BITS32(apbi.pwdata,nbits-1, 0); break;
              case 1: v.timers.arr[i].reload  = BITS32(apbi.pwdata,nbits-1, 0); break;
              case 2: 
                v.timers.arr[i].chain   = BIT32(apbi.pwdata,5);
                v.timers.arr[i].irqpen  = v.timers.arr[i].irqpen & !BIT32(apbi.pwdata,4);
		            v.timers.arr[i].irqen   = BIT32(apbi.pwdata,3);
		            v.timers.arr[i].load    = BIT32(apbi.pwdata,2);
		            v.timers.arr[i].restart = BIT32(apbi.pwdata,1);
		            v.timers.arr[i].enable  = BIT32(apbi.pwdata,0);
		          break;
              default:;
            }
          }
        }
    }
  }


  //-- reset operation
  if (!rst)
  {
    for (int32 i=1; i<=ntimers; i++)
    {
      v.timers.arr[i].enable = 0;
      v.timers.arr[i].irqen = 0;
      v.timers.arr[i].irqpen = 0;
    }
    v.scaler = MSK32(sbits-1,0);
    v.reload = MSK32(sbits-1,0);
    v.tsel = 0;
    v.dishlt = 0;
    v.timers.arr[ntimers].irq = 0;
#if (CFG_GPT_WDOG != 0)
#if (CFG_GPT_WDOGEN != 0) 
    v.timers.arr[ntimers].enable = gpti.wdogen;
#else 
    v.timers.arr[ntimers].enable = 1;
#endif       
    v.timers.arr[ntimers].load = 1;
    v.timers.arr[ntimers].reload = BITS32(CFG_GPT_WDOG, nbits-1,0);
    v.timers.arr[ntimers].chain = 0;
    v.timers.arr[ntimers].irqen = 1;
    v.timers.arr[ntimers].irqpen = 0;
    v.timers.arr[ntimers].restart = 0;
#endif
  }
  timer1 = 0;
  timer1 |= r.Q.timers.arr[1].value;

  rin = v;
  apbo.prdata = readdata; 	//-- drive apb read bus
  apbo.pirq = xirq;
  apbo.pindex = pindex;
  gpto.tick = tick | r.Q.tick;
  gpto.timer1 = timer1;	//-- output timer1 value for debugging
  gpto.wdogn = r.Q.wdogn;
  gpto.wdog  = r.Q.wdog;

//  apbo.pconfig <= pconfig;

//-- registers

  r.CLK = clk;
  r.D = rin;
}
  
