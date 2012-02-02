#include "lheaders.h"

//#define tech     0// : integer range 0 to NTECH := 0;
//#define itlbnum  CFG_ITLBNUM// : integer range 2 to 64 := 8;
//#define dtlbnum  CFG_DTLBNUM// : integer range 2 to 64 := 8;
//#define tlb_type CFG_TLB_TYPE// : integer range 0 to 3 := 1;
//#define tlb_rep  CFG_TLB_REP// : integer range 0 to 1 := 0;
//#define mmupgsz  CFG_MMU_PAGESIZE// : integer range 0 to 5  := 0

#define MMUCTX_BITS  M_CTX_SZ//: integer := M_CTX_SZ;

#define M_TLB_TYPE     (CFG_TLB_TYPE&0x1)//: integer range 0 to 1 := conv_integer(conv_std_logic_vector(tlb_type,2) and conv_std_logic_vector(1,2));  -- eather split or combined
#define M_TLB_FASTWRITE (CFG_TLB_TYPE&0x3)//: integer range 0 to 3 := conv_integer(conv_std_logic_vector(tlb_type,2) and conv_std_logic_vector(2,2));   -- fast writebuffer
#define M_ENT_I        CFG_ITLBNUM//: integer range 2 to 64 := itlbnum;   -- icache tlb entries: number
#define M_ENT_ILOG     (log2[M_ENT_I]//): integer := log2(M_ENT_I);     -- icache tlb entries: address bits
#define M_ENT_D        CFG_DTLBNUM//: integer range 2 to 64 := dtlbnum;   -- dcache tlb entries: number
#define M_ENT_DLOG     (log2[M_ENT_D])//: integer := log2(M_ENT_D);     -- dcache tlb entries: address bits
#define M_ENT_C        M_ENT_I//: integer range 2 to 64 := M_ENT_I;   -- i/dcache tlb entries: number
#define M_ENT_CLOG     M_ENT_ILOG//: integer := M_ENT_ILOG;     -- i/dcache tlb entries: address bits

mmu::mmu()
{
  itlb0 = new mmutlb(M_ENT_I, 0);
  dtlb0 = new mmutlb(M_ENT_D, CFG_TLB_TYPE);
}

mmu::~mmu()
{
  free(itlb0);
  free(dtlb0);
}


