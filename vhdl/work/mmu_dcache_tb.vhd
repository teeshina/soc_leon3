
library ieee;
use ieee.std_logic_1164.all;
library std;
use std.textio.all;
library ieee;
library grlib;
use grlib.stdlib.all;
use grlib.amba.all;
use grlib.sparc.all;
library techmap;
use techmap.gencomp.all;
library gaisler;
use gaisler.libiu.all;
use gaisler.libcache.all;
use gaisler.libmmu.all;
use gaisler.mmuconfig.all;
use gaisler.mmuiface.all;
library work;
use work.config.all;
use work.util_tb.all;

entity mmu_dcache_tb is
  constant CLK_HPERIOD : time := 10 ps;
  constant STRING_SIZE : integer := 2052; -- string size = index of the last element
end mmu_dcache_tb;
architecture behavior of mmu_dcache_tb is
  constant CFG_MMU_PAGESIZE    : integer range 0 to 5  := 0;
  
  -- input/output signals:
  signal inNRst   : std_logic:= '0';
  signal inClk    : std_logic:= '0';
  signal fpuholdn : std_logic;
  signal in_dci : dcache_in_type;
  signal ch_dco : dcache_out_type;
  signal dco : dcache_out_type;
  signal in_ico: icache_out_type;
  signal ch_mcdi : memory_dc_in_type;
  signal mcdi : memory_dc_in_type;
  signal in_mcdo : memory_dc_out_type;
  signal in_ahbsi : ahb_slv_in_type;
  signal ch_dcrami : dcram_in_type;
  signal dcrami : dcram_in_type;
  signal in_dcramo : dcram_out_type;
  signal ch_mmudci : mmudc_in_type;
  signal mmudci : mmudc_in_type;
  signal in_mmudco : mmudc_out_type;

  signal mmudci_transdata_isid  : std_logic;


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
    file InputFile:TEXT is "e:/mmu_dcache_tb.txt";
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


  -- Input signals:
  inNRst <= S(0);
  fpuholdn <= S(1);
  in_dci.asi <= S(9 downto 2);
  in_dci.maddress <= S(41 downto 10);
  in_dci.eaddress <= S(73 downto 42);
  in_dci.edata <= S(105 downto 74);
  in_dci.size <= S(107 downto 106);
  in_dci.enaddr <= S(108);
  in_dci.eenaddr <= S(109);
  in_dci.nullify <= S(110);
  in_dci.lock <= S(111);
  in_dci.read <= S(112);
  in_dci.write <= S(113);
  in_dci.flush <= S(114);
  in_dci.flushl <= S(115);
  in_dci.dsuen <= S(116);
  in_dci.msu <= S(117);
  in_dci.esu <= S(118);
  in_dci.intack <= S(119);
  in_ico.data(0) <= S(151 downto 120);
  in_ico.data(1) <= S(183 downto 152);
  in_ico.data(2) <= S(215 downto 184);
  in_ico.data(3) <= S(247 downto 216);
  in_ico.set <= S(249 downto 248);
  in_ico.mexc <= S(250);
  in_ico.hold <= S(251);
  in_ico.flush <= S(252);
  in_ico.diagrdy <= S(253);
  in_ico.diagdata <= S(285 downto 254);
  in_ico.mds <= S(286);
  in_ico.cfg <= S(318 downto 287);
  in_ico.idle <= S(319);
  in_ico.cstat.cmiss <= S(320);
  in_ico.cstat.tmiss <= S(321);
  in_ico.cstat.chold <= S(322);
  in_ico.cstat.mhold <= S(323);
  in_mcdo.data <= S(355 downto 324);
  in_mcdo.ready <= S(356);
  in_mcdo.grant <= S(357);
  in_mcdo.retry <= S(358);
  in_mcdo.mexc <= S(359);
  in_mcdo.werr <= S(360);
  in_mcdo.cache <= S(361);
  in_mcdo.ba <= S(362);
  in_mcdo.bg <= S(363);
  in_mcdo.par <= S(367 downto 364);
  in_mcdo.scanen <= S(368);
  in_mcdo.testen <= S(369);
  in_ahbsi.hsel <= S(385 downto 370);
  in_ahbsi.haddr <= S(417 downto 386);
  in_ahbsi.hwrite <= S(418);
  in_ahbsi.htrans <= S(420 downto 419);
  in_ahbsi.hsize <= S(423 downto 421);
  in_ahbsi.hburst <= S(426 downto 424);
  in_ahbsi.hwdata <= S(458 downto 427);
  in_ahbsi.hprot <= S(462 downto 459);
  in_ahbsi.hready <= S(463);
  in_ahbsi.hmaster <= S(467 downto 464);
  in_ahbsi.hmastlock <= S(468);
  in_ahbsi.hmbsel <= S(472 downto 469);
  in_ahbsi.hcache <= S(473);
  in_ahbsi.hirq <= S(505 downto 474);
  in_ahbsi.testen <= S(506);
  in_ahbsi.testrst <= S(507);
  in_ahbsi.scanen <= S(508);
  in_ahbsi.testoen <= S(509);
  in_dcramo.tag(0) <= S(541 downto 510);
  in_dcramo.tag(1) <= S(573 downto 542);
  in_dcramo.tag(2) <= S(605 downto 574);
  in_dcramo.tag(3) <= S(637 downto 606);
  in_dcramo.data(0) <= S(669 downto 638);
  in_dcramo.data(1) <= S(701 downto 670);
  in_dcramo.data(2) <= S(733 downto 702);
  in_dcramo.data(3) <= S(765 downto 734);
  in_dcramo.stag(0) <= S(797 downto 766);
  in_dcramo.stag(1) <= S(829 downto 798);
  in_dcramo.stag(2) <= S(861 downto 830);
  in_dcramo.stag(3) <= S(893 downto 862);
  in_dcramo.ctx(0) <= S(901 downto 894);
  in_dcramo.ctx(1) <= S(909 downto 902);
  in_dcramo.ctx(2) <= S(917 downto 910);
  in_dcramo.ctx(3) <= S(925 downto 918);
  in_dcramo.tpar(0) <= S(929 downto 926);
  in_dcramo.tpar(1) <= S(933 downto 930);
  in_dcramo.tpar(2) <= S(937 downto 934);
  in_dcramo.tpar(3) <= S(941 downto 938);
  in_dcramo.dpar(0) <= S(945 downto 942);
  in_dcramo.dpar(0) <= S(949 downto 946);
  in_dcramo.dpar(0) <= S(953 downto 950);
  in_dcramo.dpar(0) <= S(957 downto 954);
  in_dcramo.spar <= S(961 downto 958);
  in_mmudco.grant <= S(962);
  in_mmudco.transdata.finish <= S(963);
  in_mmudco.transdata.data <= S(995 downto 964);
  in_mmudco.transdata.cache <= S(996);
  in_mmudco.transdata.accexc <= S(997);
  in_mmudco.mmctrl2.fs.ow <= S(998);
  in_mmudco.mmctrl2.fs.fav <= S(999);
  in_mmudco.mmctrl2.fs.ft <= S(1002 downto 1000);
  in_mmudco.mmctrl2.fs.at_ls <= S(1003);
  in_mmudco.mmctrl2.fs.at_id <= S(1004);
  in_mmudco.mmctrl2.fs.at_su <= S(1005);
  in_mmudco.mmctrl2.fs.l <= S(1007 downto 1006);
  in_mmudco.mmctrl2.fs.ebe <= S(1015 downto 1008);
  in_mmudco.mmctrl2.valid <= S(1016);
  in_mmudco.mmctrl2.fa <= S(1036 downto 1017);
  in_mmudco.wbtransdata.finish <= S(1037);
  in_mmudco.wbtransdata.data <= S(1069 downto 1038);
  in_mmudco.wbtransdata.cache <= S(1070);
  in_mmudco.wbtransdata.accexc <= S(1071);
  in_mmudco.tlbmiss <= S(1072);
  ch_dco.data(0) <= S(1104 downto 1073);
  ch_dco.data(1) <= S(1136 downto 1105);
  ch_dco.data(2) <= S(1168 downto 1137);
  ch_dco.data(3) <= S(1200 downto 1169);
  ch_dco.set <= S(1202 downto 1201);
  ch_dco.mexc <= S(1203);
  ch_dco.hold <= S(1204);
  ch_dco.mds <= S(1205);
  ch_dco.werr <= S(1206);
  ch_dco.icdiag.addr <= S(1238 downto 1207);
  ch_dco.icdiag.enable <= S(1239);
  ch_dco.icdiag.read <= S(1240);
  ch_dco.icdiag.tag <= S(1241);
  ch_dco.icdiag.ctx <= S(1242);
  ch_dco.icdiag.flush <= S(1243);
  ch_dco.icdiag.ilramen <= S(1244);
  ch_dco.icdiag.cctrl.burst <= S(1245);
  ch_dco.icdiag.cctrl.dfrz <= S(1246);
  ch_dco.icdiag.cctrl.ifrz <= S(1247);
  ch_dco.icdiag.cctrl.dsnoop <= S(1248);
  ch_dco.icdiag.cctrl.dcs <= S(1250 downto 1249);
  ch_dco.icdiag.cctrl.ics <= S(1252 downto 1251);
  ch_dco.icdiag.pflush <= S(1253);
  ch_dco.icdiag.pflushaddr <= S(1273 downto 1254);
  ch_dco.icdiag.pflushtyp <= S(1274);
  --ch_dco.icdiag.ilock <= S(1278 downto 1275);
  ch_dco.icdiag.scanen <= S(1279);
  --ch_dco.cache <= S(1280);
  ch_dco.idle <= S(1281);
  ch_dco.scanen <= S(1282);
  ch_dco.testen <= S(1283);
  ch_mcdi.address <= S(1315 downto 1284);
  ch_mcdi.data <= S(1347 downto 1316);
  ch_mcdi.asi <= S(1351 downto 1348);
  ch_mcdi.size <= S(1353 downto 1352);
  ch_mcdi.burst <= S(1354);
  ch_mcdi.read <= S(1355);
  ch_mcdi.req <= S(1356);
  ch_mcdi.lock <= S(1357);
  ch_mcdi.cache <= S(1358);
  ch_dcrami.address <= S(1378 downto 1359);
  ch_dcrami.tag(0) <= S(1410 downto 1379);
  ch_dcrami.tag(1) <= S(1442 downto 1411);
  ch_dcrami.tag(2) <= S(1474 downto 1443);
  ch_dcrami.tag(3) <= S(1506 downto 1475);
  ch_dcrami.ptag(0) <= S(1538 downto 1507);
  ch_dcrami.ptag(1) <= S(1570 downto 1539);
  ch_dcrami.ptag(2) <= S(1602 downto 1571);
  ch_dcrami.ptag(3) <= S(1634 downto 1603);
  ch_dcrami.twrite <= S(1638 downto 1635);
  ch_dcrami.tpwrite <= S(1642 downto 1639);
  ch_dcrami.tenable <= S(1646 downto 1643);
  ch_dcrami.flush <= S(1647);
  ch_dcrami.data(0) <= S(1679 downto 1648);
  ch_dcrami.data(1) <= S(1711 downto 1680);
  ch_dcrami.data(2) <= S(1743 downto 1712);
  ch_dcrami.data(3) <= S(1775 downto 1744);
  ch_dcrami.denable <= S(1779 downto 1776);
  ch_dcrami.dwrite <= S(1783 downto 1780);
  ch_dcrami.senable <= S(1787 downto 1784);
  ch_dcrami.swrite <= S(1791 downto 1788);
  ch_dcrami.saddress <= S(1811 downto 1792);
  --ch_dcrami.faddress <= S(1831 downto 1812);
  --ch_dcrami.spar <= S(1832);
  ch_dcrami.ldramin.address <= S(1854 downto 1833);
  ch_dcrami.ldramin.enable <= S(1855);
  ch_dcrami.ldramin.read <= S(1856);
  ch_dcrami.ldramin.write <= S(1857);
  ch_dcrami.ctx(0) <= S(1865 downto 1858);
  ch_dcrami.ctx(1) <= S(1873 downto 1866);
  ch_dcrami.ctx(2) <= S(1881 downto 1874);
  ch_dcrami.ctx(3) <= S(1889 downto 1882);
  --ch_dcrami.tpar(0) <= S(1893 downto 1890);
  --ch_dcrami.tpar(1) <= S(1897 downto 1894);
  --ch_dcrami.tpar(2) <= S(1901 downto 1898);
  --ch_dcrami.tpar(3) <= S(1905 downto 1902);
  --ch_dcrami.dpar(0) <= S(1909 downto 1906);
  --ch_dcrami.dpar(1) <= S(1913 downto 1910);
  --ch_dcrami.dpar(2) <= S(1917 downto 1914);
  --ch_dcrami.dpar(3) <= S(1921 downto 1918);
  ch_dcrami.tdiag <= S(1925 downto 1922);
  ch_dcrami.ddiag <= S(1929 downto 1926);
  --ch_dcrami.sdiag <= S(1933 downto 1930);
  ch_mmudci.trans_op <= S(1934);
  ch_mmudci.transdata.data <= S(1966 downto 1935);
  ch_mmudci.transdata.su <= S(1967);
  ch_mmudci.transdata.read <= S(1968);
  mmudci_transdata_isid <= S(1969);
  ch_mmudci.transdata.wb_data <= S(2001 downto 1970);
  ch_mmudci.flush_op <= S(2002);
  ch_mmudci.diag_op <= S(2003);
  ch_mmudci.wb_op <= S(2004);
  ch_mmudci.fsread <= S(2005);
  ch_mmudci.mmctrl1.e <= S(2006);
  ch_mmudci.mmctrl1.nf <= S(2007);
  ch_mmudci.mmctrl1.pso <= S(2008);
  ch_mmudci.mmctrl1.pagesize <= S(2010 downto 2009);
  ch_mmudci.mmctrl1.ctx <= S(2018 downto 2011);
  ch_mmudci.mmctrl1.ctxp <= S(2048 downto 2019);
  ch_mmudci.mmctrl1.tlbdis <= S(2049);
  ch_mmudci.mmctrl1.bar <= S(2051 downto 2050);


  proc_enum : process (inClk, mmudci_transdata_isid)
  begin
    if(iClkCnt=30)then
      print("Break: " & tost(iClkCnt));
    end if;
  
    if( mmudci_transdata_isid='0') then ch_mmudci.transdata.isid <= id_icache;
    else                                ch_mmudci.transdata.isid <= id_dcache;
    end if;
  end process proc_enum;


  

  tt :mmu_dcache generic map
  (
    CFG_DSU,--dsu       : integer range 0 to 1  := 0;
    0,--dcen --doesn't use at all
    CFG_DREPL,--drepl     : integer range 0 to 3  := 0;
    CFG_DSETS,--dsets     : integer range 1 to 4  := 1;
    CFG_DLINE,--dlinesize : integer range 4 to 8  := 4;
    CFG_DSETSZ,--dsetsize  : integer range 1 to 256 := 1;
    CFG_DLOCK,--dsetlock  : integer range 0 to 1  := 0;
    CFG_DSNOOP,--dsnoop    : integer range 0 to 6 := 0;
    CFG_DLRAMEN,--dlram      : integer range 0 to 1 := 0;
    CFG_DLRAMSZ,--dlramsize  : integer range 1 to 512 := 1;
    CFG_DLRAMADDR,--dlramstart : integer range 0 to 255 := 16#8f#;
    CFG_ILRAMEN,--ilram      : integer range 0 to 1 := 0;
    CFG_ILRAMADDR,--ilramstart : integer range 0 to 255 := 16#8e#;
    CFG_ITLBNUM,--itlbnum   : integer range 2 to 64 := 8;
    CFG_DTLBNUM,--dtlbnum   : integer range 2 to 64 := 8;
    CFG_TLB_TYPE,--tlb_type  : integer range 0 to 3 := 1;
    CFG_MEMTECH,--memtech   : integer range 0 to NTECH := 0;
    CFG_DFIXED,--cached    : integer := 0;
    CFG_MMU_PAGESIZE,--mmupgsz   : integer range 0 to 5 := 0;
    (CFG_NCPU-1),--smp      : integer := 0;
    CFG_MMUEN--mmuen    : integer := 0
  ) port map(inNRst, inClk, in_dci, dco, in_ico, mcdi, in_mcdo, in_ahbsi,
      dcrami, in_dcramo, fpuholdn, mmudci, in_mmudco, inClk);

procCheck : process (inClk,ch_dco)
begin
  if(rising_edge(inClk) and (iClkCnt>13)) then
    if(ch_dco/=dco) then print("Err: dco");  iErrCnt:=iErrCnt+1; end if;
    if(ch_mcdi/=mcdi) then print("Err: mcdi");  iErrCnt:=iErrCnt+1; end if;
    if(ch_dcrami/=dcrami) then print("Err: dcrami");  iErrCnt:=iErrCnt+1; end if;
    if(ch_mmudci/=mmudci) then print("Err: mmudci");  iErrCnt:=iErrCnt+1; end if;
  end if;
end process procCheck;

  
end;
