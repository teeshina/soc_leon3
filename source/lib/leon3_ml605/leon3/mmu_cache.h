#pragma once

#include "stdtypes.h"
#include "dff.h"
#include "leon3_ml605\config.h"

#include "leon3_ml605\leon3\libiu.h"
#include "leon3_ml605\leon3\libcache.h"
#include "leon3_ml605\leon3\mmuiface.h"
#include "leon3_ml605\amba\amba.h"

#include "leon3_ml605\leon3\mmu_icache.h"
#include "leon3_ml605\leon3\mmu_dcache.h"
#include "leon3_ml605\leon3\mmu_acache.h"
#include "leon3_ml605\leon3\mmu.h"

class mmu_cache
{
  friend class dbg;
  private:
    icache_out_type icol;//  : icache_out_type;
    dcache_out_type dcol;//  : dcache_out_type;
    memory_ic_in_type mcii;// : memory_ic_in_type;
    memory_ic_out_type mcio;// : memory_ic_out_type;
    memory_dc_in_type mcdi;// : memory_dc_in_type;
    memory_dc_out_type mcdo;// : memory_dc_out_type;

    memory_mm_in_type mcmmi;//  : memory_mm_in_type;
    memory_mm_out_type mcmmo;//  : memory_mm_out_type;

    mmudc_in_type mmudci;// : mmudc_in_type;
    mmudc_out_type mmudco;// : mmudc_out_type;
    mmuic_in_type mmuici;// : mmuic_in_type;
    mmuic_out_type mmuico;// : mmuic_out_type;

    ahb_slv_in_type ahbsi2;// : ahb_slv_in_type;
    ahb_mst_in_type ahbi2;// : ahb_mst_in_type;
    ahb_mst_out_type ahbo2;// : ahb_mst_out_type;
    
    mmu_icache clICache;
    mmu_dcache clDCache;
    mmu_acache *pclACache;
    mmu clMMU;

  public:
    mmu_cache(uint32 mst_index=AHB_MASTER_LEON3)
    {
      pclACache = new mmu_acache(mst_index);
    }
    ~mmu_cache()
    {
      free(pclACache);
    }
    
    void Update(uint32 rst,//   : in  std_ulogic;
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
                uint32 hclken);// : in std_ulogic
              
    
    void ClkUpdate()
    {
      clICache.ClkUpdate();
      clDCache.ClkUpdate();
      pclACache->ClkUpdate();
      clMMU.ClkUpdate();
    }
};

