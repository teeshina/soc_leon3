
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
use gaisler.libmmu.all;
library work;
use work.config.all;
use work.util_tb.all;

entity mmulru_tb is
  constant CLK_HPERIOD : time := 10 ps;
  constant STRING_SIZE : integer := 62; -- string size = index of the last element
end mmulru_tb;
architecture behavior of mmulru_tb is
  
  -- input/output signals:
  signal inNRst   : std_logic:= '0';
  signal inClk    : std_logic:= '0';
  signal in_lrui  : mmulru_in_type;
  signal ch_lruo  : mmulru_out_type; 
  signal lruo  : mmulru_out_type; 

  signal U: std_ulogic_vector(STRING_SIZE-1 downto 0);
  signal S: std_logic_vector(STRING_SIZE-1 downto 0);
  shared variable iClkCnt : integer := 0;
  shared variable iErrCnt : integer := 0;

  -- least recently used
  component mmulru
    generic (
      entries  : integer := 8
    );  
    port (
      rst   : in std_logic;
      clk   : in std_logic;
      lrui  : in mmulru_in_type;
      lruo  : out mmulru_out_type 
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
    file InputFile:TEXT is "e:/mmulru_tb.txt";
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
      if(iClkCnt=12)then
        print("Break: " & tost(iClkCnt));
      end if;

    end loop;
  end process procReadingFile;


  -- Input signals:
  inNRst <= S(0);
  in_lrui.touch <= S(1);
  in_lrui.touchmin <= S(2);
  in_lrui.flush <= S(3);
  in_lrui.pos <= S(9 downto 4);
  in_lrui.mmctrl1.e <= S(10);
  in_lrui.mmctrl1.nf <= S(11);
  in_lrui.mmctrl1.pso <= S(12);
  in_lrui.mmctrl1.pagesize <= S(14 downto 13);
  in_lrui.mmctrl1.ctx <= S(22 downto 15);
  in_lrui.mmctrl1.ctxp <= S(52 downto 23);
  in_lrui.mmctrl1.tlbdis <= S(53);
  in_lrui.mmctrl1.bar <= S(55 downto 54);
  ch_lruo.pos <= S(61 downto 56);



  tt : mmulru generic map 
  ( 
    CFG_ITLBNUM
  )
  port map (inNRst, inClk, in_lrui, lruo);




procCheck : process (inClk,ch_lruo)
begin
  if(rising_edge(inClk) and (iClkCnt>10)) then
    if(ch_lruo/=lruo) then print("Err: lruo");  iErrCnt:=iErrCnt+1; end if;
  end if;
end process procCheck;

  
end;
