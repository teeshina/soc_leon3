-----------------------------------------------------------------------------
-- Entity: 	    3-axis Gyroscope SPI controller
-- File:	       gyrospi.vhd
-- Author:	     Sergey Khabarov - GNSS Sensor Ltd
-- Description:	This unit imeplements SPI interface with Gyroscope IC
--              by STMicroelectronics via 4-wires SPI interface
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
use gnsslib.gnssmodule.all;

entity gyrospi is
  generic (
    pindex   : integer := 0;
    paddr    : integer := 0;
    pmask    : integer := 16#ffe#
  );
  port (
    rst    : in  std_ulogic;
    clk    : in  std_ulogic;
    apbi   : in  apb_slv_in_type;
    apbo   : out apb_slv_out_type;
    inInt1 : in std_ulogic;
    inInt2 : in std_ulogic;
    inSDI  : in std_ulogic;
    outSPC : out std_ulogic;
    outSDO : out std_ulogic;
    outCSn : out std_ulogic
  );
end; 
 
architecture rtl of gyrospi is

constant VENDOR_GNSSSENSOR     : integer := 16#F1#; -- TODO: move to devices.vhd
constant GNSSSENSOR_GYROSCOPE  : integer := 16#079#;
constant REVISION              : integer := 1;

constant FIFO_SIZE             : integer := 16;

constant pconfig : apb_config_type := (
  0 => ahb_device_reg ( VENDOR_GNSSSENSOR, GNSSSENSOR_GYROSCOPE, 0, REVISION, 0),
  1 => apb_iobar(paddr, pmask));

type fifo is array (0 to FIFO_SIZE - 1) of std_logic_vector(38 downto 0);

type registers is record
  WHO_AM_I      : std_logic_vector(7 downto 0);
  CTRL_REG1     : std_logic_vector(7 downto 0);
  CTRL_REG2     : std_logic_vector(7 downto 0);
  CTRL_REG3     : std_logic_vector(7 downto 0);
  CTRL_REG4     : std_logic_vector(7 downto 0);
  CTRL_REG5     : std_logic_vector(7 downto 0);
  REFERENCE     : std_logic_vector(7 downto 0);
  OUT_TEMP      : std_logic_vector(7 downto 0);
  STATUS_REG    : std_logic_vector(7 downto 0);
  OUT_X_L       : std_logic_vector(7 downto 0);
  OUT_X_H       : std_logic_vector(7 downto 0);
  OUT_Y_L       : std_logic_vector(7 downto 0);
  OUT_Y_H       : std_logic_vector(7 downto 0);
  OUT_Z_L       : std_logic_vector(7 downto 0);
  OUT_Z_H       : std_logic_vector(7 downto 0);
  FIFO_CTRL_REG : std_logic_vector(7 downto 0);
  FIFO_SRC_REG  : std_logic_vector(7 downto 0);
  INT1_CFG      : std_logic_vector(7 downto 0);
  INT1_SRC      : std_logic_vector(7 downto 0);
  INT1_TSH_XH   : std_logic_vector(7 downto 0);
  INT1_TSH_XL   : std_logic_vector(7 downto 0);
  INT1_TSH_YH   : std_logic_vector(7 downto 0);
  INT1_TSH_YL   : std_logic_vector(7 downto 0);
  INT1_TSH_ZH   : std_logic_vector(7 downto 0);
  INT1_TSH_ZL   : std_logic_vector(7 downto 0);
  INT1_DURATION : std_logic_vector(7 downto 0);

  scale         : std_logic_vector(31 downto 0);
  cfg_icnt_ena  : std_ulogic;
  cfg_int1_ena  : std_ulogic;
  cfg_int2_ena  : std_ulogic;
  cfg_wrword_ena : std_ulogic;

  rd_interval    : std_logic_vector(31 downto 0);
  rd_intervalcnt : integer;
  rd_wordcnt     : integer range 0 to 26;

  wr_run         : std_ulogic;
  rd_word_run    : std_ulogic;
  rd_all         : std_ulogic;
  ShiftTx        : std_logic_vector(15 downto 0);
  ScaleCnt       : integer;
  ClkPosedge     : std_ulogic;
  ClkNegedge     : std_ulogic;
  SPC            : std_ulogic;
  BitCnt         : integer range 0 to 18;
  nCS            : std_ulogic;
  WriteWord      : std_logic_vector(15 downto 0);
  RxAdr          : std_logic_vector(7 downto 0);
  RxData         : std_logic_vector(7 downto 0);

  FifoData       : fifo;
  FifoCnt        : integer range 0 to FIFO_SIZE;
  Writting       : std_ulogic;
  Reading        : std_ulogic;
  WrFifo         : std_ulogic;

  Int1         : std_ulogic;
  Int2         : std_ulogic;
