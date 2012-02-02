
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
use gaisler.mmuconfig.all;
use gaisler.mmuiface.all;
library work;
use work.config.all;
use work.util_tb.all;

entity mmu_icache_tb is
  constant CLK_HPERIOD : time := 10 ps;
  constant STRING_SIZE : integer := 1728; -- string size = index of the last element
end mmu_icache_tb;
architecture behavior of mmu_icache_tb is
  type rdatatype is (itag, idata, memory);	-- sources during cache read
  type istatetype is (idle, trans, streaming, stop);
  
  -- input/output signals:
  signal inNRst   : std_logic:= '0';
  signal inClk    : std_logic:= '0';
  signal ici : icache_in_type;
  signal ico : icache_out_type;
  signal dci : dcache_in_type;
  signal dco : dcache_out_type;
  signal mcii : memory_ic_in_type;
  signal mcio : memory_ic_out_type;
  signal mmudci : mmudc_in_type;
  signal mmuici : mmuic_in_type;
  signal mmuico : mmuic_out_type;
  signal icramin : icram_in_type;
  signal icramo : icram_out_type;
  signal fpuholdn : std_logic;
  signal ch_ico : icache_out_type;
  signal ch_mcii : memory_ic_in_type;
  signal ch_icramin : icram_in_type;
  signal ch_mmuici : mmuic_in_type;

  signal t_rdatasel : rdatatype;
--  signal t_rdatasel : std_logic_vector(1 downto 0);
  signal t_twrite  : std_logic;
  signal t_diagen  : std_logic;
  signal t_dwire  : std_logic;
  signal t_taddr : std_logic_vector(TAG_HIGH downto 2);
  signal t_wtag  : std_logic_vector(18 downto 0);
  signal t_ddatain  : std_logic_vector(31 downto 0);
  signal t_diagdata  : std_logic_vector(31 downto 0);
  signal t_vmaskraw  : std_logic_vector(7 downto 0);
  signal t_vmask  : std_logic_vector(7 downto 0);
  signal t_xaddr_inc  : std_logic_vector(2 downto 0);
  signal t_lastline  : std_logic;
  signal t_nlastline  : std_logic;
  signal t_nnlastline  : std_logic;
  signal t_enable  : std_logic;
  signal t_error  : std_logic;
  signal t_whit  : std_logic;
  signal t_hit  : std_logic;
  signal t_valid  : std_logic;
  signal t_cacheon  : std_logic;
  signal t_branch  : std_logic;
  signal t_eholdn  : std_logic;
  signal t_mds  : std_logic;
  signal t_write  : std_logic;
  signal t_tparerr  : std_logic_vector(1 downto 0);
  signal t_dparerr  : std_logic_vector(1 downto 0);
  signal t_set  : std_logic_vector(3 downto 0);
  signal t_setrepl  : std_logic;
  signal t_ctwrite  : std_logic_vector(3 downto 0);
  signal t_cdwrite  : std_logic_vector(3 downto 0);
  signal t_validv  : std_logic_vector(3 downto 0);
  signal t_wlrr  : std_logic;
  signal t_vdiagset  : std_logic_vector(1 downto 0);
  signal t_rdiagset  : std_logic_vector(1 downto 0);
  signal t_lock  : std_logic_vector(1 downto 0);
  signal t_wlock  : std_logic;
  signal t_sidle  : std_logic;
  signal t_lramacc  : std_logic;
  signal t_ilramwr  : std_logic;
  signal t_lramcs  : std_logic;
  signal t_pftag  : std_logic_vector(31 downto 2);
  signal t_mmuici_trans_op  : std_logic;
  signal t_mmuici_su  : std_logic;
  signal t_v_state : istatetype;