void mmu::Update( uint32 rst,//  : in std_logic;
                  SClock clk,//  : in std_logic;
                  mmudc_in_type &mmudci,// : in  mmudc_in_type;
                  mmudc_out_type &mmudco,// : out mmudc_out_type;
                  mmuic_in_type &mmuici,// : in  mmuic_in_type;
                  mmuic_out_type &mmuico,// : out mmuic_out_type;
                  memory_mm_out_type &mcmmo,//  : in  memory_mm_out_type;
                  memory_mm_in_type &mcmmi)//  : out memory_mm_in_type
{
  v = r.Q;

  wbtransdata.finish = 0;
  wbtransdata.data   = 0;
  wbtransdata.cache  = 0;
  wbtransdata.accexc = 0;
  if ((M_TLB_TYPE == 0) && (M_TLB_FASTWRITE != 0))
    wbtransdata = tlbo_a1.wbtransdata;
  
  cmbtlbin.data = 0;
  cmbtlbin.su   = 0;
  cmbtlbin.read = 0;
  cmbtlbin.isid = id_dcache;
  
  cmbtlbout.transdata.finish = 0;
  cmbtlbout.transdata.data   = 0;
  cmbtlbout.transdata.cache  = 0;
  cmbtlbout.transdata.accexc = 0;
  
  cmbtlbout.fault.fault_pro    = 0;
  cmbtlbout.fault.fault_pri    = 0;
  cmbtlbout.fault.fault_access = 0;
  cmbtlbout.fault.fault_mexc   = 0;
  cmbtlbout.fault.fault_trans  = 0;
  cmbtlbout.fault.fault_inv = 0;
  cmbtlbout.fault.fault_lvl = 0;
  cmbtlbout.fault.fault_su  = 0;
  cmbtlbout.fault.fault_read = 0;
  cmbtlbout.fault.fault_isid  = id_dcache;
  cmbtlbout.fault.fault_addr = 0;

  cmbtlbout.nexttrans  = 0;
  cmbtlbout.s1finished = 0;
  
  mmuico_transdata.finish = 0;
  mmuico_transdata.data   = 0;
  mmuico_transdata.cache  = 0;
  mmuico_transdata.accexc = 0;

  mmudco_transdata.finish = 0;
  mmudco_transdata.data   = 0;
  mmudco_transdata.cache  = 0;
  mmudco_transdata.accexc = 0;
  
  mmuico_grant = 0;
  mmudco_grant = 0;

  twiv.walk_op_ur = 0;
  twiv.areq_ur    = 0;
  
  twiv.data  = 0;
  twiv.adata = 0;
  twiv.aaddr = 0;
  
  twod.finish = 0;
  twod.data = 0;
  twod.addr = 0;
  twod.lvl  = 0;
  twod.fault_mexc  = 0;
  twod.fault_trans = 0;
  twod.fault_inv = 0;
  twod.fault_lvl = 0;

  twoi.finish = 0;
  twoi.data   = 0;
  twoi.addr   = 0;
  twoi.lvl    = 0;
  twoi.fault_mexc = 0;
  twoi.fault_trans = 0;
  twoi.fault_inv = 0;
  twoi.fault_lvl = 0;
  
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

  fs.ow = 0;
  fs.fav = 0;
  fs.ft = 0;
  fs.at_ls = 0;
  fs.at_id = 0;
  fs.at_su = 0;
  fs.l = 0;
  fs.ebe = 0;
  
  fa = 0;

  if (M_TLB_TYPE == 0)
  {
    spltitlbout = tlbo_a0;
    spltdtlbout = tlbo_a1;
    twod = two;
    twoi = two;
    twod.finish = 0;
    twoi.finish = 0;
    spltdtlbin = mmudci.transdata;
    spltitlbin = mmuici.transdata;
    mmudco_transdata = spltdtlbout.transdata;
    mmuico_transdata = spltitlbout.transdata;

    //-- d-tlb
    if (!r.Q.splt_ds1.tlbactive | spltdtlbout.s1finished)
    {
      v.splt_ds1.tlbactive   = 0;
      v.splt_ds1.op.trans_op = 0;
      v.splt_ds1.op.flush_op = 0;
      if (mmudci.trans_op == 1)
      {
        mmudco_grant           = 1;
        v.splt_ds1.tlbactive   = 1;
        v.splt_ds1.op.trans_op = 1;
      }else if (mmudci.flush_op == 1)
      {
        v.flush = 1;
        mmudco_grant           = 1;
        v.splt_ds1.tlbactive   = 1;
        v.splt_ds1.op.flush_op = 1;
      }
    }
    
    //-- i-tlb
    if (!r.Q.splt_is1.tlbactive | spltitlbout.s1finished)
    {
      v.splt_is1.tlbactive   = 0;
      v.splt_is1.op.trans_op = 0;
      v.splt_is1.op.flush_op = 0;
      if (v.flush == 1)
      {
        v.flush                = 0;
        v.splt_is1.tlbactive   = 1;
        v.splt_is1.op.flush_op = 1;
      }else if (mmuici.trans_op == 1)
      {
        mmuico_grant           = 1;
        v.splt_is1.tlbactive   = 1;
        v.splt_is1.op.trans_op = 1;
      }
    }

    if (spltitlbout.transdata.finish & !r.Q.splt_is2.op.flush_op)
      fault = spltitlbout.fault;
    if (spltdtlbout.transdata.finish & !r.Q.splt_is2.op.flush_op)
    {
      if (spltdtlbout.fault.fault_mexc | 
          spltdtlbout.fault.fault_trans |
          spltdtlbout.fault.fault_inv |
          spltdtlbout.fault.fault_pro |
          spltdtlbout.fault.fault_pri |
          spltdtlbout.fault.fault_access) 
        fault = spltdtlbout.fault;     //-- overwrite icache fault
    }
    
    if (spltitlbout.s1finished) v.splt_is2 = r.Q.splt_is1;
    if (spltdtlbout.s1finished) v.splt_ds2 = r.Q.splt_ds1;
    
    if ( r.Q.splt_is2.op.flush_op )
      mmuico_transdata.finish = 0;
        
    //-- share tw
    if (two.finish == 1)
      v.twactive = 0;
    
    if (r.Q.twowner == id_icache)
    {
      twiv = twi_a[0];
      twoi.finish = two.finish;
    }else
    {
      twiv = twi_a[1];
      twod.finish = two.finish;
    }
    
    if (v.twactive == 0)
    {
      if (twi_a[1].areq_ur | twi_a[1].walk_op_ur)
      {
        v.twactive = 1;
        v.twowner  = id_dcache;
      }else if (twi_a[0].areq_ur | twi_a[0].walk_op_ur)
      {
        v.twactive = 1;
        v.twowner  = id_icache;
      }
    }
    
  }else
  {
    //--# combined i/d cache: 1 tlb, 1 tw
    //-- share one tlb among i and d cache
    cmbtlbout = tlbo_a0;
    mmuico_grant = 0;
    mmudco_grant = 0;
    mmuico_transdata.finish = 0;
    mmudco_transdata.finish = 0;
    twiv = twi_a[0];
    twod = two;
    twoi = two;
    twod.finish = 0;
    twoi.finish = 0;
    //-- twod.finish := two.finish;
    twoi.finish = two.finish;
  
    if (!v.cmb_s1.tlbactive | cmbtlbout.s1finished)
    {
      v.cmb_s1.tlbactive   = 0;
      v.cmb_s1.op.trans_op = 0;
      v.cmb_s1.op.flush_op = 0;
      if (mmudci.trans_op | mmudci.flush_op | mmuici.trans_op)
        v.cmb_s1.tlbactive = 1;
      if (mmuici.trans_op)
      {
        mmuico_grant = 1; 
        v.cmb_s1.tlbowner = id_icache;
        v.cmb_s1.op.trans_op = 1;
      }else if (mmudci.trans_op = 1)
      {
        mmudco_grant = 1;
        v.cmb_s1.tlbowner = id_dcache;
        v.cmb_s1.op.trans_op = 1;
      }else if (mmudci.flush_op)
      {
        mmudco_grant = 1;
        v.cmb_s1.tlbowner = id_dcache;
        v.cmb_s1.op.flush_op = 1;
      }
    }
  
    if (r.Q.cmb_s1.tlbactive & !r.Q.cmb_s2.tlbactive)
    {}
    
    if (cmbtlbout.s1finished)
      v.cmb_s2 = r.Q.cmb_s1;

    if (r.Q.cmb_s1.tlbowner == id_dcache) cmbtlbin = mmudci.transdata;
    else                                  cmbtlbin = mmuici.transdata;
  
    if (r.Q.cmb_s2.tlbowner == id_dcache) mmudco_transdata = cmbtlbout.transdata;
    else                                  mmuico_transdata = cmbtlbout.transdata;

    if (cmbtlbout.transdata.finish & !r.Q.cmb_s2.op.flush_op)
      fault = cmbtlbout.fault;
  }

  //-- # fault status register
  if (mmudci.fsread)
  {
    v.mmctrl2.valid = 0;
    v.mmctrl2.fs.fav = 0;
  }
  
#if 1
  if(iClkCnt>=41)
  bool st = true;
#endif
  if (fault.fault_mexc)
    fs.ft = FS_FT_TRANS;
  else if (fault.fault_trans)
    fs.ft = FS_FT_INV;
  else if (fault.fault_inv)
    fs.ft = FS_FT_INV;
  else if (fault.fault_pri)
    fs.ft = FS_FT_PRI;
  else if (fault.fault_pro)
    fs.ft = FS_FT_PRO;
  else if (fault.fault_access)
    fs.ft = FS_FT_BUS;
  else 
    fs.ft = FS_FT_NONE;

  
  fs.ow = 0;
  fs.l  = fault.fault_lvl;
  if (fault.fault_isid == id_dcache) fs.at_id = 0;
  else                               fs.at_id = 1;

  fs.at_su = fault.fault_su;
  fs.at_ls = !fault.fault_read;
  fs.fav   = 1;
  fs.ebe   = 0;
  
  fa = (fault.fault_addr&LSBMSK32(VA_I_U))>>VA_I_D;

  if (fault.fault_mexc | 
      fault.fault_trans |
      fault.fault_inv |
      fault.fault_pro |
      fault.fault_pri |
      fault.fault_access)
  {
          
    //--# priority
    if (v.mmctrl2.valid)
    {
      if (fault.fault_mexc)
      {
        v.mmctrl2.fs = fs;
        v.mmctrl2.fa = fa;
      }else
      {
        if (r.Q.mmctrl2.fs.ft != FS_FT_INV)
        {
          if (fault.fault_isid == id_dcache)
          {
            //-- dcache
            v.mmctrl2.fs = fs;
            v.mmctrl2.fa = fa;
          }else
          {
            //-- icache
            if (r.Q.mmctrl2.fs.at_id)
            {
              fs.ow = 1;
              v.mmctrl2.fs = fs;
              v.mmctrl2.fa = fa;
            }
          }
        }
      }
    }else
    {
      v.mmctrl2.fs = fs;
      v.mmctrl2.fa = fa;
      v.mmctrl2.valid = 1;
    }

    if (fault.fault_isid == id_dcache)
      mmudco_transdata.accexc = 1;
    else
      mmuico_transdata.accexc = 1;
  }


  //-- # reset
  if ( !rst )
  {
    if (M_TLB_TYPE == 0)
    {
      v.splt_is1.tlbactive = 0;
      v.splt_is2.tlbactive = 0;
      v.splt_ds1.tlbactive = 0;
      v.splt_ds2.tlbactive = 0;
      v.splt_is1.op.trans_op = 0;
      v.splt_is2.op.trans_op = 0;
      v.splt_ds1.op.trans_op = 0;
      v.splt_ds2.op.trans_op = 0;
      v.splt_is1.op.flush_op = 0;
      v.splt_is2.op.flush_op = 0;
      v.splt_ds1.op.flush_op = 0;
      v.splt_ds2.op.flush_op = 0;
    }else
    {
      v.cmb_s1.tlbactive = 0;
      v.cmb_s2.tlbactive = 0;
      v.cmb_s1.op.trans_op = 0;
      v.cmb_s2.op.trans_op = 0;
      v.cmb_s1.op.flush_op = 0;
      v.cmb_s2.op.flush_op = 0;
    }
    v.flush = 0;
    v.mmctrl2.valid = 0;
    v.twactive = 0;
    v.twowner = id_icache;
  }


  //-- drive signals
  if (M_TLB_TYPE == 0)
  {
    tlbi_a0.trans_op  = r.Q.splt_is1.op.trans_op;
    tlbi_a0.flush_op  = r.Q.splt_is1.op.flush_op;
    tlbi_a0.transdata = spltitlbin;
    tlbi_a0.s2valid   = r.Q.splt_is2.tlbactive;
    tlbi_a0.mmctrl1   = mmudci.mmctrl1;
    tlbi_a0.wb_op     = 0;
    tlbi_a1.trans_op  = r.Q.splt_ds1.op.trans_op;
    tlbi_a1.flush_op  = r.Q.splt_ds1.op.flush_op;
    tlbi_a1.transdata = spltdtlbin;
    tlbi_a1.s2valid   = r.Q.splt_ds2.tlbactive;
    tlbi_a1.mmctrl1   = mmudci.mmctrl1;
    tlbi_a1.wb_op     = mmudci.wb_op;
  }else
  {
    tlbi_a0.trans_op  = r.Q.cmb_s1.op.trans_op;
    tlbi_a0.flush_op  = r.Q.cmb_s1.op.flush_op;
    tlbi_a0.transdata = cmbtlbin;
    tlbi_a0.s2valid   = r.Q.cmb_s2.tlbactive;
    tlbi_a0.mmctrl1   = mmudci.mmctrl1;
    tlbi_a0.wb_op     = 0;
  }
  for (int32 i=0; i<M_ENT_MAX; i++)
  {
    tlbi_a0.tlbcami.arr[i]     = mmutlbcam_in_type_none;
    tlbi_a1.tlbcami.arr[i]     = mmutlbcam_in_type_none;
  }
      
  mmudco.transdata = mmudco_transdata;
  mmuico.transdata = mmuico_transdata;
  mmudco.grant     = mmudco_grant;
  mmuico.grant     = mmuico_grant;
  mmuico.tlbmiss   = twi_a[0].tlbmiss;
  mmudco.mmctrl2   = r.Q.mmctrl2;
  mmudco.wbtransdata = wbtransdata;

  twi      = twiv;
  two_a[0] = twoi;
  two_a[1] = twod;
  mmctrl1  = mmudci.mmctrl1;
  
  c = v;

  r.CLK = clk;
  r.D = c;
#if 1
  if(iClkCnt>=41)
  bool st = true;
#endif
  
#if (M_TLB_TYPE == 1)
  //-- i/d tlb
  ctlb0 : mmutlb
    generic map ( tech, M_ENT_C, 0, tlb_rep, mmupgsz )
    port map (rst, clk, tlbi_a0, tlbo_a0, two_a(0), twi_a(0));
    mmudco.tlbmiss   <= twi_a(0).tlbmiss;

#elif (M_TLB_TYPE == 0)
  itlb0->Update(rst, clk, tlbi_a0, tlbo_a0, two_a[0], twi_a[0]);

  dtlb0->Update(rst, clk, tlbi_a1, tlbo_a1, two_a[1], twi_a[1]);
  
  mmudco.tlbmiss   = twi_a[1].tlbmiss;
#endif
  

  //-- table walk component
  tw0.Update(rst, clk, mmctrl1, twi, two, mcmmo, mcmmi);
}
