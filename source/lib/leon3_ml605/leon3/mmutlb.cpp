//-- Description: MMU TLB logic
//------------------------------------------------------------------------------

#include "lheaders.h"


//#define tech     0//: integer range 0 to NTECH := 0;
//#define entries  8//: integer range 2 to 32 := 8;
//#define tlb_type  1//: integer range 0 to 3 := 1;
//#define tlb_rep  CFG_TLB_REP//: integer range 0 to 1 := 1;
//#define mmupgsz  CFG_MMU_PAGESIZE//: integer range 0 to 5  := 0

#define M_TLB_FASTWRITE (tlb_type&0x2)//-- fast writebuffer
  
#define entries_log (log2[entries])
#define entries_max (entries-1)

//******************************************************************
mmutlb::mmutlb(int32 entries_, uint32 tlb_type_)
{
  entries = entries_;
  tlb_type = tlb_type_;
  dataram = new syncram(entries_log, 30);
  for (int32 i=0; i<entries; i++)
    tlbcam0[i] = new mmutlbcam(tlb_type);

  lru = new mmulru(entries);
}

mmutlb::~mmutlb()
{
  free(dataram);
  for (int32 i=0; i<entries; i++)
    free(tlbcam0[i]);
  free(lru);
}

//******************************************************************
void mmutlb::Update(uint32 rst,//   : in  std_logic;
                    SClock clk,//     : in std_logic;
                    mmutlb_in_type &tlbi,//  : in  mmutlb_in_type;
                    mmutlb_out_type &tlbo,//  : out mmutlb_out_type;
                    mmutw_out_type &two,//   : in  mmutw_out_type;
                    mmutw_in_type &twi )//   : out mmutw_in_type
{
  v = r.Q;
  v.tlbmiss = 0;
  cam_addr = tlbi.transdata.data;

  wb_i_entry = 0;
  wb_ACC = 0;
  wb_PTE = 0;
  wb_LVL = 0;
  wb_CAC  = 0;
  wb_fault_pro = 0;
  wb_fault_pri = 0;
  wb_WBNEEDSYNC = 0;
      
  if ((M_TLB_FASTWRITE != 0) && (tlbi.trans_op == 0))
    cam_addr = tlbi.transdata.wb_data;

    
  wb_transdata.finish = 0;
  wb_transdata.data   = 0;
  wb_transdata.cache  = 0;
  wb_transdata.accexc = 0;

  finish   = 0;
  selstate = 0;
  
  cam_hitaddr = 0;
  cam_hit_all = 0;

  mtag.TYP = 0;
  mtag.I1 = 0;
  mtag.I2 = 0;
  mtag.I3 = 0;
  mtag.CTX = 0;
  mtag.M = 0;
  ftag.TYP = 0;
  ftag.I1 = 0;
  ftag.I2 = 0;
  ftag.I3 = 0;
  ftag.CTX = 0;
  ftag.M = 0;

  
  tlbcam_trans_op = 0;
  tlbcam_write_op = 0;
  tlbcam_flush_op = 0;
  
  twi_walk_op_ur = 0;
  twi_data = 0;
  twi_areq_ur = 0;
  twi_aaddr = 0;
  twi_adata = 0;

  two_error = 0;
  lrui_touch= 0;
  lrui_touchmin= 0;
  lrui_pos = 0;
  
  dr1write = 0;
  
  ACC = 0;
  PTE = 0;
  LVL = 0;
  CAC = 0;
  NEEDSYNC = 0;

  twACC = 0;
  twLVL = 0;
  twPTE = 0;
  twNEEDSYNC = 0;
    
  tlbcam_tagin.TYP = 0;
  tlbcam_tagin.I1 = 0;
  tlbcam_tagin.I2 = 0;
  tlbcam_tagin.I3 = 0;
  tlbcam_tagin.CTX = 0;
  tlbcam_tagin.M = 0;
  
  tlbcam_tagwrite.ET = 0;
  tlbcam_tagwrite.ACC = 0;
  tlbcam_tagwrite.M = 0;
  tlbcam_tagwrite.R = 0;
  tlbcam_tagwrite.SU = 0;
  tlbcam_tagwrite.VALID = 0;
  tlbcam_tagwrite.LVL = 0;
  tlbcam_tagwrite.I1 = 0;
  tlbcam_tagwrite.I2 = 0;
  tlbcam_tagwrite.I3 = 0;
  tlbcam_tagwrite.CTX = 0;
  tlbcam_tagwrite.PPN = 0;
  tlbcam_tagwrite.C = 0;
  
  store = 0;
  reppos = 0;
  fault_pro = 0;
  fault_pri = 0;
  fault_mexc = 0;
  fault_trans = 0;
  fault_inv = 0;
  fault_access = 0;

  transdata.finish = 0;
  transdata.data = 0;
  transdata.cache = 0;
  transdata.accexc = 0;

  fault.fault_pro = 0;
  fault.fault_pri = 0;
  fault.fault_access = 0;
  fault.fault_mexc = 0;
  fault.fault_trans = 0;
  fault.fault_inv = 0;
  fault.fault_lvl = 0;
  fault.fault_su = 0;
  fault.fault_read = 0;
  fault.fault_isid = id_dcache;
  fault.fault_addr = 0;
                         
  savewalk = 0;
  tlbo_s1finished = 0;
  
  tlbcam_trans_op = 0;
  tlbcam_write_op = 0;
  tlbcam_flush_op = 0;
  lrui_touch = 0;
  lrui_touchmin = 0;
  lrui_pos = 0;
  dr1write = 0;
  fault_pro = 0;
  fault_pri = 0;
  fault_mexc = 0;
  fault_trans = 0;
  fault_inv = 0;
  fault_access = 0;
  twi_walk_op_ur = 0;
  twi_areq_ur = 0;
  twi_aaddr = dr1_dataout<<2;
  finish = 0;
  store = 0;
  savewalk = 0;
  tlbo_s1finished = 0;
  selstate = 0; 
  
  cam_hitaddr = 0;
  cam_hit_all = 0;
  NEEDSYNC = 0;
  for (int32 i=entries-1;i>=0; i--)
  {
    NEEDSYNC = NEEDSYNC | tlbcamo.arr[i].NEEDSYNC;
    if (tlbcamo.arr[i].hit==1)
    {
      cam_hitaddr = (BITS32(cam_hitaddr,entries_log-1,0) | i) & MSK32(entries_log-1,0);
      cam_hit_all = 1;
    }
  }
  
  //-- tlbcam write operation
  tlbcam_tagwrite = TLB_CreateCamWrite( two.data, r.Q.s2_read, two.lvl, r.Q.s2_ctx, r.Q.s2_data);
    
  //-- replacement position
  reppos = 0;
#if (CFG_TLB_REP==0)
    reppos  = BITS32(lruo.pos,entries_log-1,0);
    v.touch = 0;
#elif (CFG_TLB_REP==1)
    reppos = r.Q.nrep;
#endif

  i_reppos = reppos;
  
  //-- tw
  two_error = two.fault_mexc | two.fault_trans | two.fault_inv;

  twACC = BITS32(two.data,PTE_ACC_U,PTE_ACC_D);
  twLVL = two.lvl;
  twPTE = two.data;
  twNEEDSYNC = (!BIT32(two.data,PTE_R)) | (!r.Q.s2_read & !BIT32(two.data,PTE_M)); //-- tw : writeback on next flush

#if 1
  if(iClkCnt>=258)
  bool st = true;
#endif
  switch(r.Q.s2_tlbstate)
  {
    case idle:
      if (tlbi.s2valid)
      {
        if (r.Q.s2_flush == 1)
          v.s2_tlbstate = pack;
        else
        {
          v.walk_fault.fault_pri = 0;
          v.walk_fault.fault_pro = 0;
          v.walk_fault.fault_access = 0;
          v.walk_fault.fault_trans  = 0;
          v.walk_fault.fault_inv    = 0;
          v.walk_fault.fault_mexc   = 0;
          
          if ((r.Q.s2_hm & !tlbi.mmctrl1.tlbdis )== 1) 
          {
            if (r.Q.s2_needsync == 1) v.s2_tlbstate = sync;
            else                      finish = 1;
            
#if (CFG_TLB_REP == 0)
              v.tpos = r.Q.s2_entry;
              v.touch = 1;  //-- touch lru
#endif
          }else
          {
            v.s2_entry = reppos;
            v.s2_tlbstate = walk;
            v.tlbmiss = 1;
#if (CFG_TLB_REP == 0)
              lrui_touchmin = 1;             //-- lru element consumed
#endif
          }
        }
      }
    break;

    case walk:
      if (two.finish == 1)
      {
        if (( two_error ) == 0)
        {
          tlbcam_write_op = decode(r.Q.s2_entry);
          dr1write = 1;
          TLB_CheckFault( twACC, r.Q.s2_isid, r.Q.s2_su, r.Q.s2_read, v.walk_fault.fault_pro, v.walk_fault.fault_pri );
        }

        TLB_MergeData( CFG_MMU_PAGESIZE, tlbi.mmctrl1, two.lvl , two.data, r.Q.s2_data, v.walk_transdata.data );
        v.walk_transdata.cache = BIT32(two.data,PTE_C);
        v.walk_fault.fault_lvl = two.fault_lvl;
        v.walk_fault.fault_access = 0;
        v.walk_fault.fault_mexc   = two.fault_mexc;
        v.walk_fault.fault_trans  = two.fault_trans;
        v.walk_fault.fault_inv    = two.fault_inv;
        v.walk_use = 1;
        
        if ( (twNEEDSYNC == 0) || (two_error == 1))
          v.s2_tlbstate = pack;
        else
        {
          v.s2_tlbstate = sync;
          v.sync_isw = 1;
        }
        
#if (CFG_TLB_REP == 1)
        if (r.Q.nrep == entries_max) v.nrep = 0;
        else                         v.nrep = r.Q.nrep + 1;
#endif
      }else
        twi_walk_op_ur = 1;
    break;

    case pack:
      v.s2_flush = 0;
      v.walk_use = 0;
      finish     = 1;
      v.s2_tlbstate = idle;
    break;
      
    case sync:
      tlbcam_trans_op = 1;
      if ( v.sync_isw == 1)
      {
        //-- pte address is currently written to syncram, wait one cycle before issuing twi_areq_ur
        v.sync_isw = 0;
      }else
      {
        if (two.finish == 1)
        {
          v.s2_tlbstate = pack;
          v.walk_fault.fault_mexc = two.fault_mexc;
          if (two.fault_mexc)
            v.walk_use = 1;
        }else
          twi_areq_ur = 1;
      }
    break;

    default:
      v .s2_tlbstate = idle;
  }

  if (selstate == 1)
  {
    if (tlbi.trans_op == 1) {}
    else if (tlbi.flush_op == 1){}
  }
  
  i_entry = r.Q.s2_entry;  
    
  ACC = BITS32(tlbcamo.arr[i_entry].pteout,PTE_ACC_U,PTE_ACC_D);
  PTE = tlbcamo.arr[i_entry].pteout;
  LVL = tlbcamo.arr[i_entry].LVL;
  CAC = BIT32(tlbcamo.arr[i_entry].pteout,PTE_C);
  
  transdata.cache = CAC;

  //--# fault, todo: should we flush on a fault?
  TLB_CheckFault( ACC, r.Q.s2_isid, r.Q.s2_su, r.Q.s2_read, fault_pro, fault_pri );
    
  fault.fault_pro    = 0;
  fault.fault_pri    = 0;
  fault.fault_access = 0;
  fault.fault_mexc   = 0;
  fault.fault_trans  = 0;
  fault.fault_inv    = 0;
  if ((finish == 1) && (r.Q.s2_flush == 0)) //--protect flush path
  {
    fault.fault_pro    = fault_pro;
    fault.fault_pri    = fault_pri;
    fault.fault_access = fault_access;
    fault.fault_mexc   = fault_mexc;
    fault.fault_trans  = fault_trans;
    fault.fault_inv    = fault_inv;
  }
    
  if (M_TLB_FASTWRITE != 0)
  {
    wb_i_entry = BITS32(cam_hitaddr, entries_log-1, 0);  
  
    wb_ACC = BITS32(tlbcamo.arr[wb_i_entry].pteout, PTE_ACC_U, PTE_ACC_D);
    wb_PTE = tlbcamo.arr[wb_i_entry].pteout;
    wb_LVL = tlbcamo.arr[wb_i_entry].LVL;
    wb_CAC = BIT32(tlbcamo.arr[wb_i_entry].pteout,PTE_C);
    wb_WBNEEDSYNC = tlbcamo.arr[wb_i_entry].WBNEEDSYNC;
      
    wb_transdata.cache = wb_CAC;

    TLB_MergeData( CFG_MMU_PAGESIZE, tlbi.mmctrl1, wb_LVL, wb_PTE, tlbi.transdata.data, wb_transdata.data );

    //--# fault, todo: should we flush on a fault?
    TLB_CheckFault( wb_ACC, tlbi.transdata.isid, tlbi.transdata.su, tlbi.transdata.read, wb_fault_pro, wb_fault_pri );

    wb_transdata.accexc =  wb_fault_pro | wb_fault_pri | wb_WBNEEDSYNC | (!cam_hit_all);
  }

  //--# merge data
  TLB_MergeData( CFG_MMU_PAGESIZE, tlbi.mmctrl1, LVL, PTE, r.Q.s2_data, transdata.data );
  
  //--# reset
  if (rst == 0)
  {
    v.s2_flush = 0;
    v.s2_tlbstate = idle;
#if (CFG_TLB_REP == 1)
    v.nrep = 0;
#elif (CFG_TLB_REP == 0)
    v.touch = 0;
#endif;
    v.sync_isw = 0;
  }
    
  if ((finish == 1) || (tlbi.s2valid == 0))
  {
    tlbo_s1finished = 1;
    v.s2_hm = cam_hit_all;
    v.s2_entry = BITS32(cam_hitaddr, entries_log-1, 0);
    v.s2_needsync = NEEDSYNC;
    v.s2_data = tlbi.transdata.data;
    v.s2_read = tlbi.transdata.read;
    v.s2_su = tlbi.transdata.su;
    v.s2_isid = tlbi.transdata.isid;
    v.s2_flush = tlbi.flush_op;
    v.s2_ctx = tlbi.mmctrl1.ctx;
  }

  //-- translation operation tag
  mtag = TLB_CreateCamTrans( cam_addr, tlbi.transdata.read, tlbi.mmctrl1.ctx ); 
  tlbcam_tagin = mtag;
  
  //-- flush/(probe) operation tag
  ftag = TLB_CreateCamFlush( r.Q.s2_data, tlbi.mmctrl1.ctx ); 
  if (r.Q.s2_flush == 1)
    tlbcam_tagin = ftag;
  
  if (r.Q.walk_use == 1)
  {
    transdata  = r.Q.walk_transdata;
    fault      = r.Q.walk_fault;
  }
  fault.fault_read   = r.Q.s2_read;
  fault.fault_su     = r.Q.s2_su;
  fault.fault_isid   = r.Q.s2_isid;
  fault.fault_addr   = r.Q.s2_data;
  
  transdata.finish = finish;
  transdata.accexc = 0;
    
    
  twi_adata = PTE;
  
  //--# drive signals
  tlbo.wbtransdata  = wb_transdata;
  
  tlbo.transdata    = transdata;
  tlbo.fault        = fault;
  tlbo.nexttrans    = store;
  tlbo.s1finished   = tlbo_s1finished;

  for (int32 i=0; i<M_ENT_MAX; i++)
    tlbo.tlbcamo.arr[i] = mmutlbcam_out_none;
  
  twi.walk_op_ur    = twi_walk_op_ur;
  twi.data          = r.Q.s2_data;
  twi.areq_ur       = twi_areq_ur;
  twi.adata         = twi_adata;
  twi.aaddr         = twi_aaddr;
  twi.tlbmiss       = r.Q.tlbmiss;

#if (CFG_TLB_REP == 0)
    lrui.flush        = r.Q.s2_flush;
    lrui.touch        = r.Q.touch;
    lrui.touchmin     = lrui_touchmin;
    lrui.pos          = 0;
    lrui.pos          |= BITS32(r.Q.tpos, entries_log-1, 0);
    lrui.mmctrl1      = tlbi.mmctrl1;
#endif
  
  dr1_addr          = r.Q.s2_entry;
  dr1_datain        = BITS32(two.addr,31, 2);
  dr1_enable        = 1;
  dr1_write         = dr1write;

  for (int32 i=entries-1; i>=0; i--)
  {
    tlbcami.arr[i].mmctrl    = tlbi.mmctrl1;
    tlbcami.arr[i].tagin     = tlbcam_tagin;
    tlbcami.arr[i].trans_op  = tlbi.trans_op; //--tlbcam_trans_op;
    tlbcami.arr[i].wb_op     = tlbi.wb_op; //--tlbcam_trans_op;
    tlbcami.arr[i].flush_op  = r.Q.s2_flush;
    tlbcami.arr[i].mmuen     = tlbi.mmctrl1.e;
    tlbcami.arr[i].tagwrite  = tlbcam_tagwrite;
    tlbcami.arr[i].write_op  = BIT32(tlbcam_write_op,i);
    tlbcami.arr[i].mset      = 0;
  }  //-- i
  for (int32 i=M_ENT_MAX-1; i>=entries; i--)
  {
    tlbcami.arr[i].tagin     = tlbcam_tfp_none;
    tlbcami.arr[i].trans_op  = 0;
    tlbcami.arr[i].wb_op     = 0;
    tlbcami.arr[i].flush_op  = 0;
    tlbcami.arr[i].mmuen     = 0;
    tlbcami.arr[i].tagwrite  = tlbcam_reg_none;
    tlbcami.arr[i].write_op  = 0;
    tlbcami.arr[i].mset      = 0;
    tlbcami.arr[i].mmctrl    = mmctrl_type1_none;
  }
  
  c = v;

  r.CLK = clk;
  r.D = c;

  //-- tag-cam tlb entries
  for (int32 i=entries-1; i>=0; i--)
  {
    tlbcam0[i]->Update(rst, clk, tlbcami.arr[i], tlbcamo.arr[i]);
  }
  for (int32 i=M_ENT_MAX-1; i>=entries; i--)
    tlbcamo.arr[i] = mmutlbcam_out_none;

  //-- data-ram syncram 
  dataram ->Update( clk, dr1_addr, dr1_datain, dr1_dataout, dr1_enable, dr1_write);

  //-- lru
#if (CFG_TLB_REP == 0)
  lru->Update( rst, clk, lrui, lruo );
#endif
}


