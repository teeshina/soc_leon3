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



entity soc_noram_tb is

  constant CLK_HPERIOD : time := 10 ps;
  constant STRING_SIZE : integer := 2225; -- string size = index of the last element

end soc_noram_tb;
architecture behavior of soc_noram_tb is
  
  component soc_noram port 
  ( 
    inRst       : in std_logic; -- button "Center"
    inDsuBreak  : in std_logic; -- button "North
    inSysClk_p  : in std_logic;
    inSysClk_n  : in std_logic;
    inCTS   : in std_logic;
    inRX    : in std_logic;
    nTRST : in std_logic; -- in: Test Reset
    TCK   : in std_logic;   -- in: Test Clock
    TMS   : in std_logic;   -- in: Test Mode State
    TDI   : in std_logic;   -- in: Test Data Input
    TDO   : out std_logic;   -- out: Test Data Output
    outRTS  : out std_logic;
    outTX   : out std_logic;
    inDIP   : in std_ulogic_vector(7 downto 0);
    outLED  : out std_ulogic_vector(7 downto 0);
    outClkBus : out std_logic;
    ramaddr : out std_logic_vector(CFG_SRAM_ADRBITS-1 downto 0);
    hwdata  : out std_logic_vector(AHBDW-1 downto 0);
    ramdata : in std_logic_vector(AHBDW-1 downto 0);
    ramsel  : out std_logic_vector(AHBDW/8-1 downto 0);
    write   : out std_logic_vector(AHBDW/8-1 downto 0)
  );
  end component;


  -- input/output signals:
  signal inNRst       : std_logic:= '0';
  signal inClk        : std_logic:= '0';
  --JTAG: 
  signal in_trst  : std_logic;
  signal in_tck  : std_logic;
  signal in_tms  : std_logic;
  signal in_tdi  : std_logic;
  signal ch_tdo  : std_logic;
  signal tdo     : std_logic;
  -- UART
  signal in_CTS       : std_logic;
  signal in_RX        : std_logic;
  signal ch_RTS       : std_logic;
  signal RTS          : std_logic;
  signal ch_TX        : std_logic;
  signal TX           : std_logic;
  -- others:
  signal in_DIP       : std_ulogic_vector(7 downto 0);
  signal ch_LED       : std_ulogic_vector(7 downto 0);
  signal LED          : std_ulogic_vector(7 downto 0);
  -- SRAM
  signal ch_ClkBus  : std_logic;
  signal ClkBus  : std_logic;
  signal ch_ramaddr : std_logic_vector(CFG_SRAM_ADRBITS-1 downto 0);
  signal ramaddr : std_logic_vector(CFG_SRAM_ADRBITS-1 downto 0);
  signal ch_hwdata  : std_logic_vector(AHBDW-1 downto 0);
  signal hwdata  : std_logic_vector(AHBDW-1 downto 0);
  signal in_ramdata : std_logic_vector(AHBDW-1 downto 0);
  signal ch_ramsel  : std_logic_vector(AHBDW/8-1 downto 0);
  signal ramsel  : std_logic_vector(AHBDW/8-1 downto 0);
  signal ch_write   : std_logic_vector(AHBDW/8-1 downto 0);
  signal write   : std_logic_vector(AHBDW/8-1 downto 0);

  signal t_msti : ahb_mst_in_type;
  signal t_msto : ahb_mst_out_vector := (others => ahbm_none);
  signal t_slvi : ahb_slv_in_type;
  signal t_apbi : apb_slv_in_type;
  signal t_pc  : std_logic_vector(31 downto 0);
  signal t_npc  : std_logic_vector(31 downto 0);
  signal t_psr  : std_logic_vector(31 downto 0);
  signal t_tbr  : std_logic_vector(31 downto 0);
  signal dsu_status  : std_logic_vector(11 downto 0);
  
  signal t_r : registers;

  signal t_icrami_tag0  : std_logic_vector(31 downto 0);
  signal t_icramo_tag0  : std_logic_vector(31 downto 0);
  signal t_hit  : std_logic;
  signal t_ici_inull  : std_logic;
  signal t_ici_fpc  : std_logic_vector(31 downto 0);
  signal t_setrepl  : std_logic;
  signal t_itaddr  : std_logic_vector(7 downto 0);
  signal t_itdatain0  : std_logic_vector(35 downto 0);
  signal t_itdataout0  : std_logic_vector(34 downto 0);
  signal t_itenable  : std_logic;
  signal t_itwrite  : std_logic_vector(0 to 3);

  
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
      if(iClkCnt=3405) then
        print("Break");
      end if;
    end loop;
  end process procReadingFile;
  


