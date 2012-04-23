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
library work;
use work.config.all;
use work.util_tb.all;

library gnsslib;
use gnsslib.gnssmodule.all;



entity gnssengine_tb is
  constant CLK_HPERIOD : time := 10 ps;
  constant STRING_SIZE : integer := 493; -- string size = index of the last element
end gnssengine_tb;

architecture behavior of gnssengine_tb is


  -- input/output signals:
  signal inNRst    : std_logic:= '0';
  signal inClk     : std_logic:= '0';
  signal in_ahbsi   : ahb_slv_in_type;
  signal ch_ahbso   : ahb_slv_out_type;
  signal ahbso   : ahb_slv_out_type;

  signal in_GpsI  : std_logic_vector(1 downto 0);
  signal in_GpsQ  : std_logic_vector(1 downto 0);
  signal in_GloI  : std_logic_vector(1 downto 0);
  signal in_GloQ  : std_logic_vector(1 downto 0);


  
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
    file InputFile:TEXT is "e:/gnssengine_tb.txt";--open read_mode file_name;
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
  in_ahbsi.hsel <= S(16 downto 1);
  in_ahbsi.haddr <= S(48 downto 17);
  in_ahbsi.hwrite <= S(49);
  in_ahbsi.htrans <= S(51 downto 50);
  in_ahbsi.hsize <= S(54 downto 52);
  in_ahbsi.hburst <= S(57 downto 55);
  in_ahbsi.hwdata <= S(89 downto 58);
  in_ahbsi.hprot <= S(93 downto 90);
  in_ahbsi.hready <= S(94);
  in_ahbsi.hmaster <= S(98 downto 95);
  in_ahbsi.hmastlock <= S(99);
  in_ahbsi.hmbsel <= S(103 downto 100);
  in_ahbsi.hcache <= S(104);
  in_ahbsi.hirq <= S(136 downto 105);
  in_ahbsi.testen <= S(137);
  in_ahbsi.testrst <= S(138);
  in_ahbsi.scanen <= S(139);
  in_ahbsi.testoen <= S(140);
  in_GpsI <= S(142 downto 141);
  in_GpsQ <= S(144 downto 143);
  in_GloI <= S(146 downto 145);
  in_GloQ <= S(148 downto 147);
  ch_ahbso.hready <= S(149);
  ch_ahbso.hresp <= S(151 downto 150);
  ch_ahbso.hrdata <= S(183 downto 152);
  ch_ahbso.hsplit <= S(199 downto 184);
  ch_ahbso.hcache <= S(200);
  ch_ahbso.hirq <= S(232 downto 201);
  ch_ahbso.hconfig(0) <= S(264 downto 233);
  ch_ahbso.hconfig(1) <= S(296 downto 265);
  ch_ahbso.hconfig(2) <= S(328 downto 297);
  ch_ahbso.hconfig(3) <= S(360 downto 329);
  ch_ahbso.hconfig(4) <= S(392 downto 361);
  ch_ahbso.hconfig(5) <= S(424 downto 393);
  ch_ahbso.hconfig(6) <= S(456 downto 425);
  ch_ahbso.hconfig(7) <= S(488 downto 457);
  ch_ahbso.hindex <= conv_integer(S(492 downto 489));



procCheck : process (inClk,ch_ahbso)
begin
  if(rising_edge(inClk)and(iClkCnt>2)) then
    --if((apbo.prdata(0)/='U')and(apbo.prdata(0)/='X')) then
    --  if(ch_apbo/=apbo) then print("Err: apbo: prdata=" & tost(apbo.prdata));  iErrCnt:=iErrCnt+1; end if;
    --end if;
  end if;
end process procCheck;


end;
 
 