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

library gnsslib;
use gnsslib.gnssengine.all;



entity rfctrl_tb is
  constant CLK_HPERIOD : time := 10 ps;
  constant STRING_SIZE : integer := 300; -- string size = index of the last element
end rfctrl_tb;

architecture behavior of rfctrl_tb is


  -- input/output signals:
  signal inNRst    : std_logic:= '0';
  signal inClk     : std_logic:= '0';
  signal in_apbi   : apb_slv_in_type;
  signal ch_apbo   : apb_slv_out_type;
  signal apbo      : apb_slv_out_type;
  signal in_LD  : std_logic_vector(1 downto 0);
  signal in_ExtAntStat  : std_logic;
  signal in_ExtAntDetect  : std_logic;

  signal ch_SCLK  : std_logic;
  signal ch_SDATA  : std_logic;
  signal ch_CSn  : std_logic_vector(1 downto 0);
  signal ch_ExtAntEna  : std_logic;

  signal SCLK  : std_logic;
  signal SDATA  : std_logic;
  signal CSn  : std_logic_vector(1 downto 0);
  signal ExtAntEna  : std_logic;
  
  
  signal t_BitCnt  : std_logic_vector(5 downto 0);
  signal t_SendWord  : std_logic_vector(31 downto 0);
  signal t_SClkNegedge  : std_logic;
  signal t_loading  : std_logic;


  
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
    file InputFile:TEXT is "e:/rfctrl_tb.txt";--open read_mode file_name;
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
  in_LD <= S(120 downto 119);
  in_ExtAntStat <= S(121);
  in_ExtAntDetect <= S(122);
  ch_apbo.prdata <= S(154 downto 123);
  ch_apbo.pirq <= S(186 downto 155);
  ch_apbo.pconfig(0) <= S(218 downto 187);
  ch_apbo.pconfig(1) <= S(250 downto 219);
  ch_apbo.pindex <= conv_integer(S(254 downto 251));
  ch_SCLK <= S(255);
  ch_SDATA <= S(256);
  ch_CSn <= S(258 downto 257);
  ch_ExtAntEna <= S(259);
  t_BitCnt <= S(265 downto 260);
  t_SendWord <= S(297 downto 266);
  t_SClkNegedge <= S(298);
  t_loading <= S(299);

  tt : rfctrl generic map
  (
    pindex => APB_RF_CONTROL,
    paddr  => 4,
    pmask  => 16#fff#
  ) port map
  (
    inNRst,
    inClk,
    in_apbi,
    apbo,
    in_LD,
    SCLK,
    SDATA,
    CSn,
    -- Antenna control:
    in_ExtAntStat,
    in_ExtAntDetect,
    ExtAntEna
  );


procCheck : process (inClk,ch_apbo)
begin
  if(rising_edge(inClk)and(iClkCnt>2)) then
--    if((apbo.prdata(30)/='U')and(apbo.prdata(1)/='U')) then
--      if(ch_apbo/=apbo) then print("Err: apbo: prdata=" & tost(apbo.prdata));  iErrCnt:=iErrCnt+1; end if;
--    end if;
  end if;
end process procCheck;


end;
 
 