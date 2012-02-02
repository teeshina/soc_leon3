#include "lheaders.h"

//#define tech    : integer := 0;
#define multype (CFG_V8/16)//: integer range 0 to 3 := 0;
#define pipe    ((CFG_V8%4)/2)//: integer range 0 to 1 := 0;
//#define mac     CFG_MAC//: integer range 0 to 1 := 0;
#define arch    ((CFG_V8%16)/4)//: integer range 0 to 3 := 0

#define m16x16  0//: integer := 0;
#define m32x8   1//: integer := 1;
#define m32x16  2//: integer := 2;
#define m32x32  3//: integer := 3;

#define MULTIPLIER multype//: integer := multype;
#define MULPIPE (((multype==0) || (multype==3)) && (pipe==1))//: boolean := ((multype = 0) or (multype = 3)) and (pipe = 1);
#define MACEN   ((multype==0) && (CFG_MAC==1))//: boolean := (multype = 0) and (mac = 1);

const uint64 CZero = 0;//: std_logic_vector(47 downto 0) := "000000000000000000000000000000000000000000000000";

//******************************************************************
void mul32::Update( uint32 rst,//     : in  std_ulogic;
                    SClock clk,//     : in  std_ulogic;
                    uint32 holdn,//   : in  std_ulogic;
                    mul32_in_type &muli,//    : in  mul32_in_type;
                    mul32_out_type &mulo)//    : out mul32_out_type