--  signal t_v_state : std_logic_vector(1 downto 0);
  signal t_r_req  : std_logic;


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
    file InputFile:TEXT is "e:/mmu_icache_tb.txt";
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
  ici.rpc <= S(33 downto 2);
  ici.fpc <= S(65 downto 34);
  ici.dpc <= S(97 downto 66);
  ici.rbranch <= S(98);
  ici.fbranch <= S(99);
  ici.inull <= S(100);
  ici.su <= S(101);
  ici.flush <= S(102);
  ici.flushl <= S(103);
  ici.fline <= S(132 downto 104);
  ici.pnull <= S(133);
  dci.asi <= S(141 downto 134);
  dci.maddress <= S(173 downto 142);
  dci.eaddress <= S(205 downto 174);
  dci.edata <= S(237 downto 206);
  dci.size <= S(239 downto 238);
  dci.enaddr <= S(240);
  dci.eenaddr <= S(241);
  dci.nullify <= S(242);
  dci.lock <= S(243);
  dci.read <= S(244);
  dci.write <= S(245);
  dci.flush <= S(246);
  dci.flushl <= S(247);
  dci.dsuen <= S(248);
  dci.msu <= S(249);
  dci.esu <= S(250);
  dci.intack <= S(251);
  dco.data(0) <= S(283 downto 252);
  dco.data(1) <= S(315 downto 284);
  dco.data(2) <= S(347 downto 316);
  dco.data(3) <= S(379 downto 348);
  dco.set <= S(381 downto 380);
  dco.mexc <= S(382);
  dco.hold <= S(383);
  dco.mds <= S(384);
  dco.werr <= S(385);
  dco.icdiag.addr <= S(417 downto 386);
  dco.icdiag.enable <= S(418);
  dco.icdiag.read <= S(419);
  dco.icdiag.tag <= S(420);
  dco.icdiag.ctx <= S(421);
  dco.icdiag.flush <= S(422);
  dco.icdiag.ilramen <= S(423);
  dco.icdiag.cctrl.burst <= S(424);
  dco.icdiag.cctrl.dfrz <= S(425);
  dco.icdiag.cctrl.ifrz <= S(426);
  dco.icdiag.cctrl.dsnoop <= S(427);
  dco.icdiag.cctrl.dcs <= S(429 downto 428);
  dco.icdiag.cctrl.ics <= S(431 downto 430);
  dco.icdiag.pflush <= S(432);
  dco.icdiag.pflushaddr <= S(452 downto 433);
  dco.icdiag.pflushtyp <= S(453);
  dco.icdiag.ilock <= S(457 downto 454);
  dco.icdiag.scanen <= S(458);
  dco.cache <= S(459);
  dco.idle <= S(460);
  dco.scanen <= S(461);
  dco.testen <= S(462);
  mcio.data <= S(494 downto 463);
  mcio.ready <= S(495);
  mcio.grant <= S(496);
  mcio.retry <= S(497);
  mcio.mexc <= S(498);
  mcio.cache <= S(499);
  mcio.par <= S(503 downto 500);
  mcio.scanen <= S(504);
  icramo.tag(0) <= S(536 downto 505);
  icramo.tag(1) <= S(568 downto 537);
  icramo.tag(2) <= S(600 downto 569);
  icramo.tag(3) <= S(632 downto 601);
  icramo.data(0) <= S(664 downto 633);
  icramo.data(1) <= S(696 downto 665);
  icramo.data(2) <= S(728 downto 697);
  icramo.data(3) <= S(760 downto 729);
  icramo.ctx(0) <= S(768 downto 761);
  icramo.ctx(1) <= S(776 downto 769);
  icramo.ctx(2) <= S(784 downto 777);
  icramo.ctx(3) <= S(792 downto 785);
  icramo.tpar(0) <= S(796 downto 793);
  icramo.tpar(1) <= S(800 downto 797);
  icramo.tpar(2) <= S(804 downto 801);
  icramo.tpar(3) <= S(808 downto 805);
  icramo.dpar(0) <= S(812 downto 809);
  icramo.dpar(0) <= S(816 downto 813);
  icramo.dpar(0) <= S(820 downto 817);
  icramo.dpar(0) <= S(824 downto 821);
  mmudci.trans_op <= S(825);
  mmudci.transdata.data <= S(857 downto 826);
  mmudci.transdata.su <= S(858);
  mmudci.transdata.read <= S(859);
  --topLeon3mp.mmudci.transdata.isid <= S(860);
  mmudci.transdata.wb_data <= S(892 downto 861);
  mmudci.flush_op <= S(893);
  mmudci.diag_op <= S(894);
  mmudci.wb_op <= S(895);
  mmudci.fsread <= S(896);
  mmudci.mmctrl1.e <= S(897);
  mmudci.mmctrl1.nf <= S(898);
  mmudci.mmctrl1.pso <= S(899);
  mmudci.mmctrl1.pagesize <= S(901 downto 900);
  mmudci.mmctrl1.ctx <= S(909 downto 902);
  mmudci.mmctrl1.ctxp <= S(939 downto 910);
  mmudci.mmctrl1.tlbdis <= S(940);
  mmudci.mmctrl1.bar <= S(942 downto 941);
  mmuico.transdata.data <= S(974 downto 943);
  mmuico.grant <= S(975);
  mmuico.transdata.finish <= S(976);
  mmuico.transdata.cache <= S(977);
  mmuico.transdata.accexc <= S(978);
  mmuico.tlbmiss <= S(979);

  ch_ico.data(0) <= S(1011 downto 980);
  ch_ico.data(1) <= S(1043 downto 1012);
  ch_ico.data(2) <= S(1075 downto 1044);
  ch_ico.data(3) <= S(1107 downto 1076);
  ch_ico.set <= S(1109 downto 1108);
  ch_ico.mexc <= S(1110);
  ch_ico.hold <= S(1111);
  ch_ico.flush <= S(1112);
  ch_ico.diagrdy <= S(1113);
  ch_ico.diagdata <= S(1145 downto 1114);
  ch_ico.mds <= S(1146);
  ch_ico.cfg <= S(1178 downto 1147);
  ch_ico.idle <= S(1179);
  ch_ico.cstat.cmiss <= S(1180);
  ch_ico.cstat.tmiss <= S(1181);
  ch_ico.cstat.chold <= S(1182);
  ch_ico.cstat.mhold <= S(1183);
  ch_mcii.address <= S(1215 downto 1184);
  ch_mcii.burst <= S(1216);
  ch_mcii.req <= S(1217);
  ch_mcii.su <= S(1218);
  ch_mcii.flush <= S(1219);
  ch_icramin.address <= S(1239 downto 1220);
  ch_icramin.tag(0) <= S(1271 downto 1240);
  ch_icramin.tag(1) <= S(1303 downto 1272);
  ch_icramin.tag(2) <= S(1335 downto 1304);
  ch_icramin.tag(3) <= S(1367 downto 1336);
  ch_icramin.twrite <= S(1371 downto 1368);
  ch_icramin.tenable <= S(1372);
  ch_icramin.flush <= S(1373);
  ch_icramin.data <= S(1405 downto 1374);
  ch_icramin.denable <= S(1406);
  ch_icramin.dwrite <= S(1410 downto 1407);
  ch_icramin.ldramin.enable <= S(1411);
  ch_icramin.ldramin.read <= S(1412);
  ch_icramin.ldramin.write <= S(1413);
  ch_icramin.ctx <= S(1421 downto 1414);
  ch_icramin.tpar(0) <= S(1425 downto 1422);
  ch_icramin.tpar(1) <= S(1429 downto 1426);
  ch_icramin.tpar(2) <= S(1433 downto 1430);
  ch_icramin.tpar(3) <= S(1437 downto 1434);
  ch_icramin.dpar <= S(1441 downto 1438);
  ch_mmuici.trans_op <= S(1442);
  ch_mmuici.transdata.data <= S(1474 downto 1443);
  ch_mmuici.transdata.su <= S(1475);
  ch_mmuici.transdata.read <= S(1476);
