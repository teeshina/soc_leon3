
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
use gaisler.libmmu.all;
use gaisler.mmuconfig.all;
use gaisler.mmuiface.all;
library work;
use work.config.all;
use work.util_tb.all;

entity mmutw_tb is
  constant CLK_HPERIOD : time := 10 ps;
  constant STRING_SIZE : integer := 326; -- string size = index of the last element
end mmutw_tb;
architecture behavior of mmutw_tb is
  constant CFG_MMU_PAGESIZE : integer := 0;
  
  -- input/output signals:
  signal inNRst   : std_logic:= '0';
  signal inClk    : std_logic:= '0';
  signal in_mmctrl1 : mmctrl_type1;
  signal in_twi     : mmutw_in_type;
  signal ch_two     : mmutw_out_type;
  signal two        : mmutw_out_type;
  signal in_mcmmo   : memory_mm_out_type;
  signal ch_mcmmi   : memory_mm_in_type;
  signal mcmmi      : memory_mm_in_type; 

  signal U: std_ulogic_vector(STRING_SIZE-1 downto 0);
  signal S: std_logic_vector(STRING_SIZE-1 downto 0);
  shared variable iClkCnt : integer := 0;
  shared variable iErrCnt : integer := 0;

  component mmutw 
  generic ( 
    mmupgsz   : integer range 0 to 5  := 0
  );
  port (
    rst     : in  std_logic;
    clk     : in  std_logic;
    mmctrl1 : in  mmctrl_type1;
    twi     : in  mmutw_in_type;
    two     : out mmutw_out_type;
    mcmmo   : in  memory_mm_out_type;
    mcmmi   : out memory_mm_in_type
    );
  end component;

begin

  -- Process of clock generation
  procClkgen : process
  begin
      inClk <= '0' after CLK_HPERIOD, '1' after 2*CLK_HPERIOD;
      wait for 2*CLK_HPERIOD;
  end process procClkgen;

  -- Process of reading  
  procReadingFile : process
    file InputFile:TEXT is "e:/mmutw_tb.txt";
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
      if(iClkCnt=68)then
        print("Break: " & tost(iClkCnt));
      end if;

    end loop;
  end process procReadingFile;


  -- Input signals:
  inNRst <= S(0);
  in_mcmmo.data <= S(32 downto 1);
  in_mcmmo.ready <= S(33);
  in_mcmmo.grant <= S(34);
  in_mcmmo.retry <= S(35);
  in_mcmmo.mexc <= S(36);
  in_mcmmo.werr <= S(37);
  in_mcmmo.cache <= S(38);
  in_mmctrl1.e <= S(39);
  in_mmctrl1.nf <= S(40);
  in_mmctrl1.pso <= S(41);
  in_mmctrl1.pagesize <= S(43 downto 42);
  in_mmctrl1.ctx <= S(51 downto 44);
  in_mmctrl1.ctxp <= S(81 downto 52);
  in_mmctrl1.tlbdis <= S(82);
  in_mmctrl1.bar <= S(84 downto 83);
  in_twi.walk_op_ur <= S(85);
  in_twi.areq_ur <= S(86);
  in_twi.tlbmiss <= S(87);
  in_twi.data <= S(119 downto 88);
  in_twi.adata <= S(151 downto 120);
  in_twi.aaddr <= S(183 downto 152);
  ch_two.finish <= S(184);
  ch_two.data <= S(216 downto 185);
  ch_two.addr <= S(248 downto 217);
  ch_two.lvl <= S(250 downto 249);
  ch_two.fault_mexc <= S(251);
  ch_two.fault_trans <= S(252);
  ch_two.fault_inv <= S(253);
  ch_two.fault_lvl <= S(255 downto 254);
  ch_mcmmi.address <= S(287 downto 256);
  ch_mcmmi.data <= S(319 downto 288);
  ch_mcmmi.size <= S(321 downto 320);
  ch_mcmmi.burst <= S(322);
  ch_mcmmi.read <= S(323);
  ch_mcmmi.req <= S(324);
  ch_mcmmi.lock <= S(325);


  tt : mmutw generic map ( CFG_MMU_PAGESIZE  )
  port map (inNRst, inClk, in_mmctrl1, in_twi, two, in_mcmmo, mcmmi);




procCheck : process (inClk,ch_two,ch_mcmmi)
begin
  if(rising_edge(inClk) and (iClkCnt>10)) then
    --if(ch_two/=two) then print("Err: two");  iErrCnt:=iErrCnt+1; end if;
    if(ch_two.finish/=two.finish) then print("Err: two.finish");  iErrCnt:=iErrCnt+1; end if;
    if(ch_two.data/=two.data) then print("Err: two.data");  iErrCnt:=iErrCnt+1; end if;
    if(two.addr(31)/='U') then 
      if(ch_two.addr/=two.addr) then print("Err: two.addr");  iErrCnt:=iErrCnt+1; end if;
    end if;
    if(ch_two.lvl/=two.lvl) then print("Err: two.lvl");  iErrCnt:=iErrCnt+1; end if;
    if(ch_two.fault_mexc/=two.fault_mexc) then print("Err: two.fault_mexc");  iErrCnt:=iErrCnt+1; end if;
    if(ch_two.fault_trans/=two.fault_trans) then print("Err: two.fault_trans");  iErrCnt:=iErrCnt+1; end if;
    if(ch_two.fault_inv/=two.fault_inv) then print("Err: two.fault_inv");  iErrCnt:=iErrCnt+1; end if;
    if(ch_two.fault_lvl/=two.fault_lvl) then print("Err: two.fault_lvl");  iErrCnt:=iErrCnt+1; end if;
    
    
    --if(ch_mcmmi/=mcmmi) then print("Err: mcmmi");  iErrCnt:=iErrCnt+1; end if;
    if(mcmmi.address(31)/='U') then 
      if(ch_mcmmi.address/=mcmmi.address) then print("Err: mcmmi.address");  iErrCnt:=iErrCnt+1; end if;
    end if;
    if(mcmmi.data(31)/='U') then 
      if(ch_mcmmi.data/=mcmmi.data) then print("Err: mcmmi.data");  iErrCnt:=iErrCnt+1; end if;
    end if;
    if(ch_mcmmi.size/=mcmmi.size) then print("Err: mcmmi.size");  iErrCnt:=iErrCnt+1; end if;
    if(ch_mcmmi.burst/=mcmmi.burst) then print("Err: mcmmi.burst");  iErrCnt:=iErrCnt+1; end if;
    if(ch_mcmmi.read/=mcmmi.read) then print("Err: mcmmi.read");  iErrCnt:=iErrCnt+1; end if;
    if(ch_mcmmi.req/=mcmmi.req) then print("Err: mcmmi.req");  iErrCnt:=iErrCnt+1; end if;
    if(ch_mcmmi.lock/=mcmmi.lock) then print("Err: mcmmi");  iErrCnt:=iErrCnt+1; end if;
  end if;
end process procCheck;

  
end;
