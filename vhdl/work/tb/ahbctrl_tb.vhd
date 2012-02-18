library ieee;
use ieee.std_logic_1164.all;
library std;
use std.textio.all;
library grlib;
use grlib.amba.all;
use grlib.stdlib.all;
use grlib.devices.all;
library techmap;
use techmap.gencomp.all;
library gaisler;
use gaisler.misc.all;
use gaisler.libjtagcom.all;
use gaisler.jtag.all;
library work;
use work.config.all;
use work.util_tb.all;


entity ahbctrl_tb is
  constant CLK_HPERIOD : time := 10 ps;
  constant STRING_SIZE : integer := 39; -- string size = index of the last element

  constant REREAD : integer := 1;
  constant REVISION : integer := REREAD;
  constant TAPSEL   : integer := has_tapsel(CFG_FABTECH);
  constant maxahbm : integer := CFG_NCPU+CFG_AHB_UART+CFG_GRETH+CFG_AHB_JTAG+CFG_SVGA_ENABLE;
  constant IOAEN : integer := 1;--CONFIG_DDRSP;--CFG_DDRSP;
  constant LEON3_VERSION : integer := 0;
  constant DSU3_VERSION : integer := 1;
  constant tst_nahbm : integer := 2;--maxahbm,
	constant tst_nahbs : integer := 4;--8
end ahbctrl_tb;

architecture behavior of ahbctrl_tb is

  type l0_type is array (0 to 15) of std_logic_vector(2 downto 0);
  type l1_type is array (0 to 7) of std_logic_vector(3 downto 0);
  type l2_type is array (0 to 3) of std_logic_vector(4 downto 0);
  type l3_type is array (0 to 1) of std_logic_vector(5 downto 0);

  type tztab_type is array (0 to 15) of std_logic_vector(2 downto 0);


  --returns the index number of the highest priority request
  --signal in the two lsb bits when indexed with a 4-bit
  --request vector with the highest priority signal on the
  --lsb. the returned msb bit indicates if a request was
  --active ('1' = no request active corresponds to "0000")
  constant tztab : tztab_type := ("100", "000", "001", "000",
                                  "010", "000", "001", "000",
                                  "011", "000", "001", "000",
                                  "010", "000", "001", "000");

  function tz(vect_in : std_logic_vector) return std_logic_vector is
    variable vect : std_logic_vector(63 downto 0);
    variable l0 : l0_type;
    variable l1 : l1_type;
    variable l2 : l2_type;
    variable l3 : l3_type;
    variable l4 : std_logic_vector(6 downto 0);
    variable bci_lsb, bci_msb : std_logic_vector(3 downto 0);
    variable bco_lsb, bco_msb : std_logic_vector(2 downto 0);    
    variable sel : std_logic;
  begin

    vect := (others => '1');
    vect(vect_in'length-1 downto 0) := vect_in;

    -- level 0
    for i in 0 to 7 loop
      bci_lsb := vect(8*i+3 downto 8*i);
      bci_msb := vect(8*i+7 downto 8*i+4);
      --lookup the highest priority request in each nibble
      bco_lsb := tztab(conv_integer(bci_lsb));
      bco_msb := tztab(conv_integer(bci_msb));
      --select which of two nibbles contain the highest priority ACTIVE
      --signal, and forward the corresponding vector to the next level
      sel := bco_lsb(2);
      if sel = '0' then l1(i) := '0' & bco_lsb;
      else l1(i) := bco_msb(2) & not bco_msb(2) & bco_msb(1 downto 0); end if;
    end loop;

    -- level 1
    for i in 0 to 3 loop
      sel := l1(2*i)(3);
      --select which of two 8-bit vectors contain the
      --highest priority ACTIVE signal. the msb set at the previous level
      --for each 8-bit slice determines this
      if sel = '0' then l2(i) := '0' & l1(2*i);
      else
        l2(i) := l1(2*i+1)(3) & not l1(2*i+1)(3) & l1(2*i+1)(2 downto 0);
      end if;
    end loop;

    -- level 2
    for i in 0 to 1 loop
      --16-bit vectors, the msb set at the previous level for each 16-bit
      --slice determines the higher priority slice
      sel := l2(2*i)(4);
      if sel = '0' then l3(i) := '0' & l2(2*i);
      else
        l3(i) := l2(2*i+1)(4) & not l2(2*i+1)(4) & l2(2*i+1)(3 downto 0);
      end if;
    end loop;

    --level 3
    --32-bit vectors, the msb set at the previous level for each 32-bit
    --slice determines the higher priority slice
    if l3(0)(5) = '0' then l4 := '0' & l3(0);
    else l4 := l3(1)(5) & not l3(1)(5) & l3(1)(4 downto 0); end if;

    return(l4);    
  end;


  -- input/output signals:
  signal inClk        : std_logic:= '0';
  
  signal in_vect  : std_logic_vector(31 downto 0);
  signal ch_out  : std_logic_vector(6 downto 0);

  
  signal U: std_ulogic_vector(STRING_SIZE-1 downto 0);
  signal S: std_logic_vector(STRING_SIZE-1 downto 0);
  shared variable iClkCnt : integer := 0;
  shared variable iErrCnt : integer := 0;


begin

  -- Process of clock generation
  procClkgen : process
  begin
      inClk <= '0' after CLK_HPERIOD, '1' after 2*CLK_HPERIOD;
      wait for 2*CLK_HPERIOD;
  end process procClkgen;

  -- Process of reading  
  procReadingFile : process
    file InputFile:TEXT is "e:/ahbctrl_tb.txt";--open read_mode file_name;
    variable rdLine: line;  
    variable strLine : string(STRING_SIZE downto 1);
  begin
    while not endfile(InputFile) loop
      readline(InputFile, rdLine);
      read(rdLine, strLine);
      U <= StringToUVector(strLine);
      S <= StringToSVector(strLine);
  
      wait until rising_edge(inClk);
      --wait until falling_edge(inClk);
      iClkCnt := iClkCnt + 1;
    end loop;
  end process procReadingFile;


  -- signal parsment and assignment
  in_vect <= S(31 downto 0);
  ch_out <= S(38 downto 32);
  
  procCheck : process (in_vect)
    variable t_out  : std_logic_vector(6 downto 0);
  begin
    t_out := tz(in_vect);
    if(ch_out/=t_out) then
      print("err: t_out"); iErrCnt:=iErrCnt+1;
    end if;
  end process procCheck;
  
end;
 
 