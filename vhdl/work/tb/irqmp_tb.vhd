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
use gaisler.leon3.all;
library work;
use work.config.all;
use work.util_tb.all;



entity irqmp_tb is
  constant CLK_HPERIOD : time := 10 ps;
  constant STRING_SIZE : integer := 306; -- string size = index of the last element
end irqmp_tb;

architecture behavior of irqmp_tb is


  -- input/output signals:
  signal inNRst    : std_logic:= '0';
  signal inClk     : std_logic:= '0';
  signal in_apbi   : apb_slv_in_type;
  signal ch_apbo   : apb_slv_out_type;
  signal apbo      : apb_slv_out_type;
  signal in_irqi   : irq_out_vector(0 to CFG_NCPU-1);
  signal ch_irqo   : irq_in_vector(0 to CFG_NCPU-1);
  signal irqo      : irq_in_vector(0 to CFG_NCPU-1);
  signal t_v_ipend  : std_logic_vector(15 downto 1);

  
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
    file InputFile:TEXT is "e:/irqmp_tb.txt";--open read_mode file_name;
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
  in_irqi(0).intack <= S(119);
  in_irqi(0).irl <= S(123 downto 120);
  in_irqi(0).pwd <= S(124);
  in_irqi(0).fpen <= S(125);
  in_irqi(0).idle <= S(126);
  ch_apbo.prdata <= S(158 downto 127);
  ch_apbo.pirq <= S(190 downto 159);
  ch_apbo.pconfig(0) <= S(222 downto 191);
  ch_apbo.pconfig(1) <= S(254 downto 223);
  ch_apbo.pindex <= conv_integer(S(258 downto 255));
  ch_irqo(0).irl <= S(262 downto 259);
  ch_irqo(0).rst <= S(263);
  ch_irqo(0).run <= S(264);
  ch_irqo(0).rstvec <= S(284 downto 265);
  ch_irqo(0).iact <= S(285);
  ch_irqo(0).index <= S(289 downto 286);
  ch_irqo(0).hrdrst <= S(290);
  t_v_ipend <= S(305 downto 291);
    
  tt : irqmp generic map 
  (
    pindex   => 1, 
    paddr => 2, 
    pmask => 16#fff#, 
    ncpu => CFG_NCPU, 
    eirq => 0
  )port map 
  (
    inNRst,
    inClk,
    in_apbi,
    apbo,
    in_irqi,
    irqo
  );


procCheck : process (inClk,ch_apbo,ch_irqo)
begin
  if(rising_edge(inClk)and(iClkCnt>2)) then
    if((apbo.prdata(15)/='U')and(apbo.prdata(1)/='U')) then
      if(ch_apbo/=apbo) then print("Err: apbo: prdata=" & tost(apbo.prdata));  iErrCnt:=iErrCnt+1; end if;
    end if;
    if(ch_irqo/=irqo) then print("Err: irqo(0): irl=" & tost(irqo(0).irl));  iErrCnt:=iErrCnt+1; end if;
  end if;
end process procCheck;


end;
 
 