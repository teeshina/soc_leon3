#pragma once

class mmu_acache
{
  friend class dbg;
  private:
    uint32 hindex;
  private:
    struct reg_type
    {
      uint32 bg     : 1;//std_logic;   -- bus grant
      uint32 bo     : 2;//std_logic_vector(1 downto 0);  -- bus owner
      uint32 ba     : 1;//std_logic; -- bus active
      uint32 lb    : 1;//std_ulogic; -- last burst cycle
      uint32 retry  : 1;//std_logic; -- retry/split pending
      uint32 retry2 : 1;//std_ulogic;  -- retry/split pending
      uint32 werr   : 1;//std_logic; -- write error
      uint32 hlocken : 1;//std_ulogic; -- ready to perform locked transaction
      uint32 hcache : 1;//std_logic; -- cacheable access
      uint32 nba    : 1;// std_ulogic;
      uint32 nbo    : 2;//std_logic_vector(1 downto 0);       -- bus owner
    };

    struct reg2_type
    {
      uint32 reqmsk  : 3;//std_logic_vector(2 downto 0);
      uint32 hclken2 : 1;//std_ulogic;
    };
    
    ahb_config_type hconfig;

    reg_type rin;// : reg_type;
    TDFF<reg_type> r;// : reg_type;
    reg2_type r2in;// : reg2_type;
    TDFF<reg2_type> r2;// : reg2_type;

    reg_type v;// : reg_type;
    reg2_type v2;// : reg2_type;  
    uint32 haddr;//   : std_logic_vector(31 downto 0);   -- address bus
    uint32 htrans  : 2;//std_logic_vector(1 downto 0);    -- transfer type 
    uint32 hwrite  : 1;//std_logic;           -- read/write
    uint32 hlock   : 1;//std_logic;           -- bus lock
    uint32 hsize   : 3;//std_logic_vector(2 downto 0);    -- transfer size
    uint32 hburst  : 3;//std_logic_vector(2 downto 0);    -- burst type
    uint32 hwdata;//  : std_logic_vector(31 downto 0);   -- write data
    uint32 hbusreq : 1;// : std_logic;   -- bus request
    uint32 iready  : 1;//;
    uint32 dready  : 1;//;
    uint32 mmready : 1;// : std_logic;   
    uint32 igrant  : 1;//;
    uint32 dgrant  : 1;//;
    uint32 mmgrant : 1;// : std_logic;   
    uint32 iretry  : 1;//;
    uint32 dretry  : 1;//;
    uint32 mmretry : 1;//: std_logic;   
    uint32 ihcache : 1;//;
    uint32 dhcache : 1;//;
    uint32 mmhcache   : 1;//;
    uint32 dec_hcache : 1;//: std_logic;   
    uint32 imexc   : 1;//
    uint32 dmexc   : 1;//
    uint32 mmmexc  : 1;//std_logic;
    uint32 dreq    : 1;//std_logic;
    uint32 nbo     : 2;//std_logic_vector(1 downto 0);
    uint32 su      : 1;//
    uint32 nb      : 1;//
    uint32 bo_icache : 1;//std_logic;   
    uint32 scanen    : 1;//std_ulogic;
    uint32 vreqmsk   : 1;//std_ulogic;

  public:
    mmu_acache(uint32 mst_index=AHB_MASTER_LEON3)
    {
      hindex = mst_index;
      memset((void*)&hconfig, 0, sizeof(ahb_config_type));
      ((ahb_device_reg*)(&hconfig.arr[0]))->vendor  = VENDOR_GAISLER;
      ((ahb_device_reg*)(&hconfig.arr[0]))->device  = GAISLER_LEON3;
      ((ahb_device_reg*)(&hconfig.arr[0]))->version = LEON3_VERSION;
    }
  
    void Update( uint32 rst,//    : in  std_logic;
                  SClock clk,//    : in  std_logic;
                  memory_ic_in_type &mcii,//   : in  memory_ic_in_type;
                  memory_ic_out_type &mcio,//   : out memory_ic_out_type;
                  memory_dc_in_type &mcdi,//   : in  memory_dc_in_type;
                  memory_dc_out_type &mcdo,//   : out memory_dc_out_type;
                  memory_mm_in_type &mcmmi,//  : in  memory_mm_in_type;
                  memory_mm_out_type &mcmmo,//  : out memory_mm_out_type;
                  ahb_mst_in_type &ahbi,//   : in  ahb_mst_in_type;
                  ahb_mst_out_type &ahbo,//   : out ahb_mst_out_type;
                  ahb_slv_out_vector &ahbso,//  : in  ahb_slv_out_vector;
                  uint32 hclken);// : in  std_ulogic );
                  
    void ClkUpdate()
    {
      r.ClkUpdate();
      r2.ClkUpdate();
    }

    uint32 dec_fixed(uint32 haddr,// : std_logic_vector(3 downto 0);
                     uint32 cached);// : integer) return std_ulogic is

};

