library ieee;
use ieee.std_logic_1164.all;
library std;
use std.textio.all;
library grlib;
use grlib.sparc.all;
use grlib.stdlib.all;
library techmap;
use techmap.gencomp.all;
library work;
use work.config.all;

package util_tb is

function StringToUVector(inStr: string) return std_ulogic_vector;
function StringToSVector(inStr: string) return std_logic_vector;
function UnsignedToSigned(inUnsigned: std_ulogic_vector) return std_logic_vector;

  constant ISETMSB : integer := log2x(CFG_ISETS)-1;
  constant DSETMSB : integer := log2x(CFG_DSETS)-1;
  constant RFBITS : integer range 6 to 10 := log2(CFG_NWIN+1) + 4;
  constant NWINLOG2   : integer range 1 to 5 := log2(CFG_NWIN);
  constant CWPOPT : boolean := (CFG_NWIN = (2**NWINLOG2));
  constant CWPMIN : std_logic_vector(NWINLOG2-1 downto 0) := (others => '0');
  constant CWPMAX : std_logic_vector(NWINLOG2-1 downto 0) := 
  	conv_std_logic_vector(CFG_NWIN-1, NWINLOG2);
  constant TBUFBITS : integer := 10 + log2(CFG_ITBSZ) - 4;
  

  subtype word is std_logic_vector(31 downto 0);
  subtype pctype is std_logic_vector(31 downto CFG_PCLOW);
  subtype rfatype is std_logic_vector(RFBITS-1 downto 0);
  subtype cwptype is std_logic_vector(NWINLOG2-1 downto 0);
  type icdtype is array (0 to CFG_ISETS-1) of word;
  type dcdtype is array (0 to CFG_DSETS-1) of word;

  type dc_in_type is record
    signed, enaddr, read, write, lock , dsuen : std_ulogic;
    size : std_logic_vector(1 downto 0);
    asi  : std_logic_vector(7 downto 0);    
  end record;
  
  type pipeline_ctrl_type is record
    pc    : pctype;
    inst  : word;
    cnt   : std_logic_vector(1 downto 0);
    rd    : rfatype;
    tt    : std_logic_vector(5 downto 0);
    trap  : std_ulogic;
    annul : std_ulogic;
    wreg  : std_ulogic;
    wicc  : std_ulogic;
    wy    : std_ulogic;
    ld    : std_ulogic;
    pv    : std_ulogic;
    rett  : std_ulogic;
  end record;
  
  type fetch_reg_type is record
    pc     : pctype;
    branch : std_ulogic;
  end record;
  
  type decode_reg_type is record
    pc    : pctype;
    inst  : icdtype;
    cwp   : cwptype;
    set   : std_logic_vector(ISETMSB downto 0);
    mexc  : std_ulogic;
    cnt   : std_logic_vector(1 downto 0);
    pv    : std_ulogic;
    annul : std_ulogic;
    inull : std_ulogic;
    step  : std_ulogic;        
    divrdy: std_ulogic;        
  end record;
  
  type regacc_reg_type is record
    ctrl  : pipeline_ctrl_type;
    rs1   : std_logic_vector(4 downto 0);
    rfa1, rfa2 : rfatype;
    rsel1, rsel2 : std_logic_vector(2 downto 0);
    rfe1, rfe2 : std_ulogic;
    cwp   : cwptype;
    imm   : word;
    ldcheck1 : std_ulogic;
    ldcheck2 : std_ulogic;
    ldchkra : std_ulogic;
    ldchkex : std_ulogic;
    su : std_ulogic;
    et : std_ulogic;
    wovf : std_ulogic;
    wunf : std_ulogic;
    ticc : std_ulogic;
    jmpl : std_ulogic;
    step  : std_ulogic;            
    mulstart : std_ulogic;            
    divstart : std_ulogic;            
    bp, nobp : std_ulogic;
  end record;
  
  type execute_reg_type is record
    ctrl   : pipeline_ctrl_type;
    op1    : word;
    op2    : word;
    aluop  : std_logic_vector(2 downto 0);  	-- Alu operation
    alusel : std_logic_vector(1 downto 0);  	-- Alu result select
    aluadd : std_ulogic;
    alucin : std_ulogic;
    ldbp1, ldbp2 : std_ulogic;
    invop2 : std_ulogic;
    shcnt  : std_logic_vector(4 downto 0);  	-- shift count
    sari   : std_ulogic;				-- shift msb
    shleft : std_ulogic;				-- shift left/right
    ymsb   : std_ulogic;				-- shift left/right
    rd 	   : std_logic_vector(4 downto 0);
    jmpl   : std_ulogic;
    su     : std_ulogic;
    et     : std_ulogic;
    cwp    : cwptype;
    icc    : std_logic_vector(3 downto 0);
    mulstep: std_ulogic;            
    mul    : std_ulogic;            
    mac    : std_ulogic;
    bp     : std_ulogic;
  end record;
  
  type memory_reg_type is record
    ctrl   : pipeline_ctrl_type;
    result : word;
    y      : word;
    icc    : std_logic_vector(3 downto 0);
    nalign : std_ulogic;
    dci	   : dc_in_type;
    werr   : std_ulogic;
    wcwp   : std_ulogic;
    irqen  : std_ulogic;
    irqen2 : std_ulogic;
    mac    : std_ulogic;
    divz   : std_ulogic;
    su     : std_ulogic;
    mul    : std_ulogic;
    casa   : std_ulogic;
    casaz  : std_ulogic;
  end record;

  type exception_state is (run, trap, dsu1, dsu2);
  
  type exception_reg_type is record
    ctrl   : pipeline_ctrl_type;
    result : word;
    y      : word;
    icc    : std_logic_vector( 3 downto 0);
    annul_all : std_ulogic;
    data   : dcdtype;
    set    : std_logic_vector(DSETMSB downto 0);
    mexc   : std_ulogic;
    dci	   : dc_in_type;
    laddr  : std_logic_vector(1 downto 0);
    rstate : exception_state;
    npc    : std_logic_vector(2 downto 0);
    intack : std_ulogic;
    ipend  : std_ulogic;
    mac    : std_ulogic;
    debug  : std_ulogic;
    nerror : std_ulogic;
  end record;

  type dsu_registers is record
    tt      : std_logic_vector(7 downto 0);
    err     : std_ulogic;
    tbufcnt : std_logic_vector(TBUFBITS-1 downto 0);
    asi     : std_logic_vector(7 downto 0);
    crdy    : std_logic_vector(2 downto 1);  -- diag cache access ready
  end record;

  type irestart_register is record
    addr   : pctype;
    pwd    : std_ulogic;
  end record;
  
  type pwd_register_type is record
    pwd    : std_ulogic;
    error  : std_ulogic;
  end record;

  type special_register_type is record
    cwp    : cwptype;                             -- current window pointer
    icc    : std_logic_vector(3 downto 0);	  -- integer condition codes
    tt     : std_logic_vector(7 downto 0);	  -- trap type
    tba    : std_logic_vector(19 downto 0);	  -- trap base address
    wim    : std_logic_vector(CFG_NWIN-1 downto 0);   -- window invalid mask
    pil    : std_logic_vector(3 downto 0);	  -- processor interrupt level
    ec     : std_ulogic;			   -- enable CP 
    ef     : std_ulogic;			   -- enable FP 
    ps     : std_ulogic;			   -- previous supervisor flag
    s      : std_ulogic;			   -- supervisor flag
    et     : std_ulogic;			   -- enable traps
    y      : word;
    asr18  : word;
    svt    : std_ulogic;			   -- enable traps
    dwt    : std_ulogic;			   -- disable write error trap
    dbp    : std_ulogic;			   -- disable branch prediction
  end record;
  
  type write_reg_type is record
    s      : special_register_type;
    result : word;
    wa     : rfatype;
    wreg   : std_ulogic;
    except : std_ulogic;
  end record;

  type registers is record
    f  : fetch_reg_type;
    d  : decode_reg_type;
    a  : regacc_reg_type;
    e  : execute_reg_type;
    m  : memory_reg_type;
    x  : exception_reg_type;
    w  : write_reg_type;
  end record;

  type exception_type is record
    pri   : std_ulogic;
    ill   : std_ulogic;
    fpdis : std_ulogic;
    cpdis : std_ulogic;
    wovf  : std_ulogic;
    wunf  : std_ulogic;
    ticc  : std_ulogic;
  end record;

  type watchpoint_register is record
    addr    : std_logic_vector(31 downto 2);  -- watchpoint address
    mask    : std_logic_vector(31 downto 2);  -- watchpoint mask
    exec    : std_ulogic;			    -- trap on instruction
    load    : std_ulogic;			    -- trap on load
    store   : std_ulogic;			    -- trap on store
  end record;

  type watchpoint_registers is array (0 to 3) of watchpoint_register;

  constant wpr_none : watchpoint_register := (
	zero32(31 downto 2), zero32(31 downto 2), '0', '0', '0');


