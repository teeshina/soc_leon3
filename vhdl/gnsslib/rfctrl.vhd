-----------------------------------------------------------------------------
-- Entity: 	RF front-end control
-- File:	  rfctrl.vhd
-- Author:	Sergey Khabarov - GNSS Sensor Ltd
-- Description:	This unit imeplements SPI interface with MAX2769 ICs
--              and antenna control signals
------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
library grlib;
use grlib.amba.all;
use grlib.stdlib.all;
use grlib.devices.all;
library gaisler;
use gaisler.misc.all;

library gnsslib;
use gnsslib.gnssengine.all;

entity rfctrl is
  generic (
    pindex   : integer := 0;
    paddr    : integer := 0;
    pmask    : integer := 16#fff#
  );
  port (
    rst    : in  std_ulogic;
    clk    : in  std_ulogic;
    apbi   : in  apb_slv_in_type;
    apbo   : out apb_slv_out_type;
    inLD     : in std_logic_vector(1 downto 0);
    outSCLK  : out std_ulogic;
    outSDATA : out std_ulogic;
    outCSn   : out std_logic_vector(1 downto 0);
    -- Antenna control:
    inExtAntStat   : in std_ulogic;
    inExtAntDetect : in std_ulogic;
    outExtAntEna   : out std_ulogic
  );
end; 
 
architecture rtl of rfctrl is

constant VENDOR_GNSSSENSOR     : integer := 16#F1#; -- TODO: move to devices.vhd
constant GNSSSENSOR_RF_CONTROL : integer := 16#077#;
constant REVISION              : integer := 0;

constant pconfig : apb_config_type := (
  0 => ahb_device_reg ( VENDOR_GNSSSENSOR, GNSSSENSOR_RF_CONTROL, 0, REVISION, 0),
  1 => apb_iobar(paddr, pmask));


type registers is record
  conf1       : std_logic_vector(31 downto 0);
  conf2       : std_logic_vector(31 downto 0);
  conf3       : std_logic_vector(31 downto 0);
  pllconf     : std_logic_vector(31 downto 0);
  div         : std_logic_vector(31 downto 0);
  fdiv        : std_logic_vector(31 downto 0);
  strm        : std_logic_vector(31 downto 0);
  clkdiv      : std_logic_vector(31 downto 0);
  test1       : std_logic_vector(31 downto 0);
  test2       : std_logic_vector(31 downto 0);
  scale       : std_logic_vector(31 downto 0);
  load_run    : std_ulogic;
  select_spi  : std_logic_vector(1 downto 0);
  loading     : std_ulogic;
  ScaleCnt    : std_logic_vector(31 downto 0);
  SClkPosedge  : std_ulogic;
  SClkNegedge  : std_ulogic;
  SCLK         : std_ulogic;
  BitCnt       : integer range 0 to 33;
  CS           : std_ulogic;  --!! not inversed!!
  WordSelector : std_logic_vector(8 downto 0);
  SendWord     : std_logic_vector(31 downto 0);
  ExtAntEna    : std_ulogic;
end record;

signal r, rin : registers;

