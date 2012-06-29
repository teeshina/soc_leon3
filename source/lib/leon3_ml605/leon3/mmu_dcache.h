#pragma once

#include "stdtypes.h"
#include "dff.h"
#include "leon3_ml605\config.h"

#include "leon3_ml605\leon3\libiu.h"
#include "leon3_ml605\leon3\libcache.h"
#include "leon3_ml605\amba\amba.h"
#include "leon3_ml605\leon3\mmuiface.h"

class mmu_dcache
{
  friend class dbg;
  private:
    enum rdatatype {dtag, ddata, dddata, dctx, icache, memory, sysr , misc, mmusnoop_dtag};  //-- sources during cache read
    enum vmasktype {clearone, clearall, merge, tnew};  //-- valid bits operation

    struct valid_type
    {
      uint32 arr[CFG_DLINE];// is array (0 to DSETS-1) of std_logic_vector(dlinesize - 1 downto 0);
    };
    

    struct write_buffer_type //-- write buffer 
    {
      uint32 addr;
      uint32 data1;
      uint32 data2;// : std_logic_vector(31 downto 0);
      uint32 size : 2;//: std_logic_vector(1 downto 0);
      uint32 asi  : 4;//: std_logic_vector(3 downto 0);
      uint32 read : 1;//: std_logic;
      uint32 lock : 1;// : std_logic;
    };

    enum dstatetype {idle, wread, rtrans, wwrite, wtrans, wflush, asi_idtag,dblwrite, loadpend};

    struct dcache_control_type//-- all registers
    {
      uint32 read   : 1;//std_logic;         -- access direction
      uint32 size   : 2;//std_logic_vector(1 downto 0);      -- access size
      uint32 req    : 1;
      uint32 burst  : 1;
      uint32 holdn  : 1;
      uint32 nomds  : 1;
      uint32 stpend : 1;// : std_logic;
      uint32 xaddress;// : std_logic_vector(31 downto 0);   -- common address buffer
      uint32 paddress;// : std_logic_vector(31 downto 0);   -- physical address buffer
      uint32 faddr;// : std_logic_vector(DOFFSET_BITS - 1 downto 0);  -- flush address
      valid_type valid;// : valid_type; --std_logic_vector(dlinesize - 1 downto 0); -- registered valid bits
      dstatetype dstate;// : dstatetype;      -- FSM vector
      uint32 hit     : 1;//std_logic;
      uint32 flush   : 1;//std_logic;        -- flush in progress
      uint32 flush2  : 1;//std_logic;        -- flush in progress
      uint32 mexc    : 1;//std_logic;        -- latched mexc
      write_buffer_type wb;//    : write_buffer_type;      -- write buffer
      uint32 asi           : 5;//std_logic_vector(4 downto 0);
      uint32 icenable      : 1;//std_logic;        -- icache diag access
      uint32 rndcnt;//        : std_logic_vector(log2x(DSETS)-1 downto 0); -- replace counter
      uint32 setrepl;//       : std_logic_vector(log2x(DSETS)-1 downto 0); -- set to replace
      uint32 lrr           : 1;//std_logic;            
      uint32 dsuset;//        : std_logic_vector(log2x(DSETS)-1 downto 0);
      uint32 lock          : 1;//std_logic;
      uint32 lramrd        : 1;//std_ulogic;
      uint32 ilramen       : 1;//std_logic;
      cctrltype cctrl;//      : cctrltype;
      uint32 cctrlwr       : 1;//std_ulogic;

      mmctrl_type1 mmctrl1;//       : mmctrl_type1;
      uint32 mmctrl1wr     : 1;//std_logic;
     