end;

package body util_tb is

  function StringToUVector(inStr: string) return std_ulogic_vector is
    variable temp: std_ulogic_vector(inStr'range) := (others => 'X');
  begin
    for i in inStr'range loop --
      if(inStr(inStr'high-i+1)='1')    then temp(i) := '1';
      elsif(inStr(inStr'high-i+1)='0') then temp(i) := '0';
      end if;
    end loop;
    return temp(inStr'high downto 1);
  end function StringToUVector;
  -- conversion function
  
  function StringToSVector(inStr: string) return std_logic_vector is
    variable temp: std_logic_vector(inStr'range) := (others => 'X');
  begin
    for i in inStr'range loop --
      if(inStr(inStr'high-i+1)='1')    then temp(i) := '1';
      elsif(inStr(inStr'high-i+1)='0') then temp(i) := '0';
      end if;
    end loop;
    return temp(inStr'high downto 1);
  end function StringToSVector;
  

  function UnsignedToSigned(inUnsigned: std_ulogic_vector) 
    return std_logic_vector is
    variable temp: std_logic_vector(inUnsigned'length-1 downto 0) := (others => 'X');
    variable i: integer:=0;
  begin
    while i < inUnsigned'length loop
      if(inUnsigned(i)='1')    then temp(i) := '1';
      elsif(inUnsigned(i)='0') then temp(i) := '0';
      end if;
      i := i+1;
    end loop;
    return temp;
  end function UnsignedToSigned;

end;
