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
use gaisler.jtag.all;
use gaisler.leon3.all;
use gaisler.uart.all;
library work;
use work.all;
use work.config.all;
use work.util_tb.all;



entity soc_leon3_tb is

  constant CLK_HPERIOD : time := 10 ps;
  constant STRING_SIZE : integer := 113; -- string size = index of the last element

end soc_leon3_tb;
architecture behavior of soc_leon3_tb is
  
  component soc_leon3 port 
  ( 
    inRst       : in std_logic; -- button "Center"
    inDsuBreak  : in std_logic; -- button "North
    inSysClk_p  : in std_logic;
    inSysClk_n  : in std_logic;
    inCTS   : in std_logic;
    inRX    : in std_logic;
    outRTS  : out std_logic;
    outTX   : out std_logic;
    inDIP   : in std_ulogic_vector(7 downto 0);
    outLED  : out std_ulogic_vector(7 downto 0)
  );
  end component;


  -- input/output signals:
  signal inNRst       : std_logic:= '0';
  signal inClk        : std_logic:= '0';
  signal RTS          : std_logic;
  signal TX           : std_logic;
  signal LED          : std_ulogic_vector(7 downto 0);

  signal ch_dbgo : l3_debug_out_vector(0 to CFG_NCPU-1);
  signal t_v_x_nerror  : std_logic;
  signal t_rp_error  : std_logic;
  signal t_dummy  : std_logic;
  signal t_r_f_pc  : std_logic_vector(31 downto 0);
  signal in_ir_addr  : std_logic_vector(31 downto 0);
  signal t_a_ctrl_inst  : std_logic_vector(31 downto 0);
  signal dsu_status  : std_logic_vector(11 downto 0);

  
  signal S: std_logic_vector(STRING_SIZE-1 downto 0);
  signal U: std_ulogic_vector(STRING_SIZE-1 downto 0);

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
    file InputFile:TEXT is "e:/soc_leon3_tb.txt";--open read_mode file_name;
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
  inNRst <= S(0);
  ch_dbgo(0).error <= S(1);
  t_v_x_nerror <= S(2);
  t_rp_error <= S(3);
  t_dummy <= S(4);
  t_r_f_pc <= S(36 downto 5);
  in_ir_addr <= S(68 downto 37);
  t_a_ctrl_inst <= S(100 downto 69);
  dsu_status <= S(112 downto 101);
  
  
  tt : soc_leon3  port map
  ( 
    inRst => inNRst,
    inDsuBreak => '0',
    inSysClk_p  => inClk,
    inSysClk_n  => '0',
    inCTS  => '0',
    inRX  => '0',
    outRTS  => RTS,
    outTX   => TX,
    inDIP    => "00000000",
    outLED  => LED
  );

  

procCheck : process (inNRst,inClk)
begin


  if(rising_edge(inClk) and (iClkCnt>2)) then
    --if(ch_dmai.address/=dmai.address) then print("Err: dmai.address");  iErrCnt:=iErrCnt+1; end if;
  end if;
end process procCheck;


end;
 
 