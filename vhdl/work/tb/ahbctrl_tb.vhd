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
  constant STRING_SIZE : integer := 2347; -- string size = index of the last element

end ahbctrl_tb;

architecture behavior of ahbctrl_tb is

 

  -- input/output signals:
  signal inClk        : std_logic:= '0';
  signal inNRst       : std_logic:= '0';

  signal in_msto    : ahb_mst_out_vector := (others => ahbm_none);  
  signal ch_msti    : ahb_mst_in_type;
  signal msti       : ahb_mst_in_type;
  signal in_slvo    : ahb_slv_out_vector := (others => ahbs_none);  
  signal ch_slvi    : ahb_slv_in_type;
  signal slvi       : ahb_slv_in_type;

  
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
  inNRst <= S(0);
  in_msto(0).hbusreq <= S(1);
  in_msto(0).hlock <= S(2);
  in_msto(0).htrans <= S(4 downto 3);
  in_msto(0).haddr <= S(36 downto 5);
  in_msto(0).hwrite <= S(37);
  in_msto(0).hsize <= S(40 downto 38);
  in_msto(0).hburst <= S(43 downto 41);
  in_msto(0).hprot <= S(47 downto 44);
  in_msto(0).hwdata <= S(79 downto 48);
  in_msto(0).hirq <= S(111 downto 80);
  in_msto(0).hconfig(0) <= S(143 downto 112);
  in_msto(0).hconfig(1) <= S(175 downto 144);
  in_msto(0).hconfig(2) <= S(207 downto 176);
  in_msto(0).hconfig(3) <= S(239 downto 208);
  in_msto(0).hconfig(4) <= S(271 downto 240);
  in_msto(0).hconfig(5) <= S(303 downto 272);
  in_msto(0).hconfig(6) <= S(335 downto 304);
  in_msto(0).hconfig(7) <= S(367 downto 336);
  in_msto(0).hindex <= conv_integer(S(371 downto 368));
  in_msto(1).hbusreq <= S(372);
  in_msto(1).hlock <= S(373);
  in_msto(1).htrans <= S(375 downto 374);
  in_msto(1).haddr <= S(407 downto 376);
  in_msto(1).hwrite <= S(408);
  in_msto(1).hsize <= S(411 downto 409);
  in_msto(1).hburst <= S(414 downto 412);
  in_msto(1).hprot <= S(418 downto 415);
  in_msto(1).hwdata <= S(450 downto 419);
  in_msto(1).hirq <= S(482 downto 451);
  in_msto(1).hconfig(0) <= S(514 downto 483);
  in_msto(1).hconfig(1) <= S(546 downto 515);
  in_msto(1).hconfig(2) <= S(578 downto 547);
  in_msto(1).hconfig(3) <= S(610 downto 579);
  in_msto(1).hconfig(4) <= S(642 downto 611);
  in_msto(1).hconfig(5) <= S(674 downto 643);
  in_msto(1).hconfig(6) <= S(706 downto 675);
  in_msto(1).hconfig(7) <= S(738 downto 707);
  in_msto(1).hindex <= conv_integer(S(742 downto 739));
  in_slvo(0).hready <= S(743);
  in_slvo(0).hresp <= S(745 downto 744);
  in_slvo(0).hrdata <= S(777 downto 746);
  in_slvo(0).hsplit <= S(793 downto 778);
  in_slvo(0).hcache <= S(794);
  in_slvo(0).hirq <= S(826 downto 795);
  in_slvo(0).hconfig(0) <= S(858 downto 827);
  in_slvo(0).hconfig(1) <= S(890 downto 859);
  in_slvo(0).hconfig(2) <= S(922 downto 891);
  in_slvo(0).hconfig(3) <= S(954 downto 923);
  in_slvo(0).hconfig(4) <= S(986 downto 955);
  in_slvo(0).hconfig(5) <= S(1018 downto 987);
  in_slvo(0).hconfig(6) <= S(1050 downto 1019);
  in_slvo(0).hconfig(7) <= S(1082 downto 1051);
  in_slvo(0).hindex <= conv_integer(S(1086 downto 1083));
  in_slvo(1).hready <= S(1087);
  in_slvo(1).hresp <= S(1089 downto 1088);
  in_slvo(1).hrdata <= S(1121 downto 1090);
  in_slvo(1).hsplit <= S(1137 downto 1122);
  in_slvo(1).hcache <= S(1138);
  in_slvo(1).hirq <= S(1170 downto 1139);
  in_slvo(1).hconfig(0) <= S(1202 downto 1171);
  in_slvo(1).hconfig(1) <= S(1234 downto 1203);
  in_slvo(1).hconfig(2) <= S(1266 downto 1235);
  in_slvo(1).hconfig(3) <= S(1298 downto 1267);
  in_slvo(1).hconfig(4) <= S(1330 downto 1299);
  in_slvo(1).hconfig(5) <= S(1362 downto 1331);
  in_slvo(1).hconfig(6) <= S(1394 downto 1363);
  in_slvo(1).hconfig(7) <= S(1426 downto 1395);
  in_slvo(1).hindex <= conv_integer(S(1430 downto 1427));
  in_slvo(2).hready <= S(1431);
  in_slvo(2).hresp <= S(1433 downto 1432);
  in_slvo(2).hrdata <= S(1465 downto 1434);
  in_slvo(2).hsplit <= S(1481 downto 1466);
  in_slvo(2).hcache <= S(1482);
  in_slvo(2).hirq <= S(1514 downto 1483);
  in_slvo(2).hconfig(0) <= S(1546 downto 1515);
  in_slvo(2).hconfig(1) <= S(1578 downto 1547);
  in_slvo(2).hconfig(2) <= S(1610 downto 1579);
  in_slvo(2).hconfig(3) <= S(1642 downto 1611);
  in_slvo(2).hconfig(4) <= S(1674 downto 1643);
  in_slvo(2).hconfig(5) <= S(1706 downto 1675);
  in_slvo(2).hconfig(6) <= S(1738 downto 1707);
  in_slvo(2).hconfig(7) <= S(1770 downto 1739);
  in_slvo(2).hindex <= conv_integer(S(1774 downto 1771));
  in_slvo(3).hready <= S(1775);
  in_slvo(3).hresp <= S(1777 downto 1776);
  in_slvo(3).hrdata <= S(1809 downto 1778);
  in_slvo(3).hsplit <= S(1825 downto 1810);
  in_slvo(3).hcache <= S(1826);
  in_slvo(3).hirq <= S(1858 downto 1827);
  in_slvo(3).hconfig(0) <= S(1890 downto 1859);
  in_slvo(3).hconfig(1) <= S(1922 downto 1891);
  in_slvo(3).hconfig(2) <= S(1954 downto 1923);
  in_slvo(3).hconfig(3) <= S(1986 downto 1955);
  in_slvo(3).hconfig(4) <= S(2018 downto 1987);
  in_slvo(3).hconfig(5) <= S(2050 downto 2019);
  in_slvo(3).hconfig(6) <= S(2082 downto 2051);
  in_slvo(3).hconfig(7) <= S(2114 downto 2083);
  in_slvo(3).hindex <= conv_integer(S(2118 downto 2115));
  ch_msti.hgrant <= S(2134 downto 2119);
  ch_msti.hready <= S(2135);
  ch_msti.hresp <= S(2137 downto 2136);
  ch_msti.hrdata <= S(2169 downto 2138);
  ch_msti.hcache <= S(2170);
  ch_msti.hirq <= S(2202 downto 2171);
  ch_msti.testen <= S(2203);
  ch_msti.testrst <= S(2204);
  ch_msti.scanen <= S(2205);
  ch_msti.testoen <= S(2206);
  ch_slvi.hsel <= S(2222 downto 2207);
  ch_slvi.haddr <= S(2254 downto 2223);
  ch_slvi.hwrite <= S(2255);
  ch_slvi.htrans <= S(2257 downto 2256);
  ch_slvi.hsize <= S(2260 downto 2258);
  ch_slvi.hburst <= S(2263 downto 2261);
  ch_slvi.hwdata <= S(2295 downto 2264);
  ch_slvi.hprot <= S(2299 downto 2296);
  ch_slvi.hready <= S(2300);
  ch_slvi.hmaster <= S(2304 downto 2301);
  ch_slvi.hmastlock <= S(2305);
  ch_slvi.hmbsel <= S(2309 downto 2306);
  ch_slvi.hcache <= S(2310);
  ch_slvi.hirq <= S(2342 downto 2311);
  ch_slvi.testen <= S(2343);
  ch_slvi.testrst <= S(2344);
  ch_slvi.scanen <= S(2345);
  ch_slvi.testoen <= S(2346);

  tt : ahbctrl
    generic map (defmast => CFG_AHBCTRL_DEFMST,
                 split => CFG_AHBCTRL_SPLIT,
                 rrobin  => CFG_AHBCTRL_RROBIN,
                 ioaddr => CFG_AHBCTRL_AHBIO, ioen => 1, 
                 nahbm => AHB_MASTER_TOTAL, 
                 nahbs => AHB_SLAVE_TOTAL,
                 devid => 16#0401#)--VIRTEX_ML401)
    port map (inNRst, inClk, msti, in_msto, slvi, in_slvo);

  
  procCheck : process (inClk,ch_msti,ch_slvi)
  begin
    if((rising_edge(inClk)) and (iClkCnt>1)) then
      if(ch_msti /= msti) then print("err: msti"); iErrCnt:=iErrCnt+1; end if;
      if(ch_slvi /= slvi) then print("err: slvi"); iErrCnt:=iErrCnt+1; end if;
    end if;
  end process procCheck;
  
end;
 
 