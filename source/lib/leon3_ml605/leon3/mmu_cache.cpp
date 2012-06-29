#include "mmu_cache.h"

//#define hindex    : integer              := 0;
//#define memtech   CFG_MEMTECH//: integer range 0 to NTECH := 0;
//#define dsu       : integer range 0 to 1 := 0;
//#define icen      : integer range 0 to 1 := 0;
//#define irepl     : integer range 0 to 3 := 0;
//#define isets     : integer range 1 to 4 := 1;
//#define ilinesize : integer range 4 to 8 := 4;
//#define isetsize  : integer range 1 to 256 := 1;
//#define isetlock  : integer range 0 to 1 := 0;
//#define dcen      : integer range 0 to 1 := 0;
//#define drepl     : integer range 0 to 3 := 0;
//#define dsets     : integer range 1 to 4 := 1;
//#define dlinesize : integer range 4 to 8 := 4;
//#define dsetsize  : integer range 1 to 256 := 1;
//#define dsetlock  : integer range 0 to 1 := 0;
//#define dsnoop    : integer range 0 to 6 := 0;
//#define ilram      : integer range 0 to 1 := 0;
//#define ilramsize  : integer range 1 to 512 := 1;        
//#define ilramstart : integer range 0 to 255 := 16#8e#;
//#define dlram      : integer range 0 to 1 := 0;
//#define dlramsize  : integer range 1 to 512 := 1;        
//#define dlramstart : integer range 0 to 255 := 16#8f#;
//#define itlbnum   : integer range 2 to 64 := 8;
//#define dtlbnum   : integer range 2 to 64 := 8;
//#define tlb_type  : integer range 0 to 3 := 1;
//#define tlb_rep   : integer range 0 to 1 := 0;
//#define cached    : integer := 0;
//#define clk2x     : integer := 0;
//#define scantest  : integer := 0;
//#define mmupgsz   : integer range 0 to 5  := 0;
//#define smp       : integer               := 0;
//#define mmuen     : integer range 0 to 1  := 0


//-- Description: Complete cache sub-system with controllers and rams

void mmu_cache::Update( uint32 rst,//   : in  std_ulogic;
                        SClock clk,//   : in  std_ulogic;
                        icache_in_type &ici,//   : in  icache_in_type;
                        icache_out_type &ico,//   : out icache_out_type;
                        dcache_in_type &dci,//   : in  dcache_in_type;
                        dcache_out_type &dco,//   : out dcache_out_type;
                        ahb_mst_in_type &ahbi,//  : in  ahb_mst_in_type;
                        ahb_mst_out_type &ahbo,//  : out ahb_mst_out_type;
                        ahb_slv_in_type &ahbsi,// : in  ahb_slv_in_type;
                        ahb_slv_out_vector &ahbso,//  : in  ahb_slv_out_vector;            
                        cram_in_type &crami,// : out cram_in_type;
                        cram_out_type &cramo,// : in  cram_out_type;
                        uint32 fpuholdn,// : in  std_ulogic;
                        SClock hclk,//
                        SClock sclk,// : in std_ulogic;
                        uint32 hclken// : in std_ulogic
                      )
{
 ahbsi2 = ahbsi;
 ahbi2  = ahbi;

//-- AMBA AHB interface     
  pclACache->Update(rst, sclk, mcii, mcio, mcdi, mcdo, mcmmi, mcmmo, ahbi2, ahbo2, ahbso, hclken);

//-- instruction cache controller
  clICache.Update(rst, clk, ici, icol, dci, dcol, mcii, mcio, 
       crami.icramin, cramo.icramo, fpuholdn, mmudci, mmuici, mmuico);
       

//-- data cache controller
  clDCache.Update(rst, clk, dci, dcol, icol, mcdi, mcdo, ahbsi2,
     crami.dcramin, cramo.dcramo, fpuholdn, mmudci, mmudco, sclk);


  
//-- MMU
#if (CFG_MMUEN==1)
  clMMU.Update(rst, clk, mmudci, mmudco, mmuici, mmuico, mcmmo, mcmmi);
#else
  mcmmi  = mci_zero;
  mmudco = mmudco_zero;
  mmuico = mmuico_zero;
#endif
   
  ico = icol;
  dco = dcol;


#if (CFG_CLK2X!=0)
   sync0 : clk2xsync generic map (hindex, clk2x)
     port map (rst, hclk, clk, ahbi, ahbi2, ahbo2, ahbo, ahbsi, ahbsi2, mcii, mcdi, mcdo, mcmmi.req, mcmmo.grant, hclken);
#else

   ahbo   = ahbo2;
#endif
}


