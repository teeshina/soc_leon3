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
  constant STRING_SIZE : integer := 2254; -- string size = index of the last element

end soc_noram_tb;
architecture behavior of soc_noram_tb is
  
  component soc_noram 
  port 
  ( 
    inRst       : in std_logic; -- button "Center"
    inDsuBreak  : in std_logic; -- button "North 
    inSysClk_p  : in std_logic;
    inSysClk_n  : in std_logic;
    -- UART
    inCTS   : in std_logic;
    inRX    : in std_logic;
    outRTS  : out std_logic;
    outTX   : out std_logic;
    -- JTAG:
    nTRST : in std_logic; -- in: Test Reset
    TCK   : in std_logic;   -- in: Test Clock
    TMS   : in std_logic;   -- in: Test Mode State
    TDI   : in std_logic;   -- in: Test Data Input
    TDO   : out std_logic;   -- out: Test Data Output
    -- GNSS RF inputs:
    inAdcClk : in std_logic;
    inIa     : in std_logic_vector(1 downto 0);
    inQa     : in std_logic_vector(1 downto 0);
    inIb     : in std_logic_vector(1 downto 0);
    inQb     : in std_logic_vector(1 downto 0);
    -- MAX2769 SPIs and antenna controls signals:
    inLD     : in std_logic_vector(1 downto 0);
    outSCLK  : out std_ulogic;
    outSDATA : out std_ulogic;
    outCSn   : out std_logic_vector(1 downto 0);
    inExtAntStat   : in std_ulogic;
    inExtAntDetect : in std_ulogic;
    outExtAntEna   : out std_ulogic;
    -- Gyroscope SPI interface
    inGyroSDI   : in std_logic;
    inGyroInt1  : in std_logic; -- interrupt 1
    inGyroInt2  : in std_logic; -- interrupt 2
    outGyroSDO  : out std_logic;
    outGyroCSn  : out std_logic;
    outGyroSPC  : out std_logic;
    -- Accelerometer SPI interface
    inAccelerSDI   : in std_logic;
    inAccelerInt1  : in std_logic; -- interrupt 1
    inAccelerInt2  : in std_logic; -- interrupt 2
    inAccelerSDO  : out std_logic;
    inAccelerCSn  : out std_logic;
    inAccelerSPC  : out std_logic;
    -- User pins
    inDIP   : in std_ulogic_vector(7 downto 0);
    outLED  : out std_ulogic_vector(7 downto 0);
    -- SRAM signals
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
  -- UART
  signal in_CTS       : std_logic;
  signal in_RX        : std_logic;
  signal ch_RTS       : std_logic;
  signal RTS          : std_logic;
  signal ch_TX        : std_logic;
  signal TX           : std_logic;
  -- JTAG: 
  signal in_trst  : std_logic;
  signal in_tck  : std_logic;
  signal in_tms  : std_logic;
  signal in_tdi  : std_logic;
  signal ch_tdo  : std_logic;
  signal tdo     : std_logic;
  -- ADC samples:
  signal in_Ia  : std_logic_vector(1 downto 0);
  signal in_Qa  : std_logic_vector(1 downto 0);
  signal in_Ib  : std_logic_vector(1 downto 0);
  signal in_Qb  : std_logic_vector(1 downto 0);
  -- MAX2769 SPI interface:
  signal in_LD  : std_logic_vector(1 downto 0);
  signal ch_SCLK  : std_logic;
  signal ch_SDATA  : std_logic;
  signal ch_CSn  : std_logic_vector(1 downto 0);
  signal SCLK  : std_logic;
  signal SDATA  : std_logic;
  signal CSn  : std_logic_vector(1 downto 0);
  -- Antenna control:
  signal in_ExtAntStat  : std_logic;
  signal in_ExtAntDetect  : std_logic;
  signal ch_ExtAntEna  : std_logic;
  signal ExtAntEna  : std_logic;
  -- STM gyroscope controller:
  signal in_GyroSDI  : std_logic;
  signal in_GyroInt1  : std_logic;
  signal in_GyroInt2  : std_logic;
  signal ch_GyroSDO  : std_logic;
  signal ch_GyroCSn  : std_logic;
  signal ch_GyroSPC  : std_logic;
  signal GyroSDO  : std_logic;
  signal GyroCSn  : std_logic;
  signal GyroSPC  : std_logic;
  -- STM Accelerometer controller:
  signal in_AccelerSDI  : std_logic;
  signal in_AccelerInt1  : std_logic;
  signal in_AccelerInt2  : std_logic;
  signal ch_AccelerSDO  : std_logic;
  signal ch_AccelerCSn  : std_logic;
  signal ch_AccelerSPC  : std_logic;
  signal AccelerSDO  : std_logic;
  signal AccelerCSn  : std_logic;
  signal AccelerSPC  : std_logic;
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
  in_CTS <= S(1);
  in_RX <= S(2);
  ch_RTS <= S(3);
  ch_TX <= S(4);
  in_trst <= S(5);
  in_tck <= S(6);
  in_tms <= S(7);
  in_tdi <= S(8);
  ch_tdo <= S(9);
  in_Ia <= S(11 downto 10);
  in_Qa <= S(13 downto 12);
  in_Ib <= S(15 downto 14);
  in_Qb <= S(17 downto 16);
  in_LD <= S(19 downto 18);
  ch_SCLK <= S(20);
  ch_SDATA <= S(21);
  ch_CSn <= S(23 downto 22);
  in_ExtAntStat <= S(24);
  in_ExtAntDetect <= S(25);
  ch_ExtAntEna <= S(26);
  in_GyroSDI <= S(27);
  in_GyroInt1 <= S(28);
  in_GyroInt2 <= S(29);
  ch_GyroSDO <= S(30);
  ch_GyroCSn <= S(31);
  ch_GyroSPC <= S(32);
  in_AccelerSDI <= S(33);
  in_AccelerInt1 <= S(34);
  in_AccelerInt2 <= S(35);
  ch_AccelerSDO <= S(36);
  ch_AccelerCSn <= S(37);
  ch_AccelerSPC <= S(38);
  in_DIP <= U(46 downto 39);
  ch_LED <= U(54 downto 47);
  ch_ClkBus <= U(55);
  ch_ramaddr <= S(71 downto 56);
  ch_hwdata <= S(103 downto 72);
  in_ramdata <= S(135 downto 104);
  ch_ramsel <= S(139 downto 136);
  ch_write <= S(143 downto 140);
  t_msti.hgrant <= S(159 downto 144);
  t_msti.hready <= S(160);
  t_msti.hresp <= S(162 downto 161);
  t_msti.hrdata <= S(194 downto 163);
  t_msti.hcache <= S(195);
  t_msti.hirq <= S(227 downto 196);
  t_msti.testen <= S(228);
  t_msti.testrst <= S(229);
  t_msti.scanen <= S(230);
  t_msti.testoen <= S(231);
  t_msto(0).hbusreq <= S(232);
  t_msto(0).hlock <= S(233);
  t_msto(0).htrans <= S(235 downto 234);
  t_msto(0).haddr <= S(267 downto 236);
  t_msto(0).hwrite <= S(268);
  t_msto(0).hsize <= S(271 downto 269);
  t_msto(0).hburst <= S(274 downto 272);
  t_msto(0).hprot <= S(278 downto 275);
  t_msto(0).hwdata <= S(310 downto 279);
  t_msto(0).hirq <= S(342 downto 311);
  t_msto(0).hconfig(0) <= S(374 downto 343);
  t_msto(0).hconfig(1) <= S(406 downto 375);
  t_msto(0).hconfig(2) <= S(438 downto 407);
  t_msto(0).hconfig(3) <= S(470 downto 439);
  t_msto(0).hconfig(4) <= S(502 downto 471);
  t_msto(0).hconfig(5) <= S(534 downto 503);
  t_msto(0).hconfig(6) <= S(566 downto 535);
  t_msto(0).hconfig(7) <= S(598 downto 567);
  t_msto(0).hindex <= conv_integer(S(602 downto 599));
  t_slvi.hsel <= S(618 downto 603);
  t_slvi.haddr <= S(650 downto 619);
  t_slvi.hwrite <= S(651);
  t_slvi.htrans <= S(653 downto 652);
  t_slvi.hsize <= S(656 downto 654);
  t_slvi.hburst <= S(659 downto 657);
  t_slvi.hwdata <= S(691 downto 660);
  t_slvi.hprot <= S(695 downto 692);
  t_slvi.hready <= S(696);
  t_slvi.hmaster <= S(700 downto 697);
  t_slvi.hmastlock <= S(701);
  t_slvi.hmbsel <= S(705 downto 702);
  t_slvi.hcache <= S(706);
  t_slvi.hirq <= S(738 downto 707);
  t_slvi.testen <= S(739);
  t_slvi.testrst <= S(740);
  t_slvi.scanen <= S(741);
  t_slvi.testoen <= S(742);
  t_apbi.psel <= S(758 downto 743);
  t_apbi.penable <= S(759);
  t_apbi.paddr <= S(791 downto 760);
  t_apbi.pwrite <= S(792);
  t_apbi.pwdata <= S(824 downto 793);
  t_apbi.pirq <= S(856 downto 825);
  t_apbi.testen <= S(857);
  t_apbi.testrst <= S(858);
  t_apbi.scanen <= S(859);
  t_apbi.testoen <= S(860);
  t_pc <= S(892 downto 861);
  t_npc <= S(924 downto 893);
  t_psr <= S(956 downto 925);
  t_tbr <= S(988 downto 957);
  dsu_status <= S(1000 downto 989);
  t_r.d.pc <= S(1030 downto 1001);
  t_r.d.inst(0) <= S(1062 downto 1031);
  t_r.d.inst(1) <= S(1094 downto 1063);
  t_r.d.cwp <= S(1097 downto 1095);
  t_r.d.set <= S(1098 downto 1098);
  t_r.d.mexc <= S(1099);
  t_r.d.cnt <= S(1101 downto 1100);
  t_r.d.pv <= S(1102);
  t_r.d.annul <= S(1103);
  t_r.d.inull <= S(1104);
  t_r.d.step <= S(1105);
  t_r.d.divrdy <= S(1106);
  t_r.a.ctrl.pc <= S(1136 downto 1107);
  t_r.a.ctrl.inst <= S(1168 downto 1137);
  t_r.a.ctrl.cnt <= S(1170 downto 1169);
  t_r.a.ctrl.rd <= S(1178 downto 1171);
  t_r.a.ctrl.tt <= S(1184 downto 1179);
  t_r.a.ctrl.trap <= S(1185);
  t_r.a.ctrl.annul <= S(1186);
  t_r.a.ctrl.wreg <= S(1187);
  t_r.a.ctrl.wicc <= S(1188);
  t_r.a.ctrl.wy <= S(1189);
  t_r.a.ctrl.ld <= S(1190);
  t_r.a.ctrl.pv <= S(1191);
  t_r.a.ctrl.rett <= S(1192);
  t_r.a.rs1 <= S(1197 downto 1193);
  t_r.a.rfa1 <= S(1205 downto 1198);
  t_r.a.rfa2 <= S(1213 downto 1206);
  t_r.a.rsel1 <= S(1216 downto 1214);
  t_r.a.rsel2 <= S(1219 downto 1217);
  t_r.a.rfe1 <= S(1220);
  t_r.a.rfe2 <= S(1221);
  t_r.a.cwp <= S(1224 downto 1222);
  t_r.a.imm <= S(1256 downto 1225);
  t_r.a.ldcheck1 <= S(1257);
  t_r.a.ldcheck2 <= S(1258);
  t_r.a.ldchkra <= S(1259);
  t_r.a.ldchkex <= S(1260);
  t_r.a.su <= S(1261);
  t_r.a.et <= S(1262);
  t_r.a.wovf <= S(1263);
  t_r.a.wunf <= S(1264);
  t_r.a.ticc <= S(1265);
  t_r.a.jmpl <= S(1266);
  t_r.a.step <= S(1267);
  t_r.a.mulstart <= S(1268);
  t_r.a.divstart <= S(1269);
  t_r.a.bp <= S(1270);
  t_r.a.nobp <= S(1271);
  t_r.e.ctrl.pc <= S(1301 downto 1272);
  t_r.e.ctrl.inst <= S(1333 downto 1302);
  t_r.e.ctrl.cnt <= S(1335 downto 1334);
  t_r.e.ctrl.rd <= S(1343 downto 1336);
  t_r.e.ctrl.tt <= S(1349 downto 1344);
  t_r.e.ctrl.trap <= S(1350);
  t_r.e.ctrl.annul <= S(1351);
  t_r.e.ctrl.wreg <= S(1352);
  t_r.e.ctrl.wicc <= S(1353);
  t_r.e.ctrl.wy <= S(1354);
  t_r.e.ctrl.ld <= S(1355);
  t_r.e.ctrl.pv <= S(1356);
  t_r.e.ctrl.rett <= S(1357);
  t_r.e.op1 <= S(1389 downto 1358);
  t_r.e.op2 <= S(1421 downto 1390);
  t_r.e.aluop <= S(1424 downto 1422);
  t_r.e.alusel <= S(1426 downto 1425);
  t_r.e.aluadd <= S(1427);
  t_r.e.alucin <= S(1428);
  t_r.e.ldbp1 <= S(1429);
  t_r.e.ldbp2 <= S(1430);
  t_r.e.invop2 <= S(1431);
  t_r.e.shcnt <= S(1436 downto 1432);
  t_r.e.sari <= S(1437);
  t_r.e.shleft <= S(1438);
  t_r.e.ymsb <= S(1439);
  t_r.e.rd <= S(1444 downto 1440);
  t_r.e.jmpl <= S(1445);
  t_r.e.su <= S(1446);
  t_r.e.et <= S(1447);
  t_r.e.cwp <= S(1450 downto 1448);
  t_r.e.icc <= S(1454 downto 1451);
  t_r.e.mulstep <= S(1455);
  t_r.e.mul <= S(1456);
  t_r.e.mac <= S(1457);
  t_r.e.bp <= S(1458);
  t_r.m.ctrl.pc <= S(1488 downto 1459);
  t_r.m.ctrl.inst <= S(1520 downto 1489);
  t_r.m.ctrl.cnt <= S(1522 downto 1521);
  t_r.m.ctrl.rd <= S(1530 downto 1523);
  t_r.m.ctrl.tt <= S(1536 downto 1531);
  t_r.m.ctrl.trap <= S(1537);
  t_r.m.ctrl.annul <= S(1538);
  t_r.m.ctrl.wreg <= S(1539);
  t_r.m.ctrl.wicc <= S(1540);
  t_r.m.ctrl.wy <= S(1541);
  t_r.m.ctrl.ld <= S(1542);
  t_r.m.ctrl.pv <= S(1543);
  t_r.m.ctrl.rett <= S(1544);
  t_r.m.result <= S(1576 downto 1545);
  t_r.m.y <= S(1608 downto 1577);
  t_r.m.icc <= S(1612 downto 1609);
  t_r.m.nalign <= S(1613);
  t_r.m.dci.signed <= S(1614);
  t_r.m.dci.enaddr <= S(1615);
  t_r.m.dci.read <= S(1616);
  t_r.m.dci.write <= S(1617);
  t_r.m.dci.lock <= S(1618);
  t_r.m.dci.dsuen <= S(1619);
  t_r.m.dci.size <= S(1621 downto 1620);
  t_r.m.dci.asi <= S(1629 downto 1622);
  t_r.m.werr <= S(1630);
  t_r.m.wcwp <= S(1631);
  t_r.m.irqen <= S(1632);
  t_r.m.irqen2 <= S(1633);
  t_r.m.mac <= S(1634);
  t_r.m.divz <= S(1635);
  t_r.m.su <= S(1636);
  t_r.m.mul <= S(1637);
  t_r.m.casa <= S(1638);
  t_r.m.casaz <= S(1639);
  t_r.x.ctrl.pc <= S(1669 downto 1640);
  t_r.x.ctrl.inst <= S(1701 downto 1670);
  t_r.x.ctrl.cnt <= S(1703 downto 1702);
  t_r.x.ctrl.rd <= S(1711 downto 1704);
  t_r.x.ctrl.tt <= S(1717 downto 1712);
  t_r.x.ctrl.trap <= S(1718);
  t_r.x.ctrl.annul <= S(1719);
  t_r.x.ctrl.wreg <= S(1720);
  t_r.x.ctrl.wicc <= S(1721);
  t_r.x.ctrl.wy <= S(1722);
  t_r.x.ctrl.ld <= S(1723);
  t_r.x.ctrl.pv <= S(1724);
  t_r.x.ctrl.rett <= S(1725);
  t_r.x.result <= S(1757 downto 1726);
  t_r.x.y <= S(1789 downto 1758);
  t_r.x.icc <= S(1793 downto 1790);
  t_r.x.annul_all <= S(1794);
  t_r.x.data(0) <= S(1826 downto 1795);
  t_r.x.data(1) <= S(1858 downto 1827);
  t_r.x.set <= S(1859 downto 1859);
  t_r.x.mexc <= S(1860);
  t_r.x.dci.signed <= S(1861);
  t_r.x.dci.enaddr <= S(1862);
  t_r.x.dci.read <= S(1863);
  t_r.x.dci.write <= S(1864);
  t_r.x.dci.lock <= S(1865);
  t_r.x.dci.dsuen <= S(1866);
  t_r.x.dci.size <= S(1868 downto 1867);
  t_r.x.dci.asi <= S(1876 downto 1869);
  t_r.x.laddr <= S(1878 downto 1877);
  --  in_r.x.rstate <= S(1880 downto 1879);
  t_r.x.npc <= S(1883 downto 1881);
  t_r.x.intack <= S(1884);
  t_r.x.ipend <= S(1885);
  t_r.x.mac <= S(1886);
  t_r.x.debug <= S(1887);
  t_r.x.nerror <= S(1888);
  t_r.f.pc <= S(1918 downto 1889);
  t_r.f.branch <= S(1919);
  t_r.w.s.cwp <= S(1922 downto 1920);
  t_r.w.s.icc <= S(1926 downto 1923);
  t_r.w.s.tt <= S(1934 downto 1927);
  t_r.w.s.tba <= S(1954 downto 1935);
  t_r.w.s.wim <= S(1962 downto 1955);
  t_r.w.s.pil <= S(1966 downto 1963);
  t_r.w.s.ec <= S(1967);
  t_r.w.s.ef <= S(1968);
  t_r.w.s.ps <= S(1969);
  t_r.w.s.s <= S(1970);
  t_r.w.s.et <= S(1971);
  t_r.w.s.y <= S(2003 downto 1972);
  t_r.w.s.asr18 <= S(2035 downto 2004);
  t_r.w.s.svt <= S(2036);
  t_r.w.s.dwt <= S(2037);
  t_r.w.s.dbp <= S(2038);
  t_r.w.result <= S(2070 downto 2039);
  t_icrami_tag0 <= S(2102 downto 2071);
  t_icramo_tag0 <= S(2134 downto 2103);
  t_hit <= S(2135);
  t_ici_inull <= S(2136);
  t_ici_fpc <= S(2168 downto 2137);
  t_setrepl <= S(2169);
  t_itaddr <= S(2177 downto 2170);
  t_itdatain0 <= S(2213 downto 2178);
  t_itdataout0 <= S(2248 downto 2214);
  t_itenable <= S(2249);
  t_itwrite <= S(2253 downto 2250);


  
  tt : soc_noram  port map
  ( 
    inRst => inNRst,
    inDsuBreak => '0',
    inSysClk_p  => inClk,
    inSysClk_n  => '0',
    -- UART
    inCTS  => in_CTS,
    inRX  => in_RX,
    outRTS  => RTS,
    outTX   => TX,
    -- JTAG:
    nTRST => in_trst,
    TCK => in_tck,
    TMS => in_tms,
    TDI => in_tdi,
    TDO => tdo,
    -- GNSS RF inputs:
    inAdcClk => inClk,
    inIa => in_Ia,
    inQa => in_Qa,
    inIb => in_Ib,
    inQb => in_Qb,
    -- MAX2769 SPIs and antenna controls signals:
    inLD  => in_LD,
    outSCLK  => SCLK,
    outSDATA => SDATA,
    outCSn => CSn,
    -- Antenna controller:
    inExtAntStat => in_ExtAntStat,
    inExtAntDetect => in_ExtAntDetect,
    outExtAntEna  => ExtAntEna,
    -- Gyroscope SPI interface
    inGyroSDI  => in_GyroSDI,
    inGyroInt1 => in_GyroInt1,
    inGyroInt2 => in_GyroInt2,
    outGyroSDO => GyroSDO,
    outGyroCSn => GyroCSn,
    outGyroSPC => GyroSPC,
    -- Accelerometer SPI interface
    inAccelerSDI => in_AccelerSDI,
    inAccelerInt1 => in_AccelerInt1,
    inAccelerInt2 => in_AccelerInt2,
    inAccelerSDO => AccelerSDO,
    inAccelerCSn => AccelerCSn,
    inAccelerSPC => AccelerSPC,
    -- User pins
    inDIP    => in_DIP,
    outLED  => LED,
    outClkBus => ClkBus,
    -- SRAM signals
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
 
 