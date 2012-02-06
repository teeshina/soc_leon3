//****************************************************************************
// Author:    Khabarov Sergey
// License:   GNU2
// Contact:   sergey.khabarov@gnss-sensor.com
//****************************************************************************

#pragma once

//-----------------------------------------------------------------------------
// AMBA interface type declarations and constant
//-----------------------------------------------------------------------------

typedef uint32 amba_config_word;// is (31 downto 0);
struct ahb_config_type
{
  amba_config_word arr[AHB_CFG_WORDS];// (0 to AHB_CFG_WORDS-1) of amba_config_word;
};
struct apb_config_type 
{
  amba_config_word arr[APB_CFGWORD_NUM];// (0 to NAPBCFG-1) of amba_config_word;
};

struct ahb_device_reg
{
  uint32 interrupt  : 5;
  uint32 version    : 5;
  uint32 cfgver     : 2;
  uint32 device     : 12;
  uint32 vendor     : 8;
};

struct ahb_membar_type//function ahb_membar
{
  //uint32 zero_a     : 1;
  //uint32 one        : 1;
  //uint32 zero_b     : 2;
  uint32 area_id    : 4;  // 0001=APB I/O space; 0010=AHB memory space; 0011 AHB I/O space
  uint32 addrmask   : 12;
  uint32 cache      : 1;
  uint32 prefetch   : 1;
  uint32 zero_c     : 2;
  uint32 memaddr    : 12;
};


struct ahb_dma_in_type
{
  uint32 address;
  uint32 wdata;//           : std_logic_vector(AHBDW-1 downto 0);
  uint32 start;//           : std_ulogic;
  uint32 burst;//           : std_ulogic;
  uint32 write;//           : std_ulogic;
  uint32 busy;//            : std_ulogic;
  uint32 irq;//             : std_ulogic;
  uint32 size;//            : std_logic_vector(2 downto 0);
};

struct ahb_dma_out_type
{
  uint32 start;//           : std_ulogic;
  uint32 active;//          : std_ulogic;
  uint32 ready;//           : std_ulogic;
  uint32 retry;//           : std_ulogic;
  uint32 mexc;//            : std_ulogic;
  uint32 haddr;//           : std_logic_vector(9 downto 0);
  uint32 rdata;//           : std_logic_vector(AHBDW-1 downto 0);
};

// AHB master inputs
struct ahb_mst_in_type
{
  uint32 hgrant;//[0:15]  : (0 to AHB_MASTERS_MAX-1);     -- bus grant
  uint32 hready;//  ;                           -- transfer done
  uint32 hresp;//[1:0] : (1 downto 0);   -- response type
  uint32 hrdata;//[31:0]  : (AHBDW-1 downto 0);   -- read data bus
  uint32 hcache;//  ;                           -- cacheable
  uint32 hirq;//[31:0]    : (AHB_IRQ_MAX-1 downto 0); -- interrupt result bus
  uint32 testen;//  ;                           -- scan test enable
  uint32 testrst;// ;                           -- scan test reset
  uint32 scanen;//  ;                           -- scan enable
  uint32 testoen;//   ;                           -- test output enable 
};

// AHB master outputs
struct ahb_mst_out_type
{
  uint32 hbusreq;// ;                           -- bus request
  uint32 hlock;// ;                           -- lock request
  uint32 htrans;//[1:0]  : (1 downto 0);   -- transfer type
  uint32 haddr;//[31:0] : (31 downto 0);  -- address bus (byte)
  uint32 hwrite;//  ;                           -- read/write
  uint32 hsize;//[2:0] : (2 downto 0);   -- transfer size
  uint32 hburst;//[2:0]  : (2 downto 0);   -- burst type
  uint32 hprot;//[3:0] : (3 downto 0);   -- protection control
  uint32 hwdata;//[31:0]  : (AHBDW-1 downto 0);   -- write data bus
  uint32 hirq;//[31:0]    : (AHB_IRQ_MAX-1 downto 0); -- interrupt bus
  ahb_config_type hconfig;//   : ahb_config_type;      -- memory access reg.
  uint32 hindex;//[0:15]    : integer range 0 to AHB_MASTERS_MAX-1;   -- diagnostic use only
};

struct ahb_mst_out_vector
{
  ahb_mst_out_type arr[AHB_MASTERS_MAX];
};


