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



entity jtagcom_tb is
end jtagcom_tb;

architecture behavior of jtagcom_tb is

  constant CLK_HPERIOD : time := 10 ps;
  constant STRING_SIZE : integer := 227; -- string size = index of the last element

  constant REREAD : integer := 1;
  constant TAPSEL   : integer := has_tapsel(CFG_FABTECH);

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
  --
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

  signal in_tapo      : tap_out_type;
  signal out_tapi     : tap_in_type;
  signal chk_tapi     : tap_in_type;
  signal in_dmao      : ahb_dma_out_type;    
  signal out_dmai     : ahb_dma_in_type;
  signal chk_dmai     : ahb_dma_in_type;
  
  signal wbSignals: std_ulogic_vector(STRING_SIZE-1 downto 0);
  signal wbSSignals: std_logic_vector(STRING_SIZE-1 downto 0);
  shared variable iClkCnt : integer := 0;
  
  type state_type is (shft, ahb, nxt_shft);  
  type reg_type is record
    addr  : std_logic_vector(34 downto 0);
    data  : std_logic_vector(32 downto 0);
    state : state_type;
    tck   : std_logic_vector(1 downto 0);
    tck2  : std_ulogic;    
    trst  : std_logic_vector(1 downto 0);
    tdi   : std_logic_vector(1 downto 0);
    shift : std_logic_vector(1 downto 0);
    shift2: std_ulogic;
    upd   : std_logic_vector(1 downto 0);
    upd2  : std_ulogic;
    asel  : std_logic_vector(1 downto 0);
    dsel  : std_logic_vector(1 downto 0);
    tdi2  : std_ulogic;
    seq   : std_ulogic;
    holdn : std_ulogic;
  end record;

  signal chk_r : reg_type;

begin


  -- Process of clock generation
  procClkgen : process
  begin
      inClk <= '0' after CLK_HPERIOD, '1' after 2*CLK_HPERIOD;
      wait for 2*CLK_HPERIOD;
  end process procClkgen;

  -- Process of reading  
  procReadingFile : process
    file InputFile:TEXT is "e:/jtagcom_tb.txt";--open read_mode file_name;
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
      iClkCnt := iClkCnt + 1;
    end loop;
  end process procReadingFile;
  
  procSignals : process (inClk, wbSignals(158 downto 157))
  begin
    if(wbSignals(158 downto 157)="01")    then chk_r.state <= ahb;
    elsif(wbSignals(158 downto 157)="10") then chk_r.state <= nxt_shft;
    else                                       chk_r.state <= shft;
    end if;
  end process procSignals;


-- signal parsment and assignment
  inNRst    <= wbSignals(0);
  
  -- Input signals:
  in_tapo.tck   <= wbSignals(1);--std_ulogic;
  in_tapo.tdi   <= wbSignals(2);--std_ulogic;
  in_tapo.inst  <= wbSSignals(10 downto 3);--std_logic_vector(7 downto 0);
  in_tapo.asel  <= wbSignals(11);--std_ulogic;
  in_tapo.dsel  <= wbSignals(12);--std_ulogic;
  in_tapo.reset <= wbSignals(13);--std_ulogic;
  in_tapo.capt  <= wbSignals(14);--std_ulogic;
  in_tapo.shift <= wbSignals(15);--std_ulogic;
  in_tapo.upd   <= wbSignals(16);--std_ulogic;      

  in_dmao.start  <= wbSignals(17);--std_ulogic;
  in_dmao.active <= wbSignals(18);--std_ulogic;
  in_dmao.ready  <= wbSignals(19);--std_ulogic;
  in_dmao.retry  <= wbSignals(20);--std_ulogic;
  in_dmao.mexc   <= wbSignals(21);--std_ulogic;
  in_dmao.haddr  <= wbSSignals(31 downto 22);--std_logic_vector(9 downto 0);
  in_dmao.rdata  <= wbSSignals(63 downto 32);--std_logic_vector(AHBDW-1 downto 0);

  -- Check signals:
  chk_tapi.en   <= '0';
  chk_tapi.tdo  <= wbSignals(64);

  chk_dmai.address <= wbSSignals(96 downto 65);--std_logic_vector(31 downto 0);
  chk_dmai.wdata   <= wbSSignals(128 downto 97);--std_logic_vector(AHBDW-1 downto 0);
  chk_dmai.start   <= wbSignals(129);--std_ulogic;
  chk_dmai.burst   <= wbSignals(130);--std_ulogic;
  chk_dmai.write   <= wbSignals(131);--std_ulogic;
  chk_dmai.busy    <= wbSignals(132);--std_ulogic;
  chk_dmai.irq     <= wbSignals(133);--std_ulogic;
  chk_dmai.size    <= wbSSignals(136 downto 134);--std_logic_vector(2 downto 0);

  chk_r.asel(1)    <= wbSSignals(137);--std_logic_vector;
  chk_r.asel(0)    <= wbSSignals(138);--
  chk_r.dsel(1)    <= wbSSignals(139);--std_logic_vector;
  chk_r.dsel(0)    <= wbSSignals(140);--
  chk_r.shift(1)   <= wbSSignals(141);--
  chk_r.shift(0)   <= wbSSignals(142);--
  chk_r.shift2     <= wbSSignals(143);--
  chk_r.tck(1)     <= wbSSignals(144);--
  chk_r.tck(0)     <= wbSSignals(145);--
  chk_r.tck2       <= wbSSignals(146);--
  chk_r.tdi(1)     <= wbSSignals(147);--
  chk_r.tdi(0)     <= wbSSignals(148);--
  chk_r.tdi2       <= wbSSignals(149);--
  chk_r.upd(1)     <= wbSSignals(150);--
  chk_r.upd(0)     <= wbSSignals(151);--
  chk_r.upd2       <= wbSSignals(152);--
  chk_r.seq        <= wbSSignals(153);--
  chk_r.holdn      <= wbSSignals(154);--
  chk_r.trst(1)    <= wbSSignals(155);--
  chk_r.trst(0)    <= wbSSignals(156);--

  chk_r.addr    <= wbSSignals(193 downto 159);--std_logic_vector(34 downto 0);
  chk_r.data    <= wbSSignals(226 downto 194);--std_logic_vector(32 downto 0);

  
  tt : jtagcom generic map 
  (
    isel => TAPSEL,
    nsync => 2,
    ainst => 2,
    dinst => 3,
    reread => REREAD
  )
  port map
  (
    inNRst,
    inClk,
    in_tapo,
    out_tapi,
    in_dmao,
    out_dmai
  );


procCheck : process (inClk)
begin
  if(rising_edge(inClk)) then
    --if(dma_data /= (tt.dmao.rdata)) then
      print("Debug output example: " & tost(chk_dmai.address));
      --print("Debug output example: " & tost(tt/r.addr);
    --end if
  end if;
end process procCheck;


end;
 
 