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
use gaisler.libjtagcom.all;
use gaisler.jtag.all;
library work;
use work.config.all;



entity ahbjtag_tb is
end ahbjtag_tb;

architecture behavior of ahbjtag_tb is

  constant CLK_HPERIOD : time := 10 ps;
  constant STRING_SIZE : integer := 238; -- string size = index of the last element

  -- conversion function
  function StringToUVector(inStr: string) return std_ulogic_vector is
    variable temp: std_ulogic_vector(inStr'range) := (others => 'X');
  begin
    for i in inStr'range loop --
      if(inStr(inStr'high-i+1)='1')    then temp(i) := '1';
      elsif(inStr(inStr'high-i+1)='0') then temp(i) := '0';
      end if;
    end loop;
    return temp(inStr'high downto 1);
  end function StringToUVector;
  -- conversion function
  function StringToVector(inStr: string) return std_logic_vector is
    variable temp: std_logic_vector(inStr'range) := (others => 'X');
  begin
    for i in inStr'range loop --
      if(inStr(inStr'high-i+1)='1')    then temp(i) := '1';
      elsif(inStr(inStr'high-i+1)='0') then temp(i) := '0';
      end if;
    end loop;
    return temp(inStr'high downto 1);
  end function StringToVector;

  function UnsignedToSigned(inUnsigned: std_ulogic_vector) 
    return std_logic_vector is
    variable temp: std_logic_vector(inUnsigned'length-1 downto 0) := (others => 'X');
    variable i: integer:=0;
  begin
    while i < inUnsigned'length loop
      if(inUnsigned(i)='1')    then temp(i) := '1';
      elsif(inUnsigned(i)='0') then temp(i) := '0';
      end if;
      i := i+1;
    end loop;
    return temp;
  end function UnsignedToSigned;

  -- input/output signals:
  signal inNRst       : std_logic:= '0';
  signal inClk        : std_logic:= '0';
  signal inJtagTCK    : std_ulogic_vector(1 downto 0);
  signal inJtagTDI    : std_ulogic_vector(1 downto 0);
  signal inJtagSel    : std_ulogic_vector(1 downto 0);
  signal inJtagShift  : std_ulogic_vector(1 downto 0);
  signal inJtagUpdate : std_ulogic_vector(1 downto 0);
  signal inJtagRESET  : std_ulogic_vector(1 downto 0);
  signal outJtagTDO   : std_ulogic;
  signal outTdoEna    : std_ulogic;

  --signal ahbmi        : ahb_mst_in_type;
  signal inAhbMst     : ahb_mst_in_type;
  --signal ahbmo        : ahb_mst_out_vector := (others => ahbm_none);
  signal ahbmo        : ahb_mst_out_type := ahbm_none;
  
  signal wbSignals: std_ulogic_vector(STRING_SIZE-1 downto 0);
  signal wbSSignals: std_logic_vector(STRING_SIZE-1 downto 0);
  
  
  signal dmao_start    : std_ulogic;
  signal dmao_active   : std_ulogic;
  signal dmao_ready    : std_ulogic;
  signal dmao_retry    : std_ulogic;
  signal dmao_mexc     : std_ulogic;
  signal dmao_addr     : std_logic_vector(9 downto 0);
  signal dmao_data     : std_logic_vector(31 downto 0);
  
  signal dmai_address     : std_logic_vector(31 downto 0);
  signal dmai_wdata     : std_logic_vector(31 downto 0);
  signal dmai_start     : std_ulogic;
  signal dmai_burst     : std_ulogic;
  signal dmai_write     : std_ulogic;
  signal dmai_busy     : std_ulogic;
  signal dmai_irq     : std_ulogic;
  signal dmai_size     : std_logic_vector(2 downto 0);

begin


  -- Process of clock generation
  procClkgen : process
  begin
      inClk <= '0' after CLK_HPERIOD, '1' after 2*CLK_HPERIOD;
      wait for 2*CLK_HPERIOD;
  end process procClkgen;

  -- Process of reading  
  procReadingFile : process
    file InputFile:TEXT is "e:/ahbjtag_tb.txt";--open read_mode file_name;
    variable rdLine: line;  
    variable strLine : string(STRING_SIZE downto 1);
  begin
    while not endfile(InputFile) loop
      readline(InputFile, rdLine);
      read(rdLine, strLine);
      wbSignals <= StringToUVector(strLine);
      wbSSignals <= StringToVector(strLine);--UnsignedToSigned(wbSignals);
   
      wait until rising_edge(inClk);
      --wait until falling_edge(inClk);
    end loop;
  end process procReadingFile;


-- signal parsment and assignment
  inNRst    <= wbSignals(0);
  inJtagTCK <= wbSignals(2 downto 1);
  inJtagTDI <= wbSignals(4 downto 3);
  inJtagSel <= wbSignals(6 downto 5);
  inJtagShift <= wbSignals(8 downto 7);
  inJtagUpdate <= wbSignals(10 downto 9);
  inJtagRESET <= wbSignals(12 downto 11); 
  
  inAhbMst.hgrant(CFG_NCPU+CFG_AHB_UART) <= wbSignals(13); 
  inAhbMst.hready <= wbSignals(14); 
  inAhbMst.hresp(0) <= wbSignals(15); 
  inAhbMst.hresp(1) <= wbSignals(16);
  inAhbMst.hrdata <= wbSSignals(48 downto 17);

  dmao_start  <= wbSignals(49);
  dmao_active <= wbSignals(50);
  dmao_ready  <= wbSignals(51);
  dmao_retry  <= wbSignals(52);
  dmao_mexc   <= wbSignals(53);
  dmao_addr   <= wbSSignals(63 downto 54);
  dmao_data   <= wbSSignals(95 downto 64);

  dmai_address  <= wbSSignals(127 downto 96);
  dmai_wdata    <= wbSSignals(159 downto 128);
  dmai_start    <= wbSignals(160);
  dmai_burst    <= wbSignals(161);
  dmai_write    <= wbSignals(162);
  dmai_busy     <= wbSignals(163);
  dmai_irq      <= wbSignals(164);
  dmai_size     <= wbSSignals(167 downto 165);

  
  -- run test module:
  tt : ahbjtag generic map
  (
    tech => CFG_FABTECH,
    hindex => CFG_NCPU+CFG_AHB_UART
  )
  port map
  (
    inNRst,
    inClk,
    inJtagTCK,
    inJtagTDI,
    inJtagSel,
    inJtagShift,
    inJtagUpdate,
    outJtagTDO,
    inAhbMst,
    ahbmo,
    inJtagRESET,
    outTdoEna
  );

procCheck : process (inClk)
begin
  if(rising_edge(inClk)) then
    --if(dma_data /= (tt.dmao.rdata)) then
      print("Debug output example: " & tost(dmao_data));
    --end if
  end if;
end process procCheck;


end;
 
 