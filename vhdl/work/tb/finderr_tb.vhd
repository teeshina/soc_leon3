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



entity finderr_tb is

  constant CLK_HPERIOD : time := 10 ps;
  constant STRING_SIZE : integer := 2347; -- string size = index of the last element

end finderr_tb;
architecture behavior of finderr_tb is


  -- input/output signals:
  signal inNRst       : std_logic:= '0';
  signal inClk        : std_logic:= '0';

  signal ahbmi1    : ahb_mst_in_type;
  signal ahbmo1    : ahb_mst_out_vector;
  signal ahbsi1    : ahb_slv_in_type;
  signal ahbso1    : ahb_slv_out_vector;

  signal ahbmi2    : ahb_mst_in_type;
  signal ahbmo2    : ahb_mst_out_vector := (others => ahbm_none);
  signal ahbsi2    : ahb_slv_in_type;
  signal ahbso2    : ahb_slv_out_vector := (others => ahbs_none);

  
  signal S1: std_logic_vector(STRING_SIZE-1 downto 0);
  signal U1: std_ulogic_vector(STRING_SIZE-1 downto 0);

  signal S2: std_logic_vector(STRING_SIZE-1 downto 0);
  signal U2: std_ulogic_vector(STRING_SIZE-1 downto 0);

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
    file InputFile1:TEXT is "e:/finderr1_tb.txt";--open read_mode file_name;
    variable rdLine1: line;  
    variable strLine1 : string(STRING_SIZE downto 1);

    file InputFile2:TEXT is "e:/finderr2_tb.txt";--open read_mode file_name;
    variable rdLine2: line;  
    variable strLine2 : string(STRING_SIZE downto 1);

  begin
    while not endfile(InputFile1) loop
      readline(InputFile1, rdLine1);
      read(rdLine1, strLine1);
      U1 <= StringToUVector(strLine1);
      S1 <= StringToSVector(strLine1);

      readline(InputFile2, rdLine2);
      read(rdLine2, strLine2);
      U2 <= StringToUVector(strLine2);
      S2 <= StringToSVector(strLine2);
  
      wait until rising_edge(inClk);
      --wait until falling_edge(inClk);
      iClkCnt := iClkCnt + 1;
    end loop;
  end process procReadingFile;
  


