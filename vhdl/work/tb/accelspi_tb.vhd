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
use gnsslib.gnssmodule.all;



entity accelspi_tb is
  constant CLK_HPERIOD : time := 10 ps;
  constant STRING_SIZE : integer := 346; -- string size = index of the last element
end accelspi_tb;

architecture behavior of accelspi_tb is


  -- input/output signals:
  signal inNRst    : std_logic:= '0';
  signal inClk     : std_logic:= '0';
  signal in_apbi   : apb_slv_in_type;
  signal ch_apbo   : apb_slv_out_type;
  signal apbo      : apb_slv_out_type;

  signal in_SDI  : std_logic;
  signal in_Int1  : std_logic;
  signal in_Int2  : std_logic;
  signal ch_SPC  : std_logic;
  signal ch_SDO  : std_logic;
  signal ch_CSn  : std_logic;
  signal SPC  : std_logic;
  signal SDO  : std_logic;
  signal CSn  : std_logic;

  signal r_BitCnt  : std_logic_vector(5 downto 0);
  signal r_FifoCnt  : std_logic_vector(5 downto 0);
  signal t_ScaleCnt  : std_logic_vector(31 downto 0);
  signal t_wRdFifo  : std_logic;
  signal t_wWrFifo  : std_logic;
  signal r_WriteWord  : std_logic_vector(15 downto 0);
  signal r_Writting  : std_logic;
  signal r_Reading  : std_logic;
  signal rd_possible  : std_logic;
  signal rd_all_run  : std_logic;
  signal rd_word_run  : std_logic;
  signal r_RxAdr  : std_logic_vector(7 downto 0);
  signal r_rd_wordcnt  : std_logic_vector(4 downto 0);
  signal r_RxData  : std_logic_vector(7 downto 0);
  signal rd_word_rdy  : std_logic;

  
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
    file InputFile:TEXT is "e:/accelspi_tb.txt";--open read_mode file_name;
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
      if((iClkCnt=571) or (iClkCnt=572)) then
--        print("break");
      end if;
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
  in_SDI <= S(119);
  in_Int1 <= S(120);
  in_Int2 <= S(121);
  ch_apbo.prdata <= S(153 downto 122);
  ch_apbo.pirq <= S(185 downto 154);
  ch_apbo.pconfig(0) <= S(217 downto 186);
  ch_apbo.pconfig(1) <= S(249 downto 218);
  ch_apbo.pindex <= conv_integer(S(253 downto 250));
  ch_SPC <= S(254);
  ch_SDO <= S(255);
  ch_CSn <= S(256);
  r_BitCnt <= S(262 downto 257);
  r_FifoCnt <= S(268 downto 263);
  t_ScaleCnt <= S(300 downto 269);
  t_wRdFifo <= S(301);
  t_wWrFifo <= S(302);
  r_WriteWord <= S(318 downto 303);
  r_Writting <= S(319);
  r_Reading <= S(320);
  rd_possible <= S(321);
  rd_all_run <= S(322);
  rd_word_run <= S(323);
  r_RxAdr <= S(331 downto 324);
  r_rd_wordcnt <= S(336 downto 332);
  r_RxData <= S(344 downto 337);
  rd_word_rdy <= S(345);

  tt : accelspi generic map
  (
    pindex => APB_ACCELEROMETER,
    paddr  => 8, -- 8..9 addr fields
    pmask  => 16#ffe#
  ) port map
  (
    inNRst,
    inClk,
    in_apbi,
    apbo,
    in_Int1,
    in_Int2,
    in_SDI,
    SPC,
    SDO,
    CSn
  );


procCheck : process (inClk,ch_apbo,ch_SPC,ch_SDO,ch_CSn)
begin
  if(rising_edge(inClk)and(iClkCnt>2)) then
    if((apbo.prdata(0)/='U')and(apbo.prdata(0)/='X')) then
      if(ch_apbo/=apbo) then print("Err: apbo: prdata=" & tost(apbo.prdata));  iErrCnt:=iErrCnt+1; end if;
    end if;
    if(ch_SPC/=SPC)then print("Err: SPC"); iErrCnt:=iErrCnt+1; end if;
    if(ch_SDO/=SDO)then print("Err: SDO"); iErrCnt:=iErrCnt+1; end if;
    if(ch_CSn/=CSn)then print("Err: CSn"); iErrCnt:=iErrCnt+1; end if;
  end if;
end process procCheck;


end;
 
 