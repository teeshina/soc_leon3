#pragma once

#include "stdtypes.h"
#include "dff.h"

#include "leon3_ml605\leon3\mmuconfig.h"
#include "leon3_ml605\leon3\mmuiface.h"
#include "leon3_ml605\leon3\mmutlb.h"
#include "leon3_ml605\leon3\mmutw.h"


class mmu
{
  friend class dbg;
  private:
    struct mmu_op
    {
      uint32 trans_op:1;//  : std_logic;
      uint32 flush_op:1;//  : std_logic;
      uint32 diag_op:1;//   : std_logic;
    };
    
    struct mmu_cmbpctrl
    {
      mmu_idcache tlbowner;//     : mmu_idcache;
      uint32 tlbactive:1;//    : std_logic;
      mmu_op op;//           : mmu_op;
    };

    struct mmu_rtype
    {
      mmu_cmbpctrl cmb_s1;//          : mmu_cmbpctrl; 
      mmu_cmbpctrl cmb_s2;//          : mmu_cmbpctrl;
        
      mmu_cmbpctrl splt_is1;//          : mmu_cmbpctrl;
      mmu_cmbpctrl splt_is2;//          : mmu_cmbpctrl;
      mmu_cmbpctrl splt_ds1;//          : mmu_cmbpctrl;
      mmu_cmbpctrl splt_ds2;//          : mmu_cmbpctrl;
        
      uint32 twactive:1;//     : std_logic;        -- split tlb
      mmu_idcache twowner;//      : mmu_idcache;        -- split tlb

      uint32 flush:1;//         : std_logic;
      mmctrl_type2 mmctrl2;//       : mmctrl_type2;
    };

    TDFF<mmu_rtype> r;
    mmu_rtype c;//   : mmu_rtype;
    
    mmutlb_in_type tlbi_a0;// : mmutlb_in_type;
    mmutlb_in_type tlbi_a1;// : mmutlb_in_type;
    mmutlb_out_type tlbo_a0;// : mmutlb_out_type;
    mmutlb_out_type tlbo_a1;// : mmutlb_out_type;
    mmutw_in_type twi_a[2];// : mmutwi_a(1 downto 0);
    mmutw_out_type two_a[2];// : mmutwo_a(1 downto 0);
    
    mmutw_in_type twi;//     : mmutw_in_type;
    mmutw_out_type two;//     : mmutw_out_type;
    mmctrl_type1 mmctrl1;// : mmctrl_type1;

    mmutlb *itlb0;
    mmutlb *dtlb0;
    mmutw  tw0;


    mmuidc_data_in_type cmbtlbin;//     : mmuidc_data_in_type;
    mmutlb_out_type cmbtlbout;//    : mmutlb_out_type;
  
    mmuidc_data_in_type spltitlbin;//     : mmuidc_data_in_type;
    mmuidc_data_in_type spltdtlbin;//     : mmuidc_data_in_type;
    mmutlb_out_type spltitlbout;//    : mmutlb_out_type;
    mmutlb_out_type spltdtlbout;//    : mmutlb_out_type;
  
  
    mmuidc_data_out_type mmuico_transdata;// : mmuidc_data_out_type;
    mmuidc_data_out_type mmudco_transdata;// : mmuidc_data_out_type;
    uint32 mmuico_grant;// : std_logic;
    uint32 mmudco_grant;// : std_logic;
    mmu_rtype v;//            : mmu_rtype;
    mmutw_in_type twiv;//         : mmutw_in_type;
    mmutw_out_type twod, twoi;//   : mmutw_out_type;
    mmutlbfault_out_type fault;//       : mmutlbfault_out_type;

    mmuidc_data_out_type wbtransdata;// : mmuidc_data_out_type;
  
    mmctrl_fs_type fs;// : mmctrl_fs_type;
    uint32 fa;// : std_logic_vector(VA_I_SZ-1 downto 0);


  public:
    mmu();
    ~mmu();
  
    void Update(  uint32 rst,//  : in std_logic;
                  SClock clk,//  : in std_logic;
                  mmudc_in_type &mmudci,// : in  mmudc_in_type;
                  mmudc_out_type &mmudco,// : out mmudc_out_type;
                  mmuic_in_type &mmuici,// : in  mmuic_in_type;
                  mmuic_out_type &mmuico,// : out mmuic_out_type;
                  memory_mm_out_type &mcmmo,//  : in  memory_mm_out_type;
                  memory_mm_in_type &mcmmi);//  : out memory_mm_in_type
                  
    void ClkUpdate()
    {
      itlb0->ClkUpdate();
      dtlb0->ClkUpdate();
      tw0.ClkUpdate();
      r.ClkUpdate();
    }
};
