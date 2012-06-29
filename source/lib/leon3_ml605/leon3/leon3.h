#pragma once

#include "stdtypes.h"
#include "leon3_ml605\config.h"

struct dsu_in_type
{
  uint32 enable : 1;//  : std_ulogic;
  uint32 Break  : 1;//std_ulogic;
};                        

struct dsu_out_type
{
  uint32 active          : 1;//std_ulogic;
  uint32 tstop           : 1;//std_ulogic;
  uint32 pwd             : 16;//std_logic_vector(15 downto 0);
};

struct l3_irq_in_type
{
  uint32 irl;//     : std_logic_vector(3 downto 0);
  uint32 rst;//     : std_ulogic;
  uint32 run;//     : std_ulogic;
  uint32 rstvec;//  : std_logic_vector(31 downto 12);
  uint32 iact;//        : std_ulogic;
  uint32 index;//   : std_logic_vector(3 downto 0);
  uint32 hrdrst;//      : std_ulogic;
};

struct irq_in_vector
{
  l3_irq_in_type arr[CFG_NCPU];
};

struct l3_irq_out_type
{
  uint32 intack;//  : std_ulogic;
  uint32 irl;//   : std_logic_vector(3 downto 0);
  uint32 pwd;//         : std_ulogic;
  uint32 fpen;//        : std_ulogic;
  uint32 idle;//        : std_ulogic;
};

struct irq_out_vector
{
  l3_irq_out_type arr[CFG_NCPU];
};

struct l3_debug_in_type
{
  uint32 dsuen;//   : std_ulogic;  -- DSU enable
  uint32 denable;// : std_ulogic;  -- diagnostic register access enable
  uint32 dbreak;//  : std_ulogic;  -- debug break-in
  uint32 step;//    : std_ulogic;  -- single step    
  uint32 halt;//    : std_ulogic;  -- halt processor
  uint32 reset;//   : std_ulogic;  -- reset processor
  uint32 dwrite;//  : std_ulogic;  -- read/write
  uint32 daddr;//   : std_logic_vector(23 downto 2); -- diagnostic address
  uint32 ddata;//   : std_logic_vector(31 downto 0); -- diagnostic data
  uint32 btrapa;//  : std_ulogic;	   -- break on IU trap
  uint32 btrape;//  : std_ulogic;	-- break on IU trap
  uint32 berror;//  : std_ulogic;	-- break on IU error mode
  uint32 bwatch;//  : std_ulogic;	-- break on IU watchpoint
  uint32 bsoft;//   : std_ulogic;	-- break on software breakpoint (TA 1)
  uint32 tenable;// : std_ulogic;
  uint32 timer;//   :  std_logic_vector(30 downto 0);                                                -- 
};

struct l3_debug_in_vector
{
  l3_debug_in_type arr[CFG_NCPU];
};

struct l3_cstat_type
{
  uint32 cmiss;//   : std_ulogic;			-- cache miss
  uint32 tmiss;//   : std_ulogic;			-- TLB miss
  uint32 chold;//   : std_ulogic;			-- cache hold
  uint32 mhold;//   : std_ulogic;			-- cache mmu hold
};

const l3_cstat_type cstat_none = {0, 0, 0, 0};

struct l3_debug_out_type
{
  uint32 data;//    : std_logic_vector(31 downto 0);
  uint32 crdy;//    : std_ulogic;
  uint32 dsu;//     : std_ulogic;
  uint32 dsumode;// : std_ulogic;
  uint32 error;//   : std_ulogic;
  uint32 halt;//    : std_ulogic;
  uint32 pwd;//     : std_ulogic;
  uint32 idle;//    : std_ulogic;
  uint32 ipend;//   : std_ulogic;
  uint32 icnt;//    : std_ulogic;
  uint32 fcnt;//    : std_ulogic;
  uint32 optype;//  : std_logic_vector(5 downto 0); -- instruction type
  uint32 bpmiss;//  : std_ulogic;     -- branch predict miss
  l3_cstat_type istat;//   : l3_cstat_type;
  l3_cstat_type dstat;//   : l3_cstat_type;
  uint32 wbhold;//  : std_ulogic;     -- write buffer hold
  uint32 su;//      : std_ulogic;     -- supervisor state
};

struct l3_debug_out_vector
{
  l3_debug_out_type arr[CFG_NCPU];
};

struct tracebuf_in_type
{
  uint32 addr;//             : std_logic_vector(11 downto 0);
  uint64 data[2];//             : std_logic_vector(127 downto 0);
  uint32 enable;//           : std_logic;
  uint32 write;//            : std_logic_vector(3 downto 0);
  uint32 diag;//             : std_logic_vector(3 downto 0);
};

struct tracebuf_out_type
{
  uint64 data[2];//             : std_logic_vector(127 downto 0);
};

struct fpc_pipeline_control_type
{
  uint32 pc;//    : std_logic_vector(31 downto 0);
  uint32 inst;//  : std_logic_vector(31 downto 0);
  uint32 cnt;//   : std_logic_vector(1 downto 0);
  uint32 trap;//  : std_ulogic;
  uint32 annul;// : std_ulogic;
  uint32 pv;//    : std_ulogic;
};


struct fpc_debug_in_type
{
  uint32 enable;// : std_ulogic;
  uint32 write;//  : std_ulogic;
  uint32 fsr;//    : std_ulogic;                            -- FSR access
  uint32 addr;//   : std_logic_vector(4 downto 0);
  uint32 data;//   : std_logic_vector(31 downto 0);
};

struct fpc_debug_out_type
{
  uint32 data;//   : std_logic_vector(31 downto 0);
};  

const fpc_debug_out_type fpc_debug_none = {0};


struct fpc_in_type
{
  uint32 flush;//   : std_ulogic;       -- pipeline flush
  uint32 exack;//     : std_ulogic;       -- FP exception acknowledge
  uint32 a_rs1;//   : std_logic_vector(4 downto 0);
  fpc_pipeline_control_type d;//             : fpc_pipeline_control_type;
  fpc_pipeline_control_type a;//             : fpc_pipeline_control_type;
  fpc_pipeline_control_type e;//             : fpc_pipeline_control_type;
  fpc_pipeline_control_type m;//             : fpc_pipeline_control_type;
  fpc_pipeline_control_type x;//             : fpc_pipeline_control_type;    
  uint32 lddata;//        : std_logic_vector(31 downto 0);     -- load data
  fpc_debug_in_type dbg;//           : fpc_debug_in_type;               -- debug signals
};

struct fpc_out_type
{
  uint32 data;//          : std_logic_vector(31 downto 0); -- store data
  uint32 exc;//           : std_logic;       -- FP exception
  uint32 cc;//            : std_logic_vector(1 downto 0);  -- FP condition codes
  uint32 ccv;//           : std_ulogic;      -- FP condition codes valid
  uint32 ldlock;//        : std_logic;       -- FP pipeline hold
  uint32 holdn;//          : std_ulogic;
  fpc_debug_out_type dbg;//           : fpc_debug_out_type;             -- FP debug signals    
};

const fpc_out_type  fpc_out_none = {0, 0, 0, 1, 0, 1, fpc_debug_none}; 

