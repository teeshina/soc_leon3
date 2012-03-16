#include "lheaders.h"

//****************************************************************************
void dbg::mmutlb_tb(SystemOnChipIO &io)
{
  mmutlb *ptst_mmutlb = topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->clMMU.itlb0;
  
  mmutlb_in_type *pin_tlbi  = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->clMMU.tlbi_a0;//  : in  mmutlb_in_type;
  mmutlb_out_type *pch_tlbo = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->clMMU.tlbo_a0;//  : out mmutlb_out_type;
  mmutw_out_type *pin_two   = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->clMMU.two_a[0];//   : in  mmutw_out_type;
  mmutw_in_type *pch_twi    = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->clMMU.twi_a[0];//   : out mmutw_in_type

#ifdef DBG_mmutlb
  if(io.inClk.eClock_z==SClock::CLK_POSEDGE)
  {
    //
    in_tlbi.flush_op = rand()&0x1;//   : std_logic;
    in_tlbi.wb_op = rand()&0x1;//   : std_logic;
    in_tlbi.trans_op = rand()&0x1;//   : std_logic;
    in_tlbi.transdata.data = (rand()<<17)|rand();//             : std_logic_vector(31 downto 0);
    in_tlbi.transdata.su = rand()&0x1;//            : std_logic;
    in_tlbi.transdata.read = rand()&0x1;//            : std_logic;
    in_tlbi.transdata.isid = (mmu_idcache)(rand()&0x1);//             : mmu_idcache;
    in_tlbi.transdata.wb_data = (rand()<<17) |rand();//          : std_logic_vector(31 downto 0);
    in_tlbi.s2valid = rand()&0x1;//   : std_logic;
    in_tlbi.mmctrl1.e = rand()&0x1;//    : std_logic;                -- enable
    in_tlbi.mmctrl1.nf = rand()&0x1;//    : std_logic;                -- no fault
    in_tlbi.mmctrl1.pso = rand()&0x1;//    : std_logic;                -- partial store order
    in_tlbi.mmctrl1.pagesize = rand()&0x3;// std_logic_vector(1 downto 0);-- page size
    in_tlbi.mmctrl1.ctx = rand()&MSK32(M_CTX_SZ-1,0);//     : std_logic_vector(M_CTX_SZ-1 downto 0);-- context nr
    in_tlbi.mmctrl1.ctxp = ((rand()<<15)|rand())&MSK32(MMCTRL_CTXP_SZ-1,0);//    : std_logic_vector(MMCTRL_CTXP_SZ-1 downto 0);  -- context table pointer
    in_tlbi.mmctrl1.tlbdis = rand()&0x1;// : std_logic;                            -- tlb disabled
    in_tlbi.mmctrl1.bar = rand()&0x3;// : std_logic_vector(1 downto 0);         -- preplace barrier
    for (int32 i=0; i<8; i++)
    {
      in_tlbi.tlbcami.arr[i].mmctrl.e = rand()&0x1;//    : std_logic;                -- enable
      in_tlbi.tlbcami.arr[i].mmctrl.nf = rand()&0x1;//    : std_logic;                -- no fault
      in_tlbi.tlbcami.arr[i].mmctrl.pso = rand()&0x1;//    : std_logic;                -- partial store order
      in_tlbi.tlbcami.arr[i].mmctrl.pagesize = rand()&0x3;// std_logic_vector(1 downto 0);-- page size
      in_tlbi.tlbcami.arr[i].mmctrl.ctx = rand()&MSK32(M_CTX_SZ-1,0);//     : std_logic_vector(M_CTX_SZ-1 downto 0);-- context nr
      in_tlbi.tlbcami.arr[i].mmctrl.ctxp = ((rand()<<15)|rand())&MSK32(MMCTRL_CTXP_SZ-1,0);//    : std_logic_vector(MMCTRL_CTXP_SZ-1 downto 0);  -- context table pointer
      in_tlbi.tlbcami.arr[i].mmctrl.tlbdis = rand()&0x1;// : std_logic;                            -- tlb disabled
      in_tlbi.tlbcami.arr[i].mmctrl.bar = rand()&0x3;// : std_logic_vector(1 downto 0);         -- preplace barrier
      in_tlbi.tlbcami.arr[i].tagin.TYP = rand()&0x7;//   : std_logic_vector(2 downto 0);        -- f/(p) type
      in_tlbi.tlbcami.arr[i].tagin.I1 = rand()&0xFF;//   : std_logic_vector(7 downto 0);        -- vaddr
      in_tlbi.tlbcami.arr[i].tagin.I2 = rand()&0x3F;//   : std_logic_vector(5 downto 0);
      in_tlbi.tlbcami.arr[i].tagin.I3 = rand()&0x3F;//   : std_logic_vector(5 downto 0);
      in_tlbi.tlbcami.arr[i].tagin.CTX = rand()&MSK32(M_CTX_SZ-1,0);//    : std_logic_vector(M_CTX_SZ-1 downto 0);  -- ctx number
      in_tlbi.tlbcami.arr[i].tagin.M = rand()&0x1;//     : std_logic;

      in_tlbi.tlbcami.arr[i].tagwrite.ET = rand()&0x3;//    : std_logic_vector(1 downto 0);              -- et field 
      in_tlbi.tlbcami.arr[i].tagwrite.ACC = rand()&0x7;//    : std_logic_vector(2 downto 0);              -- on flush/probe this will become FPTY
      in_tlbi.tlbcami.arr[i].tagwrite.M = rand()&0x1;//    : std_logic;                                 -- modified
      in_tlbi.tlbcami.arr[i].tagwrite.R = rand()&0x1;//    : std_logic;                                 -- referenced
      in_tlbi.tlbcami.arr[i].tagwrite.SU = rand()&0x1;//    : std_logic;                                 -- equal ACC >= 6
      in_tlbi.tlbcami.arr[i].tagwrite.VALID = rand()&0x1;// : std_logic;                                  
      in_tlbi.tlbcami.arr[i].tagwrite.LVL = rand()&0x3;//   : std_logic_vector(1 downto 0);              -- level in pth
      in_tlbi.tlbcami.arr[i].tagwrite.I1 = rand()&0xFF;//    : std_logic_vector(7 downto 0);              -- vaddr
      in_tlbi.tlbcami.arr[i].tagwrite.I2 = rand()&0x3F;//    : std_logic_vector(5 downto 0);               
      in_tlbi.tlbcami.arr[i].tagwrite.I3 = rand()&0x3F;//    : std_logic_vector(5 downto 0);               
      in_tlbi.tlbcami.arr[i].tagwrite.CTX = rand()&MSK32(M_CTX_SZ,0);//    : std_logic_vector(M_CTX_SZ-1 downto 0);     -- ctx number
      in_tlbi.tlbcami.arr[i].tagwrite.PPN = ((rand()<<15)|rand())&MSK32(PTE_PPN_S-1,0);//     : std_logic_vector(PTE_PPN_S-1 downto 0);    -- physical page number
      in_tlbi.tlbcami.arr[i].tagwrite.C = rand()&0x1;//     : std_logic;                                 -- cachable
      in_tlbi.tlbcami.arr[i].trans_op = rand()&0x1;//std_logic;
      in_tlbi.tlbcami.arr[i].flush_op = rand()&0x1;//std_logic;
      in_tlbi.tlbcami.arr[i].write_op = rand()&0x1;//std_logic;
      in_tlbi.tlbcami.arr[i].wb_op = rand()&0x1;//std_logic;
      in_tlbi.tlbcami.arr[i].mmuen = 1;//rand()&0x1;//std_logic;
      in_tlbi.tlbcami.arr[i].mset = rand()&0x1;//std_logic;
    }
    //
    in_two.finish = rand()&0x1;//     : std_logic;
    in_two.data = (rand()<<17)|rand();//             : std_logic_vector(31 downto 0);
    in_two.addr = (rand()<<17)|rand();//             : std_logic_vector(31 downto 0);
    in_two.lvl = rand()&0x3;//     : std_logic_vector(1 downto 0);
    in_two.fault_mexc = rand()&0x1;//     : std_logic;
    in_two.fault_trans = rand()&0x1;//     : std_logic;
    in_two.fault_inv = rand()&0x1;//     : std_logic;
    in_two.fault_lvl = rand()&0x3;//     : std_logic_vector(1 downto 0);
  }

  // 
  tst_mmutlb.Update(  io.inNRst,
                      io.inClk,
                      in_tlbi,//  : in  mmutlb_in_type;
                      ch_tlbo,//  : out mmutlb_out_type;
                      in_two,//   : in  mmutw_out_type;
                      ch_twi);//   : out mmutw_in_type
  
  ptst_mmutlb = &tst_mmutlb;
  pin_tlbi = &in_tlbi;
  pch_tlbo = &ch_tlbo;
  pin_two  = &in_two;
  pch_twi  = &ch_twi;
#endif

  // output file writting:
  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';

    ResetPutStr();
  
    // inputs:
    pStr = PutToStr(pStr, io.inNRst, 1, "inNRst");
    //
    pStr = PutToStr(pStr, pin_tlbi->flush_op,1,"in_tlbi.flush_op");//   : std_logic;
    pStr = PutToStr(pStr, pin_tlbi->wb_op,1,"in_tlbi.wb_op");//   : std_logic;
    pStr = PutToStr(pStr, pin_tlbi->trans_op,1,"in_tlbi.trans_op");//   : std_logic;
    pStr = PutToStr(pStr, pin_tlbi->transdata.data,32,"in_tlbi.transdata.data");//             : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pin_tlbi->transdata.su,1,"in_tlbi.transdata.su");//            : std_logic;
    pStr = PutToStr(pStr, pin_tlbi->transdata.read,1,"in_tlbi.transdata.read");//            : std_logic;
    pStr = PutToStr(pStr, (uint32)pin_tlbi->transdata.isid,1,"in_tlbi_transdata_isid");//             : mmu_idcache;
    pStr = PutToStr(pStr, pin_tlbi->transdata.wb_data,32,"in_tlbi.transdata.wb_data");//          : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pin_tlbi->s2valid,1,"in_tlbi.s2valid");//   : std_logic;
    pStr = PutToStr(pStr, pin_tlbi->mmctrl1.e,1,"in_tlbi.mmctrl1.e");//    : std_logic;                -- enable
    pStr = PutToStr(pStr, pin_tlbi->mmctrl1.nf,1,"in_tlbi.mmctrl1.nf");//    : std_logic;                -- no fault
    pStr = PutToStr(pStr, pin_tlbi->mmctrl1.pso,1,"in_tlbi.mmctrl1.pso");//    : std_logic;                -- partial store order
    pStr = PutToStr(pStr, pin_tlbi->mmctrl1.pagesize,2,"in_tlbi.mmctrl1.pagesize");// std_logic_vector(1 downto 0);-- page size
    pStr = PutToStr(pStr, pin_tlbi->mmctrl1.ctx,M_CTX_SZ,"in_tlbi.mmctrl1.ctx");//     : std_logic_vector(M_CTX_SZ-1 downto 0);-- context nr
    pStr = PutToStr(pStr, pin_tlbi->mmctrl1.ctxp,MMCTRL_CTXP_SZ,"in_tlbi.mmctrl1.ctxp");//    : std_logic_vector(MMCTRL_CTXP_SZ-1 downto 0);  -- context table pointer
    pStr = PutToStr(pStr, pin_tlbi->mmctrl1.tlbdis,1,"in_tlbi.mmctrl1.tlbdis");// : std_logic;                            -- tlb disabled
    pStr = PutToStr(pStr, pin_tlbi->mmctrl1.bar,2,"in_tlbi.mmctrl1.bar");// : std_logic_vector(1 downto 0);         -- preplace barrier
    for (int32 i=0; i<8; i++)
    {
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).mmctrl.e",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].mmctrl.e,1,chArr);//    : std_logic;                -- enable
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).mmctrl.nf",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].mmctrl.nf,1,chArr);//    : std_logic;                -- no fault
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).mmctrl.pso",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].mmctrl.pso,1,chArr);//    : std_logic;                -- partial store order
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).mmctrl.pagesize",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].mmctrl.pagesize,2,chArr);// std_logic_vector(1 downto 0);-- page size
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).mmctrl.ctx",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].mmctrl.ctx,M_CTX_SZ,chArr);//     : std_logic_vector(M_CTX_SZ-1 downto 0);-- context nr
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).mmctrl.ctxp",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].mmctrl.ctxp,MMCTRL_CTXP_SZ,chArr);//    : std_logic_vector(MMCTRL_CTXP_SZ-1 downto 0);  -- context table pointer
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).mmctrl.tlbdis",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].mmctrl.tlbdis,1,chArr);// : std_logic;                            -- tlb disabled
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).mmctrl.bar",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].mmctrl.bar,2,chArr);// : std_logic_vector(1 downto 0);         -- preplace barrier
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).tagin.TYP",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].tagin.TYP,3,chArr);//   : std_logic_vector(2 downto 0);        -- f/(p) type
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).tagin.I1",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].tagin.I1,8,chArr);//   : std_logic_vector(7 downto 0);        -- vaddr
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).tagin.I2",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].tagin.I2,6,chArr);//   : std_logic_vector(5 downto 0);
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).tagin.I3",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].tagin.I3,6,chArr);//   : std_logic_vector(5 downto 0);
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).tagin.CTX",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].tagin.CTX,M_CTX_SZ,chArr);//    : std_logic_vector(M_CTX_SZ-1 downto 0);  -- ctx number
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).tagin.M",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].tagin.M,1,chArr);//     : std_logic;
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).tagwrite.ET",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].tagwrite.ET,2,chArr);//    : std_logic_vector(1 downto 0);              -- et field 
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).tagwrite.ACC",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].tagwrite.ACC,3,chArr);//    : std_logic_vector(2 downto 0);              -- on flush/probe this will become FPTY
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).tagwrite.M",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].tagwrite.M,1,chArr);//    : std_logic;                                 -- modified
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).tagwrite.R",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].tagwrite.R,1,chArr);//    : std_logic;                                 -- referenced
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).tagwrite.SU",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].tagwrite.SU,1,chArr);//    : std_logic;                                 -- equal ACC >= 6
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).tagwrite.VALID",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].tagwrite.VALID,1,chArr);// : std_logic;                                  
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).tagwrite.LVL",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].tagwrite.LVL,2,chArr);//   : std_logic_vector(1 downto 0);              -- level in pth
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).tagwrite.I1",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].tagwrite.I1,8,chArr);//    : std_logic_vector(7 downto 0);              -- vaddr
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).tagwrite.I2",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].tagwrite.I2,6,chArr);//    : std_logic_vector(5 downto 0);               
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).tagwrite.I3",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].tagwrite.I3,6,chArr);//    : std_logic_vector(5 downto 0);               
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).tagwrite.CTX",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].tagwrite.CTX,M_CTX_SZ,chArr);//    : std_logic_vector(M_CTX_SZ-1 downto 0);     -- ctx number
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).tagwrite.PPN",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].tagwrite.PPN,PTE_PPN_S,chArr);//     : std_logic_vector(PTE_PPN_S-1 downto 0);    -- physical page number
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).tagwrite.C",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].tagwrite.C,1,chArr);//     : std_logic;                                 -- cachable
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).trans_op",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].trans_op,1,chArr);//std_logic;
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).flush_op",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].flush_op,1,chArr);//std_logic;
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).write_op",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].write_op,1,chArr);//std_logic;
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).wb_op",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].wb_op,1,chArr);//std_logic;
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).mmuen",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].mmuen,1,chArr);//std_logic;
      sprintf_s(chArr,256,"in_tlbi.tlbcami(%i).mset",i);
      pStr = PutToStr(pStr, pin_tlbi->tlbcami.arr[i].mset,1,chArr);//std_logic;
    }
    //
    pStr = PutToStr(pStr, pin_two->finish,1,"in_two.finish");//     : std_logic;
    pStr = PutToStr(pStr, pin_two->data,32,"in_two.data");//             : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pin_two->addr,32,"in_two.addr");//             : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pin_two->lvl,2,"in_two.lvl");//     : std_logic_vector(1 downto 0);
    pStr = PutToStr(pStr, pin_two->fault_mexc,1,"in_two.fault_mexc");//     : std_logic;
    pStr = PutToStr(pStr, pin_two->fault_trans,1,"in_two.fault_trans");//     : std_logic;
    pStr = PutToStr(pStr, pin_two->fault_inv,1,"in_two.fault_inv");//     : std_logic;
    pStr = PutToStr(pStr, pin_two->fault_lvl,2,"in_two.fault_lvl");//     : std_logic_vector(1 downto 0);

    //Outputs:
    pStr = PutToStr(pStr, pch_twi->walk_op_ur,1,"ch_twi.walk_op_ur");//      : std_logic;
    pStr = PutToStr(pStr, pch_twi->areq_ur,1,"ch_twi.areq_ur");//      : std_logic;
    pStr = PutToStr(pStr, pch_twi->tlbmiss,1,"ch_twi.tlbmiss");//      : std_logic;
    pStr = PutToStr(pStr, pch_twi->data,32,"ch_twi.data");//             : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pch_twi->adata,32,"ch_twi.adata");//            : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pch_twi->aaddr,32,"ch_twi.aaddr");//            : std_logic_vector(31 downto 0);
    //
    pStr = PutToStr(pStr, pch_tlbo->transdata.finish,1,"ch_tlbo.transdata.finish");//          : std_logic;
    pStr = PutToStr(pStr, pch_tlbo->transdata.data,32,"ch_tlbo.transdata.data");//          : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pch_tlbo->transdata.cache,1,"ch_tlbo.transdata.cache");//          : std_logic;
    pStr = PutToStr(pStr, pch_tlbo->transdata.accexc,1,"ch_tlbo.transdata.accexc");//          : std_logic;
    pStr = PutToStr(pStr, pch_tlbo->fault.fault_pro,1,"ch_tlbo.fault.fault_pro");//: 1;// std_logic;
    pStr = PutToStr(pStr, pch_tlbo->fault.fault_pri,1,"ch_tlbo.fault.fault_pri");//: 1;//: std_logic;
    pStr = PutToStr(pStr, pch_tlbo->fault.fault_access,1,"ch_tlbo.fault.fault_access");//    : std_logic; 
    pStr = PutToStr(pStr, pch_tlbo->fault.fault_mexc,1,"ch_tlbo.fault.fault_mexc");//    : std_logic;
    pStr = PutToStr(pStr, pch_tlbo->fault.fault_trans,1,"ch_tlbo.fault.fault_trans");//    : std_logic;
    pStr = PutToStr(pStr, pch_tlbo->fault.fault_inv,1,"ch_tlbo.fault.fault_inv");//    : std_logic;
    pStr = PutToStr(pStr, pch_tlbo->fault.fault_lvl,2,"ch_tlbo.fault.fault_lvl");//    : std_logic_vector(1 downto 0);
    pStr = PutToStr(pStr, pch_tlbo->fault.fault_su,1,"ch_tlbo.fault.fault_su");//    : std_logic;
    pStr = PutToStr(pStr, pch_tlbo->fault.fault_read,1,"ch_tlbo.fault.fault_read");//    : std_logic;
    pStr = PutToStr(pStr, (uint32)pch_tlbo->fault.fault_isid,1,"ch_tlbo_fault_fault_isid");//       : mmu_idcache;
    pStr = PutToStr(pStr, pch_tlbo->fault.fault_addr,32,"ch_tlbo.fault.fault_addr");//       : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pch_tlbo->nexttrans,1,"ch_tlbo.nexttrans");//std_logic;
    pStr = PutToStr(pStr, pch_tlbo->s1finished,1,"ch_tlbo.s1finished");//std_logic;
    for (int32 i=0; i<8; i++)
    {
      sprintf_s(chArr,256,"ch_tlbo.tlbcamo(%i).pteout",i);
      pStr = PutToStr(pStr, pch_tlbo->tlbcamo.arr[i].pteout,32,chArr);//   : std_logic_vector(31 downto 0);
      sprintf_s(chArr,256,"ch_tlbo.tlbcamo(%i).LVL",i);
      pStr = PutToStr(pStr, pch_tlbo->tlbcamo.arr[i].LVL,2,chArr);//std_logic_vector(1 downto 0);    -- level in pth
      sprintf_s(chArr,256,"ch_tlbo.tlbcamo(%i).hit",i);
      pStr = PutToStr(pStr, pch_tlbo->tlbcamo.arr[i].hit,1,chArr);//std_logic;
      sprintf_s(chArr,256,"ch_tlbo.tlbcamo(%i).ctx",i);
      pStr = PutToStr(pStr, pch_tlbo->tlbcamo.arr[i].ctx,M_CTX_SZ,chArr);//      : std_logic_vector(M_CTX_SZ-1 downto 0);    -- for diagnostic access
      sprintf_s(chArr,256,"ch_tlbo.tlbcamo(%i).valid",i);
      pStr = PutToStr(pStr, pch_tlbo->tlbcamo.arr[i].valid,1,chArr);//std_logic;                                -- for diagnostic access
      sprintf_s(chArr,256,"ch_tlbo.tlbcamo(%i).vaddr",i);
      pStr = PutToStr(pStr, pch_tlbo->tlbcamo.arr[i].vaddr,32,chArr);//    : std_logic_vector(31 downto 0);            -- for diagnostic access
      sprintf_s(chArr,256,"ch_tlbo.tlbcamo(%i).NEEDSYNC",i);
      pStr = PutToStr(pStr, pch_tlbo->tlbcamo.arr[i].NEEDSYNC,1,chArr);//std_logic;
      sprintf_s(chArr,256,"ch_tlbo.tlbcamo(%i).WBNEEDSYNC",i);
      pStr = PutToStr(pStr, pch_tlbo->tlbcamo.arr[i].WBNEEDSYNC,1,chArr);//std_logic;
    }
    pStr = PutToStr(pStr, pch_tlbo->wbtransdata.finish,1,"ch_tlbo.wbtransdata.finish");//          : std_logic;
    pStr = PutToStr(pStr, pch_tlbo->wbtransdata.data,32,"ch_tlbo.wbtransdata.data");//          : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pch_tlbo->wbtransdata.cache,1,"ch_tlbo.wbtransdata.cache");//          : std_logic;
    pStr = PutToStr(pStr, pch_tlbo->wbtransdata.accexc,1,"ch_tlbo.wbtransdata.accexc");//          : std_logic;
    
    // Internal
    pStr = PutToStr(pStr, ptst_mmutlb->dr1_addr,3,"tst_dr1_addr");
    pStr = PutToStr(pStr, ptst_mmutlb->dr1_dataout,30,"tst_dr1_dataout");
    pStr = PutToStr(pStr, ptst_mmutlb->cam_hitaddr,M_ENT_MAX_LOG,"tst_cam_hitaddr");

  
    PrintIndexStr();

    *posBench[TB_mmutlb] << chStr << "\n";
  }

#ifdef DBG_mmutlb
  // Clock update:
  tst_mmutlb.ClkUpdate();
#endif
}


