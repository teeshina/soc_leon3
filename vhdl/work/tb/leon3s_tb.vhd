
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;--"unsgined", SHIFT_RIGHT(), etc
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
use gaisler.leon3.all;
use gaisler.libiu.all;
use gaisler.libcache.all;
use gaisler.arith.all;
use grlib.sparc_disas.all;
library work;
use work.config.all;
use work.util_tb.all;

entity leon3s_tb is
  constant CLK_HPERIOD : time := 10 ps;
  constant STRING_SIZE : integer := 2171; -- string size = index of the last element

  
end leon3s_tb;
architecture behavior of leon3s_tb is
  -- input/output signals:
  signal inNRst   : std_logic:= '0';
  signal inClk    : std_logic:= '0';

  signal in_ahbi   : ahb_mst_in_type;
  signal ch_ahbo   : ahb_mst_out_type;
  signal ahbo   : ahb_mst_out_type;
  signal in_ahbsi  : ahb_slv_in_type;
  signal in_ahbso  : ahb_slv_out_vector := (others => ahbs_none);
  signal in_irqi   : l3_irq_in_type;
  signal ch_irqo   : l3_irq_out_type;
  signal irqo   : l3_irq_out_type;
  signal in_dbgi   : l3_debug_in_type;
  signal ch_dbgo   : l3_debug_out_type;
  signal dbgo   : l3_debug_out_type;

                     					
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
    file InputFile:TEXT is "e:/leon3s_tb.txt";
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
      if(iClkCnt>=770) then
