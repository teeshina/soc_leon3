#pragma once

#include "stdtypes.h"
#include "leon3_ml605\leon3\libiu.h"

const uint32 TAG_HIGH     = 31;//: integer := 31;
const uint32 CTAG_LRRPOS  = 9;//: integer := 9;
const uint32 CTAG_LOCKPOS = 8;//: integer := 8;
const uint32 MAXSETS      = 4;//: integer := 4;

// 3-way set permutations
// s012 => set 0 - least recently used
//         set 2 - most recently used
const uint32 s012 = 0x0;//: std_logic_vector(2 downto 0) := "000";
const uint32 s021 = 0x1;//: std_logic_vector(2 downto 0) := "001";
const uint32 s102 = 0x2;//: std_logic_vector(2 downto 0) := "010";
const uint32 s120 = 0x3;//: std_logic_vector(2 downto 0) := "011";
const uint32 s201 = 0x4;//: std_logic_vector(2 downto 0) := "100";
const uint32 s210 = 0x5;//: std_logic_vector(2 downto 0) := "101";


// 4-way set permutations
// s0123 => set 0 - least recently used
//          set 3 - most recently used
const uint32 s0123 = 0x00;//: std_logic_vector(4 downto 0) := "00000";
const uint32 s0132 = 0x01;//: std_logic_vector(4 downto 0) := "00001";
const uint32 s0213 = 0x02;//: std_logic_vector(4 downto 0) := "00010";
const uint32 s0231 = 0x03;//: std_logic_vector(4 downto 0) := "00011";
const uint32 s0312 = 0x04;//: std_logic_vector(4 downto 0) := "00100";
const uint32 s0321 = 0x05;//: std_logic_vector(4 downto 0) := "00101";
const uint32 s1023 = 0x06;//: std_logic_vector(4 downto 0) := "00110";
const uint32 s1032 = 0x07;//: std_logic_vector(4 downto 0) := "00111";
const uint32 s1203 = 0x08;//: std_logic_vector(4 downto 0) := "01000";
const uint32 s1230 = 0x09;//: std_logic_vector(4 downto 0) := "01001";
const uint32 s1302 = 0x0a;//: std_logic_vector(4 downto 0) := "01010";
const uint32 s1320 = 0x0b;//: std_logic_vector(4 downto 0) := "01011";
const uint32 s2013 = 0x0c;//: std_logic_vector(4 downto 0) := "01100";
const uint32 s2031 = 0x0d;//: std_logic_vector(4 downto 0) := "01101";
const uint32 s2103 = 0x0e;//: std_logic_vector(4 downto 0) := "01110";
const uint32 s2130 = 0x0f;//: std_logic_vector(4 downto 0) := "01111";
const uint32 s2301 = 0x10;//: std_logic_vector(4 downto 0) := "10000";
const uint32 s2310 = 0x11;//: std_logic_vector(4 downto 0) := "10001";
const uint32 s3012 = 0x12;//: std_logic_vector(4 downto 0) := "10010";
const uint32 s3021 = 0x13;//: std_logic_vector(4 downto 0) := "10011";
const uint32 s3102 = 0x14;//: std_logic_vector(4 downto 0) := "10100";
const uint32 s3120 = 0x15;//: std_logic_vector(4 downto 0) := "10101";
const uint32 s3201 = 0x16;//: std_logic_vector(4 downto 0) := "10110";
const uint32 s3210 = 0x17;//: std_logic_vector(4 downto 0) := "10111";

struct lru_3set_table_vector_type
{
  uint32 arr[3];// is array(0 to 2) of std_logic_vector(2 downto 0);
};
struct lru_3set_table_type
{
  lru_3set_table_vector_type arr[8];// is array (0 to 7) of lru_3set_table_vector_type;
};