--  ch_mmuici.transdata.isid <= S(1477);
  ch_mmuici.transdata.wb_data <= S(1509 downto 1478);
--  t_rdatasel <= S(1511 downto 1510);
  t_twrite <= S(1512);
  t_diagen <= S(1513);
  t_dwire <= S(1514);
  t_taddr <= S(1544 downto 1515);
  t_wtag <= S(1563 downto 1545);
  t_ddatain <= S(1595 downto 1564);
  t_diagdata <= S(1627 downto 1596);
  t_vmaskraw <= S(1635 downto 1628);
  t_vmask <= S(1643 downto 1636);
  t_xaddr_inc <= S(1646 downto 1644);
  t_lastline <= S(1647);
  t_nlastline <= S(1648);
  t_nnlastline <= S(1649);
  t_enable <= S(1650);
  t_error <= S(1651);
  t_whit <= S(1652);
  t_hit <= S(1653);
  t_valid <= S(1654);
  t_cacheon <= S(1655);
  t_branch <= S(1656);
  t_eholdn <= S(1657);
  t_mds <= S(1658);
  t_write <= S(1659);
  t_tparerr <= S(1661 downto 1660);
  t_dparerr <= S(1663 downto 1662);
  t_set <= S(1667 downto 1664);
  t_setrepl <= S(1668);
  t_ctwrite <= S(1672 downto 1669);
  t_cdwrite <= S(1676 downto 1673);
  t_validv <= S(1680 downto 1677);
  t_wlrr <= S(1681);
  t_vdiagset <= S(1683 downto 1682);
  t_rdiagset <= S(1685 downto 1684);
  t_lock <= S(1687 downto 1686);
  t_wlock <= S(1688);
  t_sidle <= S(1689);
  t_lramacc <= S(1690);
  t_ilramwr <= S(1691);
  t_lramcs <= S(1692);
  t_pftag <= S(1722 downto 1693);
  t_mmuici_trans_op <= S(1723);
  t_mmuici_su <= S(1724);
