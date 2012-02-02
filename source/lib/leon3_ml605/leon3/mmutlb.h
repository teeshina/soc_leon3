#pragma once

class mmutlb
{
  friend class dbg;
  private:
    int32 entries;//  8//: integer range 2 to 32 := 8;
    uint32 tlb_type;//  1//: integer range 0 to 3 := 1;

  private:
    enum states  {idle, match, walk, pack, flush, sync, diag, dofault};
    struct tlb_rtype
    {
      states s2_tlbstate;// : states;
      uint32 s2_entry;//    : std_logic_vector(entries_log-1 downto 0);
      uint32 s2_hm       : 1;//std_logic;
      uint32 s2_needsync : 1;//std_logic;
      uint32 s2_data;//     : std_logic_vector(31 downto 0);
      mmu_idcache s2_isid;//     : mmu_idcache;
      uint32 s2_su       : 1;//std_logic;
      uint32 s2_read     : 1;//std_logic;
      uint32 s2_flush    : 1;//std_logic;
      uint32 s2_ctx;//      : std_logic_vector(M_CTX_SZ-1 downto 0);

      uint32 walk_use       : 1;//std_logic;
      mmuidc_data_out_type walk_transdata;// : mmuidc_data_out_type;
      mmutlbfault_out_type walk_fault;//     : mmutlbfault_out_type;
      
      uint32 nrep;//        : std_logic_vector(entries_log-1 downto 0);
      uint32 tpos;//        : std_logic_vector(entries_log-1 downto 0);
      uint32 touch       : 1;//std_logic;
      uint32 sync_isw    : 1;//std_logic;
      uint32 tlbmiss     : 1;//std_logic;
    };
    tlb_rtype c;//   : tlb_rtype;
    TDFF<tlb_rtype> r;//   : tlb_rtype;

    mmutlbcami_a tlbcami;//     : mmutlbcami_a (M_ENT_MAX-1 downto 0);
    mmutlbcamo_a tlbcamo;//     : mmutlbcamo_a (M_ENT_MAX-1 downto 0);

    mmulru_in_type lrui;//  : mmulru_in_type;
    mmulru_out_type lruo;//  : mmulru_out_type;

    //-- data-ram syncram signals
    uint32 dr1_addr;//         : std_logic_vector(entries_log-1 downto 0);
    uint32 dr1_datain       : 30;//std_logic_vector(29 downto 0);
    uint32 dr1_dataout      ;//: 30;//std_logic_vector(29 downto 0);
    uint32 dr1_enable       : 1;//std_logic;
    uint32 dr1_write        : 1;//std_logic;


    tlb_rtype v;//                    : tlb_rtype;
    uint32 finish   : 1;
    uint32 selstate : 1;//     : std_logic;

    uint32 cam_hitaddr;//          : std_logic_vector(M_ENT_MAX_LOG -1 downto 0);
    uint32 cam_hit_all : 1;//         : std_logic;
    tlbcam_tfp mtag;
    tlbcam_tfp ftag;//            : tlbcam_tfp;

    //-- tlb cam input
    uint32 tlbcam_trans_op      : 1;//std_logic;
    uint32 tlbcam_write_op;//      : std_logic_vector(entries-1 downto 0);
    uint32 tlbcam_flush_op      : 1;//std_logic;

    //-- tw inputs
    uint32 twi_walk_op_ur       : 1;//std_logic;
    uint32 twi_data;//             : std_logic_vector(31 downto 0);
    uint32 twi_areq_ur          : 1;//std_logic;
    uint32 twi_aaddr;//            : std_logic_vector(31 downto 0);
    uint32 twi_adata;//            : std_logic_vector(31 downto 0);

    uint32 two_error            : 1;//std_logic;
      
    //-- lru inputs
    uint32 lrui_touch           : 1;//std_logic;
    uint32 lrui_touchmin        : 1;//std_logic;
    uint32 lrui_pos;//             : std_logic_vector(entries_log-1 downto 0);

    //-- syncram inputs
    uint32 dr1write             : 1;//std_logic;


    //-- hit tlbcam's output 
    uint32 ACC                  : 3;//std_logic_vector(2 downto 0);
    uint32 PTE;//                  : std_logic_vector(31 downto 0);
    uint32 LVL                  : 2;//std_logic_vector(1 downto 0);
    uint32 CAC                  : 1;//std_logic;
    uint32 NEEDSYNC             : 1;//std_logic;

    //-- wb hit tlbcam's output 
    uint32 wb_i_entry;//     : integer range 0 to M_ENT_MAX-1;
    uint32 wb_ACC               : 3;//std_logic_vector(2 downto 0);
    uint32 wb_PTE;//               : std_logic_vector(31 downto 0);
    uint32 wb_LVL               : 2;//std_logic_vector(1 downto 0);
    uint32 wb_CAC               : 1;//std_logic;
    uint32 wb_fault_pro         ;//: 1;
    uint32 wb_fault_pri         ;//: 1;//std_logic;
    uint32 wb_WBNEEDSYNC        : 1;//std_logic;
    
    uint32 twACC                : 3;//std_logic_vector(2 downto 0);
    uint32 twLVL                : 2;//std_logic_vector(1 downto 0);
    uint32 twPTE;//                : std_logic_vector(31 downto 0);
    uint32 twNEEDSYNC           : 1;//std_logic;
    
    
    tlbcam_tfp tlbcam_tagin;//         : tlbcam_tfp;
    tlbcam_reg tlbcam_tagwrite;//      : tlbcam_reg;

    uint32 store                : 1;//std_logic;
    
    uint32 reppos;//               : std_logic_vector(entries_log-1 downto 0);
    uint32 i_entry;//      : integer range 0 to M_ENT_MAX-1;
    uint32 i_reppos;//     : integer range 0 to M_ENT_MAX-1;
    
    uint32 fault_pro      ;//: 1;
    uint32 fault_pri      ;//: 1;//std_logic;
    uint32 fault_mexc     : 1 ;
    uint32 fault_trans    : 1; 
    uint32 fault_inv      : 1;
    uint32 fault_access   : 1;//std_logic;
    
    mmuidc_data_out_type transdata;// : mmuidc_data_out_type;
    mmutlbfault_out_type fault;//     : mmutlbfault_out_type;
    uint32 savewalk         : 1;//std_logic;
    uint32 tlbo_s1finished  : 1;//std_logic;

    mmuidc_data_out_type wb_transdata;// : mmuidc_data_out_type;

    uint32 cam_addr;//     : std_logic_vector(31 downto 0);


    syncram *dataram;
    mmutlbcam *tlbcam0[M_ENT_MAX];
    mmulru *lru;
    
  public:
    mmutlb(int32 entries_=8, uint32 tlb_type_=1);
    ~mmutlb();
    
    void Update(uint32 rst,//   : in  std_logic;
                SClock clk,//     : in std_logic;
                mmutlb_in_type &tlbi,//  : in  mmutlb_in_type;
                mmutlb_out_type &tlbo,//  : out mmutlb_out_type;
                mmutw_out_type &two,//   : in  mmutw_out_type;
                mmutw_in_type &twi );//   : out mmutw_in_type
  
    void ClkUpdate()
    {
      dataram->ClkUpdate();
      for (int32 i=0; i<entries; i++)
        tlbcam0[i]->ClkUpdate();
      
      lru->ClkUpdate();
      r.ClkUpdate();
    }
};