begin

  comb : process(rst, r, apbi, inLD, inExtAntStat, inExtAntDetect)
  variable v : registers;
  variable readdata : std_logic_vector(31 downto 0);
  variable wNewWord : std_ulogic;
  begin

    v := r;
    v.load_run := '0';

    -- read registers:

    readdata := (others => '0');
    case apbi.paddr(7 downto 2) is
      when "000000" => readdata  := "0000" & r.conf1(31 downto 4);
      when "000001" => readdata  := "0000" & r.conf2(31 downto 4);
      when "000010" => readdata  := "0000" & r.conf3(31 downto 4);
      when "000011" => readdata  := "0000" & r.pllconf(31 downto 4);
      when "000100" => readdata  := "0000" & r.div(31 downto 4);
      when "000101" => readdata  := "0000" & r.fdiv(31 downto 4);
      when "000110" => readdata  := "0000" & r.strm(31 downto 4);
      when "000111" => readdata  := "0000" & r.clkdiv(31 downto 4);
      when "001000" => readdata  := "0000" & r.test1(31 downto 4);
      when "001001" => readdata  := "0000" & r.test2(31 downto 4);
      when "001010" => readdata  := r.scale;
      when "001011" => readdata(9 downto 0)  := conv_std_logic_vector(r.BitCnt,6) & '0' & r.loading & inLD;
      when "001111" => readdata(5 downto 0)  := inExtAntStat & inExtAntDetect & "000" & r.ExtAntEna;
      when others =>
    end case;

    -- write registers

    if (apbi.psel(pindex) and apbi.penable and apbi.pwrite) = '1' then
      case apbi.paddr(7 downto 2) is
        when "000000" => v.conf1 := apbi.pwdata(27 downto 0) & "0000";
        when "000001" => v.conf2 := apbi.pwdata(27 downto 0) & "0001";
        when "000010" => v.conf3 := apbi.pwdata(27 downto 0) & "0010";
        when "000011" => v.pllconf := apbi.pwdata(27 downto 0) & "0011";
        when "000100" => v.div := apbi.pwdata(27 downto 0) & "0100";
        when "000101" => v.fdiv := apbi.pwdata(27 downto 0) & "0101";
        when "000110" => v.strm := apbi.pwdata(27 downto 0) & "0110";
        when "000111" => v.clkdiv := apbi.pwdata(27 downto 0) & "0111";
        when "001000" => v.test1 := apbi.pwdata(27 downto 0) & "1000";
        when "001001" => v.test2 := apbi.pwdata(27 downto 0) & "1001";
        when "001010" => 
          if(apbi.pwdata(31 downto 1) = zero32(31 downto 1)) then v.scale := conv_std_logic_vector(2,32);
          else v.scale := apbi.pwdata; end if;
        when "001011" => 
          v.load_run := '1';
          v.ScaleCnt := (others => '0');
          v.BitCnt := 0;
          if(conv_integer(apbi.pwdata)=0) then v.select_spi := "01";
          elsif(conv_integer(apbi.pwdata)=1) then v.select_spi := "10";
          else v.select_spi := "00"; end if;
        when "001111" => v.ExtAntEna := apbi.pwdata(0);
        when others => 
      end case;
    end if;
    
    -- loading procedure:
    if((r.SClkNegedge='1') and (r.BitCnt=33)) then wNewWord := '1';
    else wNewWord := '0'; end if;
    
    if(r.load_run='1')                                   then v.loading := '1';
    elsif((wNewWord='1')and(r.WordSelector="000000000")) then v.loading := '0'; end if;
  
    if((r.loading and r.SClkNegedge)='1') then v.ScaleCnt := (others => '0');
    elsif(r.loading='1')                  then v.ScaleCnt := r.ScaleCnt+1; end if;

    -- scaler pulse:
    if((conv_integer(r.scale)/=0)and(r.ScaleCnt=r.scale)) then v.SClkNegedge := '1';
    else                                                       v.SClkNegedge := '0'; end if;

    if((conv_integer(r.scale)/=0)and(r.ScaleCnt=('0'& r.scale(31 downto 1)))) then v.SClkPosedge := '1';
    else                                                         v.SClkPosedge := '0'; end if;

    -- SCLK former:
    if(r.SClkPosedge='1') then v.SCLK := '1';
    elsif(r.SClkNegedge='1') then v.SCLK := '0'; end if;

    -- Not inversed CS signal:
    if((r.SClkNegedge='1')and(r.BitCnt=33)) then v.BitCnt := 0;
    elsif(r.SClkNegedge='1')                then v.BitCnt := r.BitCnt + 1; end if;
  
    if((r.BitCnt=0)or((r.BitCnt=33))) then v.CS := '0';
    else                                   v.CS := '1'; end if;

    -- Word multiplexer:
    if(r.load_run='1')  then v.WordSelector := "000000001";
    elsif(wNewWord='1') then v.WordSelector := r.WordSelector(7 downto 0) & '0'; end if;
  
    if(r.load_run='1') then                      v.SendWord := r.conf1;
    elsif((wNewWord='1')and(r.WordSelector(0)='1')) then v.SendWord := r.conf2;
    elsif((wNewWord='1')and(r.WordSelector(1)='1')) then v.SendWord := r.conf3;
    elsif((wNewWord='1')and(r.WordSelector(2)='1')) then v.SendWord := r.pllconf;
    elsif((wNewWord='1')and(r.WordSelector(3)='1')) then v.SendWord := r.div;
    elsif((wNewWord='1')and(r.WordSelector(4)='1')) then v.SendWord := r.fdiv;
    elsif((wNewWord='1')and(r.WordSelector(5)='1')) then v.SendWord := r.strm;
    elsif((wNewWord='1')and(r.WordSelector(6)='1')) then v.SendWord := r.clkdiv;
    elsif((wNewWord='1')and(r.WordSelector(7)='1')) then v.SendWord := r.test1;
    elsif((wNewWord='1')and(r.WordSelector(8)='1')) then v.SendWord := r.test2;
    elsif((r.SClkNegedge='1')and(r.BitCnt/=0)and(r.BitCnt/=33)) then  v.SendWord := r.SendWord(30 downto 0)&'0'; end if;


    -- reset operation

    if rst = '0' then 
      v.load_run := '0';
      v.conf1 := (others => '0');
      v.conf2 := (others => '0');
      v.conf3 := (others => '0');
      v.pllconf := (others => '0');
      v.div := (others => '0');
      v.fdiv := (others => '0');
      v.strm := (others => '0');
      v.clkdiv := (others => '0');
      v.test1 := (others => '0');
      v.test2 := (others => '0');
      v.scale := (others => '0');
      v.SCLK := '0';
      v.BitCnt := 0;
      v.CS := '0';
      v.select_spi := (others => '0');
    end if;

    rin <= v;

    apbo.prdata <= readdata; 	-- drive apb read bus
    apbo.pirq   <= (others => '0');
    apbo.pindex <= pindex;


  end process;


  apbo.pconfig <= pconfig;

  outSCLK   <= r.SCLK;
  outCSn(0) <= not(r.CS and r.select_spi(0));
  outCSn(1) <= not(r.CS and r.select_spi(1));
  outSDATA  <= r.SendWord(31);

  outExtAntEna <= r.ExtAntEna;


  -- registers:
  regs : process(clk)
  begin if rising_edge(clk) then r <= rin; end if; end process;

end;
