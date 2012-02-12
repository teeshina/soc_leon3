//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "lheaders.h"

//#define pindex   : integer = 0;
//#define paddr    : integer = 0;
//#define pmask    : integer = 16#fff#;
//#define console  : integer = 0;
//#define IRQ_UART_CFG//pirq     : integer = 0;
//#define parity   : integer = 1;
//#define CFG_APBUART_FLOW//flow     : integer = 1;
//#define CFG_APBUART_FIFOSZ//fifosize : integer range 1 to 32 = 1;
//#define CFG_APBUART_ABITS//abits    : integer = 8;
//#define CFG_APBUART_SBITS//sbits    : integer range 12 to 32 = 12);

const uint32 REVISION = 1;
//constant  rcntzero : std_logic_vector(log2x(CFG_APBUART_FIFOSZ) downto 0) = (others => 0);

//****************************************************************************
apbuart::apbuart(uint32 pindex_, uint32 paddr_, uint32 pmask_)
{
  pindex = pindex_;
  cfg_paddr  = paddr_;
  cfg_pmask  = pmask_;
}

apbuart::~apbuart()
{
}

//****************************************************************************
void apbuart::Update(  uint32 rst,//    : in  std_ulogic;
                       SClock clk,//    : in  std_ulogic;
                       apb_slv_in_type &apbi,//   : in  apb_slv_in_type;
                       apb_slv_out_type &apbo,//   : out apb_slv_out_type;
                       uart_in_type &uarti,//  : in  uart_in_type;
                       uart_out_type &uarto)//  : out uart_out_type);
{
  ((ahb_device_reg*)(&apbo.pconfig.arr[0]))->vendor  = VENDOR_GAISLER;
  ((ahb_device_reg*)(&apbo.pconfig.arr[0]))->device  = GAISLER_APBUART;
  ((ahb_device_reg*)(&apbo.pconfig.arr[0]))->version = 1;
  ((ahb_device_reg*)(&apbo.pconfig.arr[0]))->cfgver  = 0;
  ((ahb_device_reg*)(&apbo.pconfig.arr[0]))->interrupt = IRQ_UART_CFG;
  ((apb_iobar*)(&apbo.pconfig.arr[1]))->area_id  = 0x1;
  ((apb_iobar*)(&apbo.pconfig.arr[1]))->addrmask = cfg_pmask;
  ((apb_iobar*)(&apbo.pconfig.arr[1]))->zero     = 0x0;
  ((apb_iobar*)(&apbo.pconfig.arr[1]))->memaddr  = cfg_paddr;


  v = r.Q;
  irq = 0;
  irq |= (r.Q.irq<<IRQ_UART_CFG);
  v.irq = 0;
  v.txtick = 0;
  v.rxtick = 0;
  v.tick = 0;
  rdata = 0;
  v.rxdb &= ~(1<<1);
  v.rxdb |= BIT32(r.Q.rxdb,0);
  dready = 0;
  thempty = 1;
  thalffull = 1;
  rhalffull = 0;
  v.ctsn = (BIT32(r.Q.ctsn,0)<<1) | uarti.ctsn;

  if (CFG_APBUART_FIFOSZ == 1) 
  {
    dready = BIT32(r.Q.rcnt,0);
    rfull = dready;
    tfull = BIT32(r.Q.tcnt,0);
    thempty = !tfull;
  }else
  {
    tfull = BIT32(r.Q.tcnt,log2x[CFG_APBUART_FIFOSZ]);
    rfull = BIT32(r.Q.rcnt, log2x[CFG_APBUART_FIFOSZ]);
    if( (BIT32(r.Q.rcnt,log2x[CFG_APBUART_FIFOSZ]) | BIT32(r.Q.rcnt,log2x[CFG_APBUART_FIFOSZ]-1)) == 1 )
      rhalffull = 1;
    
    if( ((BIT32(r.Q.tcnt,log2x[CFG_APBUART_FIFOSZ]) | BIT32(r.Q.tcnt,log2x[CFG_APBUART_FIFOSZ]-1))) == 1 )
      thalffull = 0;
    
    if (r.Q.rcnt != 0) dready = 1;
    if (r.Q.tcnt != 0) thempty = 0;
  }

  //-- scaler
  
  scaler = r.Q.scaler - 1;
  if (r.Q.rxen | r.Q.txen)
  {
    v.scaler = scaler;
    v.tick = BIT32(scaler,CFG_APBUART_SBITS-1) & !BIT32(r.Q.scaler,CFG_APBUART_SBITS-1);
    if (v.tick) v.scaler = r.Q.brate;
  }

  //-- optional external uart clock
  v.extclk = uarti.extclk;
  if (r.Q.extclken) v.tick = r.Q.extclk & !uarti.extclk;

  //-- read/write registers
#if 1
  if(iClkCnt>=131)
  bool sr = true;
#endif

  paddr = 0;
  paddr = apbi.paddr & MSK32(CFG_APBUART_ABITS-1,2);

  if (BIT32(apbi.psel,pindex) & apbi.penable & !apbi.pwrite)
  {
    switch(BITS32(paddr,7,2))
    {
      case 0x00://when "000000" =>
        rdata |= BITS32(r.Q.rhold.arr[r.Q.rraddr],7,0);
#if (CFG_APBUART_FIFOSZ == 1)
        v.rcnt &= ~0x1;
#else
	      if (r.Q.rcnt != 0)
        {
	        v.rraddr = r.Q.rraddr + 1;
          v.rcnt   = r.Q.rcnt - 1;
	      }
#endif
      break;
      case 0x01://when "000001" =>
#if (CFG_APBUART_FIFOSZ != 1)
	      rdata |= (r.Q.rcnt<<26);
	      rdata |= (r.Q.tcnt<<20);
	      rdata |= ((rfull<<10) | (tfull<<9) | (rhalffull<<8) | (thalffull<<7));
#endif
        rdata |= ((r.Q.frame<<6) | (r.Q.parerr<<5) | (r.Q.ovf<<4) |
        	  (r.Q.Break<<3) | (thempty<<2) | (r.Q.tsempty<<1) | dready);
      break;
      case 0x2://when "000010" =>
#if (CFG_APBUART_FIFOSZ > 1)
        rdata |= (1<<31);
#endif
        rdata |= (r.Q.tsemptyirqen<<14);
        rdata |= (r.Q.delayirqen<<13);
        rdata |= (r.Q.breakirqen<<12);
        rdata |= (r.Q.debug<<11);
#if (CFG_APBUART_FIFOSZ != 1)
	      rdata |= ((r.Q.rfifoirqen<<10) | (r.Q.tfifoirqen<<9));
#endif
        rdata |= ((r.Q.extclken<<8) | (r.Q.loopb<<7) | (r.Q.flow<<6) | (r.Q.paren<<5) 
          | (r.Q.parsel<<4) | (r.Q.tirqen<<3) | (r.Q.rirqen<<2) | (r.Q.txen<<1) | r.Q.rxen);
      break;
      case 0x3:// "000011" =>
        rdata |= r.Q.brate;
      break;
      case 0x4:// "000100" =>
        //-- Read TX FIFO.
        if ((r.Q.debug == 1) & (r.Q.tcnt != 0))
        {
          rdata |= BITS32(r.Q.thold.arr[r.Q.traddr],7,0);
#if (CFG_APBUART_FIFOSZ == 1)
          v.tcnt &= ~(0x1);
#else
          v.traddr = BITS32(r.Q.traddr+1, log2x[CFG_APBUART_FIFOSZ], 0);
          v.tcnt = BITS32(r.Q.tcnt-1, log2x[CFG_APBUART_FIFOSZ], 0);
#endif
        }
      break;
      default:;
    }
  }
  
  if (BIT32(apbi.psel,pindex) & apbi.penable & apbi.pwrite)
  {
    switch(BITS32(paddr,7,2))
    {
      case 0:break;
      case 1:
	      v.frame      = BIT32(apbi.pwdata,6);
	      v.parerr     = BIT32(apbi.pwdata,5);
	      v.ovf 	     = BIT32(apbi.pwdata,4);
	      v.Break      = BIT32(apbi.pwdata,3);
      break;
      case 0x2://"000010" =>
        v.tsemptyirqen = BIT32(apbi.pwdata,14);
        v.delayirqen = BIT32(apbi.pwdata,13);
        v.breakirqen = BIT32(apbi.pwdata,12);
        v.debug      = BIT32(apbi.pwdata,11);
#if (CFG_APBUART_FIFOSZ != 1)
	      v.rfifoirqen = BIT32(apbi.pwdata,10);
	      v.tfifoirqen = BIT32(apbi.pwdata,9);
#endif
	      v.extclken   = BIT32(apbi.pwdata,8);
	      v.loopb	     = BIT32(apbi.pwdata,7);
	      v.flow 	     = BIT32(apbi.pwdata,6);
	      v.paren      = BIT32(apbi.pwdata,5);
	      v.parsel     = BIT32(apbi.pwdata,4);
	      v.tirqen     = BIT32(apbi.pwdata,3);
	      v.rirqen     = BIT32(apbi.pwdata,2);
	      v.txen 	     = BIT32(apbi.pwdata,1);
	      v.rxen 	     = BIT32(apbi.pwdata,0);
      break;
      case 0x3://"000011" =>
	      v.brate      = BITS32(apbi.pwdata,CFG_APBUART_SBITS-1,0);
	      v.scaler     = BITS32(apbi.pwdata,CFG_APBUART_SBITS-1,0);
      break;
      case 0x4://"000100" =>
        //-- Write RX fifo and generate irq
#if (CFG_APBUART_FLOW != 0)
	      v.rhold.arr[r.Q.rwaddr] = BITS32(apbi.pwdata,7,0);
#if (CFG_APBUART_FIFOSZ == 1)
        v.rcnt |= 0x1;
#else 
        v.rwaddr = BITS32(r.Q.rwaddr+1, log2x[CFG_APBUART_FIFOSZ]-1, 0);
        v.rcnt = BITS32(v.rcnt+1, log2x[CFG_APBUART_FIFOSZ]-1, 0);
#endif
        if (r.Q.debug == 1)
          v.irq = v.irq | r.Q.rirqen;
#endif
      break;
      default:;
    }
  }

  //-- tx clock
  
  txclk = r.Q.txclk + 1;
  if (r.Q.tick == 1)
  {
    v.txclk = txclk;
    v.txtick = BIT32(r.Q.txclk,2) & !BIT32(txclk,2);
  }

  //-- rx clock

  rxclk = r.Q.rxclk + 1;
  if (r.Q.tick == 1)
  {
    v.rxclk = rxclk;
    v.rxtick = BIT32(r.Q.rxclk,2) & !BIT32(rxclk,2);
  }

  if (r.Q.rxtick & r.Q.delayirqen)
  {
    v.irqcnt = v.irqcnt + 1;
  }

  if (BITS32(r.Q.irqcnt,5,4) == 0x3)
  {
    v.irq = v.irq | (r.Q.delayirqen & r.Q.irqpend); //-- make sure no tx irqs are lost !
    v.irqpend = 0;
  }

//-- filter rx data

//--    v.rxf = r.rxf(6 downto 0) & uarti.rxd;
//--    if ((r.rxf(7) & r.rxf(7) & r.rxf(7) & r.rxf(7) & r.rxf(7) & r.rxf(7) &
//--	 r.rxf(7)) = r.rxf(6 downto 0))
//--    then v.rxdb(0) = r.rxf(7); end if;

  v.rxf &= ~MSK32(1,0);
  v.rxf |= ((BIT32(r.Q.rxf,0)<<1) | uarti.rxd);	//-- meta-stability filter
  if (r.Q.tick == 1)
  {
    v.rxf &= ~MSK32(4,2);
    v.rxf |= (BITS32(r.Q.rxf,3,1)<<2);
  }
  v.rxdb &= ~0x1;
  v.rxdb |= ((BIT32(r.Q.rxf,4) & BIT32(r.Q.rxf,3)) | (BIT32(r.Q.rxf,4) & BIT32(r.Q.rxf,2)) | 
		(BIT32(r.Q.rxf,3) & BIT32(r.Q.rxf,2)));
  //-- loop-back mode
  if (r.Q.loopb == 1)
  {
    v.rxdb &= ~0x1;
    v.rxdb |= BIT32(r.Q.tshift,0);
    ctsn = dready & !r.Q.rsempty;
  }else
  {
#if (CFG_APBUART_FLOW == 1) 
    ctsn = BIT32(r.Q.ctsn,1);
#else 
    ctsn = 0; 
#endif
  }
  rxd = BIT32(r.Q.rxdb,0);

  //-- transmitter operation
  
  switch(r.Q.txstate)
  {
    case t_idle://	-- idle state
      if (r.Q.txtick == 1) v.tsempty = 1;
      
      if ((!r.Q.debug & r.Q.txen & !thempty & r.Q.txtick) & (!ctsn | !r.Q.flow))
      {
        v.txstate = t_data;
        v.tpar = r.Q.parsel;
        v.tsempty = 0;
        v.txclk = r.Q.tick;
        v.txtick = 0;
        v.tshift = (0x1<<9) | (r.Q.thold.arr[r.Q.traddr]<<1) | 0;
#if (CFG_APBUART_FIFOSZ == 1)
        v.irq = r.Q.irq | r.Q.tirqen;
        v.tcnt &= ~0x1;
#else
        v.traddr = BITS32(r.Q.traddr+1,log2x[CFG_APBUART_FIFOSZ]-1,0);
        v.tcnt = BITS32(r.Q.tcnt-1,log2x[CFG_APBUART_FIFOSZ]-1,0);
#endif
      }
    break;
      
    case t_data://	-- transmit data frame
      if (r.Q.txtick)
      {
	      v.tpar = r.Q.tpar ^ BIT32(r.Q.tshift,1);
	      v.tshift = (1<<10) | BITS32(r.Q.tshift,10,1);
        if (BITS32(r.Q.tshift,10,1) == 0x3FE)
        {
	        if (r.Q.paren == 1)
          {
            v.tshift &= ~0x1;
	          v.tshift |= r.Q.tpar;
            v.txstate = t_cparity;
	        }else
          {
	          v.tshift |= 1;
            v.txstate = t_stopbit;
	        }
	      }
      }
    break;
    case t_cparity://	-- transmit parity bit
      if (r.Q.txtick == 1)
      {
	      v.tshift = (1<<10) | BITS32(r.Q.tshift,10,1);
        v.txstate = t_stopbit;
      }
    break;
    case t_stopbit://	-- transmit stop bit
      if (r.Q.txtick == 1)
      {
	      v.tshift = (1<<10) | BITS32(r.Q.tshift,10,1);
        v.txstate = t_idle;
      }
    default:;
  }
  
//-- writing of tx data register must be done after tx fsm to get correct
//-- operation of thempty flag

  if (BIT32(apbi.psel,pindex) & apbi.penable & apbi.pwrite)
  {
    switch(BITS32(paddr,4,2))
    {
      case 0x0://"000" =>
#if (CFG_APBUART_FIFOSZ == 1)
	      v.thold.arr[0] = BITS32(apbi.pwdata,7,0);
        v.tcnt |= 0x1;
#else
	      v.thold.arr[r.Q.twaddr] = BITS32(apbi.pwdata,7,0);
	      if (!(tfull == 1))
        {
	        v.twaddr = BITS32(r.Q.twaddr+1,log2x[CFG_APBUART_FIFOSZ]-1,0);
          v.tcnt   = BITS32(v.tcnt+1,log2x[CFG_APBUART_FIFOSZ]-1,0);
	      }
#endif
      break;
      default:;
    }
  }

//-- receiver operation

  switch(r.Q.rxstate)
  {
    case r_idle://	-- wait for start bit
      if ((r.Q.rsempty == 0) && !(rfull == 1))
      {
	      v.rsempty = 1;
	      v.rhold.arr[r.Q.rwaddr] = r.Q.rshift;
#if (CFG_APBUART_FIFOSZ == 1)
        v.rcnt |= 0x1;
#else 
        v.rwaddr = BITS32(r.Q.rwaddr+1,log2x[CFG_APBUART_FIFOSZ]-1,0);
        v.rcnt = BITS32(v.rcnt+1,log2x[CFG_APBUART_FIFOSZ]-1,0);
#endif
      }
      if (r.Q.rxen & BIT32(r.Q.rxdb,1) & !rxd)
      {
	      v.rxstate = r_startbit;
        v.rshift = MSK32(7,0);
        v.rxclk = 0x4;
	      if (v.rsempty == 0) v.ovf = 1;
	      v.rsempty = 0;
        v.rxtick = 0;
      }
    break;
    case r_startbit:	//-- check validity of start bit
      if (r.Q.rxtick == 1)
      {
	      if (rxd == 0)
        {
	        v.rshift = (rxd<<7) | BITS32(r.Q.rshift,7,1);
          v.rxstate = r_data;
	        v.dpar = r.Q.parsel;
	      }else
	        v.rxstate = r_idle;
      }
    break;
    case r_data://	-- receive data frame
      if (r.Q.rxtick == 1)
      {
	      v.dpar = r.Q.dpar ^ rxd;
	      v.rshift = (rxd<<7) | BITS32(r.Q.rshift,7,1);
	      if (BIT32(r.Q.rshift,0) == 0)
        {
	        if (r.Q.paren) v.rxstate = r_cparity;
	        else 
          { 
            v.rxstate = r_stopbit;
            v.dpar = 0;
          }
	      }
      }
    break;
    case r_cparity://	-- receive parity bit
      if (r.Q.rxtick == 1)
      {
	      v.dpar = r.Q.dpar ^ rxd;
        v.rxstate = r_stopbit;
      }
    break;
    case r_stopbit://	-- receive stop bit
      if (r.Q.rxtick == 1)
      {
        if (r.Q.delayirqen == 0)
          v.irq = v.irq | r.Q.rirqen; //-- make sure no tx irqs are lost !
        if (rxd == 1)
        {
          if (r.Q.delayirqen == 1)
          {
            v.irqpend = r.Q.rirqen;
            v.irqcnt = 0;
          }
          v.parerr = r.Q.parerr | r.Q.dpar;
          v.rsempty = r.Q.dpar;
          if (!(rfull == 1) && (r.Q.dpar == 0))
          {
	          v.rsempty = 1;
	          v.rhold.arr[r.Q.rwaddr] = r.Q.rshift;
#if (CFG_APBUART_FIFOSZ == 1)
            v.rcnt |= 0x1;
#else 
            v.rwaddr = BITS32(r.Q.rwaddr+1,log2x[CFG_APBUART_FIFOSZ]-1,0);
            v.rcnt = BITS32(v.rcnt+1,log2x[CFG_APBUART_FIFOSZ]-1,0);
#endif
	        }
        }else
        {
	        if (r.Q.rshift == 0x0)
          {
            v.Break = 1;
            v.irq = v.irq | r.Q.breakirqen;
	        }else 
            v.frame = 1;
	        v.rsempty = 1;
	      }
        v.rxstate = r_idle;
      }
    break;
    default:;
  }
  
  if (r.Q.rxtick == 1)
    v.rtsn = (rfull & !r.Q.rsempty) | r.Q.loopb;


  v.txd = BIT32(r.Q.tshift,0) | r.Q.loopb | r.Q.debug;

#if (CFG_APBUART_FIFOSZ != 1)
  if ((thempty == 0) && (v.tcnt == 0))
  {
    v.irq = v.irq | r.Q.tirqen;
  }
  v.irq = v.irq | (r.Q.tfifoirqen & r.Q.txen & thalffull);
  v.irq = v.irq | (r.Q.rfifoirqen & r.Q.rxen & rhalffull);
  if (r.Q.rfifoirqen & r.Q.rxen & rhalffull)
    v.irqpend = 0;
#endif

  v.irq = v.irq | (r.Q.tsemptyirqen & v.tsempty & !r.Q.tsempty); 

  //-- reset operation

  if (!rst)
  {
    v.frame = 0;
    v.rsempty = 1;
    v.parerr = 0;
    v.ovf = 0;
    v.Break = 0;
    v.tsempty = 1;
    v.txen = 0;
    v.rxen = 0;
    v.txstate = t_idle;
    v.rxstate = r_idle;
    v.tshift |= 0x1;
    v.extclken = 0;
    v.rtsn = 1;
    v.flow = 0;
    v.txclk = 0;
    v.rxclk = 0;
    v.rcnt = 0;
    v.tcnt = 0;
    v.rwaddr = 0;
    v.twaddr = 0;
    v.rraddr = 0;
    v.traddr = 0;
    v.irqcnt = 0;
    v.irqpend = 0;
  }

  //-- update registers

  rin = v;

  //-- drive outputs

  uarto.txd = r.Q.txd;
  uarto.rtsn = r.Q.rtsn;
  uarto.scaler = 0;
  uarto.scaler &=~MSK32(CFG_APBUART_SBITS-1,0);
  uarto.scaler |= r.Q.scaler;
  apbo.prdata = rdata;
  apbo.pirq = irq;
  apbo.pindex = pindex;
  uarto.txen = r.Q.txen;
  uarto.rxen = r.Q.rxen;
  uarto.flow = 0;
  

  //apbo.pconfig = pconfig;// see above

  r.CLK = clk;
  r.D = rin;

}