-- signal parsment and assignment
  inNRst <= S1(0);
  ahbmi1.hgrant <= S1(16 downto 1);
  ahbmi1.hready <= S1(17);
  ahbmi1.hresp <= S1(19 downto 18);
  ahbmi1.hrdata <= S1(51 downto 20);
  ahbmi1.hcache <= S1(52);
  ahbmi1.hirq <= S1(84 downto 53);
  ahbmi1.testen <= S1(85);
  ahbmi1.testrst <= S1(86);
  ahbmi1.scanen <= S1(87);
  ahbmi1.testoen <= S1(88);
  ahbmo1(0).hbusreq <= S1(89);
  ahbmo1(0).hlock <= S1(90);
  ahbmo1(0).htrans <= S1(92 downto 91);
  ahbmo1(0).haddr <= S1(124 downto 93);
  ahbmo1(0).hwrite <= S1(125);
  ahbmo1(0).hsize <= S1(128 downto 126);
  ahbmo1(0).hburst <= S1(131 downto 129);
  ahbmo1(0).hprot <= S1(135 downto 132);
  ahbmo1(0).hwdata <= S1(167 downto 136);
  ahbmo1(0).hirq <= S1(199 downto 168);
  ahbmo1(0).hconfig(0) <= S1(231 downto 200);
  ahbmo1(0).hconfig(1) <= S1(263 downto 232);
  ahbmo1(0).hconfig(2) <= S1(295 downto 264);
  ahbmo1(0).hconfig(3) <= S1(327 downto 296);
  ahbmo1(0).hconfig(4) <= S1(359 downto 328);
  ahbmo1(0).hconfig(5) <= S1(391 downto 360);
  ahbmo1(0).hconfig(6) <= S1(423 downto 392);
  ahbmo1(0).hconfig(7) <= S1(455 downto 424);
  ahbmo1(0).hindex <= conv_integer(S1(459 downto 456));
  ahbmo1(1).hbusreq <= S1(460);
  ahbmo1(1).hlock <= S1(461);
  ahbmo1(1).htrans <= S1(463 downto 462);
  ahbmo1(1).haddr <= S1(495 downto 464);
  ahbmo1(1).hwrite <= S1(496);
  ahbmo1(1).hsize <= S1(499 downto 497);
  ahbmo1(1).hburst <= S1(502 downto 500);
  ahbmo1(1).hprot <= S1(506 downto 503);
  ahbmo1(1).hwdata <= S1(538 downto 507);
  ahbmo1(1).hirq <= S1(570 downto 539);
  ahbmo1(1).hconfig(0) <= S1(602 downto 571);
  ahbmo1(1).hconfig(1) <= S1(634 downto 603);
  ahbmo1(1).hconfig(2) <= S1(666 downto 635);
  ahbmo1(1).hconfig(3) <= S1(698 downto 667);
  ahbmo1(1).hconfig(4) <= S1(730 downto 699);
  ahbmo1(1).hconfig(5) <= S1(762 downto 731);
  ahbmo1(1).hconfig(6) <= S1(794 downto 763);
  ahbmo1(1).hconfig(7) <= S1(826 downto 795);
  ahbmo1(1).hindex <= conv_integer(S1(830 downto 827));
  ahbsi1.hsel <= S1(846 downto 831);
  ahbsi1.haddr <= S1(878 downto 847);
  ahbsi1.hwrite <= S1(879);
  ahbsi1.htrans <= S1(881 downto 880);
  ahbsi1.hsize <= S1(884 downto 882);
  ahbsi1.hburst <= S1(887 downto 885);
  ahbsi1.hwdata <= S1(919 downto 888);
  ahbsi1.hprot <= S1(923 downto 920);
  ahbsi1.hready <= S1(924);
  ahbsi1.hmaster <= S1(928 downto 925);
  ahbsi1.hmastlock <= S1(929);
  ahbsi1.hmbsel <= S1(933 downto 930);
  ahbsi1.hcache <= S1(934);
  ahbsi1.hirq <= S1(966 downto 935);
  ahbsi1.testen <= S1(967);
  ahbsi1.testrst <= S1(968);
  ahbsi1.scanen <= S1(969);
  ahbsi1.testoen <= S1(970);
  ahbso1(0).hready <= S1(971);
  ahbso1(0).hresp <= S1(973 downto 972);
  ahbso1(0).hrdata <= S1(1005 downto 974);
  ahbso1(0).hsplit <= S1(1021 downto 1006);
  ahbso1(0).hcache <= S1(1022);
  ahbso1(0).hirq <= S1(1054 downto 1023);
  ahbso1(0).hconfig(0) <= S1(1086 downto 1055);
  ahbso1(0).hconfig(1) <= S1(1118 downto 1087);
  ahbso1(0).hconfig(2) <= S1(1150 downto 1119);
  ahbso1(0).hconfig(3) <= S1(1182 downto 1151);
  ahbso1(0).hconfig(4) <= S1(1214 downto 1183);
  ahbso1(0).hconfig(5) <= S1(1246 downto 1215);
  ahbso1(0).hconfig(6) <= S1(1278 downto 1247);
  ahbso1(0).hconfig(7) <= S1(1310 downto 1279);
  ahbso1(0).hindex <= conv_integer(S1(1314 downto 1311));
  ahbso1(1).hready <= S1(1315);
  ahbso1(1).hresp <= S1(1317 downto 1316);
  ahbso1(1).hrdata <= S1(1349 downto 1318);
  ahbso1(1).hsplit <= S1(1365 downto 1350);
  ahbso1(1).hcache <= S1(1366);
  ahbso1(1).hirq <= S1(1398 downto 1367);
  ahbso1(1).hconfig(0) <= S1(1430 downto 1399);
  ahbso1(1).hconfig(1) <= S1(1462 downto 1431);
  ahbso1(1).hconfig(2) <= S1(1494 downto 1463);
  ahbso1(1).hconfig(3) <= S1(1526 downto 1495);
  ahbso1(1).hconfig(4) <= S1(1558 downto 1527);
  ahbso1(1).hconfig(5) <= S1(1590 downto 1559);
  ahbso1(1).hconfig(6) <= S1(1622 downto 1591);
  ahbso1(1).hconfig(7) <= S1(1654 downto 1623);
  ahbso1(1).hindex <= conv_integer(S1(1658 downto 1655));
  ahbso1(2).hready <= S1(1659);
  ahbso1(2).hresp <= S1(1661 downto 1660);
  ahbso1(2).hrdata <= S1(1693 downto 1662);
  ahbso1(2).hsplit <= S1(1709 downto 1694);
  ahbso1(2).hcache <= S1(1710);
  ahbso1(2).hirq <= S1(1742 downto 1711);
  ahbso1(2).hconfig(0) <= S1(1774 downto 1743);
  ahbso1(2).hconfig(1) <= S1(1806 downto 1775);
  ahbso1(2).hconfig(2) <= S1(1838 downto 1807);
  ahbso1(2).hconfig(3) <= S1(1870 downto 1839);
  ahbso1(2).hconfig(4) <= S1(1902 downto 1871);
  ahbso1(2).hconfig(5) <= S1(1934 downto 1903);
  ahbso1(2).hconfig(6) <= S1(1966 downto 1935);
  ahbso1(2).hconfig(7) <= S1(1998 downto 1967);
  ahbso1(2).hindex <= conv_integer(S1(2002 downto 1999));
  ahbso1(3).hready <= S1(2003);
  ahbso1(3).hresp <= S1(2005 downto 2004);
  ahbso1(3).hrdata <= S1(2037 downto 2006);
  ahbso1(3).hsplit <= S1(2053 downto 2038);
  ahbso1(3).hcache <= S1(2054);
  ahbso1(3).hirq <= S1(2086 downto 2055);
  ahbso1(3).hconfig(0) <= S1(2118 downto 2087);
  ahbso1(3).hconfig(1) <= S1(2150 downto 2119);
  ahbso1(3).hconfig(2) <= S1(2182 downto 2151);
  ahbso1(3).hconfig(3) <= S1(2214 downto 2183);
  ahbso1(3).hconfig(4) <= S1(2246 downto 2215);
  ahbso1(3).hconfig(5) <= S1(2278 downto 2247);
  ahbso1(3).hconfig(6) <= S1(2310 downto 2279);
  ahbso1(3).hconfig(7) <= S1(2342 downto 2311);
  ahbso1(3).hindex <= conv_integer(S1(2346 downto 2343));

  ahbmi2.hgrant <= S2(16 downto 1);
  ahbmi2.hready <= S2(17);
  ahbmi2.hresp <= S2(19 downto 18);
  ahbmi2.hrdata <= S2(51 downto 20);
  ahbmi2.hcache <= S2(52);
  ahbmi2.hirq <= S2(84 downto 53);
  ahbmi2.testen <= S2(85);
  ahbmi2.testrst <= S2(86);
  ahbmi2.scanen <= S2(87);
  ahbmi2.testoen <= S2(88);
  ahbmo2(0).hbusreq <= S2(89);
  ahbmo2(0).hlock <= S2(90);
  ahbmo2(0).htrans <= S2(92 downto 91);
  ahbmo2(0).haddr <= S2(124 downto 93);
  ahbmo2(0).hwrite <= S2(125);
  ahbmo2(0).hsize <= S2(128 downto 126);
  ahbmo2(0).hburst <= S2(131 downto 129);
  ahbmo2(0).hprot <= S2(135 downto 132);
  ahbmo2(0).hwdata <= S2(167 downto 136);
  ahbmo2(0).hirq <= S2(199 downto 168);
  ahbmo2(0).hconfig(0) <= S2(231 downto 200);
  ahbmo2(0).hconfig(1) <= S2(263 downto 232);
  ahbmo2(0).hconfig(2) <= S2(295 downto 264);
  ahbmo2(0).hconfig(3) <= S2(327 downto 296);
  ahbmo2(0).hconfig(4) <= S2(359 downto 328);
  ahbmo2(0).hconfig(5) <= S2(391 downto 360);
  ahbmo2(0).hconfig(6) <= S2(423 downto 392);
  ahbmo2(0).hconfig(7) <= S2(455 downto 424);
  ahbmo2(0).hindex <= conv_integer(S2(459 downto 456));
  ahbmo2(1).hbusreq <= S2(460);
  ahbmo2(1).hlock <= S2(461);
  ahbmo2(1).htrans <= S2(463 downto 462);
  ahbmo2(1).haddr <= S2(495 downto 464);
  ahbmo2(1).hwrite <= S2(496);
  ahbmo2(1).hsize <= S2(499 downto 497);
  ahbmo2(1).hburst <= S2(502 downto 500);
  ahbmo2(1).hprot <= S2(506 downto 503);
  ahbmo2(1).hwdata <= S2(538 downto 507);
  ahbmo2(1).hirq <= S2(570 downto 539);
  ahbmo2(1).hconfig(0) <= S2(602 downto 571);
  ahbmo2(1).hconfig(1) <= S2(634 downto 603);
  ahbmo2(1).hconfig(2) <= S2(666 downto 635);
  ahbmo2(1).hconfig(3) <= S2(698 downto 667);
  ahbmo2(1).hconfig(4) <= S2(730 downto 699);
  ahbmo2(1).hconfig(5) <= S2(762 downto 731);
  ahbmo2(1).hconfig(6) <= S2(794 downto 763);
  ahbmo2(1).hconfig(7) <= S2(826 downto 795);
  ahbmo2(1).hindex <= conv_integer(S2(830 downto 827));
  ahbsi2.hsel <= S2(846 downto 831);
  ahbsi2.haddr <= S2(878 downto 847);
  ahbsi2.hwrite <= S2(879);
  ahbsi2.htrans <= S2(881 downto 880);
  ahbsi2.hsize <= S2(884 downto 882);
  ahbsi2.hburst <= S2(887 downto 885);
  ahbsi2.hwdata <= S2(919 downto 888);
  ahbsi2.hprot <= S2(923 downto 920);
  ahbsi2.hready <= S2(924);
  ahbsi2.hmaster <= S2(928 downto 925);
  ahbsi2.hmastlock <= S2(929);
  ahbsi2.hmbsel <= S2(933 downto 930);
  ahbsi2.hcache <= S2(934);
  ahbsi2.hirq <= S2(966 downto 935);
  ahbsi2.testen <= S2(967);
  ahbsi2.testrst <= S2(968);
  ahbsi2.scanen <= S2(969);
  ahbsi2.testoen <= S2(970);
  ahbso2(0).hready <= S2(971);
  ahbso2(0).hresp <= S2(973 downto 972);
  ahbso2(0).hrdata <= S2(1005 downto 974);
  ahbso2(0).hsplit <= S2(1021 downto 1006);
  ahbso2(0).hcache <= S2(1022);
  ahbso2(0).hirq <= S2(1054 downto 1023);
  ahbso2(0).hconfig(0) <= S2(1086 downto 1055);
  ahbso2(0).hconfig(1) <= S2(1118 downto 1087);
  ahbso2(0).hconfig(2) <= S2(1150 downto 1119);
  ahbso2(0).hconfig(3) <= S2(1182 downto 1151);
  ahbso2(0).hconfig(4) <= S2(1214 downto 1183);
  ahbso2(0).hconfig(5) <= S2(1246 downto 1215);
  ahbso2(0).hconfig(6) <= S2(1278 downto 1247);
  ahbso2(0).hconfig(7) <= S2(1310 downto 1279);
  ahbso2(0).hindex <= conv_integer(S2(1314 downto 1311));
  ahbso2(1).hready <= S2(1315);
  ahbso2(1).hresp <= S2(1317 downto 1316);
  ahbso2(1).hrdata <= S2(1349 downto 1318);
  ahbso2(1).hsplit <= S2(1365 downto 1350);
  ahbso2(1).hcache <= S2(1366);
  ahbso2(1).hirq <= S2(1398 downto 1367);
  ahbso2(1).hconfig(0) <= S2(1430 downto 1399);
  ahbso2(1).hconfig(1) <= S2(1462 downto 1431);
  ahbso2(1).hconfig(2) <= S2(1494 downto 1463);
  ahbso2(1).hconfig(3) <= S2(1526 downto 1495);
  ahbso2(1).hconfig(4) <= S2(1558 downto 1527);
  ahbso2(1).hconfig(5) <= S2(1590 downto 1559);
  ahbso2(1).hconfig(6) <= S2(1622 downto 1591);
  ahbso2(1).hconfig(7) <= S2(1654 downto 1623);
  ahbso2(1).hindex <= conv_integer(S2(1658 downto 1655));
  ahbso2(2).hready <= S2(1659);
  ahbso2(2).hresp <= S2(1661 downto 1660);
  ahbso2(2).hrdata <= S2(1693 downto 1662);
  ahbso2(2).hsplit <= S2(1709 downto 1694);
  ahbso2(2).hcache <= S2(1710);
  ahbso2(2).hirq <= S2(1742 downto 1711);
  ahbso2(2).hconfig(0) <= S2(1774 downto 1743);
  ahbso2(2).hconfig(1) <= S2(1806 downto 1775);
  ahbso2(2).hconfig(2) <= S2(1838 downto 1807);
  ahbso2(2).hconfig(3) <= S2(1870 downto 1839);
  ahbso2(2).hconfig(4) <= S2(1902 downto 1871);
  ahbso2(2).hconfig(5) <= S2(1934 downto 1903);
  ahbso2(2).hconfig(6) <= S2(1966 downto 1935);
  ahbso2(2).hconfig(7) <= S2(1998 downto 1967);
  ahbso2(2).hindex <= conv_integer(S2(2002 downto 1999));
  ahbso2(3).hready <= S2(2003);
  ahbso2(3).hresp <= S2(2005 downto 2004);
  ahbso2(3).hrdata <= S2(2037 downto 2006);
  ahbso2(3).hsplit <= S2(2053 downto 2038);
  ahbso2(3).hcache <= S2(2054);
  ahbso2(3).hirq <= S2(2086 downto 2055);
  ahbso2(3).hconfig(0) <= S2(2118 downto 2087);
  ahbso2(3).hconfig(1) <= S2(2150 downto 2119);
  ahbso2(3).hconfig(2) <= S2(2182 downto 2151);
  ahbso2(3).hconfig(3) <= S2(2214 downto 2183);
  ahbso2(3).hconfig(4) <= S2(2246 downto 2215);
  ahbso2(3).hconfig(5) <= S2(2278 downto 2247);
  ahbso2(3).hconfig(6) <= S2(2310 downto 2279);
  ahbso2(3).hconfig(7) <= S2(2342 downto 2311);
  ahbso2(3).hindex <= conv_integer(S2(2346 downto 2343));


procCheck : process (inNRst,inClk)
begin


  if(rising_edge(inClk) and (iClkCnt>2)) then
    --if(ch_dmai.address/=dmai.address) then print("Err: dmai.address");  iErrCnt:=iErrCnt+1; end if;
  end if;
end process procCheck;


end;
 
 