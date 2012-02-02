//-----------------------------------------------------------------------------
//-- Description: MMU TLB logic
//------------------------------------------------------------------------------
#include "lheaders.h"

//#define tlb_type  : integer range 0 to 3 := 1;
//#define mmupgsz   CFG_MMU_PAGESIZE//: integer range 0 to 5  := 0

#define M_TLB_FASTWRITE (tlb_type&0x2)//-- fast writebuffer

mmutlbcam::mmutlbcam(uint32 tlb_type_)
{
  tlb_type = tlb_type_;
}

//******************************************************************
void mmutlbcam::Update( uint32 rst,//     : in std_logic;
                        SClock clk,//     : in std_logic;
                        mmutlbcam_in_type &tlbcami,// : in mmutlbcam_in_type;
                        mmutlbcam_out_type &tlbcamo)// : out mmutlbcam_out_type
{
  v = r.Q;
  //--#init
  h_i1 = 0;
  h_i2 = 0;
  h_i3 = 0;
  h_c  = 0;
  hm = 0;
  pagesize = 0;
  hf = r.Q.btag.VALID;
  
  blvl = r.Q.btag.LVL;
  bet  = r.Q.btag.ET;
  bsu  = r.Q.btag.SU;
  bet_decode  = decode(bet);
  blvl_decode = decode(blvl);
  ref = r.Q.btag.R;
  modified = r.Q.btag.M;

  tlbcamo_pteout = 0;
  tlbcamo_LVL = 0; 

  vaddr_r = ((r.Q.btag.I1<<12) | (r.Q.btag.I2<<6) | r.Q.btag.I3) << 12;
  vaddr_i = ((tlbcami.tagin.I1<<12) | (tlbcami.tagin.I2<<6) | tlbcami.tagin.I3) << 12;

  //-- prepare tag comparision
  pagesize = MMU_getpagesize(CFG_MMU_PAGESIZE,tlbcami.mmctrl);
  switch (pagesize)
  {
    case 1:
      //-- 8k tag comparision [ 7 6 6 ]
      if (BITS32(vaddr_r,P8K_VA_I1_U,P8K_VA_I1_D)==BITS32(vaddr_i,P8K_VA_I1_U,P8K_VA_I1_D)) h_i1 = 1;
      else h_i1 = 0;
      if (BITS32(vaddr_r,P8K_VA_I2_U,P8K_VA_I2_D)==BITS32(vaddr_i,P8K_VA_I2_U,P8K_VA_I2_D)) h_i2 = 1;
      else h_i2 = 0;
      if (BITS32(vaddr_r,P8K_VA_I3_U,P8K_VA_I3_D)==BITS32(vaddr_i,P8K_VA_I3_U,P8K_VA_I3_D)) h_i3 = 1;
      else h_i3 = 0;
      if (r.Q.btag.CTX == tlbcami.tagin.CTX) h_c = 1;
      else                                 h_c = 0;
    break;
    case 2:
      //-- 16k tag comparision [ 6 6 6 ]
      if (BITS32(vaddr_r,P16K_VA_I1_U,P16K_VA_I1_D)==BITS32(vaddr_i,P16K_VA_I1_U,P16K_VA_I1_D)) h_i1 = 1;
      else h_i1 = 0;
      if (BITS32(vaddr_r,P16K_VA_I2_U,P16K_VA_I2_D)==BITS32(vaddr_i,P16K_VA_I2_U,P16K_VA_I2_D)) h_i2 = 1;
      else h_i2 = 0;
      if (BITS32(vaddr_r,P16K_VA_I3_U,P16K_VA_I3_D)==BITS32(vaddr_i,P16K_VA_I3_U,P16K_VA_I3_D)) h_i3 = 1;
      else h_i3 = 0;
      if (r.Q.btag.CTX == tlbcami.tagin.CTX) h_c = 1;
      else                                 h_c = 0;
    break;
    case 3:
      //-- 32k tag comparision [ 4 7 6 ]
      if (BITS32(vaddr_r,P32K_VA_I1_U,P32K_VA_I1_D)==BITS32(vaddr_i,P32K_VA_I1_U,P32K_VA_I1_D)) h_i1 = 1;
      else h_i1 = 0;
      if (BITS32(vaddr_r,P32K_VA_I2_U,P32K_VA_I2_D)==BITS32(vaddr_i,P32K_VA_I2_U,P32K_VA_I2_D)) h_i2 = 1;
      else h_i2 = 0;
      if (BITS32(vaddr_r,P32K_VA_I3_U,P32K_VA_I3_D)==BITS32(vaddr_i,P32K_VA_I3_U,P32K_VA_I3_D)) h_i3 = 1;
      else h_i3 = 0;
      if (r.Q.btag.CTX == tlbcami.tagin.CTX) h_c = 1;
      else                                 h_c = 0;
    break;
    default:    //-- standard 4k tag comparision [ 8 6 6 ]
      if (r.Q.btag.I1  == tlbcami.tagin.I1)  h_i1 = 1;
      else h_i1 = 0;
      if (r.Q.btag.I2  == tlbcami.tagin.I2)  h_i2 = 1;
      else h_i2 = 0;
      if (r.Q.btag.I3  == tlbcami.tagin.I3)  h_i3 = 1;
      else h_i3 = 0;
      if (r.Q.btag.CTX == tlbcami.tagin.CTX) h_c  = 1;
      else h_c  = 0;
  }
    
  //-- #level 2 hit (segment)
  h_l2 = h_i1 & h_i2 ;
  //-- #level 3 hit (page)
  h_l3 = h_i1 & h_i2 & h_i3;
  //-- # context + su
  h_su_cnt = h_c | bsu;
  
  //--# translation (match) op
  switch(blvl)
  {
    case LVL_PAGE:    hm = h_l3 & h_c & r.Q.btag.VALID; break;
    case LVL_SEGMENT: hm = h_l2 & h_c & r.Q.btag.VALID; break;
    case LVL_REGION:  hm = h_i1 & h_c & r.Q.btag.VALID; break;
    case LVL_CTX:     hm =        h_c & r.Q.btag.VALID; break;
    default:          hm = 0;//'X';
  }
    
    //--# translation: update ref/mod bit
  tlbcamo_NEEDSYNC = 0;
  if (tlbcami.trans_op & hm)
  {
    v.btag.R = 1;
    v.btag.M = r.Q.btag.M | tlbcami.tagin.M;
    tlbcamo_NEEDSYNC = (!r.Q.btag.R) | (tlbcami.tagin.M & (!r.Q.btag.M));  //-- cam: ref/modified changed, write back synchronously
  }
  tlbcamo_WBNEEDSYNC = 0;
  if ( hm == 1)
    tlbcamo_WBNEEDSYNC = !r.Q.btag.R | (tlbcami.tagin.M & !r.Q.btag.M);  //-- cam: ref/modified changed, write back synchronously
  
  //--# flush operation
  //-- tlbcam only stores PTEs, tlb does not store PTDs
  switch(tlbcami.tagin.TYP)
  {
    case FPTY_PAGE:                     //-- page
      hf = hf & h_su_cnt & h_l3 & BIT32(blvl_decode,0);  //-- only level 3 (page)
    break;
    case FPTY_SEGMENT:                  //-- segment
      hf = hf & h_su_cnt & h_l2 & (BIT32(blvl_decode,0) | BIT32(blvl_decode,1));  //-- only level 2+3 (segment,page)
    break;
    case FPTY_REGION:                      //-- region
      hf = hf & h_su_cnt & h_i1 & !BIT32(blvl_decode,3);  //-- only level 1+2+3 (region,segment,page)
    break;
    case FPTY_CTX:                    //-- context
      hf = hf & (h_c & (!bsu)); 
    break;
    case FPTY_N:                          //-- entire
    break;
    default:
      hf = 0;
  }

  //--# flush: invalidate on flush hit
  //--if (tlbcami.flush_op and hf ) = '1' then
  if (tlbcami.flush_op  )
    v.btag.VALID = 0;
  
  //--# write op
  if ( tlbcami.write_op == 1)
    v.btag = tlbcami.tagwrite;

  //--# reset
  if ((rst == 0) || (tlbcami.mmuen == 0))
    v.btag.VALID = 0;
    
  tlbcamo_pteout |= (r.Q.btag.PPN<<PTE_PPN_D);
  tlbcamo_pteout |= (r.Q.btag.C<<PTE_C);
  tlbcamo_pteout |= (r.Q.btag.M<<PTE_M);
  tlbcamo_pteout |= (r.Q.btag.R<<PTE_R);
  tlbcamo_pteout |= (r.Q.btag.ACC<<PTE_ACC_D);
  tlbcamo_pteout |= (r.Q.btag.ET<<PT_ET_D);
  tlbcamo_LVL |= r.Q.btag.LVL;
    
  //--# drive signals
  tlbcamo.pteout   = tlbcamo_pteout;
  tlbcamo.LVL      = tlbcamo_LVL;
  //--tlbcamo.hit      <= (tlbcami.trans_op and hm) or (tlbcami.flush_op and hf);
  tlbcamo.hit      = (hm) | (tlbcami.flush_op & hf);
  tlbcamo.ctx      = r.Q.btag.CTX;       //-- for diagnostic only
  tlbcamo.valid    = r.Q.btag.VALID;     //-- for diagnostic only
  tlbcamo.vaddr    = (r.Q.btag.I1<<24) | (r.Q.btag.I2<<18) | (r.Q.btag.I3<<12); //-- for diagnostic only
  tlbcamo.NEEDSYNC = tlbcamo_NEEDSYNC;
  tlbcamo.WBNEEDSYNC = tlbcamo_WBNEEDSYNC;
    
    
  //c = v;
  
  r.CLK =clk;
  r.D = v;
}