//const lru_3set_table_type lru_3set_table =
const uint32 lru_3set_table[8][3] =
  { {s120, s021, s012},                   // s012
    {s210, s021, s012},                   // s021
    {s120, s021, s102},                   // s102
    {s120, s201, s102},                   // s120
    {s210, s201, s012},                   // s201
    {s210, s201, s102},                   // s210
    {s210, s201, s102},                   // dummy
    {s210, s201, s102}                    // dummy
  };
  
struct lru_4set_table_vector_type
{
  uint32 arr[5];// is array(0 to 3) of std_logic_vector(4 downto 0);
};
struct lru_4set_table_type
{
  lru_4set_table_vector_type arr[32];//is array(0 to 31) of lru_4set_table_vector_type;
};

//const lru_4set_table_type lru_4set_table =
const uint32 lru_4set_table[32][5] =
  { {s1230, s0231, s0132, s0123},       // s0123
    {s1320, s0321, s0132, s0123},       // s0132
    {s2130, s0231, s0132, s0213},       // s0213
    {s2310, s0231, s0312, s0213},       // s0231
    {s3120, s0321, s0312, s0123},       // s0312    
    {s3210, s0321, s0312, s0213},       // s0321
    {s1230, s0231, s1032, s1023},       // s1023
    {s1320, s0321, s1032, s1023},       // s1032
    {s1230, s2031, s1032, s1203},       // s1203
    {s1230, s2301, s1302, s1203},       // s1230
    {s1320, s3021, s1302, s1023},       // s1302
    {s1320, s3201, s1302, s1203},       // s1320
    {s2130, s2031, s0132, s2013},       // s2013
    {s2310, s2031, s0312, s2013},       // s2031
    {s2130, s2031, s1032, s2103},       // s2103
    {s2130, s2301, s1302, s2103},       // s2130      
    {s2310, s2301, s3012, s2013},       // s2301
    {s2310, s2301, s3102, s2103},       // s2310
    {s3120, s3021, s3012, s0123},       // s3012
    {s3210, s3021, s3012, s0213},       // s3021
    {s3120, s3021, s3102, s1023},       // s3102
    {s3120, s3201, s3102, s1203},       // s3120
    {s3210, s3201, s3012, s2013},       // s3201
    {s3210, s3201, s3102, s2103},       // s3210
    {s3210, s3201, s3102, s2103},        // dummy
    {s3210, s3201, s3102, s2103},        // dummy
    {s3210, s3201, s3102, s2103},        // dummy
    {s3210, s3201, s3102, s2103},        // dummy
    {s3210, s3201, s3102, s2103},        // dummy
    {s3210, s3201, s3102, s2103},        // dummy
    {s3210, s3201, s3102, s2103},        // dummy
    {s3210, s3201, s3102, s2103}         // dummy
  };

struct lru3_repl_table_single_type 
{
  uint32 arr[3];//is array(0 to 2) of integer range 0 to 2;
};
struct lru3_repl_table_type
{
  lru3_repl_table_single_type arr[8];// is array(0 to 7) of lru3_repl_table_single_type;
};

//lru3_repl_table_type lru3_repl_table =
const uint32 lru3_repl_table[8][3] =
  { {0, 1, 2},      // s012
    {0, 2, 2},      // s021
    {1, 1, 2},      // s102
    {1, 1, 2},      // s120
    {2, 2, 2},      // s201
    {2, 2, 2},      // s210
    {2, 2, 2},      // dummy
    {2, 2, 2}       // dummy
  };

struct lru4_repl_table_single_type 
{
  uint32 arr[4];//is array(0 to 3) of integer range 0 to 3;
};
struct lru4_repl_table_type
{
  lru4_repl_table_single_type arr[32];//is array(0 to 31) of lru4_repl_table_single_type;
};

