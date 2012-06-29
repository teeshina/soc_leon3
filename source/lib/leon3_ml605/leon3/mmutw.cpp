//------------------------------------------------------------------------------
//-- Description: MMU table-walk logic
//------------------------------------------------------------------------------
#include "leon3_ml605\config.h"
#include "leon3_ml605\leon3\libmmu.h"
#include "mmutw.h"

//#define mmupgsz   CFG_MMU_PAGESIZE//: integer range 0 to 5  := 0
//#ifdef CFG_MMU_PAGESIZE
//  #undef CFG_MMU_PAGESIZE
//#endif
//#define CFG_MMU_PAGESIZE 4

void mmutw::Update(uint32 rst,//     : in  std_logic;
                    SClock clk,//     : in  std_logic;
                    mmctrl_type1 &mmctrl1,// : in  mmctrl_type1;
                    mmutw_in_type &twi,//     : in  mmutw_in_type;
                    mmutw_out_type &two,//     : out mmutw_out_type;
                    memory_mm_out_type &mcmmo,//   : in  memory_mm_out_type;
                    memory_mm_in_type &mcmmi)//   : out memory_mm_in_type
{
  v = r.Q;
  
  //--#init
  finish = 0; 
  index = 0;
  lvl = 0;
  fault_mexc = 0;
  fault_trans = 0;
  fault_inv = 0;
  fault_lvl = 0;
  pte = 0;
  ptd = 0;
  inv = 0;
  rvd = 0;
  goon = 0;
  found = 0;
  base = 0;
  base = BITS32(mcmmo.data,PTD_PTP32_U,PTD_PTP32_D) << PADDR_PTD_D;

  if (mcmmo.grant == 1)
    v.req = 0;

  if (mcmmo.retry == 1) v.req = 1;
  
  //-- # pte/ptd
  if (mcmmo.ready & !r.Q.req)//-- context
  {
    switch (BITS32(mcmmo.data,PT_ET_U,PT_ET_D))
    {
      case ET_INV: inv = 1; break;
      case ET_PTD: ptd = 1; goon  = 1; break;
      case ET_PTE: pte = 1; found = 1; break;
      case ET_RVD: rvd = 1; break;
      default:;
    }
  }
  fault_trans = rvd;
  fault_inv = inv;

  pagesize = MMU_getpagesize(CFG_MMU_PAGESIZE, mmctrl1);
    
  switch (pagesize)
  {
    case 1:
      //-- 8k tag comparision [ 7 6 6 ]
    case 2:
      //-- 16k tag comparision [ 6 6 6 ]
    case 3:
      //-- 32k tag comparision [ 4 7 6 ]
    default:;    //-- standard 4k tag comparision [ 8 6 6 ]
  }
    
  //-- # state machine
  switch(r.Q.state)
  {
    case idle:
      if (twi.walk_op_ur)
      {
        v.walk_op = 1;
        index |= (mmctrl1.ctx&MSK32(M_CTX_SZ-1, 0));
        base = 0;
        base = BITS32(mmctrl1.ctxp,MMCTRL_PTP32_U,MMCTRL_PTP32_D) << PADDR_PTD_D; 
        v.wb.addr = base | (index<<2);
        v.wb.read = 1;
        v.req = 1;
        v.state = lv1;
      }else if (twi.areq_ur)
      {
        index = 0;
        v.wb.addr = twi.aaddr;
        v.wb.data = twi.adata;
        v.wb.read = 0;
        v.req = 1;
        v.state = waitm;
      }
    break;
    case waitm:
      if ((mcmmo.ready & !r.Q.req)==1) //-- amba: result ready current cycle
      {
        fault_mexc = mcmmo.mexc;
        v.state = idle;
        finish = 1;
      }
    break;
    case lv1:
      if ((mcmmo.ready & !r.Q.req)==1)
      {
        lvl = LVL_CTX; 
        fault_lvl = FS_L_CTX;
        switch (pagesize)
        {
          case 1:
            //-- 8k tag comparision [ 7 6 6 ]
            index &= ~MSK32(P8K_VA_I1_SZ-1, 0);
            index |= BITS32(twi.data, P8K_VA_I1_U, P8K_VA_I1_D);       
          break;
          case 2:
            //-- 16k tag comparision [ 6 6 6 ]
            index &= ~MSK32(P16K_VA_I1_SZ-1, 0);
            index |= BITS32(twi.data, P16K_VA_I1_U, P16K_VA_I1_D);       
          break;
          case 3:
            //-- 32k tag comparision [ 4 7 6 ]
            index &= ~MSK32(P32K_VA_I1_SZ-1, 0);
            index |= BITS32(twi.data, P32K_VA_I1_U, P32K_VA_I1_D);       
          break;
          default:
            //-- standard 4k tag comparision [ 8 6 6 ]
            index &= ~MSK32(VA_I1_SZ-1, 0);
            index |= BITS32(twi.data, VA_I1_U, VA_I1_D);
        }
        v.state = lv2;
      }
    break;
    case lv2:
      if ((mcmmo.ready & !r.Q.req)==1)
      {
        lvl = LVL_REGION;
        fault_lvl = FS_L_L1;             
        switch(pagesize)
        {
          case 1:
            //-- 8k tag comparision [ 7 6 6 ]
            index &= ~MSK32(P8K_VA_I2_SZ-1, 0);
            index |= BITS32(twi.data, P8K_VA_I2_U, P8K_VA_I2_D);       
          break;
          case 2:
            //-- 16k tag comparision [ 6 6 6 ]
            index &= ~MSK32(P16K_VA_I2_SZ-1, 0);
            index |= BITS32(twi.data, P16K_VA_I2_U, P16K_VA_I2_D);       
          break;
          case 3:
            //-- 32k tag comparision [ 4 7 6 ]
            index &= ~MSK32(P32K_VA_I2_SZ-1,0);
            index |= BITS32(twi.data, P32K_VA_I2_U, P32K_VA_I2_D);       
          break;
          default:
            //-- standard 4k tag comparision [ 8 6 6 ]
            index &= ~MSK32(VA_I2_SZ-1, 0);
            index |= BITS32(twi.data, VA_I2_U, VA_I2_D);       
        }
        v.state = lv3;
      }
    break;
    case lv3:
      if ((mcmmo.ready & !r.Q.req)== 1)
      {
        lvl = LVL_SEGMENT;
        fault_lvl = FS_L_L2;             
        switch(pagesize)
        {
          case 1:
            //-- 8k tag comparision [ 7 6 6 ]
            index &= ~MSK32(P8K_VA_I3_SZ-1, 0);
            index |= BITS32(twi.data, P8K_VA_I3_U,P8K_VA_I3_D);
          break;
          case 2:
            //-- 16k tag comparision [ 6 6 6 ]
            index &= ~MSK32(P16K_VA_I3_SZ-1, 0);
            index |= BITS32(twi.data, P16K_VA_I3_U, P16K_VA_I3_D);
          break;
          case 3:
            //-- 32k tag comparision [ 4 7 6 ]
            index &= ~MSK32(P32K_VA_I3_SZ-1, 0);
            index |= BITS32(twi.data, P32K_VA_I3_U, P32K_VA_I3_D);
          break;
          default:
            //-- standard 4k tag comparision [ 8 6 6 ]
            index &= ~MSK32(VA_I3_SZ-1, 0);
            index |= BITS32(twi.data, VA_I3_U, VA_I3_D);
        }
        v.state = lv4;
      }
    break;
    case lv4:
      if ((mcmmo.ready & !r.Q.req)==1)
      {
        lvl = LVL_PAGE;
        fault_lvl = FS_L_L3;                
        fault_trans = fault_trans | ptd;
        v.state = idle;
        finish = 1;                                          
      }
    break;
    default:
      v.state = idle;
      finish = 0;
  }
  base = base | (index<<2);

  if (r.Q.walk_op == 1)
  {
    if (mcmmo.ready & !r.Q.req)
    {
      fault_mexc = mcmmo.mexc;
      if ( ptd & !fault_mexc & !fault_trans & !fault_inv) //-- tw  : break table walk?
      {
        v.wb.addr = base;
        v.req = 1;
      }else
      {
        v.walk_op = 0;
        finish    = 1;
        v.state   = idle;
      }
    }
  }
        
  //-- # reset
  if ( !rst)
  {
    v.state   = idle;
    v.req     = 0;
    v.walk_op = 0;
    v.wb.read = 0;
  }

  //--# drive signals
  two.finish      = finish;
  two.data        = mcmmo.data;
  two.addr        = BITS32(r.Q.wb.addr, 31, 0);
  two.lvl         = lvl;
  two.fault_mexc  = fault_mexc;
  two.fault_trans = fault_trans;
  two.fault_inv   = fault_inv;
  two.fault_lvl   = fault_lvl;
  
  mcmmi.address  = r.Q.wb.addr;
  mcmmi.data     = r.Q.wb.data;
  mcmmi.burst    = 0;
  mcmmi.size     = 0x2;
  mcmmi.read     = r.Q.wb.read;
  mcmmi.lock     = 0;
  mcmmi.req      = r.Q.req;

  //--#dump
  //-- pragma translate_off
  v.finish = finish;
  v.index  = index;
  v.lvl    = lvl;
  v.fault_mexc = fault_mexc;
  v.fault_trans = fault_trans;
  v.fault_lvl = fault_lvl;
  v.pte   = pte;
  v.ptd   = ptd;
  v.inv   = inv;
  v.rvd   = rvd;
  v.goon  = goon;
  v.found = found;
  v.base  = base;
  //-- pragma translate_on

  c = v;

  r.CLK = clk;
  r.D = c;
}
