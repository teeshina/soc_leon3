#include "lheaders.h"
extern leon3mp  topLeon3mp;

extern void ResetPutStr();
extern void PrintIndexStr();

#ifdef DBG_mmutlbcam
//****************************************************************************
void dbg::mmutlbcam_tb(SystemOnChipIO &io)
{
  if(io.inClk.eClock_z==SClock::CLK_POSEDGE)
  {
    uint32 tmp;

    tmp = rand()<<16; tmp |= rand();
    tlbcami.mmctrl.e         = BITS32(tmp,0,0);//    : std_logic;                -- enable
    tlbcami.mmctrl.nf        = BITS32(tmp,1,1);//    : std_logic;                -- no fault
    tlbcami.mmctrl.pso       = BITS32(tmp,2,2);//    : std_logic;                -- partial store order
    tlbcami.mmctrl.pagesize  = BITS32(tmp,4,3);// std_logic_vector(1 downto 0);-- page size
    tlbcami.mmctrl.ctx       = BITS32(tmp,M_CTX_SZ+4,5);// std_logic_vector(M_CTX_SZ-1 downto 0);-- context nr
    tlbcami.mmctrl.tlbdis    = BITS32(tmp,13,13);// : std_logic;                            -- tlb disabled
    tlbcami.mmctrl.bar       = BITS32(tmp,15,14);// : std_logic_vector(1 downto 0);         -- preplace barrier
    tmp = rand()<<16; tmp |= rand();
    tlbcami.mmctrl.ctxp      = BITS32(tmp,MMCTRL_CTXP_SZ-1,0);//    : std_logic_vector(MMCTRL_CTXP_SZ-1 downto 0);  -- context table pointer
  
    tmp = rand()<<16; tmp |= rand();
    tlbcami.tagin.TYP  = BITS32(tmp,2,0);//   : std_logic_vector(2 downto 0);        -- f/(p) type
    tlbcami.tagin.I1   = BITS32(tmp,10,3);//   : std_logic_vector(7 downto 0);        -- vaddr
    tlbcami.tagin.I2   = BITS32(tmp,16,11);//   : std_logic_vector(5 downto 0);
    tlbcami.tagin.I3   = BITS32(tmp,22,17);//   : std_logic_vector(5 downto 0);
    tlbcami.tagin.CTX  = BITS32(tmp,30,23);//    : std_logic_vector(M_CTX_SZ-1 downto 0);  -- ctx number
    tlbcami.tagin.M    = BITS32(tmp,31,31);//     : std_logic;

    tmp = rand()<<16; tmp |= rand();
    tlbcami.tagwrite.ET    = BITS32(tmp,1,0);//    : std_logic_vector(1 downto 0);              -- et field 
    tlbcami.tagwrite.ACC   = BITS32(tmp,4,2);//    : std_logic_vector(2 downto 0);              -- on flush/probe this will become FPTY
    tlbcami.tagwrite.M     = BITS32(tmp,5,5);//    : std_logic;                                 -- modified
    tlbcami.tagwrite.R     = BITS32(tmp,6,6);//    : std_logic;                                 -- referenced
    tlbcami.tagwrite.SU    = BITS32(tmp,7,7);//    : std_logic;                                 -- equal ACC >= 6
    tlbcami.tagwrite.VALID = BITS32(tmp,8,8);// : std_logic;                                  
    tlbcami.tagwrite.LVL   = BITS32(tmp,10,9);//   : std_logic_vector(1 downto 0);              -- level in pth
    tlbcami.tagwrite.I1    = BITS32(tmp,18,11);//    : std_logic_vector(7 downto 0);              -- vaddr
    tlbcami.tagwrite.I2    = BITS32(tmp,24,19);//    : std_logic_vector(5 downto 0);               
    tlbcami.tagwrite.I3    = BITS32(tmp,30,25);//    : std_logic_vector(5 downto 0);
    tmp = rand()<<16; tmp |= rand();
    tlbcami.tagwrite.CTX   = BITS32(tmp,M_CTX_SZ-1,0);//    : std_logic_vector(M_CTX_SZ-1 downto 0);     -- ctx number
    tlbcami.tagwrite.PPN   = BITS32(tmp,31,8);//     : std_logic_vector(PTE_PPN_S-1 downto 0);    -- physical page number
    tmp = rand()<<16; tmp |= rand();
    tlbcami.tagwrite.C     = BITS32(tmp,0,0);//     : std_logic;                                 -- cachable


    tmp = rand();
    tlbcami.trans_op = BITS32(tmp,0,0);
    tlbcami.flush_op = BITS32(tmp,1,1);
    tlbcami.write_op = BITS32(tmp,2,2);
    tlbcami.wb_op    = BITS32(tmp,3,3);
    tlbcami.mmuen    = 1;
    tlbcami.mset     = BITS32(tmp,4,4);
  }

  tst_mmutlbcam.Update(io.inNRst,
                      io.inClk,
                      tlbcami,
                      tlbcamo);


  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';

    ResetPutStr();
  
    pStr = PutToStr(pStr, io.inNRst, 1, "inNRst");
    pStr = PutToStr(pStr, tlbcami.mmctrl.e, 1, "tlbcami.mmctrl.e");
    pStr = PutToStr(pStr, tlbcami.mmctrl.nf, 1, "tlbcami.mmctrl.nf");
    pStr = PutToStr(pStr, tlbcami.mmctrl.pso, 1, "tlbcami.mmctrl.pso");
    pStr = PutToStr(pStr, tlbcami.mmctrl.pagesize, 2, "tlbcami.mmctrl.pagesize");
    pStr = PutToStr(pStr, tlbcami.mmctrl.ctx, M_CTX_SZ, "tlbcami.mmctrl.ctx");
    pStr = PutToStr(pStr, tlbcami.mmctrl.tlbdis, 1, "tlbcami.mmctrl.tlbdis");
    pStr = PutToStr(pStr, tlbcami.mmctrl.bar, 2, "tlbcami.mmctrl.bar");
    pStr = PutToStr(pStr, tlbcami.mmctrl.ctxp,MMCTRL_CTXP_SZ, "tlbcami.mmctrl.ctxp");

    pStr = PutToStr(pStr, tlbcami.tagin.TYP, 3, "tlbcami.tagin.TYP");
    pStr = PutToStr(pStr, tlbcami.tagin.I1, 8, "tlbcami.tagin.I1");
    pStr = PutToStr(pStr, tlbcami.tagin.I2, 6, "tlbcami.tagin.I2");
    pStr = PutToStr(pStr, tlbcami.tagin.I3, 6, "tlbcami.tagin.I3");
    pStr = PutToStr(pStr, tlbcami.tagin.CTX, M_CTX_SZ, "tlbcami.tagin.CTX");
    pStr = PutToStr(pStr, tlbcami.tagin.M, 1, "tlbcami.tagin.M");

    pStr = PutToStr(pStr, tlbcami.tagwrite.ET, 2, "tlbcami.tagwrite.ET");
    pStr = PutToStr(pStr, tlbcami.tagwrite.ACC, 3, "tlbcami.tagwrite.ACC");
    pStr = PutToStr(pStr, tlbcami.tagwrite.M, 1, "tlbcami.tagwrite.M");
    pStr = PutToStr(pStr, tlbcami.tagwrite.R, 1, "tlbcami.tagwrite.R");
    pStr = PutToStr(pStr, tlbcami.tagwrite.SU, 1, "tlbcami.tagwrite.SU");
    pStr = PutToStr(pStr, tlbcami.tagwrite.VALID, 1, "tlbcami.tagwrite.VALID");
    pStr = PutToStr(pStr, tlbcami.tagwrite.LVL, 2, "tlbcami.tagwrite.LVL");
    pStr = PutToStr(pStr, tlbcami.tagwrite.I1, 8, "tlbcami.tagwrite.I1");
    pStr = PutToStr(pStr, tlbcami.tagwrite.I2, 6, "tlbcami.tagwrite.I2");
    pStr = PutToStr(pStr, tlbcami.tagwrite.I3, 6, "tlbcami.tagwrite.I3");
    pStr = PutToStr(pStr, tlbcami.tagwrite.CTX, M_CTX_SZ, "tlbcami.tagwrite.CTX");
    pStr = PutToStr(pStr, tlbcami.tagwrite.PPN, PTE_PPN_S, "tlbcami.tagwrite.PPN");
    pStr = PutToStr(pStr, tlbcami.tagwrite.C, 1, "tlbcami.tagwrite.C");

    pStr = PutToStr(pStr, tlbcami.trans_op, 1, "tlbcami.trans_op");
    pStr = PutToStr(pStr, tlbcami.flush_op, 1, "tlbcami.flush_op");
    pStr = PutToStr(pStr, tlbcami.write_op, 1, "tlbcami.write_op");
    pStr = PutToStr(pStr, tlbcami.wb_op, 1, "tlbcami.wb_op");
    pStr = PutToStr(pStr, tlbcami.mmuen, 1, "tlbcami.mmuen");
    pStr = PutToStr(pStr, tlbcami.mset, 1, "tlbcami.mset");

    // Output:
    pStr = PutToStr(pStr, tlbcamo.pteout, 32, "ch_tlbcamo.pteout");
    pStr = PutToStr(pStr, tlbcamo.LVL, 2, "ch_tlbcamo.LVL");
    pStr = PutToStr(pStr, tlbcamo.hit, 1, "ch_tlbcamo.hit");
    pStr = PutToStr(pStr, tlbcamo.ctx, M_CTX_SZ, "ch_tlbcamo.ctx");
    pStr = PutToStr(pStr, tlbcamo.valid, 1, "ch_tlbcamo.valid");
    pStr = PutToStr(pStr, tlbcamo.vaddr, 32, "ch_tlbcamo.vaddr");
    pStr = PutToStr(pStr, tlbcamo.NEEDSYNC, 1, "ch_tlbcamo.NEEDSYNC");
    pStr = PutToStr(pStr, tlbcamo.WBNEEDSYNC, 1, "ch_tlbcamo.WBNEEDSYNC");

    // internal registers:
    pStr = PutToStr(pStr, tst_mmutlbcam.blvl_decode, 4, "tmp_blvl_decode");
    pStr = PutToStr(pStr, tst_mmutlbcam.pagesize, 2, "tmp_pagesize");
    pStr = PutToStr(pStr, tst_mmutlbcam.h_c, 1, "tmp_h_c");
    pStr = PutToStr(pStr, tst_mmutlbcam.hm, 1, "tmp_hm");
    pStr = PutToStr(pStr, tst_mmutlbcam.vaddr_r,32,"tmp_vaddr_r");
    pStr = PutToStr(pStr, tst_mmutlbcam.vaddr_i,32,"tmp_vaddr_i");
    pStr = PutToStr(pStr, tst_mmutlbcam.h_i1,1,"tmp_h_i1");
    pStr = PutToStr(pStr, tst_mmutlbcam.h_i2,1,"tmp_h_i2");
    pStr = PutToStr(pStr, tst_mmutlbcam.h_i3,1,"tmp_h_i3");
    pStr = PutToStr(pStr, tst_mmutlbcam.h_su_cnt,1,"tmp_h_su_cnt");
    pStr = PutToStr(pStr, tst_mmutlbcam.hf,1,"tmp_hf");

    PrintIndexStr();

    *posBench[TB_mmutlbcam] << chStr << "\n";
  }

  tst_mmutlbcam.ClkUpdate();
}
#endif