//const lru4_repl_table_type lru4_repl_table =
const uint32 lru4_repl_table[32][4] =
  { {0, 1, 2, 3}, // s0123
    {0, 1, 3, 3}, // s0132
    {0, 2, 2, 3}, // s0213
    {0, 2, 2, 3}, // s0231
    {0, 3, 3, 3}, // s0312
    {0, 3, 3, 3}, // s0321
    {1, 1, 2, 3}, // s1023
    {1, 1, 3, 3}, // s1032
    {1, 1, 2, 3}, // s1203
    {1, 1, 2, 3}, // s1230
    {1, 1, 3, 3}, // s1302
    {1, 1, 3, 3}, // s1320
    {2, 2, 2, 3}, // s2013
    {2, 2, 2, 3}, // s2031
    {2, 2, 2, 3}, // s2103
    {2, 2, 2, 3}, // s2130
    {2, 2, 2, 3}, // s2301
    {2, 2, 2, 3}, // s2310
    {3, 3, 3, 3}, // s3012
    {3, 3, 3, 3}, // s3021
    {3, 3, 3, 3}, // s3102
    {3, 3, 3, 3}, // s3120
    {3, 3, 3, 3}, // s3201
    {3, 3, 3, 3}, // s3210
    {0, 0, 0, 0}, // dummy
    {0, 0, 0, 0}, // dummy
    {0, 0, 0, 0}, // dummy
    {0, 0, 0, 0}, // dummy
    {0, 0, 0, 0}, // dummy
    {0, 0, 0, 0}, // dummy
    {0, 0, 0, 0}, // dummy
    {0, 0, 0, 0}  // dummy
  };

struct ildram_in_type
{
  uint32 enable;//        : std_ulogic;                       
  uint32 read;//          : std_ulogic;                         
  uint32 write;//         : std_ulogic;                       
};

typedef uint32 ctxword;//subtype ctxword is std_logic_vector(M_CTX_SZ-1 downto 0);
struct ctxdatatype
{
  ctxword arr[4];// is array (0 to 3) of ctxword;
};

struct icram_in_type
{
  uint32 address;// : std_logic_vector(19 downto 0);
  cdatatype tag;//     : cdatatype;
  uint32 twrite;//  : std_logic_vector(0 to 3);
  uint32 tenable;// : std_ulogic;
  uint32 flush;//   : std_ulogic;
  uint32 data;//    : std_logic_vector(31 downto 0);
  uint32 denable;// : std_ulogic;
  uint32 dwrite;//  : std_logic_vector(0 to 3);
  ildram_in_type ldramin;//       : ildram_in_type;
  uint32 ctx;//           : std_logic_vector(M_CTX_SZ-1 downto 0);
  cpartype tpar;//          : cpartype;
  uint32 dpar;//          : std_logic_vector(3 downto 0);
};

struct icram_out_type
{
  cdatatype tag;//     : cdatatype;
  cdatatype data;//      : cdatatype;
  ctxdatatype ctx;//           : ctxdatatype;
  cpartype tpar;//          : cpartype;
  cpartype dpar;//          : cpartype;
};

struct ldram_in_type
{
  uint32 address;// : std_logic_vector(23 downto 2);
  uint32 enable;//  : std_ulogic;
  uint32 read;//    : std_ulogic;
  uint32 write;//   : std_ulogic;
};

struct dcram_in_type
{
  uint32 address;// : std_logic_vector(19 downto 0);
  cdatatype tag;//     : cdatatype; //std_logic_vector(31  downto 0);
  cdatatype ptag;//      : cdatatype; //std_logic_vector(31  downto 0);
  uint32 twrite;//  : std_logic_vector(0 to 3);
  uint32 tpwrite;// : std_logic_vector(0 to 3);
  uint32 tenable;// : std_logic_vector(0 to 3);
  uint32 flush;//   : std_ulogic;
  cdatatype data;//    : cdatatype;
  uint32 denable;// : std_logic_vector(0 to 3);
  uint32 dwrite;//  : std_logic_vector(0 to 3);
  uint32 senable;// : std_logic_vector(0 to 3);
  uint32 swrite;//  : std_logic_vector(0 to 3);
  uint32 saddress;//  : std_logic_vector(19 downto 0);
  uint32 faddress;//  : std_logic_vector(19 downto 0);
  uint32 spar;//          : std_logic;
  ldram_in_type ldramin;// : ldram_in_type;
  ctxdatatype ctx;//           : ctxdatatype;
  cpartype tpar;//          : cpartype;
  cpartype dpar;//          : cpartype;
  uint32 tdiag;//         : std_logic_vector(3 downto 0);
  uint32 ddiag;//         : std_logic_vector(3 downto 0);
  uint32 sdiag;//         : std_logic_vector(3 downto 0);
};

