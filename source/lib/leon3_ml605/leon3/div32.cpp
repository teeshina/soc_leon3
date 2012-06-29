#include "div32.h"



void div32::Update( uint32 rst,//     : in  std_ulogic;
                    SClock clk,//     : in  std_ulogic;
                    uint32 holdn,//   : in  std_ulogic;
                    div32_in_type &divi,//    : in  div32_in_type;
                    div32_out_type &divo)//    : out div32_out_type
{
  vready = 0;
  vnready = 0;
  v = r.Q;  // this value is strange afftecting the "state" value when "flush" signal occurs
  if (addout == 0) v.zero = 1;
  else             v.zero = 0;

  vaddin1 = BITS64(r.Q.x[0],63, 31);
  vaddin2 = divi.op2; 
  vaddsub = !(BIT64(divi.op2,32) ^ BIT64(r.Q.x[1],0));
  v.zero2 = r.Q.zero;

  switch (r.Q.state)
  {
    case 0:
      v.cnt = 0;
      if (divi.start == 1)
      {
        v.x[1] &= ~0x1;
        v.x[1] |= BIT64(divi.y,32);
        v.state = 1;
      }
    break;
    case 1:
      v.x[0] = (BITS64(divi.y,31,0)<<32) | BITS64(divi.op1,31,0);
      v.x[1] = BIT64(divi.y,32);
      v.neg = BIT64(divi.op2,32) ^ BIT64(divi.y,32);
      if (divi.Signed == 1)
      {
        vaddin1 = (BITS64(divi.y,31,0)<<1) | BIT64(divi.op1,31);
        v.ovf = !(BIT64(addout,32) ^ BIT64(divi.y,32));
      }else
      {
        vaddin1 = divi.y;
        vaddsub = 1;
        v.ovf = !BIT64(addout,32);
      }
      v.state = 2;
    break;
    case 2:
      if (((divi.Signed & r.Q.neg & r.Q.zero) == 1) && (divi.op1 == 0)) v.ovf = 0;
      v.qmsb  = vaddsub;
      v.qzero = 1;
      v.x[1] = BIT64(addout,32);
      v.x[0] = BITS64(addout,31,0)<<32;
      v.x[0] |= ( (BITS64(r.Q.x[0],30,0)<<1) | vaddsub );
      v.state = 3;
      v.zcorr = v.zero;
      v.cnt   = r.Q.cnt + 1;
    break;
    case 3:
      v.qzero = r.Q.qzero & (vaddsub ^ r.Q.qmsb);
      v.zcorr = r.Q.zcorr | v.zero;
      v.x[0]= v.x[1] =0;
      v.x[1] = ((addout>>32)&0x1);
      v.x[0] = BITS64(addout,31,0)<<32;
      v.x[0] |= ( (BITS64(r.Q.x[0],30,0)<<1) | vaddsub );
      if (r.Q.cnt == 0x1F)
      {
        v.state = 4;
        vnready = 1;
      }else
      {
        v.cnt = r.Q.cnt + 1;
      }
      v.qcorr = (v.x[1]&0x1) ^ ((divi.y>>32)&0x1);
    break;
    case 4:
      vaddin1 = (r.Q.x[1]<<63) | BITS64(r.Q.x[0],63,32);
      v.state = 5;
    break;
    default:
      vaddin1 = (uint64((!(r.Q.x[0]>>31))&0x1)<<32) | (BITS64(r.Q.x[0],30,0)<<1) | 0x1;
      vaddin2 = 0x1;
      vaddsub = (!r.Q.neg);//-- or (r.zcorr and not r.qcorr); 
      if ( (r.Q.qcorr | r.Q.zero) & !r.Q.zero2)
      {
        if (r.Q.zero & (!r.Q.qcorr & r.Q.zcorr) )
        {
           vaddsub = r.Q.neg;
           v.qzero = 0;
        }
        v.x[1] = ((addout>>32)&0x1);
        v.x[0] &= ~MSK64(63,32);
        v.x[0] |= (BITS64(addout,31,0)<<32); 
      }
      else
      {
        v.x[1] = ((vaddin1>>32)&0x1);
        v.x[0] &= ~MSK64(63,32);
        v.x[0] |= (BITS64(vaddin1,31,0)<<32);
        v.qzero = 0;
      }
      if (r.Q.ovf == 1)
      {
        v.qzero = 0;
        v.x[0] |= MSK64(63,32);//(others => '1');
        if (divi.Signed == 1)
        {
          if (r.Q.neg == 1) v.x[0] &= ~MSK64(62,32);
          else              v.x[0] &= ~(uint64(0x1)<<63);
        }
      }
      vready  = 1;
      v.state = 0;
    break;
  }

  divo.icc = uint32(((r.Q.x[0]>>63)&0x1)<<3) | (r.Q.qzero<<2) | (r.Q.ovf<<1) | 0;
  if (divi.flush == 1) v.state = 0;
  if (!rst)
  {
    v.state = 0;
    v.cnt   = 0;
  }
  rin = v;
  divo.ready  = vready;
  divo.nready = vnready;
  divo.result = (uint32)(BITS64(r.Q.x[0],63,32));
  addin1 = vaddin1;
  addin2 = vaddin2;
  addsub = vaddsub;


  uint64 b;// : std_logic_vector(32 downto 0);
  //if(addin1&0x100000000) addin1 |= 0xFFFFFFFF00000000;
  //if(addin2&0x100000000) addin2 |= 0xFFFFFFFF00000000;
  
  if (addsub == 1) b = addin2^MSK64(63,0);
  else             b = addin2;
  addout = addin1 + b + addsub;
  addout &= 0x1FFFFFFFF;

  r.CLK = clk;
  if (holdn) r.D = rin;
  if (!rst)
  {
    r.D.state = 0;
    r.D.cnt   = 0;
  }
}


