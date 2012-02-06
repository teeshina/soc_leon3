
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

entity div32_tb is
  constant CLK_HPERIOD : time := 10 ps;
  constant STRING_SIZE : integer := 392; -- string size = index of the last element
end div32_tb;
architecture behavior of div32_tb is

  -- input/output signals:
  signal inNRst   : std_logic:= '0';
  signal inClk    : std_logic:= '0';
  signal holdnx   : std_logic;
  signal divi     : div32_in_type;
  signal divo     : div32_out_type;
  signal ch_divo     : div32_out_type;
  signal ch_vready : std_logic;
  signal ch_vnready : std_logic;
  signal ch_vaddin1 : std_logic_vector(32 downto 0);
  signal ch_vaddin2 : std_logic_vector(32 downto 0);
  signal ch_vaddsub : std_logic;
  signal ch_ymsd : std_logic;
  signal ch_v_x : std_logic_vector(64 downto 0);
  signal ch_v_state : std_logic_vector(2 downto 0);
  signal ch_v_zero : std_logic;
  signal ch_v_zero2 : std_logic;
  signal ch_v_qcorr : std_logic;
  signal ch_v_zcorr : std_logic;
  signal ch_v_qzero : std_logic;
  signal ch_v_qmsb : std_logic;
  signal ch_v_ovf : std_logic;
  signal ch_v_neg : std_logic;
  signal ch_v_cnt : std_logic_vector(4 downto 0);
  signal ch_addin1 : std_logic_vector(32 downto 0);
  signal ch_addin2 : std_logic_vector(32 downto 0);
  signal ch_addout : std_logic_vector(32 downto 0);


  
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
    file InputFile:TEXT is "e:/div32_tb.txt";
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
  divi.y <= S(34 downto 2);
  divi.op1 <= S(67 downto 35);
  divi.op2 <= S(100 downto 68);
  divi.flush <= S(101);
  divi.Signed <= S(102);
  divi.start <= S(103);
  ch_divo.ready <= S(104);
  ch_divo.nready <= S(105);
  ch_divo.icc <= S(109 downto 106);
  ch_divo.result <= S(141 downto 110);
  ch_vready <= S(142);
  ch_vnready <= S(143);
  ch_vaddin1 <= S(176 downto 144);
  ch_vaddin2 <= S(209 downto 177);
  ch_vaddsub <= S(210);
  ch_ymsd <= S(211);
  ch_v_x<= S(276 downto 212);
  ch_v_state <= S(279 downto 277);
  ch_v_zero <= S(280);
  ch_v_zero2 <= S(281);
  ch_v_qcorr <= S(282);
  ch_v_zcorr <= S(283);
  ch_v_qzero <= S(284);
  ch_v_qmsb <= S(285);
  ch_v_ovf <= S(286);
  ch_v_neg <= S(287);
  ch_v_cnt <= S(292 downto 288);
  ch_addin1 <= S(325 downto 293);
  ch_addin2 <= S(358 downto 326);
  ch_addout <= S(391 downto 359);

  
  tt : div32 port map 
  (
    inNRst,
    inClk,
    holdnx,
    divi,
    divo
  );

procCheck : process (inClk)
  variable iErrCnt : integer := 0;
begin
  if(rising_edge(inClk) and (iClkCnt>13)) then
    if(ch_divo.ready /= divo.ready) then print("Err: ready");  iErrCnt:=iErrCnt+1; end if;
    if(ch_divo.nready /= divo.nready) then print("Err: nready");  iErrCnt:=iErrCnt+1; end if;
    if(ch_divo.icc /= divo.icc) then print("Err: icc");  iErrCnt:=iErrCnt+1; end if;
    if(ch_divo.result /= divo.result) then print("Err: result");  iErrCnt:=iErrCnt+1; end if;
  end if;
end process procCheck;

  
end;