struct dcram_out_type
{
  cdatatype tag;//     : cdatatype;
  cdatatype data;//    : cdatatype;
  cdatatype stag;//      : cdatatype;
  ctxdatatype ctx;//           : ctxdatatype;
  cpartype tpar;//          : cpartype; // tag parity
  cpartype dpar;//          : cpartype;   // data parity
  uint32 spar;//          : std_logic_vector(3 downto 0);   // snoop tag parity
};

struct cram_in_type
{
  icram_in_type icramin;// : icram_in_type;
  dcram_in_type dcramin;// : dcram_in_type;
};

struct cram_out_type
{
  icram_out_type icramo;//  : icram_out_type;
  dcram_out_type dcramo;//  : dcram_out_type;
};

struct memory_ic_in_type
{
  uint32 address;//          : std_logic_vector(31 downto 0); // memory address
  uint32 burst;//            : std_ulogic;      // burst request
  uint32 req;//              : std_ulogic;      // memory cycle request
  uint32 su;//               : std_ulogic;      // supervisor address space
  uint32 flush;//            : std_ulogic;      // flush in progress
};

struct memory_ic_out_type
{
  uint32 data;//             : std_logic_vector(31 downto 0); // memory data
  uint32 ready;//            : std_ulogic;          // cycle ready
  uint32 grant;//            : std_ulogic;          // 
  uint32 retry;//            : std_ulogic;          // 
  uint32 mexc;//             : std_ulogic;          // memory exception
  uint32 cache;//            : std_ulogic;    // cacheable data
  uint32 par;//              : std_logic_vector(3 downto 0);  // parity
  uint32 scanen;//           : std_ulogic;
};

struct memory_dc_in_type
{
  uint32 address;//          : std_logic_vector(31 downto 0); 
  uint32 data;//             : std_logic_vector(31 downto 0);
  uint32 asi;//              : std_logic_vector(3 downto 0); // ASI for load/store
  uint32 size;//             : std_logic_vector(1 downto 0);
  uint32 burst;//            : std_ulogic;
  uint32 read;//             : std_ulogic;
  uint32 req;//              : std_ulogic;
  uint32 lock;//             : std_ulogic;
  uint32 cache;//            : std_ulogic;
};

struct memory_dc_out_type
{
  uint32 data;//             : std_logic_vector(31 downto 0); // memory data
  uint32 ready;//            : std_ulogic;    // cycle ready
  uint32 grant;//            : std_ulogic;
  uint32 retry;//            : std_ulogic;
  uint32 mexc;//             : std_ulogic;    // memory exception
  uint32 werr;//             : std_ulogic;    // memory write error
  uint32 cache;//            : std_ulogic;    // cacheable data
  uint32 ba;//               : std_ulogic;    // bus active (used for snooping)
  uint32 bg;//               : std_ulogic;    // bus grant  (used for snooping)
  uint32 par;//              : std_logic_vector(3 downto 0);  // parity
  uint32 scanen;//           : std_ulogic;
  uint32 testen;//           : std_ulogic;
};

static const uint32 dir  = 3;//: integer := 3;
static const uint32 rnd  = 2;//: integer := 2;
static const uint32 lrr  = 1;//: integer := 1;
static const uint32 lru  = 0;//: integer := 0;
struct cache_replalgbits_type
{
  uint32 arr[4];// is array (0 to 3) of integer;
};
const cache_replalgbits_type creplalg_tbl = {0, 1, 0, 0};

