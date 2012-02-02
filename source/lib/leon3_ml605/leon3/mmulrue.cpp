//-----------------------------------------------------------------------------
//-- Description: MMU LRU logic
//------------------------------------------------------------------------------
#include "lheaders.h"

//#define position : integer;
//#define entries  : integer := 8 );

#define entries_log (log2[entries])

void mmulrue::Update( uint32 rst,//    : in std_logic;
                      SClock clk,//    : in std_logic;
                      mmulrue_in_type &lruei,//  : in mmulrue_in_type;
                      mmulrue_out_type &lrueo)//  : out mmulrue_out_type );
{
  v  = r.Q;
  ov = mmulrue_out_none;
  //-- #init

  if (r.Q.movetop)
  {
    if (lruei.fromleft == 0)
    {
      v.pos = BITS32(lruei.left, entries_log-1, 0); 
      v.movetop = 0;
    }
  }else if (lruei.fromright)
  {
    v.pos = BITS32(lruei.right, entries_log-1, 0);
    v.movetop = !lruei.clear;
  }

  if ((lruei.touch & !lruei.clear) == 1)//-- touch request
  {
    if (v.pos == BITS32(lruei.pos, entries_log-1, 0))//-- check
        v.movetop = 1;
  }

  if ((rst==0) || (lruei.flush == 1))
  {
    v.pos = position;
    v.movetop = 0;
  }
  
  //--# Drive signals
  ov.pos &= ~MSK32(entries_log-1, 0);
  ov.pos |= r.Q.pos; 
  ov.movetop = r.Q.movetop;
  lrueo = ov; 
  
  c = v;

  r.CLK = clk;  
  r.D = c;
}





