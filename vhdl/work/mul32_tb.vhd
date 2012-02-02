
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
use gaisler.leon3.all;
use gaisler.libiu.all;
use gaisler.arith.all;
library work;
use work.config.all;
use work.util_tb.all;

entity mul32_tb is
  constant CLK_HPERIOD : time := 10 ps;
  constant STRING_SIZE : integer := 530; -- string size = index of the last element
end mul32_tb;
architecture behavior of mul32_tb is

  -- input/output signals:
  signal inNRst   : std_logic:= '0';
  signal inClk    : std_logic:= '0';
  signal holdnx   : std_logic;
  signal muli     : mul32_in_type;
  signal mulo     : mul32_out_type;
  signal ch_mulo     : mul32_out_type;
  signal tmp_prod : std_logic_vector(65 downto 0);
  signal tmp_mop1 : std_logic_vector(32 downto 0);
  signal tmp_mop2 : std_logic_vector(32 downto 0);
  signal tmp_acc  : std_logic_vector(48 downto 0);
  signal tmp_acc1 : std_logic_vector(48 downto 0);
  signal tmp_acc2 : std_logic_vector(48 downto 0);
  signal tmp_v_acc : std_logic_vector(63 downto 0);
  signal tmp_v_state : std_logic_vector(1 downto 0);
  signal tmp_v_start : std_logic;
  signal tmp_v_ready : std_logic;
  signal tmp_v_nready : std_logic;

  
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
    file InputFile:TEXT is "e:/mul32_tb.txt";
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
  holdnx <= S(1);
  muli.op1 <= S(34 downto 2);
  muli.op2 <= S(67 downto 35);
  muli.flush <= S(68);
  muli.Signed <= S(69);
  muli.start <= S(70);
  muli.mac <= S(71);
  muli.acc <= S(111 downto 72);
  ch_mulo.ready <= S(112);
  ch_mulo.nready <= S(113);
  ch_mulo.icc <= S(117 downto 114);
  ch_mulo.result <= S(181 downto 118);
  tmp_prod <= S(247 downto 182);
  tmp_mop1 <= S(280 downto 248);
  tmp_mop2 <= S(313 downto 281);
  tmp_acc <= S(362 downto 314);
  tmp_acc1 <= S(411 downto 363);
  tmp_acc2 <= S(460 downto 412);
  tmp_v_acc <= S(524 downto 461);
  tmp_v_state <= S(526 downto 525);
  tmp_v_start <= S(527);
  tmp_v_ready <= S(528);
  tmp_v_nready <= S(529);

  
  tt : mul32 generic map 
  (
    tech => inferred,
    multype => CFG_V8/16,
    pipe => (CFG_V8 mod 4)/2,
    mac => CFG_MAC,
    arch => (CFG_V8 mod 16)/4
  ) port map 
  (
    inNRst,
    inClk,
    holdnx,
    muli,
    mulo
  );

procCheck : process (inClk)
  variable iErrCnt : integer := 0;
begin
  if(rising_edge(inClk) and (iClkCnt>1)) then
    if(iClkCnt=3) then 
      print("CFG_V8  = " & tost(CFG_V8)); 
      print("multype = " & tost(CFG_V8/16)); 
      print("pipe    = " & tost((CFG_V8 mod 4)/2)); 
      print("arch    = " & tost((CFG_V8 mod 16)/4)); 
    end if;
    if(ch_mulo.ready /= mulo.ready) then print("Err: ready");  iErrCnt:=iErrCnt+1; end if;
    if(ch_mulo.nready /= mulo.nready) then print("Err: nready");  iErrCnt:=iErrCnt+1; end if;
    if(ch_mulo.icc /= mulo.icc) then print("Err: icc");  iErrCnt:=iErrCnt+1; end if;
    if(ch_mulo.result /= mulo.result) then print("Err: result");  iErrCnt:=iErrCnt+1; end if;
  end if;
end process procCheck;

  
end;