      uint32 pflush        : 1;//std_logic;
      uint32 pflushr       : 1;//std_logic;
      uint32 pflushaddr;//    : std_logic_vector(VA_I_U downto VA_I_D);
      uint32 pflushtyp     : 1;//std_logic;
      uint32 vaddr;//         : std_logic_vector(31 downto 0);
      uint32 ready         : 1;//std_logic;
      uint32 wbinit        : 1;//std_logic;
      uint32 cache         : 1;//std_logic;
      uint32 dlock         : 1;//std_logic;
      uint32 su            : 1;//std_logic;
      uint32 dblwdata      : 1;//std_logic;

      uint32 trans_op      : 1;//std_logic;
      uint32 flush_op      : 1;//std_logic;
      uint32 diag_op       : 1;//std_logic;
    };

    struct snoop_reg_type//-- snoop control registers
    {
      uint32 snoop   : 1;//std_logic;        -- snoop access to tags
      uint32 addr;//    : std_logic_vector(TAG_HIGH downto OFFSET_LOW);-- snoop tag address
    };

    typedef uint32 snoop_hit_bits_type;
    struct snoop_hit_reg_type
    {
      snoop_hit_bits_type *hit;//     : // is array (0 to 2**DOFFSET_BITS-1) of std_logic_vector(0 to DSETS-1);                              -- snoop hit bits  
      uint32 taddr;//   : std_logic_vector(OFFSET_HIGH downto OFFSET_LOW);  -- saved tag clear address
      uint32 hitaddr;// : std_logic_vector(OFFSET_HIGH downto OFFSET_LOW);  -- saved tag hit address
      uint32 clear   : CFG_DSETS;//std_logic_vector(0 to DSETS-1);               -- clear snoop hit
      uint32 snhit   : CFG_DSETS;//std_logic_vector(0 to DSETS-1);               -- set on AHB snoop hit
      uint32 snmiss  : CFG_DSETS;//std_logic_vector(0 to DSETS-1);               -- set on cache miss due to snoop hit
      snoop_hit_reg_type()
      { 
        taddr = hitaddr = 0;
        clear = snhit = snmiss = 0;
        hit = (snoop_hit_bits_type*)malloc((0x1<<DOFFSET_BITS)*sizeof(snoop_hit_bits_type));
      }
      ~snoop_hit_reg_type() {free(hit);}
      
      snoop_hit_reg_type &operator=( const snoop_hit_reg_type &other)
      {
        if(this==&other) return *this;
        memcpy(hit, other.hit, (0x1<<DOFFSET_BITS)*sizeof(snoop_hit_bits_type));
        taddr   = other.taddr;
        hitaddr = other.hitaddr;
        clear   = other.clear;
        snhit   = other.snhit;
        snmiss  = other.snmiss;
        return *this;
      }
    };


    typedef uint32 lru_type;// is std_logic_vector(DLRUBITS-1 downto 0);
    struct par_type
    {
      uint32 arr[CFG_DSETS];// is array (0 to DSETS-1) of std_logic_vector(1 downto 0);
    };

    struct lru_reg_type
    {
      uint32 write : 1;//std_logic;
      uint32 waddr;// : std_logic_vector(DOFFSET_BITS-1 downto 0);
      uint32 set;//   :  std_logic_vector(SETBITS-1 downto 0); --integer range 0 to DSETS-1;
      lru_type *lru;//   : lru_array;
      lru_reg_type()
      {
        write = 0;
        waddr = 0;
        set   = 0;
        int32 size = (0x1<<DOFFSET_BITS);
        lru = (lru_type*)malloc( (0x1<<DOFFSET_BITS)*sizeof(lru_type) );
      }
      ~lru_reg_type() {free(lru);}
      lru_reg_type &operator=(const lru_reg_type &other)
      {
        if(this==&other) return *this;
        write = other.write;
        waddr = other.waddr;
        set = other.set;
        memcpy( lru, other.lru, (0x1<<DOFFSET_BITS)*sizeof(lru_type) );
        return *this;
      }
    };


    typedef uint32 lock_type;// is std_logic_vector(0 to DSETS-1);

