------------------------------------------------------------------------------
--  INFORMATION:  http://www.GNSS-sensor.com
--  PROPERTY:     GNSS Sensor Ltd
--  E-MAIL:       alex.kosin@gnss-sensor.com
--  DESCRIPTION:  Global timer dummy version.
------------------------------------------------------------------------------
--  WARNING:      
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
entity dp_snapram is
  port
  (
    clk_wr      : in std_ulogic;
    we          : in std_ulogic;
    adr_wr      : in std_logic_vector(CFG_GNSS_ADDR_WIDTH-4 downto 0);
    idata       : in std_logic_vector(63 downto 0);
    clk_rd      : in std_ulogic;
    re          : in std_ulogic;
    adr_rd      : in std_logic_vector(CFG_GNSS_ADDR_WIDTH-4 downto 0);
    odata       : out std_logic_vector(63 downto 0)
  );
end;

architecture rtl of dp_snapram is

  type ram_type is array ((2**(CFG_GNSS_ADDR_WIDTH-3))-1 downto 0) of std_logic_vector (63 downto 0);
  signal RAM : ram_type;

begin

  -- registers:
  wrregs : process(clk_wr) begin 
    if rising_edge(clk_wr) then 
      if(we='1') then
        RAM(conv_integer(adr_wr)) <= idata; 
      end if;
    end if;
  end process;

  rdregs : process(clk_rd) begin 
    if rising_edge(clk_rd) then 
      if(re='1') then
        odata <= RAM(conv_integer(adr_rd)); 
      end if;
    end if;
  end process;

end;




