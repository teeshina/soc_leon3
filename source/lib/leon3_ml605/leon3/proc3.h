#pragma once

class proc3
{
  friend class dbg;
  private:
    icache_in_type ici;// : icache_in_type;
    icache_out_type ico;// : icache_out_type;
    dcache_in_type dci;// : dcache_in_type;
    dcache_out_type dco;// : dcache_out_type;

    uint32 holdnx, pholdn;// : std_logic;
    mul32_in_type muli;//  : mul32_in_type;
    mul32_out_type mulo;//  : mul32_out_type;
    div32_in_type divi;//  : div32_in_type;
    div32_out_type divo;//  : div32_out_type;
    
    iu3 clIU3;
    mul32 clMul32;
    div32 clDiv32;
    mmu_cache clCacheMMU;

  public:
    void Update(SClock clk,//    : in  std_ulogic;
                uint32 rstn,//   : in  std_ulogic;
                uint32 &holdn,//  : out std_ulogic;
                ahb_mst_in_type &ahbi,//   : in  ahb_mst_in_type;
                ahb_mst_out_type &ahbo,//   : out ahb_mst_out_type;
                ahb_slv_in_type &ahbsi,//  : in  ahb_slv_in_type;
                ahb_slv_out_vector &ahbso,//  : in  ahb_slv_out_vector;
                iregfile_in_type &rfi,//    : out iregfile_in_type;
                iregfile_out_type &rfo,//    : in  iregfile_out_type;
                cram_in_type &crami,//  : out cram_in_type;
                cram_out_type &cramo,//  : in  cram_out_type;
                tracebuf_in_type &tbi,//    : out tracebuf_in_type;
                tracebuf_out_type &tbo,//    : in  tracebuf_out_type;
                fpc_in_type &fpi,//    : out fpc_in_type;
                fpc_out_type &fpo,//    : in  fpc_out_type;
                fpc_in_type &cpi,//    : out fpc_in_type;
                fpc_out_type &cpo,//    : in  fpc_out_type;
                l3_irq_in_type &irqi,//   : in  l3_irq_in_type;
                l3_irq_out_type &irqo,//   : out l3_irq_out_type;
                l3_debug_in_type &dbgi,//   : in  l3_debug_in_type;
                l3_debug_out_type &dbgo,//   : out l3_debug_out_type;
                SClock hclk,//
                SClock sclk,//   : in std_ulogic;
                uint32 hclken);//   : in std_ulogic
                    
    
    void ClkUpdate()
    {
      clIU3.ClkUpdate();
      clMul32.ClkUpdate();
      clDiv32.ClkUpdate();
      clCacheMMU.ClkUpdate();
    }
};

