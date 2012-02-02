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

entity mmu_tb is
  constant CLK_HPERIOD : time := 10 ps;
  constant STRING_SIZE : integer := 443; -- string size = index of the last element
end mmu_tb;
architecture behavior of mmu_tb is

  -- input/output signals:
  signal inNRst       : std_logic:= '0';
  signal inClk        : std_logic:= '0';
  signal in_mmudci : mmudc_in_type;
  signal ch_mmudco : mmudc_out_type;
  signal mmudco : mmudc_out_type;
  signal in_mmuici : mmuic_in_type;
  signal ch_mmuico : mmuic_out_type;
  signal mmuico : mmuic_out_type;
  signal in_mcmmo  : memory_mm_out_type;
  signal ch_mcmmi  : memory_mm_in_type;
  signal mcmmi  : memory_mm_in_type;
  signal in_mmudci_transdata_isid  : std_logic;
  signal in_mmuici_transdata_isid  : std_logic;
  
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
    file InputFile:TEXT is "e:/mmu_tb.txt";
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


  -- Input signals:
  inNRst <= S(0);
  in_mmudci.trans_op <= S(1);
  in_mmudci.transdata.data <= S(33 downto 2);
  in_mmudci.transdata.su <= S(34);
  in_mmudci.transdata.read <= S(35);
  in_mmudci_transdata_isid <= S(36);
  in_mmudci.transdata.wb_data <= S(68 downto 37);
  in_mmudci.flush_op <= S(69);
  in_mmudci.diag_op <= S(70);
  in_mmudci.wb_op <= S(71);
  in_mmudci.fsread <= S(72);
  in_mmudci.mmctrl1.e <= S(73);
  in_mmudci.mmctrl1.nf <= S(74);
  in_mmudci.mmctrl1.pso <= S(75);
  in_mmudci.mmctrl1.pagesize <= S(77 downto 76);
  in_mmudci.mmctrl1.ctx <= S(85 downto 78);
  in_mmudci.mmctrl1.ctxp <= S(115 downto 86);
  in_mmudci.mmctrl1.tlbdis <= S(116);
  in_mmudci.mmctrl1.bar <= S(118 downto 117);
  in_mmuici.trans_op <= S(119);
  in_mmuici.transdata.data <= S(151 downto 120);
  in_mmuici.transdata.su <= S(152);
  in_mmuici.transdata.read <= S(153);
  in_mmuici_transdata_isid <= S(154);
  in_mmuici.transdata.wb_data <= S(186 downto 155);
  in_mcmmo.data <= S(218 downto 187);
  in_mcmmo.ready <= S(219);
  in_mcmmo.grant <= S(220);
  in_mcmmo.retry <= S(221);
  in_mcmmo.mexc <= S(222);
  in_mcmmo.werr <= S(223);
  in_mcmmo.cache <= S(224);
  ch_mmudco.grant <= S(225);
  ch_mmudco.transdata.finish <= S(226);
  ch_mmudco.transdata.data <= S(258 downto 227);
  ch_mmudco.transdata.cache <= S(259);
  ch_mmudco.transdata.accexc <= S(260);
  ch_mmudco.mmctrl2.fs.ow <= S(261);
  ch_mmudco.mmctrl2.fs.fav <= S(262);
  ch_mmudco.mmctrl2.fs.ft <= S(265 downto 263);
  ch_mmudco.mmctrl2.fs.at_ls <= S(266);
  ch_mmudco.mmctrl2.fs.at_id <= S(267);
  ch_mmudco.mmctrl2.fs.at_su <= S(268);
  ch_mmudco.mmctrl2.fs.l <= S(270 downto 269);
  ch_mmudco.mmctrl2.fs.ebe <= S(278 downto 271);
  ch_mmudco.mmctrl2.valid <= S(279);
  ch_mmudco.mmctrl2.fa <= S(299 downto 280);
  ch_mmudco.wbtransdata.finish <= S(300);
  ch_mmudco.wbtransdata.data <= S(332 downto 301);
  ch_mmudco.wbtransdata.cache <= S(333);
  ch_mmudco.wbtransdata.accexc <= S(334);
  ch_mmudco.tlbmiss <= S(335);
  ch_mmuico.transdata.data <= S(367 downto 336);
  ch_mmuico.grant <= S(368);
  ch_mmuico.transdata.finish <= S(369);
  ch_mmuico.transdata.cache <= S(370);
  ch_mmuico.transdata.accexc <= S(371);
  ch_mmuico.tlbmiss <= S(372);
  ch_mcmmi.address <= S(404 downto 373);
  ch_mcmmi.data <= S(436 downto 405);
  ch_mcmmi.size <= S(438 downto 437);
  ch_mcmmi.burst <= S(439);
  ch_mcmmi.read <= S(440);
  ch_mcmmi.req <= S(441);
  ch_mcmmi.lock <= S(442);


  enum : process (inClk,in_mmuici_transdata_isid,in_mmudci_transdata_isid)
  begin
    if(in_mmuici_transdata_isid='0') then in_mmuici.transdata.isid <= id_icache;
    else                                  in_mmuici.transdata.isid <= id_dcache; end if;

    if(in_mmudci_transdata_isid='0') then in_mmudci.transdata.isid <= id_icache;
    else                                  in_mmudci.transdata.isid <= id_dcache; end if;
  end process enum;


  tt : mmu generic map
  ( 
    inferred,		--tech      : integer range 0 to NTECH := 0;
    CFG_ITLBNUM,--itlbnum   : integer range 2 to 64 := 8;
    CFG_DTLBNUM,--dtlbnum   : integer range 2 to 64 := 8;
    2,					--tlb_type  : integer range 0 to 3 := 1;
    CFG_TLB_REP,--tlb_rep   : integer range 0 to 1 := 0;
    0						--mmupgsz   : integer range 0 to 5  := 0
  )port map
  (
    inNRst,
    inClk,
    in_mmudci,
    mmudco,
    in_mmuici,
    mmuico,
    in_mcmmo,
    mcmmi
  );  

