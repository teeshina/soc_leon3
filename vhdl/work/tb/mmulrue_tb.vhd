
library ieee;
use ieee.std_logic_1164.all;
library std;
use std.textio.all;
library ieee;
library grlib;
use grlib.stdlib.all;
library techmap;
use techmap.gencomp.all;
library gaisler;
use gaisler.libiu.all;
use gaisler.libcache.all;
use gaisler.leon3.all;
use gaisler.mmuconfig.all;
use gaisler.mmuiface.all;
library work;
use work.config.all;
use work.util_tb.all;

entity mmulrue_tb is
  constant CLK_HPERIOD : time := 10 ps;
  constant STRING_SIZE : integer := 31; -- string size = index of the last element
end mmulrue_tb;
architecture behavior of mmulrue_tb is
  constant CFG_MMU_PAGESIZE : integer := 0;
  
  -- input/output signals:
  signal inNRst   : std_logic:= '0';
  signal inClk    : std_logic:= '0';
  signal in_lruei  : mmulrue_in_type;
  signal ch_lrueo  : mmulrue_out_type; 
  signal lrueo  : mmulrue_out_type; 

  signal U: std_ulogic_vector(STRING_SIZE-1 downto 0);
  signal S: std_logic_vector(STRING_SIZE-1 downto 0);
  shared variable iClkCnt : integer := 0;
  shared variable iErrCnt : integer := 0;

  component mmulrue 
  generic (
    position : integer;
    entries  : integer := 8
    );
  port (
    rst      : in std_logic;
    clk      : in std_logic;
    lruei    : in mmulrue_in_type;
    lrueo    : out mmulrue_out_type 
  );
  end component;

begin

  -- Process of clock generation
  procClkgen : process
  begin
      inClk <= '0' after CLK_HPERIOD, '1' after 2*CLK_HPERIOD;
      wait for 2*CLK_HPERIOD;
  end process procClkgen;

  -- Process of reading  
  procReadingFile : process
    file InputFile:TEXT is "e:/mmulrue_tb.txt";
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
      if(iClkCnt=68)then
        print("Break: " & tost(iClkCnt));
      end if;

    end loop;
  end process procReadingFile;


  -- Input signals:
  inNRst <= S(0);
  in_lruei.touch <= S(1);
  in_lruei.pos <= S(7 downto 2);
  in_lruei.clear <= S(8);
  in_lruei.flush <= S(9);
  in_lruei.left <= S(15 downto 10);
  in_lruei.fromleft <= S(16);
  in_lruei.right <= S(22 downto 17);
  in_lruei.fromright <= S(23);
  ch_lrueo.pos <= S(29 downto 24);
  ch_lrueo.movetop <= S(30);



  tt : mmulrue generic map 
  ( 
    0,-- various value were checked in a range 0..7
    CFG_ITLBNUM
  )
  port map (inNRst, inClk, in_lruei, lrueo);




procCheck : process (inClk,ch_lrueo)
begin
  if(rising_edge(inClk) and (iClkCnt>10)) then
    if(ch_lrueo/=lrueo) then print("Err: lrueo");  iErrCnt:=iErrCnt+1; end if;
  end if;
end process procCheck;

  
end;
