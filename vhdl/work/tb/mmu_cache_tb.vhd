library ieee;
use ieee.std_logic_1164.all;
library std;
use std.textio.all;
library ieee;
use ieee.std_logic_1164.all;
library grlib;
use grlib.amba.all;
use grlib.stdlib.all;
library techmap;
use techmap.gencomp.all;
library gaisler;
use gaisler.libiu.all;
use gaisler.libcache.all;
use gaisler.leon3.all;
use gaisler.mmuconfig.all;
use gaisler.mmuiface.all;
use gaisler.libmmu.all;
library work;
use work.config.all;
use work.util_tb.all;

entity mmu_cache_tb is
  constant CLK_HPERIOD : time := 10 ps;
  constant STRING_SIZE : integer := 3727; -- string size = index of the last element
end mmu_cache_tb;
architecture behavior of mmu_cache_tb is

  -- input/output signals:
  signal inNRst       : std_logic:= '0';
  signal inClk        : std_logic:= '0';
  signal in_ici   : icache_in_type;
  signal ch_ico   : icache_out_type;
  signal ico   : icache_out_type;
  signal in_dci   : dcache_in_type;
  signal ch_dco   : dcache_out_type;
  signal dco   : dcache_out_type;
  signal in_ahbi  : ahb_mst_in_type;
  signal ch_ahbo  : ahb_mst_out_type;
  signal ahbo  : ahb_mst_out_type;
  signal in_ahbsi : ahb_slv_in_type;
  signal in_ahbso  : ahb_slv_out_vector;            
  signal ch_crami : cram_in_type;
  signal crami : cram_in_type;
  signal in_cramo : cram_out_type;
  signal in_fpuholdn : std_ulogic;
  signal in_hclken : std_ulogic;
  
  signal U: std_ulogic_vector(STRING_SIZE-1 downto 0);
  signal S: std_logic_vector(STRING_SIZE-1 downto 0);
  shared variable iClkCnt : integer := 0;

