library ieee;
use ieee.std_logic_1164.all;
library std;
use std.textio.all;
library grlib;
use grlib.stdlib.all;
use grlib.devices.all;
library techmap;
use techmap.gencomp.all;
use techmap.alltap.all;
library work;
use work.config.all;
use work.util_tb.all;



entity jtagpad_tb is
  constant CLK_HPERIOD : time := 10 ps;
  constant STRING_SIZE : integer := 13; -- string size = index of the last element
end jtagpad_tb;

architecture behavior of jtagpad_tb is


  -- input/output signals:
  signal in_TCK    : std_logic:= '0';
  signal in_nTRST  : std_logic:='0';
  signal in_TMS  : std_logic;
  signal in_TDI  : std_logic;
  signal in_tdo1  : std_logic;
  signal ch_TDO  : std_logic;
  signal ch_SelUser1  : std_logic;
  signal ch_SelUser2  : std_logic;
  signal ch_TDI  : std_logic;
  signal ch_DRCK  : std_logic;
  signal ch_Capture  : std_logic;
  signal ch_Shift  : std_logic;
  signal ch_Update  : std_logic;
  signal ch_Reset  : std_logic;

  signal TCK  : std_logic;
  signal TDO  : std_logic;
  signal SelUser1  : std_logic;
  signal SelUser2  : std_logic;
  signal TDI  : std_logic;
  signal DRCK  : std_logic;
  signal Capture  : std_logic;
  signal Shift  : std_logic;
  signal Update  : std_logic;
  signal Reset  : std_logic;

  
  signal U: std_ulogic_vector(STRING_SIZE-1 downto 0);
  signal S: std_logic_vector(STRING_SIZE-1 downto 0);
  shared variable iClkCnt : integer := 0;
  shared variable iErrCnt : integer := 0;
  
begin


  -- Process of clock generation
  procClkgen : process
  begin
      in_TCK <= '0' after CLK_HPERIOD, '1' after 2*CLK_HPERIOD;
      wait for 2*CLK_HPERIOD;
  end process procClkgen;

  -- Process of reading  
  procReadingFile : process
    file InputFile:TEXT is "e:/jtagpad_tb.txt";--open read_mode file_name;
    variable rdLine: line;  
    variable strLine : string(STRING_SIZE downto 1);
  begin
    while not endfile(InputFile) loop
      readline(InputFile, rdLine);
      read(rdLine, strLine);
      U <= StringToUVector(strLine);
      S <= StringToSVector(strLine);
  
      wait until rising_edge(in_TCK);
      --wait until falling_edge(in_TCK);
      iClkCnt := iClkCnt + 1;
    end loop;
  end process procReadingFile;
  

-- signal parsment and assignment
  in_nTRST <= S(0);
  in_TMS <= S(1);
  in_TDI <= S(2);
  in_tdo1 <= S(3);
  ch_TDO <= S(4);
  ch_SelUser1 <= S(5);
  ch_SelUser2 <= S(6);
  ch_TDI <= S(7);
  ch_DRCK <= S(8);
  ch_Capture <= S(9);
  ch_Shift <= S(10);
  ch_Update <= S(11);
  ch_Reset <= S(12);

    
  tt : tap_gen port map 
  (
    trst        => in_nTRST,
    tckp        => in_TCK,
    tckn        => '0',
    tms         => in_TMS,
    tdi         => in_TDI,
    tdo         => TDO,
    tapi_en1    => '1',
    tapi_tdo1   => in_tdo1,
    tapi_tdo2   => '0',
    tapo_tck    => TCK,
    tapo_tdi    => TDI,
    tapo_inst   => open,
    tapo_rst    => Reset,
    tapo_capt   => Capture,
    tapo_shft   => Shift,
    tapo_upd    => Update,
    tapo_xsel1  => SelUser1,
    tapo_xsel2  => SelUser2,
    tdoen       => open
  );


procCheck : process (in_TCK)
begin
  if(rising_edge(in_TCK)and(iClkCnt>2)) then
  end if;
end process procCheck;


end;
 
 