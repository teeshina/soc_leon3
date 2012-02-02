#pragma once

const int32  RDBITS = 32;// : integer := 32;
const int32  IDBITS = 32;// : integer := 32;

typedef uint32 cword;// is std_logic_vector(IDBITS-1 downto 0);
struct cdatatype
{
  cword arr[4];// (0 to 3) of cword;
};

//--type ctagpartype is array (0 to 3) of std_logic_vector(1 downto 0);
//--type cdatapartype is array (0 to 3) of std_logic_vector(3 downto 0);
//--type cvalidtype is array (0 to 3) of std_logic_vector(7 downto 0);
struct cpartype
{
  uint32 arr[4];// is array (0 to 3) of std_logic_vector(3 downto 0); -- byte parity
};

struct iregfile_in_type
{
  uint32 raddr1;//  : std_logic_vector(9 downto 0); -- read address 1
  uint32 raddr2;//  : std_logic_vector(9 downto 0); -- read address 2
  uint32 waddr;//   : std_logic_vector(9 downto 0); -- write address
  uint32 wdata;//   : std_logic_vector(31 downto 0); -- write data
  uint32 ren1;//          : std_ulogic;      -- read 1 enable
  uint32 ren2;//          : std_ulogic;      -- read 2 enable
  uint32 wren;//          : std_ulogic;      -- write enable
  uint32 diag;//    : std_logic_vector(3 downto 0); -- write data
};

struct iregfile_out_type
{
  uint32 data1;//     : std_logic_vector(RDBITS-1 downto 0); -- read data 1
  uint32 data2;//     : std_logic_vector(RDBITS-1 downto 0); -- read data 2
};


struct cctrltype
{
  uint32 burst;//  : std_ulogic;				-- icache burst enable
  uint32 dfrz;//   : std_ulogic;				-- dcache freeze enable
  uint32 ifrz;//   : std_ulogic;				-- icache freeze enable
  uint32 dsnoop;// : std_ulogic;				-- data cache snooping
  uint32 dcs;//    : std_logic_vector(1 downto 0);	-- dcache state
  uint32 ics;//    : std_logic_vector(1 downto 0);	-- icache state
};

struct icache_in_type
{
  uint32 rpc;//              : std_logic_vector(31 downto 0); -- raw address (npc)
  uint32 fpc;//              : std_logic_vector(31 downto 0); -- latched address (fpc)
  uint32 dpc;//              : std_logic_vector(31 downto 0); -- latched address (dpc)
  uint32 rbranch;//          : std_ulogic;      -- Instruction branch
  uint32 fbranch;//          : std_ulogic;      -- Instruction branch
  uint32 inull;//            : std_ulogic;      -- instruction nullify
  uint32 su;//               : std_ulogic;      -- super-user
  uint32 flush;//            : std_ulogic;      -- flush icache
  uint32 flushl;//           : std_ulogic;                        -- flush line
  uint32 fline;//            : std_logic_vector(31 downto 3);     -- flush line offset
  uint32 pnull;//            : std_ulogic;
};

struct icache_out_type
{
  cdatatype data;//             : cdatatype;
  uint32 set;//              : std_logic_vector(1 downto 0);
  uint32 mexc;//             : std_ulogic;
  uint32 hold;//             : std_ulogic;
  uint32 flush;//            : std_ulogic;			-- flush in progress
  uint32 diagrdy;//          : std_ulogic;			-- diagnostic access ready
  uint32 diagdata;//         : std_logic_vector(IDBITS-1 downto 0);-- diagnostic data
  uint32 mds;//              : std_ulogic;			-- memory data strobe
  uint32 cfg;//              : std_logic_vector(31 downto 0);
  uint32 idle;//             : std_ulogic;			-- idle mode
  l3_cstat_type cstat;//            : l3_cstat_type;
};

struct icdiag_in_type
{
  uint32 addr;//             : std_logic_vector(31 downto 0); -- memory stage address
  uint32 enable;//           : std_ulogic;
  uint32 read;//             : std_ulogic;
  uint32 tag;//              : std_ulogic;
  uint32 ctx;//              : std_ulogic;
  uint32 flush;//            : std_ulogic;
  uint32 ilramen;//          : std_ulogic;
  cctrltype cctrl;//		   : cctrltype;
  uint32 pflush;//           : std_ulogic;
  uint32 pflushaddr;//       : std_logic_vector(VA_I_U downto VA_I_D); 
  uint32 pflushtyp;//        : std_ulogic;
  uint32 ilock;//            : std_logic_vector(0 to 3); 
  uint32 scanen;//           : std_ulogic;
};

struct dcache_in_type
{
  uint32 asi;//              : std_logic_vector(7 downto 0); 
  uint32 maddress;//         : std_logic_vector(31 downto 0); 
  uint32 eaddress;//         : std_logic_vector(31 downto 0); 
  uint32 edata;//            : std_logic_vector(31 downto 0); 
  uint32 size;//             : std_logic_vector(1 downto 0);
  uint32 enaddr;//           : std_ulogic;
  uint32 eenaddr;//          : std_ulogic;
  uint32 nullify;//          : std_ulogic;
  uint32 lock;//             : std_ulogic;
  uint32 read;//             : std_ulogic;
  uint32 write;//            : std_ulogic;
  uint32 flush;//            : std_ulogic;
  uint32 flushl;//           : std_ulogic;                        -- flush line  
  uint32 dsuen;//            : std_ulogic;
  uint32 msu;//              : std_ulogic;                   -- memory stage supervisor
  uint32 esu;//              : std_ulogic;                   -- execution stage supervisor
  uint32 intack;//           : std_ulogic;
};

struct dcache_out_type
{
  cdatatype data;//             : cdatatype;
  uint32 set;//              : std_logic_vector(1 downto 0);
  uint32 mexc;//             : std_ulogic;
  uint32 hold;//             : std_ulogic;
  uint32 mds;//              : std_ulogic;
  uint32 werr;//             : std_ulogic;	
  icdiag_in_type icdiag;//           : icdiag_in_type;
  uint32 cache;//            : std_ulogic;
  uint32 idle;//             : std_ulogic;			-- idle mode
  uint32 scanen;//           : std_ulogic;
  uint32 testen;//           : std_ulogic;
};