typedef uint32 lru_bits_type;
const lru_bits_type lru_table[5] = {0,1,1,3,5}; //: lru_bits_type := (1,1,3,5);

// SH: change function to struct
struct cache_cfg
{
  uint32 cfg_zerobits   : 3;
  uint32 cfg_mmuen      : 1;
  uint32 cfg_lramstart  : 8;
  uint32 cfg_lramsize   : 4;
  uint32 cfg_linesize   : 3;
  uint32 cfg_lram       : 1;
  uint32 cfg_setsize    : 4;
  uint32 cfg_sets       : 3;
  uint32 cfg_snoop      : 1;
  uint32 cfg_repl       : 3;
  uint32 cfg_lock       : 1;
};
  

#if 0

  component acache
  generic (
    hindex    : integer range 0 to NAHBMST-1  := 0;
    ilinesize : integer range 4 to 8 := 4;
    cached    : integer := 0;
    clk2x     : integer := 0;        
    scantest : integer := 0);
  port (
    rst    : in  std_ulogic;
    clk    : in  std_ulogic;
    mcii   : in  memory_ic_in_type;
    mcio   : out memory_ic_out_type;
    mcdi   : in  memory_dc_in_type;
    mcdo   : out memory_dc_out_type;
    ahbi   : in  ahb_mst_in_type;
    ahbo   : out ahb_mst_out_type;
    ahbso  : in  ahb_slv_out_vector;
    hclken : in std_ulogic
  );
  end component;

  component dcache
  generic (
    dsu       : integer range 0 to 1  := 0;
    dcen      : integer range 0 to 1  := 0;
    drepl     : integer range 0 to 3  := 0;
    dsets     : integer range 1 to 4  := 1;
    dlinesize : integer range 4 to 8  := 4;
    dsetsize  : integer range 1 to 256 := 1;
    dsetlock  : integer range 0 to 1  := 0;
    dsnoop    : integer range 0 to 6 := 0;
    dlram      : integer range 0 to 1 := 0;
    dlramsize  : integer range 1 to 512 := 1;
    dlramstart : integer range 0 to 255 := 16#8f#;
    ilram      : integer range 0 to 1 := 0;
    ilramstart : integer range 0 to 255 := 16#8e#;
    memtech    : integer range 0 to NTECH := 0;
    cached    : integer := 0);    
  port (
    rst    : in  std_ulogic;
    clk    : in  std_ulogic;
    dci    : in  dcache_in_type;
    dco    : out dcache_out_type;
    ico    : in  icache_out_type;
    mcdi   : out memory_dc_in_type;
    mcdo   : in  memory_dc_out_type;
    ahbsi : in  ahb_slv_in_type;
    dcrami : out dcram_in_type;
    dcramo : in  dcram_out_type;
    fpuholdn : in  std_ulogic;
    sclk : in std_ulogic
  );
  end component; 

  component icache 
  generic (
    icen      : integer range 0 to 1  := 0;
    irepl     : integer range 0 to 3  := 0;
    isets     : integer range 1 to 4  := 1;
    ilinesize : integer range 4 to 8  := 4;
    isetsize  : integer range 1 to 256 := 1;
    isetlock  : integer range 0 to 1  := 0;
    lram      : integer range 0 to 1 := 0;
    lramsize  : integer range 1 to 512 := 1;
    lramstart : integer range 0 to 255 := 16#8e#);        
  port (
    rst : in  std_ulogic;
    clk : in  std_ulogic;
    ici : in  icache_in_type;
    ico : out icache_out_type;
    dci : in  dcache_in_type;
    dco : in  dcache_out_type;
    mcii : out memory_ic_in_type;
    mcio : in  memory_ic_out_type;
    icrami : out icram_in_type;
    icramo : in  icram_out_type;
    fpuholdn : in  std_ulogic);
  end component;

  component cache
  generic (
    hindex    : integer              := 0;
    dsu       : integer range 0 to 1 := 0;
    icen      : integer range 0 to 1  := 0;
    irepl     : integer range 0 to 3 := 0;
    isets     : integer range 1 to 4 := 1;
    ilinesize : integer range 4 to 8 := 4;
    isetsize  : integer range 1 to 256:= 1;
    isetlock  : integer range 0 to 1 := 0;
    dcen      : integer range 0 to 1  := 0;
    drepl     : integer range 0 to 3 := 0;
    dsets     : integer range 1 to 4 := 1;
    dlinesize : integer range 4 to 8 := 4;
    dsetsize  : integer range 1 to 256:= 1;
    dsetlock  : integer range 0 to 1 := 0;
    dsnoop    : integer range 0 to 6 := 0;
    ilram      : integer range 0 to 1 := 0;
    ilramsize  : integer range 1 to 512 := 1;        
    ilramstart : integer range 0 to 255 := 16#8e#;
    dlram      : integer range 0 to 1 := 0;
    dlramsize  : integer range 1 to 512 := 1;        
    dlramstart : integer range 0 to 255 := 16#8f#;
    cached     : integer := 0;
    clk2x      : integer := 0;
    memtech    : integer range 0 to NTECH := 0;    
    scantest   : integer := 0);
  port (
    rst   : in  std_ulogic;
    clk   : in  std_ulogic;
    ici   : in  icache_in_type;
    ico   : out icache_out_type;
    dci   : in  dcache_in_type;
    dco   : out dcache_out_type;
    ahbi  : in  ahb_mst_in_type;
    ahbo  : out ahb_mst_out_type;
    ahbsi : in  ahb_slv_in_type;
    ahbso  : in  ahb_slv_out_vector;        
    crami : out cram_in_type;
    cramo : in  cram_out_type;
    fpuholdn : in  std_ulogic;
    hclk, sclk : in std_ulogic;
    hclken : in std_ulogic
  );
  end component; 

  component cachemem 
  generic (
    tech      : integer range 0 to NTECH := 0;
    icen      : integer range 0 to 1 := 0;
    irepl     : integer range 0 to 3 := 0;
    isets     : integer range 1 to 4 := 1;
    ilinesize : integer range 4 to 8 := 4;
    isetsize  : integer range 1 to 256 := 1;
    isetlock  : integer range 0 to 1 := 0;
    dcen      : integer range 0 to 1 := 0;
    drepl     : integer range 0 to 3 := 0;
    dsets     : integer range 1 to 4 := 1;
    dlinesize : integer range 4 to 8 := 4;
    dsetsize  : integer range 1 to 256 := 1;
    dsetlock  : integer range 0 to 1 := 0;
    dsnoop    : integer range 0 to 6 := 0;
    ilram      : integer range 0 to 1 := 0;
    ilramsize  : integer range 1 to 512 := 1;        
    dlram      : integer range 0 to 1 := 0;
    dlramsize  : integer range 1 to 512 := 1;
    mmuen     : integer range 0 to 1 := 0;
    testen    : integer range 0 to 3 := 0
  );
  port (
      clk   : in  std_ulogic;
  crami : in  cram_in_type;
  cramo : out cram_out_type;
      sclk  : in  std_ulogic
  );
  end component;

  -- mmu versions
  component mmu_acache 
    generic (
      hindex    : integer range 0 to NAHBMST-1  := 0;
      ilinesize : integer range 4 to 8 := 4;
      cached    : integer := 0;
      clk2x     : integer := 0;
      scantest : integer := 0);
    port (
      rst    : in  std_logic;
      clk    : in  std_logic;
      mcii   : in  memory_ic_in_type;
      mcio   : out memory_ic_out_type;
      mcdi   : in  memory_dc_in_type;
      mcdo   : out memory_dc_out_type;
      mcmmi  : in  memory_mm_in_type;
      mcmmo  : out memory_mm_out_type;
      ahbi   : in  ahb_mst_in_type;
      ahbo   : out ahb_mst_out_type;
      ahbso  : in  ahb_slv_out_vector;
      hclken : in std_ulogic
    );
  end component;
  
  component mmu_icache 
    generic (
      icen      : integer range 0 to 1  := 0;
      irepl     : integer range 0 to 3  := 0;
      isets     : integer range 1 to 4  := 1;
      ilinesize : integer range 4 to 8  := 4;
      isetsize  : integer range 1 to 256 := 1;
      isetlock  : integer range 0 to 1  := 0;
      lram      : integer range 0 to 1 := 0;
      lramsize  : integer range 1 to 512 := 1;
      lramstart : integer range 0 to 255 := 16#8e#;
      mmuen     : integer              := 0
    );
    port (
      rst : in  std_logic;
      clk : in  std_logic;
      ici : in  icache_in_type;
      ico : out icache_out_type;
      dci : in  dcache_in_type;
      dco : in  dcache_out_type;
      mcii : out memory_ic_in_type;
      mcio : in  memory_ic_out_type;
      icrami : out icram_in_type;
      icramo : in  icram_out_type;
      fpuholdn : in  std_logic;
      mmudci : in  mmudc_in_type;
      mmuici : out mmuic_in_type;
      mmuico : in mmuic_out_type
    );
  end component; 

  component mmu_dcache 
    generic (
      dsu       : integer range 0 to 1  := 0;
      dcen      : integer range 0 to 1  := 0;
      drepl     : integer range 0 to 3  := 0;
      dsets     : integer range 1 to 4  := 1;
      dlinesize : integer range 4 to 8  := 4;
      dsetsize  : integer range 1 to 256 := 1;
      dsetlock  : integer range 0 to 1  := 0;
      dsnoop    : integer range 0 to 6 := 0;
      dlram      : integer range 0 to 1 := 0;
      dlramsize  : integer range 1 to 512 := 1;
      dlramstart : integer range 0 to 255 := 16#8f#;
      ilram      : integer range 0 to 1 := 0;
      ilramstart : integer range 0 to 255 := 16#8e#;
      itlbnum   : integer range 2 to 64 := 8;
      dtlbnum   : integer range 2 to 64 := 8;
      tlb_type  : integer range 0 to 3 := 1;
      memtech   : integer range 0 to NTECH := 0;    
      cached    : integer := 0;
      mmupgsz   : integer range 0 to 5  := 0;
      smp      : integer := 0;
      mmuen    : integer := 0
    );    
    port (
      rst : in  std_logic;
      clk : in  std_logic;
      dci : in  dcache_in_type;
      dco : out dcache_out_type;
      ico : in  icache_out_type;
      mcdi : out memory_dc_in_type;
      mcdo : in  memory_dc_out_type;
      ahbsi : in  ahb_slv_in_type;
      dcrami : out dcram_in_type;
      dcramo : in  dcram_out_type;
      fpuholdn : in  std_logic;
      mmudci : out mmudc_in_type;
      mmudco : in mmudc_out_type;
      sclk : in std_ulogic      
  );
  end component; 
  
  component mmu_cache 
    generic (
      hindex    : integer              := 0;
      memtech   : integer range 0 to NTECH := 0;
      dsu       : integer range 0 to 1 := 0;
      icen      : integer range 0 to 1 := 0;
      irepl     : integer range 0 to 3 := 0;
      isets     : integer range 1 to 4 := 1;
      ilinesize : integer range 4 to 8 := 4;
      isetsize  : integer range 1 to 256 := 1;
      isetlock  : integer range 0 to 1 := 0;
      dcen      : integer range 0 to 1 := 0;
      drepl     : integer range 0 to 3 := 0;
      dsets     : integer range 1 to 4 := 1;
      dlinesize : integer range 4 to 8 := 4;
      dsetsize  : integer range 1 to 256 := 1;
      dsetlock  : integer range 0 to 1 := 0;
      dsnoop    : integer range 0 to 6 := 0;
      ilram      : integer range 0 to 1 := 0;
      ilramsize  : integer range 1 to 512 := 1;        
      ilramstart : integer range 0 to 255 := 16#8e#;
      dlram      : integer range 0 to 1 := 0;
      dlramsize  : integer range 1 to 512 := 1;        
      dlramstart : integer range 0 to 255 := 16#8f#;
      itlbnum   : integer range 2 to 64 := 8;
      dtlbnum   : integer range 2 to 64 := 8;
      tlb_type  : integer range 0 to 3 := 1;
      tlb_rep   : integer range 0 to 1 := 0;
      cached    : integer := 0;
      clk2x     : integer := 0;
      scantest   : integer := 0;
      mmupgsz   : integer range 0 to 5  := 0;
      smp       : integer               := 0;
      mmuen     : integer range 0 to 1  := 0
      );
    port (
      rst   : in  std_ulogic;
      clk   : in  std_ulogic;
      ici   : in  icache_in_type;
      ico   : out icache_out_type;
      dci   : in  dcache_in_type;
      dco   : out dcache_out_type;
      ahbi  : in  ahb_mst_in_type;
      ahbo  : out ahb_mst_out_type;
      ahbsi : in  ahb_slv_in_type;
      ahbso : in  ahb_slv_out_vector;              
      crami : out cram_in_type;
      cramo : in  cram_out_type;
      fpuholdn : in  std_ulogic;
      hclk, sclk : in std_ulogic;
      hclken : in std_ulogic      
    );
  end component; 

  component clk2xqual 
  port (
    rst   : in std_ulogic;
    clk   : in std_ulogic;
    clk2  : in std_ulogic;
    clken : out std_ulogic);
  end component;
  
  component clk2xsync 
  generic (hindex : integer := 0;
           clk2x  : integer := 1);
  port (
    rst    : in  std_ulogic;
    hclk   : in  std_ulogic;
    clk    : in  std_ulogic;
    ahbi   : in  ahb_mst_in_type;
    ahbi2  : out ahb_mst_in_type;
    ahbo   : in  ahb_mst_out_type;
    ahbo2  : out ahb_mst_out_type;    
    ahbsi  : in  ahb_slv_in_type;
    ahbsi2 : out ahb_slv_in_type;
    mcii   : in memory_ic_in_type;
    mcdi   : in memory_dc_in_type;
    mcdo   : in memory_dc_out_type;
    mmreq  : in std_ulogic;
    mmgrant: in std_ulogic;    
    hclken : in std_ulogic
    );
  end component;
  
  function cache_cfg(repl, sets, linesize, setsize, lock, snoop,
    lram, lramsize, lramstart, mmuen : integer) return std_logic_vector;

end;

package body libcache is

  function cache_cfg(repl, sets, linesize, setsize, lock, snoop,
      lram, lramsize, lramstart, mmuen : integer) return std_logic_vector is 
  variable cfg : std_logic_vector(31 downto 0);
  begin
    cfg := (others => '0');
    cfg(31 downto 31) := conv_std_logic_vector(lock, 1);
    cfg(30 downto 28) := conv_std_logic_vector(repl+1, 3);
    if snoop /= 0 then cfg(27) := '1'; end if;
    --cfg(27 downto 27) := conv_std_logic_vector(snoop, 1);
    cfg(26 downto 24) := conv_std_logic_vector(sets-1, 3);
    cfg(23 downto 20) := conv_std_logic_vector(log2(setsize), 4);
    cfg(19 downto 19) := conv_std_logic_vector(lram, 1);
    cfg(18 downto 16) := conv_std_logic_vector(log2(linesize), 3);
    cfg(15 downto 12) := conv_std_logic_vector(log2(lramsize), 4);
    cfg(11 downto  4) := conv_std_logic_vector(lramstart, 8);
    cfg(3  downto  3) := conv_std_logic_vector(mmuen, 1);
    return(cfg);
  end;
end;
#endif

