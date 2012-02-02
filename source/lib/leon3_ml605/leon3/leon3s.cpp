#include "lheaders.h"

//#define hindex AHB_MASTER_LEON3//0//   : integer               := 0;
//#define fabtech   CFG_MEMTECH//: integer range 0 to NTECH  := DEFFABTECH;
//#define memtech   CFG_FABTECH: integer range 0 to NTECH  := DEFMEMTECH;
//#define nwindows  CFG_NWIN//: integer range 2 to 32 := 8;
//#define dsu       CFG_DSU//: integer range 0 to 1  := 0;
//#define fpu       CFG_FPU//: integer range 0 to 31 := 0;
//#define v8        CFG_V8//: integer range 0 to 63 := 0;
//#define cp        CFG_COPROC_ENA//: integer range 0 to 1  := 0;
//#define mac       CFG_MAC//: integer range 0 to 1  := 0;
//#define pclow     CFG_PCLOW//: integer range 0 to 2  := 2;
//#define notag     CFG_NOTAG//: integer range 0 to 1  := 0;
//#define nwp       CFG_NWP//: integer range 0 to 4  := 0;
//#define icen      CFG_ICEN//: integer range 0 to 1  := 0;
//#define irepl     CFG_IREPL//: integer range 0 to 3  := 2;
//#define isets     CFG_ISETS//: integer range 1 to 4  := 1;
//#define ilinesize CFG_ILINE//: integer range 4 to 8  := 4;
//#define isetsize  CFG_ISETSZ//: integer range 1 to 256 := 1;
//#define isetlock  CFG_ILOCK//: integer range 0 to 1  := 0;
//#define dcen      CFG_DCEN//: integer range 0 to 1  := 0;
//#define drepl     CFG_DREPL//: integer range 0 to 3  := 2;
//#define dsets     CFG_DSETS//: integer range 1 to 4  := 1;
//#define dlinesize CFG_DLINE//: integer range 4 to 8  := 4;
//#define dsetsize  CFG_DSETSZ//: integer range 1 to 256 := 1;
//#define dsetlock  CFG_DLOCK//: integer range 0 to 1  := 0;
//#define dsnoop    CFG_DSNOOP//: integer range 0 to 6  := 0;
//#define ilram      CFG_ILRAMEN//: integer range 0 to 1 := 0;
//#define ilramsize  CFG_ILRAMSZ//: integer range 1 to 512 := 1;
//#define ilramstart CFG_ILRAMADDR//: integer range 0 to 255 := 16#8e#;
//#define dlram      CFG_DLRAMEN//: integer range 0 to 1 := 0;
//#define dlramsize  CFG_DLRAMSZ//: integer range 1 to 512 := 1;
//#define dlramstart CFG_DLRAMADDR//: integer range 0 to 255 := 16#8f#;
//#define mmuen     CFG_MMUEN//: integer range 0 to 1  := 0;
//#define itlbnum   CFG_ITLBNUM//: integer range 2 to 64 := 8;
//#define dtlbnum   CFG_DTLBNUM//: integer range 2 to 64 := 8;
//#define tlb_type  CFG_TLB_TYPE//: integer range 0 to 3  := 1;
//#define tlb_rep   CFG_TLB_REP//: integer range 0 to 1  := 0;
//#define lddel     CFG_LDDEL//: integer range 1 to 2  := 2;
//#define disas     CFG_DISAS//: integer range 0 to 2  := 0;
//#define tbuf      CFG_ITBSZ: integer range 0 to 64 := 0;
//#define pwd       CFG_PWD//: integer range 0 to 2  := 2;     -- power-down
//#define svt       CFG_SVT//: integer range 0 to 1  := 1;     -- single vector trapping
//#define rstaddr   CFG_RSTADDR//: integer               := 0;
//#define smp       (CFG_NCPU-1)//: integer range 0 to 15 := 0;     -- support SMP systems
//#define cached    CFG_DFIXED//: integer               := 0; -- cacheability table
//#define scantest  CFG_SCANTEST_ENA//: integer               := 0;
//#define mmupgsz   CFG_MMU_PAGESIZE//: integer range 0 to 5  := 0;
//#define bp        CFG_BP//: integer               := 1