    dcache_control_type c;// : dcache_control_type;  -- r is registers, c is combinational
    TDFF<dcache_control_type> r;// : dcache_control_type;  -- r is registers, c is combinational
    TDFFx<snoop_reg_type> rs;
    snoop_reg_type cs;// : snoop_reg_type;   -- rs is registers, cs is combinational
    snoop_hit_reg_type ch;// : snoop_hit_reg_type; -- rs is registers, cs is combinational
    TDFFx<snoop_hit_reg_type> rh;// : snoop_hit_reg_type; -- rs is registers, cs is combinational
    lru_reg_type cl;// : lru_reg_type;           -- rl is registers, cl is combinational
    TDFFx<lru_reg_type> rl;// : lru_reg_type;           -- rl is registers, cl is combinational

    dcram_out_type dcramov;// : dcram_out_type;
    rdatatype rdatasel;// : rdatatype;
    uint32 maddress;// : std_logic_vector(31 downto 0);
    uint32 maddrlow   : 2;//std_logic_vector(1 downto 0);
    uint32 edata;// : std_logic_vector(31 downto 0);
    uint32 size       : 2;//std_logic_vector(1 downto 0);
    uint32 read       : 1;//std_logic;
    uint32 twrite     : 1;
    uint32 tpwrite    : 1;
    uint32 tdiagwrite : 1;
    uint32 ddiagwrite : 1;
    uint32 dwrite     : 1;//std_logic;
    uint32 taddr;// : std_logic_vector(OFFSET_HIGH  downto LINE_LOW); -- tag address
    uint32 newtag;// : std_logic_vector(TAG_HIGH  downto TAG_LOW); -- new tag
    uint32 newptag;// : std_logic_vector(TAG_HIGH  downto TAG_LOW); -- new tag
    uint32 align_data;// : std_logic_vector(31 downto 0); -- aligned data
    cdatatype ddatainv;
    cdatatype rdatav;
    cdatatype align_datav;// : cdatatype;
    uint32 rdata;// : std_logic_vector(31 downto 0);
    
    uint32 vmaskraw;// : std_logic_vector((dlinesize -1) downto 0);
    valid_type vmask;// : valid_type; --std_logic_vector((dlinesize -1) downto 0);
    uint32 vmaskdbl;// : std_logic_vector((dlinesize/2 -1) downto 0);
    uint32 enable     : 4;
    uint32 senable    : 4;
    uint32 scanen     : 4;//std_logic_vector(0 to 3);
    uint32 mds        : 1;//std_logic;
    uint32  mexc      : 1;//std_logic;
    uint32 hit        : 1;
    uint32 valid      : 1;
    uint32 validraw   : 1;
    uint32 forcemiss  : 1;//std_logic;
    uint32 flush      : 1;//std_logic;
    uint32 iflush     : 1;//std_logic;
    dcache_control_type v;// : dcache_control_type;
    uint32 eholdn     : 1;//std_logic;        -- external hold
    uint32 snoopwe    : 1;//std_logic;
    uint32 hcache     : 1;//std_logic;
    uint32 lramcs     : 1;
    uint32 lramen     : 1;
    uint32 lramrd     : 1;
    uint32 lramwr     : 1;
    uint32 ilramen    : 1;//std_logic;
    uint32 snoopaddr;// : std_logic_vector(OFFSET_HIGH downto OFFSET_LOW);
    snoop_reg_type vs;// : snoop_reg_type;
    snoop_hit_reg_type vh;// : snoop_hit_reg_type;
    uint32 dsudata;//   : std_logic_vector(31 downto 0);
    uint32 set        : CFG_DSETS;//integer range 0 to DSETS-1;
    uint32 ddset      : MAXSETS;//integer range 0 to MAXSETS-1;
    uint32 snoopset   : CFG_DSETS;//integer range 0 to DSETS-1;
    uint32 validv     : MAXSETS;
    uint32 hitv       : MAXSETS;
    uint32 validrawv  : MAXSETS;//std_logic_vector(0 to MAXSETS-1);
    uint32 csnoopwe   : MAXSETS;//std_logic_vector(0 to MAXSETS-1);
    uint32 ctwrite    : MAXSETS;
    uint32 ctpwrite   : MAXSETS;
    uint32 cdwrite    : MAXSETS;//std_logic_vector(0 to MAXSETS-1);
    uint32 setrepl;//    : std_logic_vector(log2x(DSETS)-1 downto 0);
    uint32 wlrr       : 4;//std_logic_vector(0 to 3);
    lru_reg_type vl;// : lru_reg_type;
    uint32 diagset;// : std_logic_vector(TAG_LOW + SETBITS -1 downto TAG_LOW);
    uint32 lock       : CFG_DSETS;//std_logic_vector(0 to DSETS-1);
    uint32 wlock      : MAXSETS;//std_logic_vector(0 to MAXSETS-1);
    uint32 snoophit   : CFG_DSETS;//std_logic_vector(0 to DSETS-1);
    uint32 laddr;// : std_logic_vector(31  downto 0); -- local ram addr
    cdatatype tag;// : cdatatype; --std_logic_vector(31  downto 0);
    cdatatype ptag;// : cdatatype; --std_logic_vector(31  downto 0);
    uint32 rlramrd    : 1;//std_logic;
    ctxdatatype ctx;// : ctxdatatype;
   
