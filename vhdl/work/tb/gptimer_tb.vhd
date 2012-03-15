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
library work;
use work.config.all;
use work.util_tb.all;



entity gptimer_tb is
  constant CLK_HPERIOD : time := 10 ps;
  constant STRING_SIZE : integer := 316; -- string size = index of the last element
end gptimer_tb;

architecture behavior of gptimer_tb is


  -- input/output signals:
  signal inNRst    : std_logic:= '0';
  signal inClk     : std_logic:= '0';
  signal in_apbi   : apb_slv_in_type;
  signal ch_apbo   : apb_slv_out_type;
  signal apbo      : apb_slv_out_type;
  signal in_gpti   : gptimer_in_type;
  signal ch_gpto   : gptimer_out_type;
  signal gpto      : gptimer_out_type;
  signal t_scaler  : std_logic_vector(CFG_GPT_SW downto 0);
  signal t_v_tick  : std_logic;
  signal t_v_tsel  : integer range 0 to 3;

  
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
    file InputFile:TEXT is "e:/gptimer_tb.txt";--open read_mode file_name;
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
  in_apbi.psel <= S(16 downto 1);
  in_apbi.penable <= S(17);
  in_apbi.paddr <= S(49 downto 18);
  in_apbi.pwrite <= S(50);
  in_apbi.pwdata <= S(82 downto 51);
  in_apbi.pirq <= S(114 downto 83);
  in_apbi.testen <= S(115);
  in_apbi.testrst <= S(116);
  in_apbi.scanen <= S(117);
  in_apbi.testoen <= S(118);
  in_gpti.dhalt <= S(119);
  in_gpti.extclk <= S(120);
  in_gpti.wdogen <= S(121);
  ch_apbo.prdata <= S(153 downto 122);
  ch_apbo.pirq <= S(185 downto 154);
  ch_apbo.pconfig(0) <= S(217 downto 186);
  ch_apbo.pconfig(1) <= S(249 downto 218);
  ch_apbo.pindex <= conv_integer(S(253 downto 250));
  ch_gpto.tick <= S(261 downto 254);
  ch_gpto.timer1 <= S(293 downto 262);
  ch_gpto.wdogn <= S(294);
  ch_gpto.wdog <= S(295);
  t_scaler <= S(312 downto 296);
  t_v_tick <= S(313);
  t_v_tsel <= conv_integer(S(315 downto 314));

    
  tt : gptimer generic map 
  (
    pindex   => APB_TIMER, 
    paddr => 3, 
    pmask => 16#fff#,
    pirq => IRQ_TIMER,
    sepirq => CFG_GPT_SEPIRQ,
    sbits => CFG_GPT_SW,
    ntimers => CFG_GPT_NTIM,
    nbits  => CFG_GPT_TW
  )port map 
  (
    inNRst,
    inClk,
    in_apbi,
    apbo,
    in_gpti,
    gpto
  );


procCheck : process (inClk,ch_apbo,ch_gpto)
begin
  if(rising_edge(inClk)and(iClkCnt>2)) then
    if((apbo.prdata(30)/='U')and(apbo.prdata(1)/='U')) then
      if(ch_apbo/=apbo) then print("Err: apbo: prdata=" & tost(apbo.prdata));  iErrCnt:=iErrCnt+1; end if;
    end if;
    if((gpto.timer1(0)/='U')and(gpto.timer1(0)/='X')) then
      if(ch_gpto/=gpto) then print("Err: gpto" & tost(gpto.timer1));  iErrCnt:=iErrCnt+1; end if;
    end if;
  end if;
end process procCheck;


end;
 
 