procCheck : process (inClk, ch_mmudco, ch_mmuico, ch_mcmmi)
  variable iErrCnt : integer := 0;
begin
  if(rising_edge(inClk) and (iClkCnt>35)) then
    if(ch_mmudco.grant /= mmudco.grant) then print("Err: mmudco.grant");  iErrCnt:=iErrCnt+1; end if;
    if(ch_mmudco.transdata.finish /= mmudco.transdata.finish) then print("Err: mmudco.transdata.finish");  iErrCnt:=iErrCnt+1; end if;
    if(mmudco.transdata.data/="XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX") then
      if(ch_mmudco.transdata.data /= mmudco.transdata.data) then print("Err: mmudco.transdata.data");  iErrCnt:=iErrCnt+1; end if;
    end if;
    if((mmudco.transdata.cache/='U') and (iClkCnt>58)) then
      if(ch_mmudco.transdata.cache /= mmudco.transdata.cache) then print("Err: mmudco.transdata.cache");  iErrCnt:=iErrCnt+1; end if;
    end if;
    if(ch_mmudco.transdata.accexc /= mmudco.transdata.accexc) then print("Err: mmudco.transdata.accexc");  iErrCnt:=iErrCnt+1; end if;
    if(ch_mmudco.mmctrl2 /= mmudco.mmctrl2) then print("Err: mmudco.mmctrl2");  iErrCnt:=iErrCnt+1; end if;
    if(ch_mmudco.wbtransdata /= mmudco.wbtransdata) then print("Err: mmudco.wbtransdata");  iErrCnt:=iErrCnt+1; end if;
    if(ch_mmudco.tlbmiss /= mmudco.tlbmiss) then print("Err: mmudco.tlbmiss");  iErrCnt:=iErrCnt+1; end if;

    if(ch_mmuico.grant /= mmuico.grant) then print("Err: mmuico.grant");  iErrCnt:=iErrCnt+1; end if;
    if(ch_mmuico.transdata.finish /= mmuico.transdata.finish) then print("Err: mmuico.transdata.finish");  iErrCnt:=iErrCnt+1; end if;
    if(mmuico.transdata.data/="XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX") then
      if(ch_mmuico.transdata.data /= mmuico.transdata.data) then print("Err: mmuico.transdata.data");  iErrCnt:=iErrCnt+1; end if;
    end if;
    if((mmuico.transdata.cache/='U')) then
      if(ch_mmuico.transdata.cache /= mmuico.transdata.cache) then print("Err: mmuico.transdata.cache");  iErrCnt:=iErrCnt+1; end if;
    end if;
    if(ch_mmuico.transdata.accexc /= mmuico.transdata.accexc) then print("Err: mmuico.transdata.accexc");  iErrCnt:=iErrCnt+1; end if;
    if(ch_mmuico.tlbmiss /= mmuico.tlbmiss) then print("Err: mmuico.tlbmiss");  iErrCnt:=iErrCnt+1; end if;

  if(ch_mcmmi /= mcmmi) then print("Err: mcmmi");  iErrCnt:=iErrCnt+1; end if;
  end if;
end process procCheck;

  
end;