    uint32 miscdata;//  : std_logic_vector(31 downto 0);
    uint32 pflush     : 1;//std_logic;
    uint32 pflushaddr;// : std_logic_vector(VA_I_U downto VA_I_D);
    uint32 pflushtyp  : 1;//std_logic;
    uint32 pftag;// : std_logic_vector(31 downto 2);

    uint32 mmudci_fsread    : 1;
    uint32 tagclear         : 1;//std_logic;
    uint32 mmudci_trans_op  : 1;//std_logic;
    uint32 mmudci_flush_op  : 1;//std_logic;
    uint32 mmudci_wb_op     : 1;//std_logic;
    uint32 mmudci_diag_op   : 1;//std_logic;
    uint32 mmudci_su        : 1;//std_logic;
    uint32 mmudci_read      : 1;//std_logic;
    uint32 su               : 1;//std_logic;
    uint32 mmuisdis         : 1;//std_logic;
    uint32 ctxp             : 1;//std_logic;
    uint32 sidle            : 1;//std_logic;
    
    uint32 mmudci_transdata_data;// : std_logic_vector(31 downto 0);
    uint32 paddress;// : std_logic_vector(31 downto 0);    -- physical address buffer
    uint32 pagesize;// : integer range 0 to 3;


  public:
    
    void Update(uint32 rst,// : in  std_logic;
                SClock clk,// : in  std_logic;
                dcache_in_type &dci,// : in  dcache_in_type;
                dcache_out_type &dco,// : out dcache_out_type;
                icache_out_type &ico,// : in  icache_out_type;
                memory_dc_in_type &mcdi,// : out memory_dc_in_type;
                memory_dc_out_type &mcdo,// : in  memory_dc_out_type;
                ahb_slv_in_type &ahbsi,// : in  ahb_slv_in_type;
                dcram_in_type &dcrami,// : out dcram_in_type;
                dcram_out_type &dcramo,// : in  dcram_out_type;
                uint32 fpuholdn,// : in  std_logic;
                mmudc_in_type &mmudci,// : out mmudc_in_type;
                mmudc_out_type &mmudco,// : in mmudc_out_type;
                SClock sclk);// : in  std_ulogic
    
    uint32 lru_set(lru_type lru,// : lru_type,
                   lock_type lock);// : lock_type) return std_logic_vector is
                   
    lru_type lru_calc(lru_type lru,// : lru_type;
                      uint32 set);// : integer) return lru_type is

    
    void ClkUpdate()
    {
      r.ClkUpdate();
      rl.ClkUpdate();
      rs.ClkUpdate();
      rh.ClkUpdate();
    }
};

