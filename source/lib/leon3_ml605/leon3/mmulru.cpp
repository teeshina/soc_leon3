//-----------------------------------------------------------------------------
//-- Description: MMU LRU logic
//------------------------------------------------------------------------------
#include "lheaders.h"

//#define entries  : integer := 8

#define entries_log (log2[entries])

mmulru::mmulru(uint32 entries_)
{
  entries = entries_;
  lruei = new mmulruei_a(entries);
  lrueo = new mmulrueo_a(entries);

  for (int32 i=entries-1; i>=0; i--)
    l1[i] = new mmulrue((uint32)i, entries);
}

mmulru::~mmulru()
{
  free(lruei);
  free(lrueo);
  for (int32 i=entries-1; i>=0; i--)
    free(l1[i]);
}

//******************************************************************
void mmulru::Update(uint32 rst,//   : in std_logic;
                    SClock clk,//   : in std_logic;
                    mmulru_in_type &lrui,//  : in mmulru_in_type;
                    mmulru_out_type &lruo)//  : out mmulru_out_type 
{
  v = r.Q;
  //-- #init
  reinit = 0;
  v_lruei_clk = rst;
     
  //--# eather element in luri or element 0 to top
  pos = BITS32(lrui.pos,entries_log-1, 0);
  touch = lrui.touch;
  if (lrui.touchmin)
  {
    pos  = BITS32(lrueo->arr[0].pos, entries_log-1, 0);
    touch = 1;
  }
  for (int32 i=entries-1; i>=0; i--)
  {
    lruei->arr[i].pos   = 0;  //-- this is really ugly ...
    lruei->arr[i].left  = 0;
    lruei->arr[i].right = 0;
    lruei->arr[i].pos &= ~MSK32(entries_log-1, 0);
    lruei->arr[i].pos   |= pos;
    lruei->arr[i].touch = touch;
    lruei->arr[i].clear = (r.Q.clear>>((entries-1)-i))&0x1;  //-- reverse order
    lruei->arr[i].flush = lrui.flush;
  }
  
  lruei->arr[entries-1].fromleft  = 0;
  lruei->arr[entries-1].fromright = lrueo->arr[entries-2].movetop;
  lruei->arr[entries-1].right &= ~MSK32(entries_log-1, 0);
  lruei->arr[entries-1].right |= BITS32(lrueo->arr[entries-2].pos, entries_log-1, 0);
    
  for (int32 i=entries-2; i>=1; i--)
  {
    lruei->arr[i].left &= ~MSK32(entries_log-1, 0);
    lruei->arr[i].left |= BITS32(lrueo->arr[i+1].pos, entries_log-1, 0);
    lruei->arr[i].right &= ~MSK32(entries_log-1, 0);
    lruei->arr[i].right |= BITS32(lrueo->arr[i-1].pos, entries_log-1, 0);
    lruei->arr[i].fromleft  = lrueo->arr[i+1].movetop;
    lruei->arr[i].fromright = lrueo->arr[i-1].movetop;
  }
  
  lruei->arr[0].fromleft = lrueo->arr[1].movetop;
  lruei->arr[0].fromright = 0;
  lruei->arr[0].left &= ~MSK32(entries_log-1, 0);
  lruei->arr[0].left |= BITS32(lrueo->arr[1].pos, entries_log-1, 0);

  if (r.Q.bar != lrui.mmctrl1.bar)
    reinit = 1;

  //-- pragma translate_off
  
  //-- pragma translate_on
  if (!rst)
  {
    v.bar  = lrui.mmctrl1.bar;
    reinit = 1;
  }

  if (reinit)
  {
    v.bar = lrui.mmctrl1.bar;
    v.clear = 0;
    switch (lrui.mmctrl1.bar)
    {
      case 1:
        v.clear |= 0x3;  //-- reverse order
      break;
      case 2:
        v.clear |= 0x7;  //-- reverse order
      break;
      case 3:
        v.clear |= 0x1F;//"11111"; -- reverse order
      break;
      default:
        v.clear |= 0x1; 
    }
  }

  //--# drive signals
    
  //-- pragma translate_off
  v.reinit = reinit;
  v.pos    = pos;
  //-- pragma translate_on
  
  lruo.pos  = lrueo->arr[0].pos;

  c = v;
  
  r.CLK = clk;
  r.D = c;

  //--# lru entries
  for (int32 i=entries-1; i>=0; i--)
    l1[i]->Update(rst, clk, lruei->arr[i], lrueo->arr[i]);
}