-- signal parsment and assignment
  inNRst <= S(0);
  in_trst <= S(1);
  in_tck <= S(2);
  in_tms <= S(3);
  in_tdi <= S(4);
  ch_tdo <= S(5);
  in_CTS <= S(6);
  in_RX <= S(7);
  ch_RTS <= S(8);
  ch_TX <= S(9);
  in_DIP <= U(17 downto 10);
  ch_LED <= U(25 downto 18);
  ch_ClkBus <= U(26);
  ch_ramaddr <= S(42 downto 27);
  ch_hwdata <= S(74 downto 43);
  in_ramdata <= S(106 downto 75);
  ch_ramsel <= S(110 downto 107);
  ch_write <= S(114 downto 111);
  t_msti.hgrant <= S(130 downto 115);
  t_msti.hready <= S(131);
  t_msti.hresp <= S(133 downto 132);
  t_msti.hrdata <= S(165 downto 134);
  t_msti.hcache <= S(166);
  t_msti.hirq <= S(198 downto 167);
  t_msti.testen <= S(199);
  t_msti.testrst <= S(200);
  t_msti.scanen <= S(201);
  t_msti.testoen <= S(202);
  t_msto(0).hbusreq <= S(203);
  t_msto(0).hlock <= S(204);
  t_msto(0).htrans <= S(206 downto 205);
  t_msto(0).haddr <= S(238 downto 207);
  t_msto(0).hwrite <= S(239);
  t_msto(0).hsize <= S(242 downto 240);
  t_msto(0).hburst <= S(245 downto 243);
  t_msto(0).hprot <= S(249 downto 246);
  t_msto(0).hwdata <= S(281 downto 250);
  t_msto(0).hirq <= S(313 downto 282);
  t_msto(0).hconfig(0) <= S(345 downto 314);
  t_msto(0).hconfig(1) <= S(377 downto 346);
  t_msto(0).hconfig(2) <= S(409 downto 378);
  t_msto(0).hconfig(3) <= S(441 downto 410);
  t_msto(0).hconfig(4) <= S(473 downto 442);
  t_msto(0).hconfig(5) <= S(505 downto 474);
  t_msto(0).hconfig(6) <= S(537 downto 506);
  t_msto(0).hconfig(7) <= S(569 downto 538);
  t_msto(0).hindex <= conv_integer(S(573 downto 570));
  t_slvi.hsel <= S(589 downto 574);
  t_slvi.haddr <= S(621 downto 590);
  t_slvi.hwrite <= S(622);
  t_slvi.htrans <= S(624 downto 623);
  t_slvi.hsize <= S(627 downto 625);
  t_slvi.hburst <= S(630 downto 628);
  t_slvi.hwdata <= S(662 downto 631);
  t_slvi.hprot <= S(666 downto 663);
  t_slvi.hready <= S(667);
  t_slvi.hmaster <= S(671 downto 668);
  t_slvi.hmastlock <= S(672);
  t_slvi.hmbsel <= S(676 downto 673);
  t_slvi.hcache <= S(677);
  t_slvi.hirq <= S(709 downto 678);
  t_slvi.testen <= S(710);
  t_slvi.testrst <= S(711);
  t_slvi.scanen <= S(712);
  t_slvi.testoen <= S(713);
  t_apbi.psel <= S(729 downto 714);
  t_apbi.penable <= S(730);
  t_apbi.paddr <= S(762 downto 731);
  t_apbi.pwrite <= S(763);
  t_apbi.pwdata <= S(795 downto 764);
  t_apbi.pirq <= S(827 downto 796);
  t_apbi.testen <= S(828);
  t_apbi.testrst <= S(829);
  t_apbi.scanen <= S(830);
  t_apbi.testoen <= S(831);
  t_pc <= S(863 downto 832);
  t_npc <= S(895 downto 864);
  t_psr <= S(927 downto 896);
  t_tbr <= S(959 downto 928);
  dsu_status <= S(971 downto 960);
  t_r.d.pc <= S(1001 downto 972);
  t_r.d.inst(0) <= S(1033 downto 1002);
  t_r.d.inst(1) <= S(1065 downto 1034);
  t_r.d.cwp <= S(1068 downto 1066);
  t_r.d.set <= S(1069 downto 1069);
  t_r.d.mexc <= S(1070);
  t_r.d.cnt <= S(1072 downto 1071);
  t_r.d.pv <= S(1073);
  t_r.d.annul <= S(1074);
  t_r.d.inull <= S(1075);
  t_r.d.step <= S(1076);
  t_r.d.divrdy <= S(1077);
  t_r.a.ctrl.pc <= S(1107 downto 1078);
  t_r.a.ctrl.inst <= S(1139 downto 1108);
  t_r.a.ctrl.cnt <= S(1141 downto 1140);
  t_r.a.ctrl.rd <= S(1149 downto 1142);
  t_r.a.ctrl.tt <= S(1155 downto 1150);
  t_r.a.ctrl.trap <= S(1156);
  t_r.a.ctrl.annul <= S(1157);
  t_r.a.ctrl.wreg <= S(1158);
  t_r.a.ctrl.wicc <= S(1159);
  t_r.a.ctrl.wy <= S(1160);
  t_r.a.ctrl.ld <= S(1161);
  t_r.a.ctrl.pv <= S(1162);
  t_r.a.ctrl.rett <= S(1163);
  t_r.a.rs1 <= S(1168 downto 1164);
  t_r.a.rfa1 <= S(1176 downto 1169);
  t_r.a.rfa2 <= S(1184 downto 1177);
  t_r.a.rsel1 <= S(1187 downto 1185);
  t_r.a.rsel2 <= S(1190 downto 1188);
  t_r.a.rfe1 <= S(1191);
  t_r.a.rfe2 <= S(1192);
  t_r.a.cwp <= S(1195 downto 1193);
  t_r.a.imm <= S(1227 downto 1196);
  t_r.a.ldcheck1 <= S(1228);
  t_r.a.ldcheck2 <= S(1229);
  t_r.a.ldchkra <= S(1230);
  t_r.a.ldchkex <= S(1231);
  t_r.a.su <= S(1232);
  t_r.a.et <= S(1233);
  t_r.a.wovf <= S(1234);
  t_r.a.wunf <= S(1235);
  t_r.a.ticc <= S(1236);
  t_r.a.jmpl <= S(1237);
  t_r.a.step <= S(1238);
  t_r.a.mulstart <= S(1239);
  t_r.a.divstart <= S(1240);
  t_r.a.bp <= S(1241);
  t_r.a.nobp <= S(1242);
  t_r.e.ctrl.pc <= S(1272 downto 1243);
  t_r.e.ctrl.inst <= S(1304 downto 1273);
  t_r.e.ctrl.cnt <= S(1306 downto 1305);
  t_r.e.ctrl.rd <= S(1314 downto 1307);
  t_r.e.ctrl.tt <= S(1320 downto 1315);
  t_r.e.ctrl.trap <= S(1321);
  t_r.e.ctrl.annul <= S(1322);
  t_r.e.ctrl.wreg <= S(1323);
  t_r.e.ctrl.wicc <= S(1324);
  t_r.e.ctrl.wy <= S(1325);
  t_r.e.ctrl.ld <= S(1326);
  t_r.e.ctrl.pv <= S(1327);
  t_r.e.ctrl.rett <= S(1328);
  t_r.e.op1 <= S(1360 downto 1329);
  t_r.e.op2 <= S(1392 downto 1361);
  t_r.e.aluop <= S(1395 downto 1393);
  t_r.e.alusel <= S(1397 downto 1396);
  t_r.e.aluadd <= S(1398);
  t_r.e.alucin <= S(1399);
  t_r.e.ldbp1 <= S(1400);
  t_r.e.ldbp2 <= S(1401);
  t_r.e.invop2 <= S(1402);
  t_r.e.shcnt <= S(1407 downto 1403);
  t_r.e.sari <= S(1408);
  t_r.e.shleft <= S(1409);
  t_r.e.ymsb <= S(1410);
  t_r.e.rd <= S(1415 downto 1411);
  t_r.e.jmpl <= S(1416);
  t_r.e.su <= S(1417);
  t_r.e.et <= S(1418);
  t_r.e.cwp <= S(1421 downto 1419);
  t_r.e.icc <= S(1425 downto 1422);
  t_r.e.mulstep <= S(1426);
  t_r.e.mul <= S(1427);
  t_r.e.mac <= S(1428);
  t_r.e.bp <= S(1429);
  t_r.m.ctrl.pc <= S(1459 downto 1430);
  t_r.m.ctrl.inst <= S(1491 downto 1460);
  t_r.m.ctrl.cnt <= S(1493 downto 1492);
  t_r.m.ctrl.rd <= S(1501 downto 1494);
  t_r.m.ctrl.tt <= S(1507 downto 1502);
  t_r.m.ctrl.trap <= S(1508);
  t_r.m.ctrl.annul <= S(1509);
  t_r.m.ctrl.wreg <= S(1510);
  t_r.m.ctrl.wicc <= S(1511);
  t_r.m.ctrl.wy <= S(1512);
  t_r.m.ctrl.ld <= S(1513);
  t_r.m.ctrl.pv <= S(1514);
  t_r.m.ctrl.rett <= S(1515);
  t_r.m.result <= S(1547 downto 1516);
  t_r.m.y <= S(1579 downto 1548);
  t_r.m.icc <= S(1583 downto 1580);
  t_r.m.nalign <= S(1584);
  t_r.m.dci.signed <= S(1585);
  t_r.m.dci.enaddr <= S(1586);
  t_r.m.dci.read <= S(1587);
  t_r.m.dci.write <= S(1588);
  t_r.m.dci.lock <= S(1589);
  t_r.m.dci.dsuen <= S(1590);
  t_r.m.dci.size <= S(1592 downto 1591);
  t_r.m.dci.asi <= S(1600 downto 1593);
  t_r.m.werr <= S(1601);
  t_r.m.wcwp <= S(1602);
  t_r.m.irqen <= S(1603);
  t_r.m.irqen2 <= S(1604);
  t_r.m.mac <= S(1605);
  t_r.m.divz <= S(1606);
  t_r.m.su <= S(1607);
  t_r.m.mul <= S(1608);
  t_r.m.casa <= S(1609);
  t_r.m.casaz <= S(1610);
  t_r.x.ctrl.pc <= S(1640 downto 1611);
  t_r.x.ctrl.inst <= S(1672 downto 1641);
  t_r.x.ctrl.cnt <= S(1674 downto 1673);
  t_r.x.ctrl.rd <= S(1682 downto 1675);
  t_r.x.ctrl.tt <= S(1688 downto 1683);
  t_r.x.ctrl.trap <= S(1689);
  t_r.x.ctrl.annul <= S(1690);
  t_r.x.ctrl.wreg <= S(1691);
  t_r.x.ctrl.wicc <= S(1692);
  t_r.x.ctrl.wy <= S(1693);
  t_r.x.ctrl.ld <= S(1694);
  t_r.x.ctrl.pv <= S(1695);
  t_r.x.ctrl.rett <= S(1696);
  t_r.x.result <= S(1728 downto 1697);
  t_r.x.y <= S(1760 downto 1729);
  t_r.x.icc <= S(1764 downto 1761);
  t_r.x.annul_all <= S(1765);
  t_r.x.data(0) <= S(1797 downto 1766);
  t_r.x.data(1) <= S(1829 downto 1798);
  t_r.x.set <= S(1830 downto 1830);
  t_r.x.mexc <= S(1831);
  t_r.x.dci.signed <= S(1832);
  t_r.x.dci.enaddr <= S(1833);
  t_r.x.dci.read <= S(1834);
  t_r.x.dci.write <= S(1835);
  t_r.x.dci.lock <= S(1836);
  t_r.x.dci.dsuen <= S(1837);
  t_r.x.dci.size <= S(1839 downto 1838);
  t_r.x.dci.asi <= S(1847 downto 1840);
  t_r.x.laddr <= S(1849 downto 1848);
  --  in_r.x.rstate <= S(1851 downto 1850);
  t_r.x.npc <= S(1854 downto 1852);
  t_r.x.intack <= S(1855);
  t_r.x.ipend <= S(1856);
  t_r.x.mac <= S(1857);
  t_r.x.debug <= S(1858);
  t_r.x.nerror <= S(1859);
  t_r.f.pc <= S(1889 downto 1860);
  t_r.f.branch <= S(1890);
  t_r.w.s.cwp <= S(1893 downto 1891);
  t_r.w.s.icc <= S(1897 downto 1894);
  t_r.w.s.tt <= S(1905 downto 1898);
  t_r.w.s.tba <= S(1925 downto 1906);
  t_r.w.s.wim <= S(1933 downto 1926);
  t_r.w.s.pil <= S(1937 downto 1934);
  t_r.w.s.ec <= S(1938);
  t_r.w.s.ef <= S(1939);
  t_r.w.s.ps <= S(1940);
  t_r.w.s.s <= S(1941);
  t_r.w.s.et <= S(1942);
  t_r.w.s.y <= S(1974 downto 1943);
  t_r.w.s.asr18 <= S(2006 downto 1975);
  t_r.w.s.svt <= S(2007);
  t_r.w.s.dwt <= S(2008);
  t_r.w.s.dbp <= S(2009);
  t_r.w.result <= S(2041 downto 2010);
  t_icrami_tag0 <= S(2073 downto 2042);
  t_icramo_tag0 <= S(2105 downto 2074);
  t_hit <= S(2106);
  t_ici_inull <= S(2107);
  t_ici_fpc <= S(2139 downto 2108);
  t_setrepl <= S(2140);
  t_itaddr <= S(2148 downto 2141);
  t_itdatain0 <= S(2184 downto 2149);
  t_itdataout0 <= S(2219 downto 2185);
  t_itenable <= S(2220);
  t_itwrite <= S(2224 downto 2221);


  
  tt : soc_noram  port map
  ( 
    inRst => inNRst,
    inDsuBreak => '0',
    inSysClk_p  => inClk,
    inSysClk_n  => '0',
    inCTS  => in_CTS,
    inRX  => in_RX,
    outRTS  => RTS,
    outTX   => TX,
    nTRST => in_trst,
    TCK => in_tck,
    TMS => in_tms,
    TDI => in_tdi,
    TDO => tdo,
    inDIP    => in_DIP,
    outLED  => LED,
    outClkBus => ClkBus,
    ramaddr => ramaddr,
    hwdata => hwdata,
    ramdata => in_ramdata,
    ramsel => ramsel,
    write => write
  );

  

procCheck : process (inNRst,inClk)
begin


  if(rising_edge(inClk) and (iClkCnt>2)) then
    --if(ch_dmai.address/=dmai.address) then print("Err: dmai.address");  iErrCnt:=iErrCnt+1; end if;
  end if;
end process procCheck;


end;
 
 