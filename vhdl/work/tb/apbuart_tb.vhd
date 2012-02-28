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
use gaisler.uart.all;
library work;
use work.config.all;
use work.util_tb.all;



entity apbuart_tb is
  constant CLK_HPERIOD : time := 10 ps;
  constant STRING_SIZE : integer := 330; -- string size = index of the last element

  constant REVISION : integer := 1;
end apbuart_tb;

architecture behavior of apbuart_tb is


  -- input/output signals:
  signal inNRst       : std_logic:= '0';
  signal inClk        : std_logic:= '0';
  signal in_apbi   : apb_slv_in_type;
  signal ch_apbo   : apb_slv_out_type;
  signal apbo      : apb_slv_out_type;
  signal in_uarti  : uart_in_type;
  signal ch_uarto  : uart_out_type;
  signal uarto     : uart_out_type;
  signal t_paddr  : std_logic_vector(7 downto 2);
  signal t_rdata  : std_logic_vector(31 downto 0);
  signal t_r_tsempty  : std_logic;
  
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
    file InputFile:TEXT is "e:/apbuart_tb.txt";--open read_mode file_name;
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
  in_uarti.rxd <= S(119);
  in_uarti.ctsn <= S(120);
  in_uarti.extclk <= S(121);
  ch_apbo.prdata <= S(153 downto 122);
  ch_apbo.pirq <= S(185 downto 154);
  ch_apbo.pconfig(0) <= S(217 downto 186);
  ch_apbo.pconfig(1) <= S(249 downto 218);
  ch_apbo.pindex <= conv_integer(S(253 downto 250));
  ch_uarto.rtsn <= S(254);
  ch_uarto.txd <= S(255);
  ch_uarto.scaler <= S(287 downto 256);
  ch_uarto.txen <= S(288);
  ch_uarto.flow <= S(289);
  ch_uarto.rxen <= S(290);
  t_paddr <= S(296 downto 291);
  t_rdata <= S(328 downto 297);
  t_r_tsempty <= S(329);
    
  tt : apbuart generic map 
  (
    pindex   => 0, 
    paddr => 1, 
    pirq => 2, 
    console => 0, 
    fifosize => 8--CFG_UART1_FIFO
  )port map 
  (
    inNRst,
    inClk,
    in_apbi,
    apbo,
    in_uarti,
    uarto
  );


procCheck : process (inClk,ch_apbo,ch_uarto)
begin
  if(rising_edge(inClk)and(iClkCnt>2)) then
    if(ch_apbo/=apbo) then print("Err: apbo: prdata=" & tost(apbo.prdata));  iErrCnt:=iErrCnt+1; end if;
    if(uarto.scaler(0)/='U') then
      if(ch_uarto/=uarto) then print("Err: uarto" );  iErrCnt:=iErrCnt+1; end if;
    end if;
  end if;
end process procCheck;


end;
 
 