--  t_v_state <= S(1726 downto 1725);
  t_r_req <= S(1727);


  proc_enum : process (inClk,S(1511 downto 1510),S(1726 downto 1725))
  begin
    if(S(1511 downto 1510)="01")    then t_rdatasel <= idata;
    elsif(S(1511 downto 1510)="10") then t_rdatasel <= memory;
    else                                 t_rdatasel <= itag;
    end if;

    if(S(1726 downto 1725)="00")    then t_v_state <= idle;
    elsif(S(1726 downto 1725)="01") then t_v_state <= trans;
    elsif(S(1726 downto 1725)="10") then t_v_state <= streaming;
    else                                 t_v_state <= stop;
    end if;
  end process proc_enum;


  
  tt : mmu_icache generic map 
  (
    0,--icen,   --doesn't use at all
    CFG_IREPL,--irepl,
    CFG_ISETS,--isets,
    CFG_ILINE,--ilinesize,
    CFG_ISETSZ,--isetsize,
    CFG_ILOCK,--isetlock,
    CFG_ILRAMEN,--ilram,
    CFG_ILRAMSZ,--ilramsize,
    CFG_ILRAMADDR,--ilramstart,
    CFG_MMUEN--mmuen
  )port map ( inNRst, inClk, ici, ico, dci, dco, mcii, mcio, 
       icramin, icramo, fpuholdn, mmudci, mmuici, mmuico);


procCheck : process (inClk)
begin
  if(rising_edge(inClk) and (iClkCnt>6)) then
    if(ico.data(0)/=ch_ico.data(0))then print("Err: ico.data(0)");  iErrCnt:=iErrCnt+1; end if;
    if(ico.data(1)/=ch_ico.data(1))then print("Err: ico.data(1)");  iErrCnt:=iErrCnt+1; end if;
    if(ico.data(2)/=ch_ico.data(2))then print("Err: ico.data(2)");  iErrCnt:=iErrCnt+1; end if;
    if(ico.data(3)/=ch_ico.data(3))then print("Err: ico.data(3)");  iErrCnt:=iErrCnt+1; end if;
    if(ico.set/=ch_ico.set)then print("Err: ico.set");  iErrCnt:=iErrCnt+1; end if;
    if(ico.mexc/=ch_ico.mexc)then print("Err: ico.mexc");  iErrCnt:=iErrCnt+1; end if;
    if(ico.hold/=ch_ico.hold)then print("Err: ico.hold");  iErrCnt:=iErrCnt+1; end if;
    if(ico.flush/=ch_ico.flush)then print("Err: ico.flush");  iErrCnt:=iErrCnt+1; end if;
    if(ico.diagrdy/=ch_ico.diagrdy)then print("Err: ico.diagrdy");  iErrCnt:=iErrCnt+1; end if;
    if(ico.diagdata/=ch_ico.diagdata)then print("Err: ico.diagdata");  iErrCnt:=iErrCnt+1; end if;
    if(ico.mds/=ch_ico.mds)then print("Err: ico.mds");  iErrCnt:=iErrCnt+1; end if;
    if(ico.cfg/=ch_ico.cfg)then print("Err: ico.cfg");  iErrCnt:=iErrCnt+1; end if;
    if(ico.idle/=ch_ico.idle)then print("Err: ico.idle");  iErrCnt:=iErrCnt+1; end if;
--x    if(ico.cstat.cmiss/=ch_ico.cstat.cmiss)then print("Err: ico.cstat.cmiss");  iErrCnt:=iErrCnt+1; end if;
--x    if(ico.cstat.tmiss/=ch_ico.cstat.tmiss)then print("Err: ico.cstat.tmiss");  iErrCnt:=iErrCnt+1; end if;
--x    if(ico.cstat.chold/=ch_ico.cstat.chold)then print("Err: ico.cstat.chold");  iErrCnt:=iErrCnt+1; end if;
--x    if(ico.cstat.mhold/=ch_ico.cstat.mhold)then print("Err: ico.cstat.mhold");  iErrCnt:=iErrCnt+1; end if;

    if(mcii.address/=ch_mcii.address)then print("Err: mcii.address");  iErrCnt:=iErrCnt+1; end if;
    if(mcii.burst/=ch_mcii.burst)then print("Err: mcii.burst");  iErrCnt:=iErrCnt+1; end if;
    if(mcii.req/=ch_mcii.req)then print("Err: mcii.req");  iErrCnt:=iErrCnt+1; end if;
    if(mcii.su/=ch_mcii.su)then print("Err: mcii.su");  iErrCnt:=iErrCnt+1; end if;
    if(mcii.flush/=ch_mcii.flush)then print("Err: mcii.flush");  iErrCnt:=iErrCnt+1; end if;

    if(icramin.address/=ch_icramin.address)then print("Err: icramin.address");  iErrCnt:=iErrCnt+1; end if;
    if(icramin.tag(0)/=ch_icramin.tag(0))then print("Err: icramin.tag(0)");  iErrCnt:=iErrCnt+1; end if;
    if(icramin.tag(1)/=ch_icramin.tag(1))then print("Err: icramin.tag(1)");  iErrCnt:=iErrCnt+1; end if;
