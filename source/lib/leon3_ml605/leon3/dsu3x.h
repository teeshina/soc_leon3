#pragma once

class dsu3x
{
  friend class dbg;
  private:
    uint32 hindex;
    uint32 uiMemaddr;
    uint32 uiAddrmask;
    ahb_config_type hconfig;

  private:
    struct slv_reg_type
    {
      uint32 hsel     : 1;//std_ulogic;
      uint32 haddr;//    : std_logic_vector(PROC_H downto 0);
      uint32 hwrite   : 1;//std_ulogic;
      uint32 hwdata;//   : std_logic_vector(31 downto 0);
      uint32 hrdata;//   : std_logic_vector(31 downto 0);    
      uint32 hready  : 1;//std_ulogic;
      uint32 hready2 : 1;//std_ulogic;
    };

    struct reg_type
    {
      slv_reg_type slv;//  : slv_reg_type;
      uint32 en[CFG_NCPU];//std_logic_vector(0 to NCPU-1);
      uint32 te[CFG_NCPU];//std_logic_vector(0 to NCPU-1);
      uint32 be[CFG_NCPU];//std_logic_vector(0 to NCPU-1);
      uint32 bw[CFG_NCPU];//std_logic_vector(0 to NCPU-1);
      uint32 bs[CFG_NCPU];//std_logic_vector(0 to NCPU-1);
      uint32 bx[CFG_NCPU];//std_logic_vector(0 to NCPU-1);
      uint32 bz[CFG_NCPU];//std_logic_vector(0 to NCPU-1);
      uint32 halt[CFG_NCPU];//std_logic_vector(0 to NCPU-1);
      uint32 reset[CFG_NCPU];//std_logic_vector(0 to NCPU-1);
      uint32 bn     : CFG_NCPU;//std_logic_vector(NCPU-1 downto 0);
      uint32 ss     : CFG_NCPU;//std_logic_vector(NCPU-1 downto 0);
      uint32 bmsk   : CFG_NCPU;//std_logic_vector(NCPU-1 downto 0);
      uint32 dmsk   : CFG_NCPU;//std_logic_vector(NCPU-1 downto 0);
      uint32 cnt    : 3;//std_logic_vector(2 downto 0);
      uint32 dsubre : 3;//std_logic_vector(2 downto 0);
      uint32 dsuen  : 3;//std_logic_vector(2 downto 0);
      uint32 act    : 1;//std_ulogic;
      uint32 timer  : CFG_DSU_TBITS;// : std_logic_vector(tbits-1 downto 0);
      uint32 pwd    : CFG_NCPU;//   : std_logic_vector(NCPU-1 downto 0);    
      uint32 tstop  : 1;//std_ulogic;
    };
                       
    struct trace_break_reg 
    {
      uint32 addr;//          : std_logic_vector(31 downto 2);
      uint32 mask;//          : std_logic_vector(31 downto 2);
      uint32 read          : 1;//std_logic;
      uint32 write         : 1;//std_logic;
    };

    struct tregtype
    {
      uint32 haddr;//         : std_logic_vector(31 downto 0);
      uint32 hwrite        : 1;//std_logic;
      uint32 htrans	       : 2;//std_logic_vector(1 downto 0);
      uint32 hsize         : 3;//std_logic_vector(2 downto 0);
      uint32 hburst        : 3;//std_logic_vector(2 downto 0);
      uint32 hwdata;//        : std_logic_vector(31 downto 0);
      uint32 hmaster       : 4;//std_logic_vector(3 downto 0);
      uint32 hmastlock     : 1;//std_logic;
      uint32 hsel          : 1;//std_logic;
      uint32 ahbactive     : 1;//std_logic;
      uint32 aindex;//  	  : std_logic_vector(TBUFABITS - 1 downto 0); -- buffer index
      uint32 enable        : 1;//std_logic;	-- trace enable
      uint32 bphit         : 1;//std_logic;	-- AHB breakpoint hit
      uint32 bphit2        : 1;//std_logic;  -- delayed bphit
      uint32 dcnten        : 1;//std_logic;	-- delay counter enable
      uint32 delaycnt;//      : std_logic_vector(TBUFABITS - 1 downto 0); -- delay counter
      trace_break_reg tbreg1;//	  : trace_break_reg;
      trace_break_reg tbreg2;//	  : trace_break_reg;
      uint32 tbwr          : 1;//std_logic;	-- trace buffer write enable
      uint32 Break         : 1;//std_logic;	-- break CPU when AHB tracing stops    
    };

    struct hclk_reg_type
    {
      uint32 irq  : 1;//std_ulogic;
      uint32 oen  : 1;//std_ulogic;
    };                          
    
    tracebuf_in_type  tbi;//   : tracebuf_in_type;
    tracebuf_out_type tbo;//   : tracebuf_out_type;

    tregtype trin;// : tregtype;
    TDFF<tregtype> tr;// : tregtype;
    reg_type rin;// : reg_type;
    TDFF<reg_type> r;// : reg_type;

    hclk_reg_type rh, rhin;// : hclk_reg_type;
    ahb_slv_in_type ahbsi2;// : ahb_slv_in_type;
    uint32 hrdata2x;// : std_logic_vector(31 downto 0);

    reg_type v;// : reg_type;
    uint32 iuacc    : 1;//std_ulogic;
    uint32 dbgmode  : 1;
    uint32 tstop    : 1;//std_ulogic;
    uint32 rawindex;// : integer range 0 to (2**NBITS)-1;
    uint32 index;// : natural range 0 to NCPU-1;
    uint32 hasel1;// : std_logic_vector(AREA_H-1 downto AREA_L);
    uint32 hasel2;//std_logic_vector(6 downto 2);
    tregtype tv;// : tregtype;
    tracebuf_in_type vabufi;// : tracebuf_in_type;
    uint32 aindex;// : std_logic_vector(TBUFABITS - 1 downto 0); -- buffer index
    uint32 hirq;// : std_logic_vector(NAHBIRQ-1 downto 0);
    uint32 cpwd : 16;//std_logic_vector(15 downto 0);     
    uint32 hrdata;// : std_logic_vector(31 downto 0);
    uint32 bphit1 : 1;
    uint32 bphit2 : 1;//std_ulogic;    
    hclk_reg_type vh;// : hclk_reg_type;    

#if (CFG_ATBSZ!=0)
    tbufmem *pclMem0;
#endif


  public:
    dsu3x(uint32 ind_=AHB_SLAVE_DSU, uint32 uiMemaddr_=0x900, uint32 uiAddrmask_=0xf00);
    ~dsu3x();
  
    void Update(uint32 rst,//    : in  std_ulogic;
                SClock hclk,//   : in  std_ulogic;
                SClock cpuclk,// : in std_ulogic;
                ahb_mst_in_type &in_ahbmi,//  : in  ahb_mst_in_type;
                ahb_slv_in_type &in_ahbsi,//  : in  ahb_slv_in_type;
                ahb_slv_out_type &ch_ahbso,//  : out ahb_slv_out_type;
                l3_debug_out_vector &in_dbgi,//   : in l3_debug_out_vector(0 to NCPU-1);
                l3_debug_in_vector &ch_dbgo,//   : out l3_debug_in_vector(0 to NCPU-1);
                dsu_in_type &in_dsui,//   : in dsu_in_type;
                dsu_out_type &ch_dsuo,//   : out dsu_out_type;
                uint32 in_hclken// : in std_ulogic
                );
    void ClkUpdate()
    {
      tr.ClkUpdate();
      r.ClkUpdate();
#if (CFG_ATBSZ!=0)
      pclMem0->ClkUpdate();
#endif
    }
  
};
