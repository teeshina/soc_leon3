#pragma once

#include "stdtypes.h"
#include "dff.h"
#include "leon3_ml605\config.h"

#include "leon3_ml605\leon3\libiu.h"
#include "leon3_ml605\leon3\libcache.h"
#include "leon3_ml605\leon3\mmuiface.h"

class mmu_icache
{
  friend class dbg;
  friend struct lru_reg_type;
  private:
    typedef uint32 lru_type;// is std_logic_vector(ILRUBITS-1 downto 0);
    typedef uint32 lock_type;// is std_logic_vector(0 to ISETS-1);

    enum rdatatype {itag, idata, memory}; // -- sources during cache read

    struct par_type
    {
      uint32 arr[CFG_ISETS];// is array (0 to ISETS-1) of std_logic_vector(1 downto 0);
    };
    
    enum istatetype {idle, trans, streaming, stop};
    struct icache_control_type //-- all registers
    {
      uint32 req     : 1;
      uint32 burst   : 1;
      uint32 holdn   : 1 ;// : std_logic;
      uint32 overrun : 1;//       : std_logic;      -- 
      uint32 underrun: 1;//      : std_logic;      -- 
      istatetype istate;//  : istatetype;             -- FSM
      uint32 waddress;//      : std_logic_vector(31 downto PCLOW); -- write address buffer
      uint32 vaddress;//      : std_logic_vector(31 downto PCLOW); -- virtual address buffer
      uint32 valid;//         : std_logic_vector(ILINE_SIZE-1 downto 0); -- valid bits
      uint32 hit        : 1;//           : std_logic;
      uint32 su         : 1;//    : std_logic;
      uint32 flush      : 1;//   : std_logic;        -- flush in progress
      uint32 flush2     : 1;//  : std_logic;        -- flush in progress
      uint32 flush3     : 1;//  : std_logic;        -- flush in progress
      uint32 faddr;//   : std_logic_vector(IOFFSET_BITS - 1 downto 0);  -- flush address
      uint32 diagrdy    : 1;//   : std_logic;
      uint32 rndcnt;//        : std_logic_vector(log2x(ISETS)-1 downto 0); -- replace counter
      uint32 lrr        : 1;//           : std_logic;
      uint32 setrepl;//       : std_logic_vector(log2x(ISETS)-1 downto 0); -- set to replace
      uint32 diagset;//       : std_logic_vector(log2x(ISETS)-1 downto 0);
      uint32 lock       : 1;//          : std_logic;

      uint32 pflush     : 1;//        : std_logic;
      uint32 pflushr    : 1;//       : std_logic;
      uint32 pflushaddr;//    : std_logic_vector(VA_I_U downto VA_I_D);
      uint32 pflushtyp  : 1;//     : std_logic;
      uint32 cache      : 1;//         : std_logic;
      uint32 trans_op   : 1;//      : std_logic;
    };

    struct lru_reg_type
    {
      uint32 write : 1;//: std_logic;
      uint32 waddr;// : std_logic_vector(IOFFSET_BITS-1 downto 0);
      uint32 set;//   : std_logic_vector(SETBITS-1 downto 0); --integer range 0 to ISETS-1;
      //lru_array *lru;//   : lru_array;
      lru_type *arr;
      lru_reg_type() 
      { 
        write = 0;
        waddr = 0;
        set   = 0;
        int32 size = (0x1<<IOFFSET_BITS);
        arr = (lru_type*)malloc( (0x1<<IOFFSET_BITS)*sizeof(lru_type) );
      }
      ~lru_reg_type() { free(arr); }
      lru_reg_type &operator=(const lru_reg_type& other)
      {
        if(this==&other) return *this;
        write = other.write;
        waddr = other.waddr;
        set = other.set;
        memcpy( arr,other.arr,(0x1<<IOFFSET_BITS)*sizeof(lru_type) );
        return *this;
      }
    };

    icache_control_type c;// : icache_control_type;  -- r is registers, c is combinational
    TDFF<icache_control_type> r;
    lru_reg_type cl;// : lru_reg_type;           -- rl is registers, cl is combinational
    TDFFx<lru_reg_type> rl;

    rdatatype rdatasel;// : rdatatype;
    uint32 twrite, diagen, dwrite : 1;//std_logic;
    uint32 taddr;// : std_logic_vector(TAG_HIGH  downto LINE_LOW); -- tag address
    uint32 wtag;// : std_logic_vector(TAG_HIGH downto TAG_LOW); -- write tag value
    uint32 ddatain;// : std_logic_vector(31 downto 0);
    cdatatype rdata;// : cdatatype;
    uint32 diagdata;// : std_logic_vector(31 downto 0);
    uint32 vmaskraw, vmask;// : std_logic_vector((ILINE_SIZE -1) downto 0);
    uint32 xaddr_inc;// : std_logic_vector((ILINE_BITS -1) downto 0);
    uint32 lastline, nlastline, nnlastline : 1;//std_logic;
    uint32 enable : 1;//std_logic;
    uint32 error : 1;//std_logic;
    uint32 whit, hit, valid : 1;//std_logic;
    uint32 cacheon  : 1;//std_logic;
    icache_control_type v;// : icache_control_type;
    uint32 branch : 1;//  : std_logic;
    uint32 eholdn : 1;//  : std_logic;
    uint32 mds, write : 1;//  : std_logic;
    uint32 tparerr, dparerr;//  : std_logic_vector(0 to ISETS-1);
    uint32 set;//     : integer range 0 to MAXSETS-1;
    uint32 setrepl;// : std_logic_vector(log2x(ISETS)-1 downto 0); -- set to replace
    uint32 ctwrite, cdwrite, validv;// : std_logic_vector(0 to MAXSETS-1);
    uint32 wlrr : 1;// : std_logic;
    lru_reg_type vl;// : lru_reg_type;
    uint32 vdiagset, rdiagset;// : integer range 0 to ISETS-1;
    uint32 lock;// : std_logic_vector(0 to ISETS-1);
    uint32 wlock : 1;
    uint32 sidle : 1;// : std_logic;
    cdatatype tag;// : cdatatype;
    uint32 lramacc, ilramwr, lramcs : 1;//  : std_ulogic;

    uint32 pftag;// : std_logic_vector(31 downto 2);
    uint32 mmuici_trans_op : 1;// : std_logic;
    uint32 mmuici_su       : 1;// : std_logic;


  public:
  
    void Update(uint32 rst,// : in  std_logic;
                SClock clk,// : in  std_logic;
                icache_in_type &ici,// : in  icache_in_type;
                icache_out_type &ico,// : out icache_out_type;
                dcache_in_type &dci,// : in  dcache_in_type;
                dcache_out_type &dco,// : in  dcache_out_type;
                memory_ic_in_type &mcii,// : out memory_ic_in_type;
                memory_ic_out_type &mcio,// : in  memory_ic_out_type;
                icram_in_type &icrami,// : out icram_in_type;
                icram_out_type &icramo,// : in  icram_out_type;
                uint32 fpuholdn,// : in  std_logic;
                mmudc_in_type &mmudci,// : in  mmudc_in_type;
                mmuic_in_type &mmuici,// : out mmuic_in_type;
                mmuic_out_type &mmuico);// : in mmuic_out_type))


    uint32 lru_set ( lru_type lru, lock_type lock);// return std_logic_vector is    
    lru_type lru_calc(lru_type lru,uint32 set);// : integer) return lru_type is
    
    void ClkUpdate()
    {
      r.ClkUpdate();
      rl.ClkUpdate();
    }
};