--x    if(icramin.tag(2)/=ch_icramin.tag(2))then print("Err: icramin.tag(2)");  iErrCnt:=iErrCnt+1; end if;
--x    if(icramin.tag(3)/=ch_icramin.tag(3))then print("Err: icramin.tag(3)");  iErrCnt:=iErrCnt+1; end if;
    if(icramin.twrite/=ch_icramin.twrite)then print("Err: icramin.twrite");  iErrCnt:=iErrCnt+1; end if;
    if(icramin.tenable/=ch_icramin.tenable)then print("Err: icramin.tenable");  iErrCnt:=iErrCnt+1; end if;
    if(icramin.flush/=ch_icramin.flush)then print("Err: icramin.flush");  iErrCnt:=iErrCnt+1; end if;
    if(icramin.data/=ch_icramin.data)then print("Err: icramin.data");  iErrCnt:=iErrCnt+1; end if;
    if(icramin.denable/=ch_icramin.denable)then print("Err: icramin.denable");  iErrCnt:=iErrCnt+1; end if;
    if(icramin.dwrite/=ch_icramin.dwrite)then print("Err: icramin.dwrite");  iErrCnt:=iErrCnt+1; end if;
    if(icramin.ldramin.enable/=ch_icramin.ldramin.enable)then print("Err: icramin.ldramin.enable");  iErrCnt:=iErrCnt+1; end if;
    if(icramin.ldramin.read/=ch_icramin.ldramin.read)then print("Err: icramin.ldramin.read");  iErrCnt:=iErrCnt+1; end if;
    if(icramin.ldramin.write/=ch_icramin.ldramin.write)then print("Err: icramin.ldramin.write");  iErrCnt:=iErrCnt+1; end if;
    if(icramin.ctx/=ch_icramin.ctx)then print("Err: icramin.ctx");  iErrCnt:=iErrCnt+1; end if;
--x    if(icramin.tpar(0)/=ch_icramin.tpar(0))then print("Err: icramin.tpar(0)");  iErrCnt:=iErrCnt+1; end if;
--x    if(icramin.tpar(1)/=ch_icramin.tpar(1))then print("Err: icramin.tpar(1)");  iErrCnt:=iErrCnt+1; end if;
--x    if(icramin.tpar(2)/=ch_icramin.tpar(2))then print("Err: icramin.tpar(2)");  iErrCnt:=iErrCnt+1; end if;
--x    if(icramin.tpar(3)/=ch_icramin.tpar(3))then print("Err: icramin.tpar(3)");  iErrCnt:=iErrCnt+1; end if;
--x    if(icramin.dpar/=ch_icramin.dpar)then print("Err: icramin.dpar");  iErrCnt:=iErrCnt+1; end if;

    if(mmuici.trans_op/=ch_mmuici.trans_op)then print("Err: mmuici.trans_op");  iErrCnt:=iErrCnt+1; end if;
    if(mmuici.transdata.data/=ch_mmuici.transdata.data)then print("Err: mmuici.transdata.data");  iErrCnt:=iErrCnt+1; end if;
    if(mmuici.transdata.su/=ch_mmuici.transdata.su)then print("Err: mmuici.transdata.su");  iErrCnt:=iErrCnt+1; end if;
    if(mmuici.transdata.read/=ch_mmuici.transdata.read)then print("Err: mmuici.transdata.read");  iErrCnt:=iErrCnt+1; end if;
    --if(mmuici.transdata.isid/=ch_mmuici.transdata.isid)then print("Err: mmuici.transdata.isid");  iErrCnt:=iErrCnt+1; end if;
    if(mmuici.transdata.wb_data/=ch_mmuici.transdata.wb_data)then print("Err: mmuici.transdata.wb_data");  iErrCnt:=iErrCnt+1; end if;
  end if;
end process procCheck;

  
end;