{
  vcc = 1; 

  v = rm.Q;
  w = mm;
  v.start = muli.start; 
  v.ready = 0;
  v.nready = 0;
  mop1 = muli.op1;
  mop2 = muli.op2;
  acc1 = 0; 
  acc2 = 0;
  zero = 0;
  w.mmac = muli.mac;
  w.xmac = mm.mmac;
  w.msigned = muli.Signed;
  w.xsigned = mm.msigned;

  if (MULPIPE) 
  {
    rsigned = mm.xsigned;
    rmac    = mm.xmac;
  }else 
  {
    rsigned = mm.msigned;
    rmac    = mm.mmac;
  }

//-- select input 2 to accumulator
  switch (MULTIPLIER)
  {
    case m16x16:
      acc2 |= (mreg&MSK64(32,0));//(32 downto 0);
    break;
    case m32x8:
      acc2 |= (mreg&MSK64(40,0));//(40 downto 0);
    break;
    case m32x16:
      acc2 |= (mreg&MSK64(48,0));//(48 downto 0);
    break;
    default:;
  }

//-- state machine + inputs to multiplier and accumulator input 1
  switch (rm.Q.state)
  {
    case 0x0:
      switch(MULTIPLIER)
      {
        case m16x16:
          mop1 &= ~(MSK64(16,0));
          mop1 |= (muli.op1&MSK64(15,0));
          mop2 &= ~(MSK64(16,0));
          mop2 |= (muli.op2&MSK64(15,0));
          if (MULPIPE && (rm.Q.ready==1))
            acc1 |= BITS64(rm.Q.acc, 48, 16);
          else
          {
            acc1 &= ~(MSK64(32,0));
            acc1 |=  BITS64(rm.Q.acc, 63, 32);
          }
        break;
        case m32x8:
          mop1 = muli.op1;
          mop2 &= ~MSK64(8,0);
          mop2 |= BITS64(muli.op2, 7, 0);
          acc1 &= ~MSK64(40, 0);
          acc1 |= BITS64(rm.Q.acc, 63, 24);
        break;
        case m32x16:
          mop1 = muli.op1;
          mop2 &= ~MSK64(16,0);
          mop2 |= BITS64(muli.op2, 15, 0);
          acc1 &= ~MSK64(48,0);
          acc1 |= BITS64(rm.Q.acc,63, 16);
        break;
        default:;
      }
      if (rm.Q.start==1) v.state = 0x1;
    break;
    case 0x1:
      switch(MULTIPLIER)
      {
        case m16x16:
          mop1 &= ~MSK64(16,0);
          mop1 |= BITS64(muli.op1, 32, 16);
          mop2 &= ~MSK64(16,0);
          mop2 |= BITS64(muli.op2, 15, 0);
          if (MULPIPE) 
          {
            acc1 &= MSK64(32,0);
            acc1 |= BITS64(rm.Q.acc, 63, 32);
          }
          v.state = 0x2;
        break;
        case m32x8:
          mop1 = muli.op1;
          mop2 &= ~MSK64(8,0);
          mop2 |= BITS64(muli.op2, 15, 8);
          v.state = 0x2;
        break;
        case m32x16:
          mop1 = muli.op1;
          mop2 &= ~MSK64(16,0);
          mop2 |= BITS64(muli.op2, 32, 16);
          v.state = 0x0;
        break;
        default:;
      }
    break;
    case 0x2:
      switch(MULTIPLIER)
      {
        case m16x16:
          mop1 &= ~MSK64(16,0);
          mop1 |= BITS64(muli.op1,15,0);
          mop2 &= ~MSK64(16,0);
          mop2 |= BITS64(muli.op2,32,16);
          if (MULPIPE)
          {
            acc1 = 0;
            acc2 = 0;
          }else
          {
            acc1 &= ~MSK64(32,0);
            acc1 |= BITS64(rm.Q.acc,48,16);
          }
          v.state = 0x3;
        break;
        case m32x8:
          mop1 = muli.op1;
          mop2 &= ~MSK64(8,0);
          mop2 |= BITS64(muli.op2,23,16);
          acc1 &= ~MSK64(40,0);
          acc1 |= BITS64(rm.Q.acc,48,8);
          v.state = 0x3;
        break;
        default:;
      }
    break;
    default:
      switch(MULTIPLIER)
      {
        case m16x16:
          mop1 &= ~MSK64(16,0);
          mop1 |= BITS64(muli.op1,32,16);
          mop2 &= ~MSK64(16,0);
          mop2 |= BITS64(muli.op2,32,16);
          if (MULPIPE)
          {
            acc1 &= ~MSK64(32,0);
            acc1 |= BITS64(rm.Q.acc,48,16);
          }
          else
          {
            acc1 &= ~MSK64(32,0);
            acc1 |= BITS64(rm.Q.acc,48,16);
          }
          v.state = 0x0;
        break;
        case m32x8:
          mop1 = muli.op1;
          mop2 &= ~MSK64(8,0);
          mop2 |= BITS64(muli.op2,32,24);
          acc1 &= ~MSK64(40,0);
          acc1 |= BITS64(rm.Q.acc,56,16);
          v.state = 0x0;
        break;
        default:;
      }
    break;
  }

//-- optional UMAC/SMAC support

  if (MACEN)
  {
    if ((muli.mac & muli.Signed) == 1)
    {
      mop1 &= ~(uint64(0x1)<<16);
      mop1 |= ((muli.op1&(0x1<<15))<<1);
      mop2 |= ((muli.op2&(0x1<<15))<<1);
    }
    if (rmac == 1)
    {
      acc1 &= ~MSK64(32,0);
      acc1 |= BITS64(muli.acc,32,0);//--muli.y(0) & muli.asr18;
      if (rsigned == 1)
      {
        if((mreg>>31)&0x1) acc2 |= MSK64(39,32);
        else               acc2 &= ~MSK64(39,32);
      }else 
        acc2 &= ~MSK64(39,32);
    }
    acc1 &= ~MSK64(39,33);
    acc1 |= (muli.acc&MSK64(39,33));//--muli.y(7 downto 1);
  }

//-- accumulator for iterative multiplication (and MAC)
  switch(MULTIPLIER)
  {
    case m16x16:
      if (MACEN)
      {
        acc &= ~MSK64(39,0);
        acc |= ( ((acc1&MSK64(39,0)) + (acc2&MSK64(39,0))) & MSK64(39,0) );
      }else
      {
        acc &= ~MSK64(32,0);
        acc = ( ((acc1&MSK64(32,0)) + (acc2&MSK64(32,0))) & MSK64(32,0) );
      }
    break;
    case m32x8:
      acc &= ~MSK64(40,0);
      acc |= ( ((acc1&MSK64(40,0)) + (acc2&MSK64(40,0))) & MSK64(40,0) );
    break;
    case m32x16:
      acc &= ~MSK64(48,0);
      acc |= ( ((acc1&MSK64(48,0)) + (acc2&MSK64(48,0))) & MSK64(48,0) );
    break;
    case m32x32:
      acc &= ~MSK64(31,0);
      v.acc |= BITS64(prod[0],63,32);
    break;
    default:;
  }


//-- save intermediate result to accumulator
  switch(rm.Q.state)
  {
    case 0x0:
      switch(MULTIPLIER)
      {
        case m16x16:
          if (MULPIPE && (rm.Q.ready==1))
          {
            v.acc &= ~MSK64(48,16);
            v.acc |= ((BITS64(acc,32,0)) << 16);
            if (rsigned == 1)
            {
              if((acc>>32)&0x1) v.acc |= MSK64(63,49);
              else              v.acc &= ~MSK64(63,49);
            }
          }else
          {
            v.acc &= ~MSK64(63,32);
            v.acc |= BITS64(acc,31,0);
          }
        break;
        case m32x8:
          v.acc &= ~MSK64(63,24);
          v.acc |= ((BITS64(acc,39,0))<<24);
        break;
        case m32x16:
          v.acc &= ~MSK64(63,16);
          v.acc |= ((BITS64(acc,47,0))<<16);
        break;
        default:;
      }
    break;
    case 0x1:
      switch(MULTIPLIER)
      {
        case m16x16:
          if (MULPIPE) v.acc = 0;
          else         v.acc = BITS64(mreg,31,0);
        break;
        case m32x8:
          v.acc = BITS64(mreg,39,0);
          if (muli.Signed == 1)
          {
            if((acc>>40)&0x1) v.acc |= MSK64(48,40);
            else              v.acc &= ~MSK64(48,40);
          }
        break;
        case m32x16:
          v.acc = BITS64(mreg,47,0);
          v.ready = 1;
          if (muli.Signed == 1)
          {
            if((acc>>48)&0x1) v.acc |= MSK64(63,48);
            else              v.acc &= ~MSK64(63,48);
          }
        break;
        default:;
      }
      v.nready = 1;
    break;
    case 0x2:
      switch(MULTIPLIER)
      {
        case m16x16:
          if (MULPIPE)
          {
            v.acc = BITS64(mreg,31, 0);
          }else
          {
            v.acc &= ~MSK64(48,16);
            v.acc |= ((BITS64(acc,32,0))<<16);
          }
        break;
        case m32x8:
          v.acc &= ~MSK64(48,8);
          v.acc |= ((BITS64(acc,40,0))<<8);
          if (muli.Signed == 1)
          {
            if((acc>>40)&0x1) v.acc |= MSK64(56,49);
            else              v.acc &= ~MSK64(56,49);
          }
        break;
        default:;
      }
    break;
    default:
      switch(MULTIPLIER)
      {
        case m16x16:
          v.acc &= ~MSK64(48,16);
          if (MULPIPE)
          {
            v.acc |= ((BITS64(acc,32,0))<<16);
          }else
          {
            v.acc |= ((BITS64(acc,32,0))<<16);
            if (rsigned == 1)
            {
              if((acc>>32)&0x1) v.acc |= MSK64(63,49);
              else              v.acc &= ~MSK64(63,49);
            }
          }
          v.ready = 1;
        break;
        case m32x8:
          v.acc &= ~MSK64(56,16);
          v.acc |= ((BITS64(acc,40,0))<<16); 
          v.ready = 1;
          if (muli.Signed == 1)
          {
            if((acc>>40)&0x1) v.acc |= MSK64(63,57);
            else              v.acc &= ~MSK64(63,57);
          }
        break;
        default:;
      }
    break;
  }
    

//-- drive result and condition codes
  if (muli.flush == 1) 
  {
    v.state = 0x0;
    v.start = 0;
  }
  if (rst == 0)
  {
    v.nready = 0;
    v.ready  = 0;
    v.state  = 0x0;
    v.start  = 0;
  }
  rmin = v;
  ma   = mop1;
  mb   = mop2;
  mmin = w;
  if (MULPIPE)
  {
    mulo.ready  = rm.Q.ready;
    mulo.nready = rm.Q.nready;
  }else
  {
    mulo.ready  = v.ready;
    mulo.nready = v.nready;
  }

  switch(MULTIPLIER)
  {
    case m16x16:
      if (BITS64(rm.Q.acc,31,0)== 0) zero = 1;
      if (MACEN && (rmac == 1))
      {
        mulo.result = BITS64(acc,39,0);
        if (rsigned == 1)
        {
          if((acc>>39)&0x1)  mulo.result |= MSK64(63,40);
        }
      }else
      {
        mulo.result = BITS64(rm.Q.acc,31,0);
        mulo.result |= ((BITS64(v.acc,63,32)) << 32);
      }
      mulo.icc = uint32(((rm.Q.acc>>31)&0x1)<<3) | 0x0;
    break;
    case m32x8:
      if ((BITS64(rm.Q.acc,23,0)==0) && (BITS64(v.acc,31,24)==0))  zero = 1;
      mulo.result = BITS64(rm.Q.acc,23,0);
      mulo.result |= ((BITS64(v.acc,63,24)) <<24);
      mulo.icc = uint32(((v.acc>>31)&0x1)<<3) | 0x0;
    break;
    case m32x16:
      if ((BITS64(rm.Q.acc,15,0)==0) && (BITS64(v.acc,31,16)==0))  zero = 1;
      mulo.result = BITS64(rm.Q.acc,15,0);
      mulo.result |= ((BITS64(v.acc,63,16))<<16);
      mulo.icc    = uint32(((v.acc>>31)&0x1)<<3) | 0x0;
    break;
    case m32x32:
//--      mulo.result <= rm.acc(31 downto 0) & prod(31 downto 0);
      mulo.result = prod[0];
      mulo.icc &= ~0x3;
      if (BITS64(prod[0],31,0)==0) mulo.icc |= (0x1<<2) ;
      else                         mulo.icc &= ~(0x1<<2);
      mulo.icc &= ~(0x1<<3);
      mulo.icc |= (((prod[0]>>31)&0x1)<<3);
    break;
    default:
      mulo.result = 0xcccccccc;
      mulo.icc    = 0xcccccccc;//(others => '-');
  }


#if (MULTIPLIER == m16x16)
    m1616 : techmult generic map (tech, arch, 17, 17, pipe+1, pipe)
      port map (ma(16 downto 0), mb(16 downto 0), clk, holdn, vcc,  prod(33 downto 0));
    reg : process(clk)
    begin
      if rising_edge(clk) then
        if (holdn = '1') then
          mm <= mmin;
          mreg(33 downto 0) <= prod(33 downto 0);
        end if;
      end if;
    end process;
#elif (MULTIPLIER == m32x8)
    m3208 : techmult generic map (tech, arch, 33, 8, 2, 1)
      port map (ma(32 downto 0), mb(8 downto 0), clk, holdn, vcc,  mreg(41 downto 0));
#elif (MULTIPLIER == m32x16)
    m3216 : techmult generic map (tech, arch, 33, 17, 2, 1)
      port map (ma(32 downto 0), mb(16 downto 0), clk, holdn, vcc,  mreg(49 downto 0));
#elif (MULTIPLIER == m32x32)
    //m3232 : techmult generic map (tech, arch, 33, 33, pipe+1, pipe)
    //  port map (ma(32 downto 0), mb(32 downto 0), clk, holdn, vcc,  prod(65 downto 0));
    m3232.Update(ma, mb, clk, holdn, vcc,  prod);
#endif

  rm.CLK = clk;
  if(holdn==1) rm.D = rmin;
  if(!rst)//-- needed to preserve sync resets in synopsys ...
  {
    rm.D.nready = 0;
    rm.D.ready  = 0;
    rm.D.state  = 0x0;
    rm.D.start  = 0;
  }
}



