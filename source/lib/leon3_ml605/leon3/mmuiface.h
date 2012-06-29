#pragma once

#include <memory>

#include "stdtypes.h"
#include "leon3_ml605\leon3\mmuconfig.h"

struct mmutlbcam_in_type
{
  mmctrl_type1 mmctrl;//   : mmctrl_type1;
  tlbcam_tfp   tagin;//    : tlbcam_tfp;
  tlbcam_reg   tagwrite;// : tlbcam_reg;
  uint32 trans_op : 1;//std_logic;
  uint32 flush_op : 1;//std_logic;
  uint32 write_op : 1;//std_logic;
  uint32 wb_op    : 1;//std_logic;
  uint32 mmuen    : 1;//std_logic;
  uint32 mset     : 1;//std_logic;
};
struct mmutlbcami_a
{
  mmutlbcam_in_type arr[M_ENT_MAX];//is array (natural range <>) of mmutlbcam_in_type;
};

const mmutlbcam_in_type mmutlbcam_in_type_none = {mmctrl_type1_none, tlbcam_tfp_none,
  tlbcam_reg_none, 0, 0, 0, 0, 0, 0};
  
struct mmutlbcam_out_type
{
  uint32 pteout;//   : std_logic_vector(31 downto 0);
  uint32 LVL        : 2;//std_logic_vector(1 downto 0);    -- level in pth
  uint32 hit        : 1;//std_logic;
  uint32 ctx;//      : std_logic_vector(M_CTX_SZ-1 downto 0);    -- for diagnostic access
  uint32 valid      : 1;//std_logic;                                -- for diagnostic access
  uint32 vaddr;//    : std_logic_vector(31 downto 0);            -- for diagnostic access
  uint32 NEEDSYNC   : 1;//std_logic;
  uint32 WBNEEDSYNC : 1;//std_logic;
};
struct mmutlbcamo_a
{
  mmutlbcam_out_type arr[M_ENT_MAX];//is array (natural range <>) of mmutlbcam_out_type;
};

const mmutlbcam_out_type mmutlbcam_out_none = {0, 0, 0, 0, 0, 0, 0, 0};

//-- mmu i/o

struct mmuidc_data_in_type
{
  uint32 data;//             : std_logic_vector(31 downto 0);
  uint32 su   :1;//            : std_logic;
  uint32 read :1;//            : std_logic;
  mmu_idcache isid;//             : mmu_idcache;
  uint32 wb_data;//          : std_logic_vector(31 downto 0);
};

struct mmuidc_data_out_type
{
  uint32 finish :1;//          : std_logic;
  uint32 data   ;//          : std_logic_vector(31 downto 0);
  uint32 cache  :1;//          : std_logic;
  uint32 accexc :1;//          : std_logic;
};

const mmuidc_data_out_type mmuidco_zero = {0, 0, 0, 0};

struct mmudc_in_type
{
  uint32 trans_op         : 1;//std_logic; 
  mmuidc_data_in_type transdata;//        : mmuidc_data_in_type;
  
  //-- dcache extra signals
  uint32 flush_op         : 1;//std_logic;
  uint32 diag_op          : 1;//std_logic;
  uint32 wb_op            : 1;//std_logic;

  uint32 fsread           : 1;//std_logic;
  mmctrl_type1 mmctrl1    ;//      : mmctrl_type1;
};

struct mmudc_out_type
{
  uint32 grant   :1;//         : std_logic;
  mmuidc_data_out_type transdata;//        : mmuidc_data_out_type;
  //-- dcache extra signals
  mmctrl_type2 mmctrl2;//          : mmctrl_type2;
  //-- writebuffer out
  mmuidc_data_out_type wbtransdata;//      : mmuidc_data_out_type;
  uint32 tlbmiss :1;//         : std_logic;
};

struct mmuic_in_type
{
  uint32 trans_op :1;//        : std_logic; 
  mmuidc_data_in_type transdata;//        : mmuidc_data_in_type;
};

struct mmuic_out_type
{
  uint32 grant :1;//           : std_logic;
  mmuidc_data_out_type transdata;//        : mmuidc_data_out_type;
  uint32 tlbmiss :1;//         : std_logic;
};

const mmudc_out_type mmudco_zero = {0, mmuidco_zero,
  mmctrl2_zero, mmuidco_zero, 0};

const mmuic_out_type mmuico_zero = {0, mmuidco_zero, 0};

//--#lrue i/o
struct mmulrue_in_type
{
  uint32 touch     :1;//   : std_logic;
  uint32 pos;//   : std_logic_vector(M_ENT_MAX_LOG-1 downto 0);
  uint32 clear     :1;//   : std_logic;
  uint32 flush     :1;//   : std_logic;
  
  uint32 left;//   : std_logic_vector(M_ENT_MAX_LOG-1 downto 0);
  uint32 fromleft  :1;//   : std_logic;
  uint32 right;//   : std_logic_vector(M_ENT_MAX_LOG-1 downto 0);
  uint32 fromright :1;//    : std_logic;
};
struct mmulruei_a
{
  mmulrue_in_type *arr;// is array (natural range <>) of mmulrue_in_type;
  mmulruei_a(uint32 size){arr=(mmulrue_in_type*)malloc(size*sizeof(mmulrue_in_type));}
  ~mmulruei_a(){free(arr);}
};

struct mmulrue_out_type
{
  uint32 pos;//          : std_logic_vector(M_ENT_MAX_LOG-1 downto 0);
  uint32 movetop :1;//      : std_logic;
};
const mmulrue_out_type mmulrue_out_none = {0, 0};
struct mmulrueo_a
{
  mmulrue_out_type *arr;// is array (natural range <>) of mmulrue_out_type;
  mmulrueo_a(uint32 size){arr=(mmulrue_out_type*)malloc(size*sizeof(mmulrue_out_type));}
  ~mmulrueo_a(){free(arr);}
};