struct ahb_slv_in_type
{
  uint32 hsel;//[0:15]  : (0 to AHB_SLAVES_MAX-1);     -- slave select
  uint32 haddr;// : (31 downto 0);  -- address bus (byte)
  uint32 hwrite;//  ;                           -- read/write
  uint32 htrans;//  : (1 downto 0);   -- transfer type
  uint32 hsize;// : (2 downto 0);   -- transfer size
  uint32 hburst;//  : (2 downto 0);   -- burst type
  uint32 hwdata;//[31:0]  : (AHBDW-1 downto 0);   -- write data bus
  uint32 hprot;//[3:0] : (3 downto 0);   -- protection control
  uint32 hready;//  ;                -- transfer done
  uint32 hmaster;// : (3 downto 0);  -- current master
  uint32 hmastlock;// ;              -- locked access
  uint32 hmbsel;//[0:3]  : (0 to AHB_MEM_ID_WIDTH-1); -- memory bank select
  uint32 hcache;//  ;                -- cacheable
  uint32 hirq;//[31:0]    : (AHB_IRQ_MAX-1 downto 0); -- interrupt result bus
  uint32 testen;//                        -- scan test enable
  uint32 testrst;//                       -- scan test reset
  uint32 scanen;//                        -- scan enable
  uint32 testoen;//                       -- test output enable 
};

// AHB slave outputs
struct ahb_slv_out_type
{
  uint32 hready;//                           -- transfer done
  uint32 hresp;//[1:0] : (1 downto 0);   -- response type
  uint32 hrdata;//[31:0]  : (AHBDW-1 downto 0);   -- read data bus
  uint32 hsplit;//[15:0]  : (15 downto 0);  -- split completion
  uint32 hcache;//                           -- cacheable
  uint32 hirq;//[31:0]    : (AHB_IRQ_MAX-1 downto 0); -- interrupt bus
  ahb_config_type hconfig;//   : ahb_config_type;      -- memory access reg.
  uint32         hindex;//[15:0]    : integer range 0 to AHB_SLAVES_MAX-1;   -- diagnostic use only
};

struct ahb_slv_out_vector
{
  ahb_slv_out_type arr[AHB_SLAVES_MAX];
};


// APB slave inputs
struct apb_slv_in_type
{
  uint32 psel;//[0:15]  : (0 to APB_SLAVES_MAX-1); -- slave select
  uint32 penable;// ;                       -- strobe
  uint32 paddr;//[31:0] : (31 downto 0);  -- address bus (byte)
  uint32 pwrite;//  ;                       -- write
  uint32 pwdata;//[31:0]  : (31 downto 0);  -- write data bus
  uint32 pirq;//[31:0]  : (AHB_IRQ_MAX-1 downto 0); -- interrupt result bus
  uint32 testen;//                          -- scan test enable
  uint32 testrst;//                         -- scan test reset
  uint32 scanen;//                          -- scan enable
  uint32 testoen;//                         -- test output enable
};

// APB slave outputs
struct apb_slv_out_type
{
  uint32 prdata;//[31:0]  : (31 downto 0);  -- read data bus
  uint32 pirq;//[31:0]  : (AHB_IRQ_MAX-1 downto 0); -- interrupt bus
  apb_config_type pconfig;//   : apb_config_type;      -- memory access reg.
  int             pindex;//[0:15]      : integer range 0 to APB_SLAVES_MAX -1;  -- diag use only
};

// array types
struct apb_slv_out_vector
{
  apb_slv_out_type arr[APB_SLAVES_MAX-1];// is array (0 to APB_SLAVES_MAX-1) of apb_slv_out_type;
};




const uint32 zx      = 0;// (31 downto 0) := (others => '0');
const uint32 zahbdw  = 0;// (AHBDW-1 downto 0) := (others => '0');
const uint32 zxirq   = 0;// (AHB_IRQ_MAX-1 downto 0) := (others => '0');
const uint32 zy      = 0;// (0 to 31) := (others => '0');



const apb_slv_out_type apb_none = { zx, zxirq, (zx), 0 };
const ahb_mst_out_type ahbm_none = { 0, 0, 0, zx,
   0, 0, 0, 0, zahbdw, zxirq, (zx), 0};
const ahb_slv_out_type ahbs_none = { 1, 0, zahbdw, zx, 0, zxirq, (zx), 0 };
const ahb_slv_in_type ahbs_in_none = {
   zy, zx, 0, 0, 0, 0, zahbdw,
   0, 1, 0, 0, zy, 0, zxirq,
   0, 0, 0, 0};