--        print("break");
      end if;

    end loop;
  end process procReadingFile;


  -- Input signals:

  inNRst <= S(0);
  in_ahbi.hgrant <= S(16 downto 1);
  in_ahbi.hready <= S(17);
  in_ahbi.hresp <= S(19 downto 18);
  in_ahbi.hrdata <= S(51 downto 20);
  in_ahbi.hcache <= S(52);
  in_ahbi.hirq <= S(84 downto 53);
  in_ahbi.testen <= S(85);
  in_ahbi.testrst <= S(86);
  in_ahbi.scanen <= S(87);
  in_ahbi.testoen <= S(88);
  in_ahbsi.hsel <= S(104 downto 89);
  in_ahbsi.haddr <= S(136 downto 105);
  in_ahbsi.hwrite <= S(137);
  in_ahbsi.htrans <= S(139 downto 138);
  in_ahbsi.hsize <= S(142 downto 140);
  in_ahbsi.hburst <= S(145 downto 143);
  in_ahbsi.hwdata <= S(177 downto 146);
  in_ahbsi.hprot <= S(181 downto 178);
  in_ahbsi.hready <= S(182);
  in_ahbsi.hmaster <= S(186 downto 183);
  in_ahbsi.hmastlock <= S(187);
  in_ahbsi.hmbsel <= S(191 downto 188);
  in_ahbsi.hcache <= S(192);
  in_ahbsi.hirq <= S(224 downto 193);
  in_ahbsi.testen <= S(225);
  in_ahbsi.testrst <= S(226);
  in_ahbsi.scanen <= S(227);
  in_ahbsi.testoen <= S(228);
  in_ahbso(0).hready <= S(229);
  in_ahbso(0).hresp <= S(231 downto 230);
  in_ahbso(0).hrdata <= S(263 downto 232);
  in_ahbso(0).hsplit <= S(279 downto 264);
  in_ahbso(0).hcache <= S(280);
  in_ahbso(0).hirq <= S(312 downto 281);
  in_ahbso(0).hconfig(0) <= S(344 downto 313);
  in_ahbso(0).hconfig(1) <= S(376 downto 345);
  in_ahbso(0).hconfig(2) <= S(408 downto 377);
  in_ahbso(0).hconfig(3) <= S(440 downto 409);
  in_ahbso(0).hconfig(4) <= S(472 downto 441);
  in_ahbso(0).hconfig(5) <= S(504 downto 473);
  in_ahbso(0).hconfig(6) <= S(536 downto 505);
  in_ahbso(0).hconfig(7) <= S(568 downto 537);
  in_ahbso(0).hindex <= conv_integer(S(572 downto 569));
  in_ahbso(1).hready <= S(573);
  in_ahbso(1).hresp <= S(575 downto 574);
  in_ahbso(1).hrdata <= S(607 downto 576);
  in_ahbso(1).hsplit <= S(623 downto 608);
  in_ahbso(1).hcache <= S(624);
  in_ahbso(1).hirq <= S(656 downto 625);
  in_ahbso(1).hconfig(0) <= S(688 downto 657);
  in_ahbso(1).hconfig(1) <= S(720 downto 689);
  in_ahbso(1).hconfig(2) <= S(752 downto 721);
  in_ahbso(1).hconfig(3) <= S(784 downto 753);
  in_ahbso(1).hconfig(4) <= S(816 downto 785);
  in_ahbso(1).hconfig(5) <= S(848 downto 817);
  in_ahbso(1).hconfig(6) <= S(880 downto 849);
  in_ahbso(1).hconfig(7) <= S(912 downto 881);
  in_ahbso(1).hindex <= conv_integer(S(916 downto 913));
  in_ahbso(2).hready <= S(917);
  in_ahbso(2).hresp <= S(919 downto 918);
  in_ahbso(2).hrdata <= S(951 downto 920);
  in_ahbso(2).hsplit <= S(967 downto 952);
  in_ahbso(2).hcache <= S(968);
  in_ahbso(2).hirq <= S(1000 downto 969);
  in_ahbso(2).hconfig(0) <= S(1032 downto 1001);
  in_ahbso(2).hconfig(1) <= S(1064 downto 1033);
  in_ahbso(2).hconfig(2) <= S(1096 downto 1065);
  in_ahbso(2).hconfig(3) <= S(1128 downto 1097);
  in_ahbso(2).hconfig(4) <= S(1160 downto 1129);
  in_ahbso(2).hconfig(5) <= S(1192 downto 1161);
  in_ahbso(2).hconfig(6) <= S(1224 downto 1193);
  in_ahbso(2).hconfig(7) <= S(1256 downto 1225);
  in_ahbso(2).hindex <= conv_integer(S(1260 downto 1257));
  in_ahbso(3).hready <= S(1261);
  in_ahbso(3).hresp <= S(1263 downto 1262);
  in_ahbso(3).hrdata <= S(1295 downto 1264);
  in_ahbso(3).hsplit <= S(1311 downto 1296);
  in_ahbso(3).hcache <= S(1312);
  in_ahbso(3).hirq <= S(1344 downto 1313);
  in_ahbso(3).hconfig(0) <= S(1376 downto 1345);
  in_ahbso(3).hconfig(1) <= S(1408 downto 1377);
  in_ahbso(3).hconfig(2) <= S(1440 downto 1409);
  in_ahbso(3).hconfig(3) <= S(1472 downto 1441);
  in_ahbso(3).hconfig(4) <= S(1504 downto 1473);
  in_ahbso(3).hconfig(5) <= S(1536 downto 1505);
  in_ahbso(3).hconfig(6) <= S(1568 downto 1537);
  in_ahbso(3).hconfig(7) <= S(1600 downto 1569);
  in_ahbso(3).hindex <= conv_integer(S(1604 downto 1601));
  in_irqi.irl <= S(1608 downto 1605);
  in_irqi.rst <= S(1609);
  in_irqi.run <= S(1610);
  in_irqi.rstvec <= S(1630 downto 1611);
  in_irqi.iact <= S(1631);
  in_irqi.index <= S(1635 downto 1632);
  in_dbgi.dsuen <= S(1636);
  in_dbgi.denable <= S(1637);
  in_dbgi.dbreak <= S(1638);
  in_dbgi.step <= S(1639);
  in_dbgi.halt <= S(1640);
  in_dbgi.reset <= S(1641);
  in_dbgi.dwrite <= S(1642);
  in_dbgi.daddr <= S(1664 downto 1643);
  in_dbgi.ddata <= S(1696 downto 1665);
  in_dbgi.btrapa <= S(1697);
  in_dbgi.btrape <= S(1698);
  in_dbgi.berror <= S(1699);
  in_dbgi.bwatch <= S(1700);
  in_dbgi.bsoft <= S(1701);
  in_dbgi.tenable <= S(1702);
  in_dbgi.timer <= S(1733 downto 1703);
  ch_ahbo.hbusreq <= S(1734);
  ch_ahbo.hlock <= S(1735);
  ch_ahbo.htrans <= S(1737 downto 1736);
  ch_ahbo.haddr <= S(1769 downto 1738);
  ch_ahbo.hwrite <= S(1770);
  ch_ahbo.hsize <= S(1773 downto 1771);
  ch_ahbo.hburst <= S(1776 downto 1774);
  ch_ahbo.hprot <= S(1780 downto 1777);
  ch_ahbo.hwdata <= S(1812 downto 1781);
  ch_ahbo.hirq <= S(1844 downto 1813);
  ch_ahbo.hconfig(0) <= S(1876 downto 1845);
  ch_ahbo.hconfig(1) <= S(1908 downto 1877);
  ch_ahbo.hconfig(2) <= S(1940 downto 1909);
  ch_ahbo.hconfig(3) <= S(1972 downto 1941);
  ch_ahbo.hconfig(4) <= S(2004 downto 1973);
  ch_ahbo.hconfig(5) <= S(2036 downto 2005);
  ch_ahbo.hconfig(6) <= S(2068 downto 2037);
  ch_ahbo.hconfig(7) <= S(2100 downto 2069);
  ch_ahbo.hindex <= conv_integer(S(2104 downto 2101));
  ch_irqo.intack <= S(2105);
  ch_irqo.irl <= S(2109 downto 2106);
  ch_irqo.pwd <= S(2110);
  --ch_irqo.fpen <= S(2111);
  ch_dbgo.data <= S(2143 downto 2112);
  ch_dbgo.crdy <= S(2144);
  ch_dbgo.dsu <= S(2145);
  ch_dbgo.dsumode <= S(2146);
  ch_dbgo.error <= S(2147);
  ch_dbgo.halt <= S(2148);
  ch_dbgo.pwd <= S(2149);
  ch_dbgo.idle <= S(2150);
  ch_dbgo.ipend <= S(2151);
  ch_dbgo.icnt <= S(2152);
  --ch_dbgo.fcnt <= S(2153);
  --ch_dbgo.optype <= S(2159 downto 2154);
  --ch_dbgo.bpmiss <= S(2160);
  --ch_dbgo.istat.cmiss <= S(2161);
  --ch_dbgo.istat.tmiss <= S(2162);
  --ch_dbgo.istat.chold <= S(2163);
  --ch_dbgo.istat.mhold <= S(2164);
  --ch_dbgo.dstat.cmiss <= S(2165);
  --ch_dbgo.dstat.tmiss <= S(2166);
  --ch_dbgo.dstat.chold <= S(2167);
  --ch_dbgo.dstat.mhold <= S(2168);
  --ch_dbgo.wbhold <= S(2169);
  --ch_dbgo.su <= S(2170);


  tt : leon3s generic map 
  (
    0,--hindex    : integer               := 0;
    inferred,--fabtech   : integer range 0 to NTECH  := DEFFABTECH;
    inferred,--memtech   : integer range 0 to NTECH  := DEFMEMTECH;
    CFG_NWIN,--nwindows  : integer range 2 to 32 := 8;
    CFG_DSU,--dsu       : integer range 0 to 1  := 0;
    CFG_FPU,--fpu       : integer range 0 to 31 := 0;
    CFG_V8,--v8        : integer range 0 to 63 := 0;
    0,--cp        : integer range 0 to 1  := 0;
    CFG_MAC,--mac       : integer range 0 to 1  := 0;
    CFG_PCLOW,--pclow     : integer range 0 to 2  := 2;
    CFG_NOTAG,--notag     : integer range 0 to 1  := 0;
    CFG_NWP,--nwp       : integer range 0 to 4  := 0;
    CFG_ICEN,--icen      : integer range 0 to 1  := 0;
    CFG_IREPL,--irepl     : integer range 0 to 3  := 2;
    CFG_ISETS,--isets     : integer range 1 to 4  := 1;
    CFG_ILINE,--ilinesize : integer range 4 to 8  := 4;
    CFG_ISETSZ,--isetsize  : integer range 1 to 256 := 1;
    CFG_ILOCK,--isetlock  : integer range 0 to 1  := 0;
    CFG_DCEN,--dcen      : integer range 0 to 1  := 0;
    CFG_DREPL,--drepl     : integer range 0 to 3  := 2;
    CFG_DSETS,--dsets     : integer range 1 to 4  := 1;
    CFG_DLINE,--dlinesize : integer range 4 to 8  := 4;
    CFG_DSETSZ,--dsetsize  : integer range 1 to 256 := 1;
    CFG_DLOCK,--dsetlock  : integer range 0 to 1  := 0;
    CFG_DSNOOP,--dsnoop    : integer range 0 to 6  := 0;
    CFG_ILRAMEN,--ilram      : integer range 0 to 1 := 0;
    CFG_ILRAMSZ,--ilramsize  : integer range 1 to 512 := 1;
    CFG_ILRAMADDR,--ilramstart : integer range 0 to 255 := 16#8e#;
    CFG_DLRAMEN,--dlram      : integer range 0 to 1 := 0;
    CFG_DLRAMSZ,--dlramsize  : integer range 1 to 512 := 1;
    CFG_DLRAMADDR,--dlramstart : integer range 0 to 255 := 16#8f#;
    CFG_MMUEN,--mmuen     : integer range 0 to 1  := 0;
    CFG_ITLBNUM,--itlbnum   : integer range 2 to 64 := 8;
    CFG_DTLBNUM,--dtlbnum   : integer range 2 to 64 := 8;
    CFG_TLB_TYPE,--tlb_type  : integer range 0 to 3  := 1;
    CFG_TLB_REP,--tlb_rep   : integer range 0 to 1  := 0;
    CFG_LDDEL,--lddel     : integer range 1 to 2  := 2;
    CFG_DISAS,--disas     : integer range 0 to 2  := 0;
    CFG_ITBSZ,--tbuf      : integer range 0 to 64 := 0;
    CFG_PWD,--pwd       : integer range 0 to 2  := 2;     -- power-down
    CFG_SVT,--svt       : integer range 0 to 1  := 1;     -- single vector trapping
    CFG_RSTADDR,--rstaddr   : integer               := 0;
    (CFG_NCPU-1),--smp       : integer range 0 to 15 := 0;     -- support SMP systems
    CFG_DFIXED,--cached    : integer               := 0;	-- cacheability table
    0,--scantest  : integer               := 0;
    0,--mmupgsz   : integer range 0 to 5  := 0;
    CFG_BP--bp        : integer               := 1

  )port map 
  (
    inClk,
    inNRst,
    in_ahbi,
    ahbo,
    in_ahbsi,
    in_ahbso,
    in_irqi,
    irqo,
    in_dbgi,
    dbgo
  );
  
  procCheck : process(inNRst,inClk, ch_ahbo, ch_irqo, ch_dbgo)
  begin
    
    if(rising_edge(inClk) and (iClkCnt>5)) then
     --if(ch_ahbo/=ahbo) then print("Err: ahbo");  iErrCnt:=iErrCnt+1; end if;
     if(ch_ahbo.hbusreq/=ahbo.hbusreq) then print("Err: ahbo.hbusreq");  iErrCnt:=iErrCnt+1; end if;
     if(ch_ahbo.hlock/=ahbo.hlock) then print("Err: ahbo.hlock");  iErrCnt:=iErrCnt+1; end if;
     if(ch_ahbo.htrans/=ahbo.htrans) then print("Err: ahbo.htrans");  iErrCnt:=iErrCnt+1; end if;
     if(ch_ahbo.haddr/=ahbo.haddr) then print("Err: ahbo.haddr");  iErrCnt:=iErrCnt+1; end if;
     if(ch_ahbo.hwrite/=ahbo.hwrite) then print("Err: ahbo.hwrite");  iErrCnt:=iErrCnt+1; end if;
     if(ch_ahbo.hsize/=ahbo.hsize) then print("Err: ahbo.hsize");  iErrCnt:=iErrCnt+1; end if;
     if(ch_ahbo.hburst/=ahbo.hburst) then print("Err: ahbo.hburst");  iErrCnt:=iErrCnt+1; end if;
     if(ch_ahbo.hprot/=ahbo.hprot) then print("Err: ahbo.hprot");  iErrCnt:=iErrCnt+1; end if;
     if(ahbo.hwdata(31)/='U') then
       if(ch_ahbo.hwdata/=ahbo.hwdata) then print("Err: ahbo.hwdata");  iErrCnt:=iErrCnt+1; end if;
     end if;
     if(ch_ahbo.hirq/=ahbo.hirq) then print("Err: ahbo.hirq");  iErrCnt:=iErrCnt+1; end if;
     if(ch_ahbo.hconfig/=ahbo.hconfig) then print("Err: ahbo.hconfig");  iErrCnt:=iErrCnt+1; end if;
     if(ch_ahbo.hindex/=ahbo.hindex) then print("Err: ahbo.hindex");  iErrCnt:=iErrCnt+1; end if;
     
     if((dbgo.data(31)/='U')and(dbgo.data(0)/='U')) then
       if(ch_dbgo.data/=dbgo.data) then print("Err: dbgo.data");  iErrCnt:=iErrCnt+1; end if;
     end if;
     if(ch_dbgo.crdy/=dbgo.crdy) then print("Err: dbgo.crdy");  iErrCnt:=iErrCnt+1; end if;
     if(ch_dbgo.dsu/=dbgo.dsu) then print("Err: dbgo.dsu");  iErrCnt:=iErrCnt+1; end if;
     if(ch_dbgo.dsumode/=dbgo.dsumode) then print("Err: dbgo.dsumode");  iErrCnt:=iErrCnt+1; end if;
     if(ch_dbgo.error/=dbgo.error) then print("Err: dbgo.error");  iErrCnt:=iErrCnt+1; end if;
     if(ch_dbgo.halt/=dbgo.halt) then print("Err: dbgo.halt");  iErrCnt:=iErrCnt+1; end if;
     if(ch_dbgo.pwd/=dbgo.pwd) then print("Err: dbgo.pwd");  iErrCnt:=iErrCnt+1; end if;
     if(ch_dbgo.idle/=dbgo.idle) then print("Err: dbgo.idle");  iErrCnt:=iErrCnt+1; end if;
     if(ch_dbgo.ipend/=dbgo.ipend) then print("Err: dbgo.ipend");  iErrCnt:=iErrCnt+1; end if;
     if(ch_dbgo.icnt/=dbgo.icnt) then print("Err: dbgo.icnt");  iErrCnt:=iErrCnt+1; end if;
     
     if(ch_irqo/=irqo) then print("Err: irqo");  iErrCnt:=iErrCnt+1; end if;
    end if;
  end process procCheck;
  

  
end;