//#define IRFBITS  (log2[NWINDOWS+1]+4)//: integer range 6 to 10 := log2(NWINDOWS+1) + 4;
//#define IREGNUM  (16*NWINDOWS+8)//: integer := NWINDOWS * 16 + 8;

//#define IRFWT     CFG_IRFWT//1//: integer := 1;--regfile_3p_write_through(memtech);
#define FPURFHARD 1//: integer := 1; --1-is_fpga(memtech);
#define fpuarch   (CFG_FPU%16)//: integer := fpu mod 16;
#define fpunet    (CFG_FPU/16)//: integer := fpu / 16;

//******************************************************************
leon3s::leon3s()
{
  pclTBufMem = new tbufmem(CFG_ITBSZ);
}

leon3s::~leon3s()
{
  free(pclTBufMem);
}

//******************************************************************
void leon3s::Update(SClock clk,//    : in  std_ulogic;
                    uint32 rstn,//   : in  std_ulogic;
                    ahb_mst_in_type &ahbi,//   : in  ahb_mst_in_type;
                    ahb_mst_out_type &ahbo,//   : out ahb_mst_out_type;
                    ahb_slv_in_type &ahbsi,//  : in  ahb_slv_in_type;
                    ahb_slv_out_vector &ahbso,//  : in  ahb_slv_out_vector;
                    l3_irq_in_type &irqi,//   : in  l3_irq_in_type;
                    l3_irq_out_type &irqo,//   : out l3_irq_out_type;
                    l3_debug_in_type &dbgi,//   : in  l3_debug_in_type;
                    l3_debug_out_type &dbgo)//   : out l3_debug_out_type)
{
  gnd = 0;
  clk_gnd.eClock_z = clk_gnd.eClock = SClock::CLK_NEGATIVE;
  vcc = 1;

  // leon3 processor core (iu, caches & mul/div) 
  clProc3.Update(clk, rst.Q, holdn, ahbi, ahbo, ahbsi, ahbso, rfi, rfo, crami, cramo, 
    tbi, tbo, fpi, fpo, cpi, cpo, irqi, irqo, dbgi, dbgo, clk_gnd, clk, vcc);
  
  // IU register file
  clRegfile3Port.Update(clk, BITS32(rfi.waddr,CFG_IRFBITS-1,0), rfi.wdata, rfi.wren, 
                        clk, BITS32(rfi.raddr1,CFG_IRFBITS-1,0), rfi.ren1, rfo.data1, 
                             BITS32(rfi.raddr2,CFG_IRFBITS-1,0), rfi.ren2, rfo.data2, rfi.diag);

  // cache memory
  clCacheMem.Update(clk, crami, cramo, clk);


  // instruction trace buffer memory
  if (CFG_ITBSZ != 0)
  {
    pclTBufMem->Update(clk, tbi, tbo);
  }

// FPU
#if (CFG_FPU==0)
  fpo = fpc_out_none;
#endif

#if ((fpuarch > 0) && (fpuarch < 8))
  fpu0: grfpwx
    generic map (fabtech, FPURFHARD*memtech, (fpuarch-1), pclow, dsu, disas, fpunet, hindex)
    port map (rst, clk, holdn, fpi, fpo);
#endif

#if (fpuarch == 15)
  fpu0 : mfpwx
    generic map (FPURFHARD*memtech, pclow, dsu, disas)
    port map (rst, clk, holdn, fpi, fpo);
#endif

#if ((fpuarch >= 8) && (fpuarch < 15))
   fpu0 : grlfpwx
     generic map (FPURFHARD*memtech, pclow, dsu, disas, (fpuarch-8), fpunet, hindex)
     port map (rst, clk, holdn, fpi, fpo);
#endif

  // Default Co-Proc drivers
  cpodb.data = 0;
  cpo.data   = 0;
  cpo.exc    = 0;
  cpo.cc     = 0;
  cpo.ccv    = 0;
  cpo.ldlock = 0;
  cpo.holdn  = 0;
  cpo.dbg    = cpodb;
 
  // 1-clock reset delay
  rst.CLK = clk;  
  rst.D   = rstn;
}
