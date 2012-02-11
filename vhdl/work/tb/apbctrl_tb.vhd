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


entity apbctrl_tb is
  constant CLK_HPERIOD : time := 10 ps;
  constant STRING_SIZE : integer := 735; -- string size = index of the last element
end apbctrl_tb;

architecture behavior of apbctrl_tb is


  -- input/output signals:
  signal inNRst       : std_logic:= '0';
  signal inClk        : std_logic:= '0';

  signal in_ahbsi   : ahb_slv_in_type;
  signal ch_ahbso   : ahb_slv_out_type;
  signal ahbso   : ahb_slv_out_type;
  signal ch_apbi    : apb_slv_in_type;
  signal apbi    : apb_slv_in_type;
  signal in_apbo    : apb_slv_out_vector := (others => apb_none);
  
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
    file InputFile:TEXT is "e:/apbctrl_tb.txt";--open read_mode file_name;
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
  in_apbo(0).prdata <= S(172 downto 141);
  in_apbo(0).pirq <= S(204 downto 173);
  in_apbo(0).pconfig(0) <= S(236 downto 205);
  in_apbo(0).pconfig(1) <= S(268 downto 237);
  in_apbo(0).pindex <= conv_integer(S(272 downto 269));
  ch_apbi.psel <= S(288 downto 273);
  ch_apbi.penable <= S(289);
  ch_apbi.paddr <= S(321 downto 290);
  ch_apbi.pwrite <= S(322);
  ch_apbi.pwdata <= S(354 downto 323);
  ch_apbi.pirq <= S(386 downto 355);
  ch_apbi.testen <= S(387);
  ch_apbi.testrst <= S(388);
  ch_apbi.scanen <= S(389);
  ch_apbi.testoen <= S(390);
  ch_ahbso.hready <= S(391);
  ch_ahbso.hresp <= S(393 downto 392);
  ch_ahbso.hrdata <= S(425 downto 394);
  ch_ahbso.hsplit <= S(441 downto 426);
  ch_ahbso.hcache <= S(442);
  ch_ahbso.hirq <= S(474 downto 443);
  ch_ahbso.hconfig(0) <= S(506 downto 475);
  ch_ahbso.hconfig(1) <= S(538 downto 507);
  ch_ahbso.hconfig(2) <= S(570 downto 539);
  ch_ahbso.hconfig(3) <= S(602 downto 571);
  ch_ahbso.hconfig(4) <= S(634 downto 603);
  ch_ahbso.hconfig(5) <= S(666 downto 635);
  ch_ahbso.hconfig(6) <= S(698 downto 667);
  ch_ahbso.hconfig(7) <= S(730 downto 699);
  ch_ahbso.hindex <= conv_integer(S(734 downto 731));



  tt : apbctrl generic map (3, 16#800#)
  port map 
	(
	  inNRst,
	  inClk,
    in_ahbsi,
    ahbso,
    apbi,
    in_apbo
	);
 
procCheck : process (inClk,ch_ahbso)
begin
  if(rising_edge(inClk)and(iClkCnt>2)) then
    if(ch_ahbso/=ahbso) then print("Err: ahbso");  iErrCnt:=iErrCnt+1; end if;
    if(apbi.pwdata/="UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU") then
      if(ch_apbi/=apbi) then print("Err: apbi");  iErrCnt:=iErrCnt+1; end if;
    end if;
  end if;
end process procCheck;
  
end;
 
 