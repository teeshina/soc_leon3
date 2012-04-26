------------------------------------------------------------------------------
--  INFORMATION:  http://www.GNSS-sensor.com
--  PROPERTY:     GNSS Sensor Ltd
--  E-MAIL:       alex.kosin@gnss-sensor.com
--  DESCRIPTION:  Dual-clock FIFO implementation. Dummy version uses simple 
--                counters instead of Code Gray and built-in memory
------------------------------------------------------------------------------
--  WARNING:      Set FIFO size to forget about overflow protection
------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library grlib;
use grlib.stdlib.all;
library gnsslib;
use gnsslib.gnssmodule.all;
use gnsslib.dummies.all;

------------------------------------------------------------------------------
entity wrfifo is
  port
  (
    inNRst   : in std_ulogic;
    inBusClk : in std_ulogic;
    inAdcClk : in std_ulogic;
    inWrEna  : in std_ulogic;
    inWrAdr  : in std_logic_vector(CFG_GNSS_ADDR_WIDTH-1 downto 0);
    inWrData : in std_logic_vector(31 downto 0);
    outDataRdy : out std_ulogic;
    outAdr     : out std_logic_vector(CFG_GNSS_ADDR_WIDTH-1 downto 0);
    outData    : out std_logic_vector(31 downto 0)
  );
end;

architecture rtl of wrfifo is

  constant FIFO_BUS_SIZE : integer := 5;

  type bus_regs is record
    wr_adr : std_logic_vector(FIFO_BUS_SIZE-1 downto 0);
  end record;

  type adc_regs is record
    wr_adr1 : std_logic_vector(FIFO_BUS_SIZE-1 downto 0);
    wr_adr2 : std_logic_vector(FIFO_BUS_SIZE-1 downto 0);
    rd_adr  : std_logic_vector(FIFO_BUS_SIZE-1 downto 0);
  end record;

  type ram_type is array (2**FIFO_BUS_SIZE-1 downto 0) of std_logic_vector (32+CFG_GNSS_ADDR_WIDTH-1 downto 0);
  
  signal RAM : ram_type;
  signal br, brin : bus_regs;
  signal ar, arin : adc_regs;

begin

  comb : process(inNRst, br, ar, inWrAdr, inWrEna, inWrData)
    variable bv : bus_regs;
    variable av : adc_regs;
    variable nempty : std_ulogic;
  begin
    
    bv := br;
    av := ar;

    nempty := '1';
    if(ar.rd_adr = ar.wr_adr2) then nempty := '0'; end if;
  
    --
    -- TODO: Code Gray counter implementation:
    --
    if(inWrEna='1') then bv.wr_adr := br.wr_adr+1; end if;

    -- clock transition BusClk to AdcClk:
    av.wr_adr1 := br.wr_adr;
    av.wr_adr2 := ar.wr_adr1;
  
    -- reading address:
    if(nempty='1') then av.rd_adr := ar.rd_adr+1; end if;


    -- reset:
    if(inNRst='0') then
      av.rd_adr := (others => '0');
      av.wr_adr1 := (others => '0');
      av.wr_adr2 := (others => '0');
      bv.wr_adr := (others => '0');
    end if;

    outDataRdy <= nempty;
    outData <= RAM(conv_integer(ar.rd_adr))(31 downto 0);
    outAdr <= RAM(conv_integer(ar.rd_adr))(32+CFG_GNSS_ADDR_WIDTH-1 downto 32);

    brin <= bv;
    arin <= av;

  end process;


  -- registers:
  bregs : process(inBusClk) begin 
    if rising_edge(inBusClk) then 
      if(inWrEna='1') then
        RAM(conv_integer(br.wr_adr)) <= (inWrAdr(CFG_GNSS_ADDR_WIDTH-1 downto 0) & inWrData);
      end if;
      br <= brin; 
    end if;
    
  end process;

  aregs : process(inAdcClk)
  begin 
    if rising_edge(inAdcClk) then 
      ar <= arin;
    end if;
  end process;

end;




