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
  constant STRING_SIZE : integer := 1671; -- string size = index of the last element

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
  signal t_slvi : ahb_slv_in_type;
  signal t_apbi : apb_slv_in_type;
  signal t_pc  : std_logic_vector(31 downto 0);
  signal t_npc  : std_logic_vector(31 downto 0);
  signal t_psr  : std_logic_vector(31 downto 0);
  signal t_tbr  : std_logic_vector(31 downto 0);
  signal dsu_status  : std_logic_vector(11 downto 0);
  
  signal t_r : registers;


  
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
  t_slvi.hsel <= S(218 downto 203);
  t_slvi.haddr <= S(250 downto 219);
  t_slvi.hwrite <= S(251);
  t_slvi.htrans <= S(253 downto 252);
  t_slvi.hsize <= S(256 downto 254);
  t_slvi.hburst <= S(259 downto 257);
  t_slvi.hwdata <= S(291 downto 260);
  t_slvi.hprot <= S(295 downto 292);
  t_slvi.hready <= S(296);
  t_slvi.hmaster <= S(300 downto 297);
  t_slvi.hmastlock <= S(301);
  t_slvi.hmbsel <= S(305 downto 302);
  t_slvi.hcache <= S(306);
  t_slvi.hirq <= S(338 downto 307);
  t_slvi.testen <= S(339);
  t_slvi.testrst <= S(340);
  t_slvi.scanen <= S(341);
  t_slvi.testoen <= S(342);
  t_apbi.psel <= S(358 downto 343);
  t_apbi.penable <= S(359);
  t_apbi.paddr <= S(391 downto 360);
  t_apbi.pwrite <= S(392);
  t_apbi.pwdata <= S(424 downto 393);
  t_apbi.pirq <= S(456 downto 425);
  t_apbi.testen <= S(457);
  t_apbi.testrst <= S(458);
  t_apbi.scanen <= S(459);
  t_apbi.testoen <= S(460);
  t_pc <= S(492 downto 461);
  t_npc <= S(524 downto 493);
  t_psr <= S(556 downto 525);
  t_tbr <= S(588 downto 557);
  dsu_status <= S(600 downto 589);
  t_r.d.pc <= S(630 downto 601);
  t_r.d.inst(0) <= S(662 downto 631);
  t_r.d.inst(1) <= S(694 downto 663);
  t_r.d.cwp <= S(697 downto 695);
  t_r.d.set <= S(698 downto 698);
  t_r.d.mexc <= S(699);
  t_r.d.cnt <= S(701 downto 700);
  t_r.d.pv <= S(702);
  t_r.d.annul <= S(703);
  t_r.d.inull <= S(704);
  t_r.d.step <= S(705);
  t_r.d.divrdy <= S(706);
  t_r.a.ctrl.pc <= S(736 downto 707);
  t_r.a.ctrl.inst <= S(768 downto 737);
  t_r.a.ctrl.cnt <= S(770 downto 769);
  t_r.a.ctrl.rd <= S(778 downto 771);
  t_r.a.ctrl.tt <= S(784 downto 779);
  t_r.a.ctrl.trap <= S(785);
  t_r.a.ctrl.annul <= S(786);
  t_r.a.ctrl.wreg <= S(787);
  t_r.a.ctrl.wicc <= S(788);
  t_r.a.ctrl.wy <= S(789);
  t_r.a.ctrl.ld <= S(790);
  t_r.a.ctrl.pv <= S(791);
  t_r.a.ctrl.rett <= S(792);
  t_r.a.rs1 <= S(797 downto 793);
  t_r.a.rfa1 <= S(805 downto 798);
  t_r.a.rfa2 <= S(813 downto 806);
  t_r.a.rsel1 <= S(816 downto 814);
  t_r.a.rsel2 <= S(819 downto 817);
  t_r.a.rfe1 <= S(820);
  t_r.a.rfe2 <= S(821);
  t_r.a.cwp <= S(824 downto 822);
  t_r.a.imm <= S(856 downto 825);
  t_r.a.ldcheck1 <= S(857);
  t_r.a.ldcheck2 <= S(858);
  t_r.a.ldchkra <= S(859);
  t_r.a.ldchkex <= S(860);
  t_r.a.su <= S(861);
  t_r.a.et <= S(862);
  t_r.a.wovf <= S(863);
  t_r.a.wunf <= S(864);
  t_r.a.ticc <= S(865);
  t_r.a.jmpl <= S(866);
  t_r.a.step <= S(867);
  t_r.a.mulstart <= S(868);
  t_r.a.divstart <= S(869);
  t_r.a.bp <= S(870);
  t_r.a.nobp <= S(871);
  t_r.e.ctrl.pc <= S(901 downto 872);
  t_r.e.ctrl.inst <= S(933 downto 902);
  t_r.e.ctrl.cnt <= S(935 downto 934);
  t_r.e.ctrl.rd <= S(943 downto 936);
  t_r.e.ctrl.tt <= S(949 downto 944);
  t_r.e.ctrl.trap <= S(950);
  t_r.e.ctrl.annul <= S(951);
  t_r.e.ctrl.wreg <= S(952);
  t_r.e.ctrl.wicc <= S(953);
  t_r.e.ctrl.wy <= S(954);
  t_r.e.ctrl.ld <= S(955);
  t_r.e.ctrl.pv <= S(956);
  t_r.e.ctrl.rett <= S(957);
  t_r.e.op1 <= S(989 downto 958);
  t_r.e.op2 <= S(1021 downto 990);
  t_r.e.aluop <= S(1024 downto 1022);
  t_r.e.alusel <= S(1026 downto 1025);
  t_r.e.aluadd <= S(1027);
  t_r.e.alucin <= S(1028);
  t_r.e.ldbp1 <= S(1029);
  t_r.e.ldbp2 <= S(1030);
  t_r.e.invop2 <= S(1031);
  t_r.e.shcnt <= S(1036 downto 1032);
  t_r.e.sari <= S(1037);
  t_r.e.shleft <= S(1038);
  t_r.e.ymsb <= S(1039);
  t_r.e.rd <= S(1044 downto 1040);
  t_r.e.jmpl <= S(1045);
  t_r.e.su <= S(1046);
  t_r.e.et <= S(1047);
  t_r.e.cwp <= S(1050 downto 1048);
  t_r.e.icc <= S(1054 downto 1051);
  t_r.e.mulstep <= S(1055);
  t_r.e.mul <= S(1056);
  t_r.e.mac <= S(1057);
  t_r.e.bp <= S(1058);
  t_r.m.ctrl.pc <= S(1088 downto 1059);
  t_r.m.ctrl.inst <= S(1120 downto 1089);
  t_r.m.ctrl.cnt <= S(1122 downto 1121);
  t_r.m.ctrl.rd <= S(1130 downto 1123);
  t_r.m.ctrl.tt <= S(1136 downto 1131);
  t_r.m.ctrl.trap <= S(1137);
  t_r.m.ctrl.annul <= S(1138);
  t_r.m.ctrl.wreg <= S(1139);
  t_r.m.ctrl.wicc <= S(1140);
  t_r.m.ctrl.wy <= S(1141);
  t_r.m.ctrl.ld <= S(1142);
  t_r.m.ctrl.pv <= S(1143);
  t_r.m.ctrl.rett <= S(1144);
  t_r.m.result <= S(1176 downto 1145);
  t_r.m.y <= S(1208 downto 1177);
  t_r.m.icc <= S(1212 downto 1209);
  t_r.m.nalign <= S(1213);
  t_r.m.dci.signed <= S(1214);
  t_r.m.dci.enaddr <= S(1215);
  t_r.m.dci.read <= S(1216);
  t_r.m.dci.write <= S(1217);
  t_r.m.dci.lock <= S(1218);
  t_r.m.dci.dsuen <= S(1219);
  t_r.m.dci.size <= S(1221 downto 1220);
  t_r.m.dci.asi <= S(1229 downto 1222);
  t_r.m.werr <= S(1230);
  t_r.m.wcwp <= S(1231);
  t_r.m.irqen <= S(1232);
  t_r.m.irqen2 <= S(1233);
  t_r.m.mac <= S(1234);
  t_r.m.divz <= S(1235);
  t_r.m.su <= S(1236);
  t_r.m.mul <= S(1237);
  t_r.m.casa <= S(1238);
  t_r.m.casaz <= S(1239);
  t_r.x.ctrl.pc <= S(1269 downto 1240);
  t_r.x.ctrl.inst <= S(1301 downto 1270);
  t_r.x.ctrl.cnt <= S(1303 downto 1302);
  t_r.x.ctrl.rd <= S(1311 downto 1304);
  t_r.x.ctrl.tt <= S(1317 downto 1312);
  t_r.x.ctrl.trap <= S(1318);
  t_r.x.ctrl.annul <= S(1319);
  t_r.x.ctrl.wreg <= S(1320);
  t_r.x.ctrl.wicc <= S(1321);
  t_r.x.ctrl.wy <= S(1322);
  t_r.x.ctrl.ld <= S(1323);
  t_r.x.ctrl.pv <= S(1324);
  t_r.x.ctrl.rett <= S(1325);
  t_r.x.result <= S(1357 downto 1326);
  t_r.x.y <= S(1389 downto 1358);
  t_r.x.icc <= S(1393 downto 1390);
  t_r.x.annul_all <= S(1394);
  t_r.x.data(0) <= S(1426 downto 1395);
  t_r.x.data(1) <= S(1458 downto 1427);
  t_r.x.set <= S(1459 downto 1459);
  t_r.x.mexc <= S(1460);
  t_r.x.dci.signed <= S(1461);
  t_r.x.dci.enaddr <= S(1462);
  t_r.x.dci.read <= S(1463);
  t_r.x.dci.write <= S(1464);
  t_r.x.dci.lock <= S(1465);
  t_r.x.dci.dsuen <= S(1466);
  t_r.x.dci.size <= S(1468 downto 1467);
  t_r.x.dci.asi <= S(1476 downto 1469);
  t_r.x.laddr <= S(1478 downto 1477);
  --  in_r.x.rstate <= S(1480 downto 1479);
  t_r.x.npc <= S(1483 downto 1481);
  t_r.x.intack <= S(1484);
  t_r.x.ipend <= S(1485);
  t_r.x.mac <= S(1486);
  t_r.x.debug <= S(1487);
  t_r.x.nerror <= S(1488);
  t_r.f.pc <= S(1518 downto 1489);
  t_r.f.branch <= S(1519);
  t_r.w.s.cwp <= S(1522 downto 1520);
  t_r.w.s.icc <= S(1526 downto 1523);
  t_r.w.s.tt <= S(1534 downto 1527);
  t_r.w.s.tba <= S(1554 downto 1535);
  t_r.w.s.wim <= S(1562 downto 1555);
  t_r.w.s.pil <= S(1566 downto 1563);
  t_r.w.s.ec <= S(1567);
  t_r.w.s.ef <= S(1568);
  t_r.w.s.ps <= S(1569);
  t_r.w.s.s <= S(1570);
  t_r.w.s.et <= S(1571);
  t_r.w.s.y <= S(1603 downto 1572);
  t_r.w.s.asr18 <= S(1635 downto 1604);
  t_r.w.s.svt <= S(1636);
  t_r.w.s.dwt <= S(1637);
  t_r.w.s.dbp <= S(1638);
  t_r.w.result <= S(1670 downto 1639);


  
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
 
 