//--#lru i/o
struct mmulru_in_type
{
  uint32 touch     : 1;//std_logic;
  uint32 touchmin  : 1;//std_logic;
  uint32 flush     : 1;//std_logic;
  uint32 pos;//       : std_logic_vector(M_ENT_MAX_LOG-1 downto 0);
  mmctrl_type1 mmctrl1;//   : mmctrl_type1;
};

struct mmulru_out_type
{
  uint32 pos;//      : std_logic_vector(M_ENT_MAX_LOG-1 downto 0);
};

//--#mmu: tw i/o
struct memory_mm_in_type
{
  uint32 address;//          : std_logic_vector(31 downto 0); 
  uint32 data;//             : std_logic_vector(31 downto 0);
  uint32 size   :2;//          : std_logic_vector(1 downto 0);
  uint32 burst  :1;//          : std_logic;
  uint32 read   :1;//          : std_logic;
  uint32 req    :1;//          : std_logic;
  uint32 lock   :1;//          : std_logic;
};

const memory_mm_in_type mci_zero = {0, 0, 0, 0, 0, 0, 0};

struct memory_mm_out_type
{
  uint32 data;//             : std_logic_vector(31 downto 0); -- memory data
  uint32 ready :1;//           : std_logic;         -- cycle ready
  uint32 grant :1;//           : std_logic;         -- 
  uint32 retry :1;//           : std_logic;         -- 
  uint32 mexc  :1;//           : std_logic;         -- memory exception
  uint32 werr  :1;//           : std_logic;         -- memory write error
  uint32 cache :1;//           : std_logic;               -- cacheable data
};

struct mmutw_in_type
{
  uint32 walk_op_ur :1;//      : std_logic;
  uint32 areq_ur    :1;//      : std_logic;
  uint32 tlbmiss    :1;//      : std_logic;
  uint32 data;//             : std_logic_vector(31 downto 0);
  uint32 adata;//            : std_logic_vector(31 downto 0);
  uint32 aaddr;//            : std_logic_vector(31 downto 0);
};
/*struct mmutwi_a
{
  mmutw_in_type *arr;//is array (natural range <>) of mmutw_in_type;
  mmutwi_a(uint32 num){arr = (mmutw_in_type*)malloc(num*sizeof(mmutw_in_type)); }
  ~mmutwi_a() {free(arr); }
};*/

struct mmutw_out_type
{
  uint32 finish      :1;//     : std_logic;
  uint32 data;//             : std_logic_vector(31 downto 0);
  uint32 addr;//             : std_logic_vector(31 downto 0);
  uint32 lvl         :2;//     : std_logic_vector(1 downto 0);
  uint32 fault_mexc  :1;//     : std_logic;
  uint32 fault_trans :1;//     : std_logic;
  uint32 fault_inv   :1;//     : std_logic;
  uint32 fault_lvl   :2;//     : std_logic_vector(1 downto 0);
};
/*struct mmutwo_a
{
  mmutw_out_type *arr;//is array (natural range <>) of mmutw_out_type;
  mmutwo_a(uint32 num){arr = (mmutw_out_type*)malloc(num*sizeof(mmutw_out_type)); }
  ~mmutwo_a() {free(arr); }
};*/

//-- mmu tlb i/o

struct mmutlb_in_type
{
  uint32 flush_op :1;//   : std_logic;
  uint32 wb_op    :1;//   : std_logic;
  
  uint32 trans_op :1;//   : std_logic;
  mmuidc_data_in_type transdata;//   : mmuidc_data_in_type;
  uint32 s2valid  :1;//   : std_logic;
  
  mmctrl_type1 mmctrl1;//     : mmctrl_type1;
  
  //-- fast writebuffer signals
  mmutlbcami_a tlbcami;//          : mmutlbcami_a (M_ENT_MAX-1 downto 0);
};
struct mmutlbi_a
{
  mmutlb_in_type *arr;// is array (natural range <>) of mmutlb_in_type;
};

struct mmutlbfault_out_type
{
  uint32 fault_pro   ;//: 1;// std_logic;
  uint32 fault_pri   ;//: 1;//: std_logic;
  uint32 fault_access : 1;//    : std_logic; 
  uint32 fault_mexc   : 1;//    : std_logic;
  uint32 fault_trans  : 1;//    : std_logic;
  uint32 fault_inv    : 1;//    : std_logic;
  uint32 fault_lvl    : 2;//    : std_logic_vector(1 downto 0);
  uint32 fault_su     : 1;//    : std_logic;
  uint32 fault_read   : 1;//    : std_logic;
  mmu_idcache fault_isid;//       : mmu_idcache;
  uint32 fault_addr;//       : std_logic_vector(31 downto 0);
};
  
struct mmutlb_out_type
{
  mmuidc_data_out_type transdata;//   : mmuidc_data_out_type;
  mmutlbfault_out_type fault;//       : mmutlbfault_out_type;
  uint32 nexttrans   : 1;//std_logic;
  uint32 s1finished  : 1;//std_logic;

  //-- fast writebuffer signals
  mmutlbcamo_a tlbcamo;//          : mmutlbcamo_a (M_ENT_MAX-1 downto 0);
  //-- writebuffer out
  mmuidc_data_out_type wbtransdata;//      : mmuidc_data_out_type;
}; 
struct mmutlbo_a
{
  mmutlb_out_type *arr;//is array (natural range <>) of mmutlb_out_type;
};