end record;

signal r, rin : registers;

begin

  comblogic : process(rst, r, apbi, inSDI, inInt1, inInt2)
  variable v : registers;
  variable readdata : std_logic_vector(31 downto 0);
  variable wWrFifo : std_ulogic;
  variable wRdFifo : std_ulogic;
  variable wr_run : std_ulogic;
  variable rd_possible : std_ulogic;
  variable rd_run_icnt : std_ulogic;
  variable rd_run_int1 : std_ulogic;
  variable rd_run_int2 : std_ulogic;
  variable rd_run_wrword : std_ulogic;
  variable rd_all_run : std_ulogic;
  variable rd_word_run : std_ulogic;
  variable rd_word_rdy : std_ulogic;

  begin

    v := r;
    -- read registers:

    readdata := (others => '0');
    case apbi.paddr(8 downto 2) is
      when "0001111" => readdata(7 downto 0) := r.WHO_AM_I;
      when "0100000" => readdata(7 downto 0) := r.CTRL_REG1;
      when "0100001" => readdata(7 downto 0) := r.CTRL_REG2;
      when "0100010" => readdata(7 downto 0) := r.CTRL_REG3;
      when "0100011" => readdata(7 downto 0) := r.CTRL_REG4;
      when "0100100" => readdata(7 downto 0) := r.CTRL_REG5;
      when "0100101" => readdata(7 downto 0) := r.REFERENCE;
      when "0100110" => readdata(7 downto 0) := r.OUT_TEMP;
      when "0100111" => readdata(7 downto 0) := r.STATUS_REG;
      when "0101000" => readdata(7 downto 0) := r.OUT_X_L;
      when "0101001" => readdata(7 downto 0) := r.OUT_X_H;
      when "0101010" => readdata(7 downto 0) := r.OUT_Y_L;
      when "0101011" => readdata(7 downto 0) := r.OUT_Y_H;
      when "0101100" => readdata(7 downto 0) := r.OUT_Z_L;
      when "0101101" => readdata(7 downto 0) := r.OUT_Z_H;
      when "0101110" => readdata(7 downto 0) := r.FIFO_CTRL_REG;
      when "0101111" => readdata(7 downto 0) := r.FIFO_SRC_REG;
      when "0110000" => readdata(7 downto 0) := r.INT1_CFG;
      when "0110001" => readdata(7 downto 0) := r.INT1_SRC;
      when "0110010" => readdata(7 downto 0) := r.INT1_TSH_XH;
      when "0110011" => readdata(7 downto 0) := r.INT1_TSH_XL;
      when "0110100" => readdata(7 downto 0) := r.INT1_TSH_YH;
      when "0110101" => readdata(7 downto 0) := r.INT1_TSH_YL;
      when "0110110" => readdata(7 downto 0) := r.INT1_TSH_ZH;
      when "0110111" => readdata(7 downto 0) := r.INT1_TSH_ZL;
      when "0111000" => readdata(7 downto 0) := r.INT1_DURATION;

      when "1000000" => readdata  := r.scale;
      when "1000001" => readdata  := r.rd_interval;
      when "1000010" => 
        readdata(0) := r.cfg_icnt_ena;
        readdata(1) := r.cfg_int1_ena;
        readdata(2) := r.cfg_int2_ena;
        readdata(3) := r.cfg_wrword_ena;
        readdata(15) := r.rd_all;
        readdata(20 downto 16) := conv_std_logic_vector(r.rd_wordcnt,5);
        readdata(28 downto 24) := conv_std_logic_vector(r.FifoCnt,5);
      when others =>
    end case;

    -- FIFO implementation:
    wWrFifo := apbi.psel(pindex) and apbi.penable and apbi.pwrite and (not conv_std_logic_vector(r.FifoCnt,5)(4));
    if((r.FifoCnt/=0) and (r.Writting='0') and (r.Reading='0')) then wRdFifo := '1';
    else                                                             wRdFifo := '0';end if;

    if(wWrFifo='1') then
      for i in 1 to (FIFO_SIZE-1) loop
        v.FifoData(i) := r.FifoData(i-1);
      end loop;
      v.FifoData(0) := apbi.paddr(8 downto 2) & apbi.pwdata;
      if(wRdFifo='0') then v.FifoCnt := r.FifoCnt+1;  end if;
    end if;
    
    if(wRdFifo='1') then
      if(wWrFifo='0') then v.FifoCnt := r.FifoCnt-1; end if;
    end if;

    -- write registers
    wr_run := '0';
    rd_run_wrword := '0';
    if(wRdFifo='1') then
      case(r.FifoData(r.FifoCnt-1)(38 downto 32)) is
        when "0100000"|"0100001"|"0100010"|"0100011"|"0100100"
            |"0100101"|"0101110"|"0110000"|"0110010"|"0110011"
            |"0110100"|"0110101"|"0110110"|"0110111"|"0111000" =>
            -- BITS: [15]=nRW (1=read;0-write); [14]=auto-incr ena;
            v.WriteWord := "00" & r.FifoData(r.FifoCnt-1)(37 downto 32) & r.FifoData(r.FifoCnt-1)(7 downto 0);
            wr_run := '1';
        when "1000000" => v.scale := r.FifoData(r.FifoCnt-1)(31 downto 0);
        when "1000001" =>
          v.rd_interval := r.FifoData(r.FifoCnt-1)(31 downto 0);
          v.rd_intervalcnt := 0;
        when "1000010" =>
          v.cfg_icnt_ena := r.FifoData(r.FifoCnt-1)(0);
          v.cfg_int1_ena := r.FifoData(r.FifoCnt-1)(1);
          v.cfg_int2_ena := r.FifoData(r.FifoCnt-1)(2);
          v.cfg_wrword_ena := r.FifoData(r.FifoCnt-1)(3);
        when "1000011" => rd_run_wrword := r.cfg_wrword_ena;
        when others =>
      end case;
    end if;

    v.wr_run := wr_run;
    
    v.Int1 := inInt1;
    v.Int2 := inInt2;
    
    -- sending word:
    if(r.wr_run='1') then v.ShiftTx := r.WriteWord;
    elsif((r.BitCnt/=0)and(r.ClkNegedge='1')) then v.ShiftTx := r.ShiftTx(14 downto 0) & '0'; end if;
      
      
    -- reading event selector:
    if((r.FifoCnt=0)and(r.Writting='0')and(wr_run='0')) then rd_possible := '1';
    else                                                     rd_possible := '0'; end if;
    if((conv_integer(r.rd_interval)=r.rd_intervalcnt)and(wr_run='0')and(wRdFifo='0')) 
    then rd_run_icnt := r.cfg_icnt_ena;
    else rd_run_icnt := '0'; end if;
    rd_run_int1 := inInt1 and (not r.Int1) and r.cfg_int1_ena;
    rd_run_int2 := inInt2 and (not r.Int2) and r.cfg_int2_ena;

    rd_all_run := rd_run_icnt or rd_run_int1 or rd_run_int2 or rd_run_wrword;
    
    -- Full reading cycle includes 26 independent addresses
    if((rd_all_run='1')or(r.cfg_icnt_ena='0')) then v.rd_intervalcnt := 0;
    elsif ((rd_possible='1')and(r.rd_all='0')) then v.rd_intervalcnt := r.rd_intervalcnt+1; end if;

    if((rd_all_run='1')and(r.rd_all='0')) then v.rd_all := '1';
    elsif ((r.BitCnt=17)and(r.ClkNegedge='1')and(r.rd_wordcnt=25)) then v.rd_all := '0'; end if;

    if((rd_all_run='1')and(r.rd_all='0')) then v.rd_wordcnt := 0;
    elsif ((r.BitCnt=17)and(r.ClkNegedge='1')and(r.Reading='1')) then v.rd_wordcnt := r.rd_wordcnt+1; end if;
      
    rd_word_run := rd_all_run or (r.rd_all and (not r.Reading) and (not wRdFifo) and (not r.Writting));
    
    v.rd_word_run := rd_word_run;
    
    -- form address and flags for the reading:
    if(rd_all_run='1') then                        v.RxAdr := "10" & conv_std_logic_vector(16#0F#,6);
    elsif(rd_word_run='1') then                    v.RxAdr := "10" & conv_std_logic_vector(16#1F#+r.rd_wordcnt,6);
    elsif((r.BitCnt/=0)and(r.ClkNegedge='1')) then v.RxAdr := r.RxAdr(6 downto 0) & '0'; end if;
    
    -- latch input data:
    if((r.Reading='1') and (r.ClkPosedge='1')) then v.RxData := r.RxData(6 downto 0) & inSDI; end if;
    
    if((r.Reading='1')and(r.ClkNegedge='1')and(r.BitCnt=16)) then rd_word_rdy := '1';
    else rd_word_rdy := '0'; end if;
    
    if(rd_word_rdy='1') then
      case(r.rd_wordcnt) is
        when 16#00# => v.WHO_AM_I := r.RxData;
        when 16#01# => v.CTRL_REG1 := r.RxData;
        when 16#02# => v.CTRL_REG2 := r.RxData;
        when 16#03# => v.CTRL_REG3 := r.RxData;
        when 16#04# => v.CTRL_REG4 := r.RxData;
        when 16#05# => v.CTRL_REG5 := r.RxData;
        when 16#06# => v.REFERENCE := r.RxData;
        when 16#07# => v.OUT_TEMP := r.RxData;
        when 16#08# => v.STATUS_REG := r.RxData;
        when 16#09# => v.OUT_X_L := r.RxData;
        when 16#0A# => v.OUT_X_H := r.RxData;
        when 16#0B# => v.OUT_Y_L := r.RxData;
        when 16#0C# => v.OUT_Y_H := r.RxData;
        when 16#0D# => v.OUT_Z_L := r.RxData;
        when 16#0E# => v.OUT_Z_H := r.RxData;
        when 16#0F# => v.FIFO_CTRL_REG := r.RxData;
        when 16#10# => v.FIFO_SRC_REG := r.RxData;
        when 16#11# => v.INT1_CFG := r.RxData;
        when 16#12# => v.INT1_SRC := r.RxData;
        when 16#13# => v.INT1_TSH_XH := r.RxData;
        when 16#14# => v.INT1_TSH_XL := r.RxData;
        when 16#15# => v.INT1_TSH_YH := r.RxData;
        when 16#16# => v.INT1_TSH_YL := r.RxData;
        when 16#17# => v.INT1_TSH_ZH := r.RxData;
        when 16#18# => v.INT1_TSH_ZL := r.RxData;
        when 16#19# => v.INT1_DURATION := r.RxData;
        when others => 
      end case;
    end if;

    -- loading procedure:
    if(wr_run='1') then                            v.Writting := '1';
    elsif((r.BitCnt=17)and(r.ClkNegedge='1')) then v.Writting := '0'; end if;

    if(rd_word_run='1') then                       v.Reading := '1';
    elsif((r.BitCnt=17)and(r.ClkNegedge='1')) then v.Reading := '0'; end if;
  
    if(((r.Writting='0')and(r.Reading='0'))or(r.ClkPosedge='1')) then v.ScaleCnt := 0;
    else                                                              v.ScaleCnt := r.ScaleCnt+1; end if;
  
    if((wr_run='1')or(rd_word_run='1')or ((r.Writting or r.Reading)='0')) then v.BitCnt := 0;
    elsif(((r.Writting or r.Reading)='1') and (r.ClkNegedge='1')) then   v.BitCnt := r.BitCnt+1; end if;

    -- scaler pulse:
    if((conv_integer(r.scale)/=0)and((r.ScaleCnt+1)=conv_integer(r.scale(31 downto 1)))) 
    then v.ClkNegedge := '1';
    else v.ClkNegedge := '0'; end if;

    if((conv_integer(r.scale)/=0) and (r.ScaleCnt=conv_integer(r.scale))) then v.ClkPosedge := '1';
    else v.ClkPosedge := '0'; end if;



    -- SCLK former:
    if((r.ClkPosedge='1') or ((r.Writting or r.Reading)='0')) then       v.SPC := '1';
    elsif((r.ClkNegedge='1') and (r.BitCnt/=16) and (r.BitCnt/=17)) then v.SPC := '0'; end if;

    -- CS signal:
    if((r.Writting='0') and (r.Reading='0')) then   v.nCS := '1';
    elsif((r.ClkNegedge='1')and(r.BitCnt=16)) then  v.nCS := '1';
    elsif((r.wr_run='1')or(r.rd_word_run='1')) then v.nCS := '0'; end if;
  
    
    -- reset operation

    if rst = '0' then 
      v.Writting := '0';
      v.Reading := '0';
      v.scale := conv_std_logic_vector(4,32);
      v.ScaleCnt := 0;
      v.ClkNegedge := '0';
      v.ClkPosedge := '0';
      v.SPC := '1';
      v.nCS := '1';
      v.BitCnt := 0;
      v.rd_interval := conv_std_logic_vector(66000,32);
      v.rd_intervalcnt := 0;
      v.wr_run := '0';
      v.rd_word_run := '0';
      v.rd_all := '0';
      v.cfg_icnt_ena := '0';    -- run reading using internal counter
      v.cfg_int1_ena := '0';    -- run reading using gyroscope interrupt 1
      v.cfg_int2_ena := '0';    -- run reading using gyroscope interrupt 2
      v.cfg_wrword_ena := '1';  -- run reading by writting into certain address
      v.RxAdr := (others => '0');
      v.ShiftTx := (others => '0');
    end if;

    rin <= v;

    apbo.prdata <= readdata; 	-- drive apb read bus
    apbo.pirq   <= (others => '0');
    apbo.pindex <= pindex;


  end process;


  apbo.pconfig <= pconfig;

  outSPC <= r.SPC;
  outCSn <= r.nCS;
  outSDO <= (r.Writting and r.ShiftTx(15)) or (not r.Writting and r.RxAdr(7));



  -- registers:
  regs : process(clk)
  begin if rising_edge(clk) then r <= rin; end if; end process;

end;
