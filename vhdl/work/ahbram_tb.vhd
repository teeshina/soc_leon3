
library ieee;
use ieee.std_logic_1164.all;
library std;
use std.textio.all;
library ieee;
library grlib;
use grlib.stdlib.all;
use grlib.amba.all;
library techmap;
use techmap.gencomp.all;
library gaisler;
use gaisler.misc.all;
library work;
use work.config.all;
use work.util_tb.all;

entity ahbram_tb is
  constant CLK_HPERIOD : time := 10 ps;
  constant STRING_SIZE : integer := 485; -- string size = index of the last element

  
end ahbram_tb;
architecture behavior of ahbram_tb is
  -- input/output signals:
  signal inNRst   : std_logic:= '0';
  signal inClk    : std_logic:= '0';

  signal in_ahbsi  : ahb_slv_in_type;
  signal ch_ahbso  : ahb_slv_out_type;
  signal ahbso  : ahb_slv_out_type;

                     					
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
    file InputFile:TEXT is "e:/ahbram_tb.txt";
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
      if(iClkCnt=365) then
        print("break");
      end if;

    end loop;
  end process procReadingFile;


  -- Input signals:

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
  ch_ahbso.hready <= S(141);
  ch_ahbso.hresp <= S(143 downto 142);
  ch_ahbso.hrdata <= S(175 downto 144);
  ch_ahbso.hsplit <= S(191 downto 176);
  ch_ahbso.hcache <= S(192);
  ch_ahbso.hirq <= S(224 downto 193);
  ch_ahbso.hconfig(0) <= S(256 downto 225);
  ch_ahbso.hconfig(1) <= S(288 downto 257);
  ch_ahbso.hconfig(2) <= S(320 downto 289);
  ch_ahbso.hconfig(3) <= S(352 downto 321);
  ch_ahbso.hconfig(4) <= S(384 downto 353);
  ch_ahbso.hconfig(5) <= S(416 downto 385);
  ch_ahbso.hconfig(6) <= S(448 downto 417);
  ch_ahbso.hconfig(7) <= S(480 downto 449);
  ch_ahbso.hindex <= conv_integer(S(484 downto 481));
  

  tt : ahbram generic map 
  (
    2,--hindex    : integer               := 0;
    16#300#,
    16#fff#,
    inferred
  )port map 
  (
    inNRst,
    inClk,
    in_ahbsi,
    ahbso
  );
  
  check : process(inNRst,inClk, ch_ahbso)
  begin
    if(rising_edge(inClk) and (iClkCnt>3)) then
      if(ch_ahbso.hready/=ahbso.hready) then print("Err: ahbso.hready");  iErrCnt:=iErrCnt+1; end if;
      if((ahbso.hrdata(31)/='U')and(ahbso.hrdata(20)/='U')and(ahbso.hrdata(10)/='U')and(ahbso.hrdata(10)/='X')) then
        if(ch_ahbso.hrdata/=ahbso.hrdata) then print("Err: ahbso.hrdata");  iErrCnt:=iErrCnt+1; end if;
      end if;
    end if;
  end process check;
  

  
end;