begin

  -- Process of clock generation
  procClkgen : process
  begin
      inClk <= '0' after CLK_HPERIOD, '1' after 2*CLK_HPERIOD;
      wait for 2*CLK_HPERIOD;
  end process procClkgen;

  -- Process of reading  
  procReadingFile : process
    file InputFile:TEXT is "e:/mmu_cache_tb.txt";
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
      if(iClkCnt=41) then
        print("break");
      end if;
    end loop;
  end process procReadingFile;

  in_ahbso(15 downto 3) <= (others => ahbs_none);
  
  -- Input signals:
  inNRst <= S(0);
  in_fpuholdn <= S(1);
  in_ici.rpc <= S(33 downto 2);
  in_ici.fpc <= S(65 downto 34);
  in_ici.dpc <= S(97 downto 66);
  in_ici.rbranch <= S(98);
  in_ici.fbranch <= S(99);
  in_ici.inull <= S(100);
  in_ici.su <= S(101);
  in_ici.flush <= S(102);
  in_ici.flushl <= S(103);
  in_ici.fline <= S(132 downto 104);
  in_ici.pnull <= S(133);
  in_dci.asi <= S(141 downto 134);
  in_dci.maddress <= S(173 downto 142);
  in_dci.eaddress <= S(205 downto 174);
  in_dci.edata <= S(237 downto 206);
  in_dci.size <= S(239 downto 238);
  in_dci.enaddr <= S(240);
  in_dci.eenaddr <= S(241);
  in_dci.nullify <= S(242);
  in_dci.lock <= S(243);
  in_dci.read <= S(244);
  in_dci.write <= S(245);
  in_dci.flush <= S(246);
  in_dci.flushl <= S(247);
  in_dci.dsuen <= S(248);
  in_dci.msu <= S(249);
  in_dci.esu <= S(250);
  in_dci.intack <= S(251);
  in_ahbi.hgrant <= S(267 downto 252);
  in_ahbi.hready <= S(268);
  in_ahbi.hresp <= S(270 downto 269);
  in_ahbi.hrdata <= S(302 downto 271);
  in_ahbi.hcache <= S(303);
  in_ahbi.hirq <= S(335 downto 304);
  in_ahbi.testen <= S(336);
  in_ahbi.testrst <= S(337);
  in_ahbi.scanen <= S(338);
  in_ahbi.testoen <= S(339);
  in_ahbso(0).hready <= S(340);
  in_ahbso(0).hresp <= S(342 downto 341);
  in_ahbso(0).hrdata <= S(374 downto 343);
  in_ahbso(0).hsplit <= S(390 downto 375);
  in_ahbso(0).hcache <= S(391);
  in_ahbso(0).hirq <= S(423 downto 392);
  in_ahbso(0).hconfig(0) <= S(455 downto 424);
  in_ahbso(0).hconfig(1) <= S(487 downto 456);
  in_ahbso(0).hconfig(2) <= S(519 downto 488);
  in_ahbso(0).hconfig(3) <= S(551 downto 520);
  in_ahbso(0).hconfig(4) <= S(583 downto 552);
  in_ahbso(0).hconfig(5) <= S(615 downto 584);
  in_ahbso(0).hconfig(6) <= S(647 downto 616);
  in_ahbso(0).hconfig(7) <= S(679 downto 648);
  in_ahbso(0).hindex <= conv_integer(S(683 downto 680));
  in_ahbso(1).hready <= S(684);
  in_ahbso(1).hresp <= S(686 downto 685);
  in_ahbso(1).hrdata <= S(718 downto 687);
  in_ahbso(1).hsplit <= S(734 downto 719);
  in_ahbso(1).hcache <= S(735);
  in_ahbso(1).hirq <= S(767 downto 736);
  in_ahbso(1).hconfig(0) <= S(799 downto 768);
  in_ahbso(1).hconfig(1) <= S(831 downto 800);
  in_ahbso(1).hconfig(2) <= S(863 downto 832);
  in_ahbso(1).hconfig(3) <= S(895 downto 864);
  in_ahbso(1).hconfig(4) <= S(927 downto 896);
  in_ahbso(1).hconfig(5) <= S(959 downto 928);
  in_ahbso(1).hconfig(6) <= S(991 downto 960);
  in_ahbso(1).hconfig(7) <= S(1023 downto 992);
  in_ahbso(1).hindex <= conv_integer(S(1027 downto 1024));
  in_ahbso(2).hready <= S(1028);
  in_ahbso(2).hresp <= S(1030 downto 1029);
  in_ahbso(2).hrdata <= S(1062 downto 1031);
  in_ahbso(2).hsplit <= S(1078 downto 1063);
  in_ahbso(2).hcache <= S(1079);
  in_ahbso(2).hirq <= S(1111 downto 1080);
  in_ahbso(2).hconfig(0) <= S(1143 downto 1112);
  in_ahbso(2).hconfig(1) <= S(1175 downto 1144);
  in_ahbso(2).hconfig(2) <= S(1207 downto 1176);
  in_ahbso(2).hconfig(3) <= S(1239 downto 1208);
  in_ahbso(2).hconfig(4) <= S(1271 downto 1240);
  in_ahbso(2).hconfig(5) <= S(1303 downto 1272);
  in_ahbso(2).hconfig(6) <= S(1335 downto 1304);
  in_ahbso(2).hconfig(7) <= S(1367 downto 1336);
  in_ahbso(2).hindex <= conv_integer(S(1371 downto 1368));
  in_cramo.icramo.tag(0) <= S(1403 downto 1372);
  in_cramo.icramo.tag(1) <= S(1435 downto 1404);
  in_cramo.icramo.tag(2) <= S(1467 downto 1436);
  in_cramo.icramo.tag(3) <= S(1499 downto 1468);
  in_cramo.icramo.data(0) <= S(1531 downto 1500);
  in_cramo.icramo.data(1) <= S(1563 downto 1532);
  in_cramo.icramo.data(2) <= S(1595 downto 1564);
  in_cramo.icramo.data(3) <= S(1627 downto 1596);
  in_cramo.icramo.ctx(0) <= S(1635 downto 1628);
  in_cramo.icramo.ctx(1) <= S(1643 downto 1636);
  in_cramo.icramo.ctx(2) <= S(1651 downto 1644);
  in_cramo.icramo.ctx(3) <= S(1659 downto 1652);
  in_cramo.icramo.tpar(0) <= S(1663 downto 1660);
  in_cramo.icramo.tpar(1) <= S(1667 downto 1664);
  in_cramo.icramo.tpar(2) <= S(1671 downto 1668);
  in_cramo.icramo.tpar(3) <= S(1675 downto 1672);
  in_cramo.icramo.dpar(0) <= S(1679 downto 1676);
  in_cramo.icramo.dpar(0) <= S(1683 downto 1680);
  in_cramo.icramo.dpar(0) <= S(1687 downto 1684);
  in_cramo.icramo.dpar(0) <= S(1691 downto 1688);
  in_cramo.dcramo.tag(0) <= S(1723 downto 1692);
  in_cramo.dcramo.tag(1) <= S(1755 downto 1724);
  in_cramo.dcramo.tag(2) <= S(1787 downto 1756);
  in_cramo.dcramo.tag(3) <= S(1819 downto 1788);
  in_cramo.dcramo.data(0) <= S(1851 downto 1820);
  in_cramo.dcramo.data(1) <= S(1883 downto 1852);
  in_cramo.dcramo.data(2) <= S(1915 downto 1884);
  in_cramo.dcramo.data(3) <= S(1947 downto 1916);
  in_cramo.dcramo.stag(0) <= S(1979 downto 1948);
  in_cramo.dcramo.stag(1) <= S(2011 downto 1980);
  in_cramo.dcramo.stag(2) <= S(2043 downto 2012);
  in_cramo.dcramo.stag(3) <= S(2075 downto 2044);
  in_cramo.dcramo.ctx(0) <= S(2083 downto 2076);
  in_cramo.dcramo.ctx(1) <= S(2091 downto 2084);
  in_cramo.dcramo.ctx(2) <= S(2099 downto 2092);
  in_cramo.dcramo.ctx(3) <= S(2107 downto 2100);
  in_cramo.dcramo.tpar(0) <= S(2111 downto 2108);
  in_cramo.dcramo.tpar(1) <= S(2115 downto 2112);
  in_cramo.dcramo.tpar(2) <= S(2119 downto 2116);
  in_cramo.dcramo.tpar(3) <= S(2123 downto 2120);
  in_cramo.dcramo.dpar(0) <= S(2127 downto 2124);
  in_cramo.dcramo.dpar(0) <= S(2131 downto 2128);
  in_cramo.dcramo.dpar(0) <= S(2135 downto 2132);
  in_cramo.dcramo.dpar(0) <= S(2139 downto 2136);
  in_cramo.dcramo.spar <= S(2143 downto 2140);
  ch_ico.data(0) <= S(2175 downto 2144);
  ch_ico.data(1) <= S(2207 downto 2176);
  ch_ico.data(2) <= S(2239 downto 2208);
  ch_ico.data(3) <= S(2271 downto 2240);
  ch_ico.set <= S(2273 downto 2272);
  ch_ico.mexc <= S(2274);
  ch_ico.hold <= S(2275);
  ch_ico.flush <= S(2276);
  ch_ico.diagrdy <= S(2277);
  ch_ico.diagdata <= S(2309 downto 2278);
  ch_ico.mds <= S(2310);
  ch_ico.cfg <= S(2342 downto 2311);
  ch_ico.idle <= S(2343);
  --ch_ico.cstat.cmiss <= S(2344);
  --ch_ico.cstat.tmiss <= S(2345);
  --ch_ico.cstat.chold <= S(2346);
  --ch_ico.cstat.mhold <= S(2347);
  ch_dco.data(0) <= S(2379 downto 2348);
  ch_dco.data(1) <= S(2411 downto 2380);
  ch_dco.data(2) <= S(2443 downto 2412);
  ch_dco.data(3) <= S(2475 downto 2444);
  ch_dco.set <= S(2477 downto 2476);
  ch_dco.mexc <= S(2478);
  ch_dco.hold <= S(2479);
  ch_dco.mds <= S(2480);
  ch_dco.werr <= S(2481);
  ch_dco.icdiag.addr <= S(2513 downto 2482);
  ch_dco.icdiag.enable <= S(2514);
  ch_dco.icdiag.read <= S(2515);
  ch_dco.icdiag.tag <= S(2516);
  ch_dco.icdiag.ctx <= S(2517);
  ch_dco.icdiag.flush <= S(2518);
  ch_dco.icdiag.ilramen <= S(2519);
  ch_dco.icdiag.cctrl.burst <= S(2520);
  ch_dco.icdiag.cctrl.dfrz <= S(2521);
  ch_dco.icdiag.cctrl.ifrz <= S(2522);
  ch_dco.icdiag.cctrl.dsnoop <= S(2523);
  ch_dco.icdiag.cctrl.dcs <= S(2525 downto 2524);
  ch_dco.icdiag.cctrl.ics <= S(2527 downto 2526);
  ch_dco.icdiag.pflush <= S(2528);
  ch_dco.icdiag.pflushaddr <= S(2548 downto 2529);
  ch_dco.icdiag.pflushtyp <= S(2549);
  --ch_dco.icdiag.ilock <= S(2553 downto 2550);
  ch_dco.icdiag.scanen <= S(2554);
  --ch_dco.cache <= S(2555);
  ch_dco.idle <= S(2556);
  ch_dco.scanen <= S(2557);
  ch_dco.testen <= S(2558);
  ch_ahbo.hbusreq <= S(2559);
  ch_ahbo.hlock <= S(2560);
  ch_ahbo.htrans <= S(2562 downto 2561);
  ch_ahbo.haddr <= S(2594 downto 2563);
  ch_ahbo.hwrite <= S(2595);
  ch_ahbo.hsize <= S(2598 downto 2596);
  ch_ahbo.hburst <= S(2601 downto 2599);
  ch_ahbo.hprot <= S(2605 downto 2602);
  ch_ahbo.hwdata <= S(2637 downto 2606);
  ch_ahbo.hirq <= S(2669 downto 2638);
  ch_ahbo.hconfig(0) <= S(2701 downto 2670);
  ch_ahbo.hconfig(1) <= S(2733 downto 2702);
  ch_ahbo.hconfig(2) <= S(2765 downto 2734);
  ch_ahbo.hconfig(3) <= S(2797 downto 2766);
  ch_ahbo.hconfig(4) <= S(2829 downto 2798);
  ch_ahbo.hconfig(5) <= S(2861 downto 2830);
  ch_ahbo.hconfig(6) <= S(2893 downto 2862);
  ch_ahbo.hconfig(7) <= S(2925 downto 2894);
  ch_ahbo.hindex <= conv_integer(S(2929 downto 2926));
  ch_crami.icramin.address <= S(2949 downto 2930);
  ch_crami.icramin.tag(0) <= S(2981 downto 2950);
  ch_crami.icramin.tag(1) <= S(3013 downto 2982);
  --ch_crami.icramin.tag(2) <= S(3045 downto 3014);
  --ch_crami.icramin.tag(3) <= S(3077 downto 3046);
  ch_crami.icramin.twrite <= S(3081 downto 3078);
  ch_crami.icramin.tenable <= S(3082);
  ch_crami.icramin.flush <= S(3083);
  ch_crami.icramin.data <= S(3115 downto 3084);
  ch_crami.icramin.denable <= S(3116);
  ch_crami.icramin.dwrite <= S(3120 downto 3117);
  ch_crami.icramin.ldramin.enable <= S(3121);
  ch_crami.icramin.ldramin.read <= S(3122);
  ch_crami.icramin.ldramin.write <= S(3123);
  ch_crami.icramin.ctx <= S(3131 downto 3124);
  --ch_crami.icramin.tpar(0) <= S(3135 downto 3132);
  --ch_crami.icramin.tpar(1) <= S(3139 downto 3136);
  --ch_crami.icramin.tpar(2) <= S(3143 downto 3140);
  --ch_crami.icramin.tpar(3) <= S(3147 downto 3144);
  --ch_crami.icramin.dpar <= S(3151 downto 3148);
  ch_crami.dcramin.address <= S(3171 downto 3152);
  ch_crami.dcramin.tag(0) <= S(3203 downto 3172);
  ch_crami.dcramin.tag(1) <= S(3235 downto 3204);
  ch_crami.dcramin.tag(2) <= S(3267 downto 3236);
  ch_crami.dcramin.tag(3) <= S(3299 downto 3268);
  ch_crami.dcramin.ptag(0) <= S(3331 downto 3300);
  ch_crami.dcramin.ptag(1) <= S(3363 downto 3332);
  ch_crami.dcramin.ptag(2) <= S(3395 downto 3364);
  ch_crami.dcramin.ptag(3) <= S(3427 downto 3396);
  ch_crami.dcramin.twrite <= S(3431 downto 3428);
  ch_crami.dcramin.tpwrite <= S(3435 downto 3432);
  ch_crami.dcramin.tenable <= S(3439 downto 3436);
  ch_crami.dcramin.flush <= S(3440);
  ch_crami.dcramin.data(0) <= S(3472 downto 3441);
  ch_crami.dcramin.data(1) <= S(3504 downto 3473);
  ch_crami.dcramin.data(2) <= S(3536 downto 3505);
  ch_crami.dcramin.data(3) <= S(3568 downto 3537);
  ch_crami.dcramin.denable <= S(3572 downto 3569);
  ch_crami.dcramin.dwrite <= S(3576 downto 3573);
  ch_crami.dcramin.senable <= S(3580 downto 3577);
  ch_crami.dcramin.swrite <= S(3584 downto 3581);
  ch_crami.dcramin.saddress <= S(3604 downto 3585);
  --ch_dcrami.faddress <= S(3624 downto 3605);
  --ch_dcrami.spar <= S(3625);
  ch_crami.dcramin.ldramin.address <= S(3647 downto 3626);
  ch_crami.dcramin.ldramin.enable <= S(3648);
  ch_crami.dcramin.ldramin.read <= S(3649);
  ch_crami.dcramin.ldramin.write <= S(3650);
  ch_crami.dcramin.ctx(0) <= S(3658 downto 3651);
  ch_crami.dcramin.ctx(1) <= S(3666 downto 3659);
  ch_crami.dcramin.ctx(2) <= S(3674 downto 3667);
  ch_crami.dcramin.ctx(3) <= S(3682 downto 3675);
  --ch_dcrami.tpar(0) <= S(3686 downto 3683);
  --ch_dcrami.tpar(1) <= S(3690 downto 3687);
  --ch_dcrami.tpar(2) <= S(3694 downto 3691);
  --ch_dcrami.tpar(3) <= S(3698 downto 3695);
  --ch_dcrami.dpar(0) <= S(3702 downto 3699);
  --ch_dcrami.dpar(1) <= S(3706 downto 3703);
  --ch_dcrami.dpar(2) <= S(3710 downto 3707);
  --ch_dcrami.dpar(3) <= S(3714 downto 3711);
  ch_crami.dcramin.tdiag <= S(3718 downto 3715);
  ch_crami.dcramin.ddiag <= S(3722 downto 3719);
  --ch_dcrami.sdiag <= S(3726 downto 3723);



  tt : mmu_cache generic map
  ( 
    0,--hindex    : integer              := 0;
    inferred,--memtech   : integer range 0 to NTECH := 0;
    CFG_DSU,--dsu       : integer range 0 to 1 := 0;
    CFG_ICEN,--icen      : integer range 0 to 1 := 0;
    CFG_IREPL,--irepl     : integer range 0 to 3 := 0;
    CFG_ISETS,--isets     : integer range 1 to 4 := 1;
    CFG_ILINE,--ilinesize : integer range 4 to 8 := 4;
    CFG_ISETSZ,--isetsize  : integer range 1 to 256 := 1;
    CFG_ILOCK,--isetlock  : integer range 0 to 1 := 0;
    CFG_DCEN,--dcen      : integer range 0 to 1 := 0;
    CFG_DREPL,--drepl     : integer range 0 to 3 := 0;
    CFG_DSETS,--dsets     : integer range 1 to 4 := 1;
    CFG_DLINE,--dlinesize : integer range 4 to 8 := 4;
    CFG_DSETSZ,--dsetsize  : integer range 1 to 256 := 1;
    CFG_DLOCK,--dsetlock  : integer range 0 to 1 := 0;
    CFG_DSNOOP,--dsnoop    : integer range 0 to 6 := 0;
    CFG_ILRAMEN,--ilram      : integer range 0 to 1 := 0;
    CFG_ILRAMSZ,--ilramsize  : integer range 1 to 512 := 1;        
    CFG_ILRAMADDR,--ilramstart : integer range 0 to 255 := 16#8e#;
    CFG_DLRAMEN,--dlram      : integer range 0 to 1 := 0;
    CFG_DLRAMSZ,--dlramsize  : integer range 1 to 512 := 1;        
    CFG_DLRAMADDR,--dlramstart : integer range 0 to 255 := 16#8f#;
    CFG_ITLBNUM,--itlbnum   : integer range 2 to 64 := 8;
    CFG_DTLBNUM,--dtlbnum   : integer range 2 to 64 := 8;
    2,--tlb_type  : integer range 0 to 3 := 1;
    CFG_TLB_REP,--tlb_rep   : integer range 0 to 1 := 0;
    CFG_DFIXED,--cached    : integer := 0;
    0,--clk2x     : integer := 0;
    0,--scantest  : integer := 0;
    0,--mmupgsz   : integer range 0 to 5  := 0;
    (CFG_NCPU-1),--smp       : integer               := 0;
    1--mmuen     : integer range 0 to 1  := 0
  )port map
  (
    inNRst,
    inClk,
    in_ici,
    ico,
    in_dci,
    dco,
    in_ahbi,
    ahbo,
    in_ahbsi,
    in_ahbso,
    crami,
    in_cramo,
    in_fpuholdn,
    inClk,
    inClk,
    in_hclken
  );  

procCheck : process (inClk, ch_ico, ch_dco, ch_ahbo, ch_crami)
  variable iErrCnt : integer := 0;
begin
  if(rising_edge(inClk) and (iClkCnt>2)) then
    if(ch_ico/=ico) then print("Err: ico");  iErrCnt:=iErrCnt+1; end if;
    if((dco.data(0)(31)/='U')and(dco.data(0)(0)/='U')) then
      if(ch_dco/=dco) then print("Err: dco");  iErrCnt:=iErrCnt+1; end if;
    end if;
    if(ch_ahbo/=ahbo) then print("Err: ahbo");  iErrCnt:=iErrCnt+1; end if;
    if(ch_crami.icramin/=crami.icramin) then print("Err: crami.icramin");  iErrCnt:=iErrCnt+1; end if;
    if(ch_crami.dcramin/=crami.dcramin) then print("Err: crami.dcramin");  iErrCnt:=iErrCnt+1; end if;
  end if;
end process procCheck;

  
end;
