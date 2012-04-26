------------------------------------------------------------------------------
--  INFORMATION:  http://www.GNSS-sensor.com
--  PROPERTY:     GNSS Sensor Ltd
--  E-MAIL:       alex.kosin@gnss-sensor.com
--  DESCRIPTION:  Dummy version of a single channel. It is implemented as 
--                simple accumulators with latch registers
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
entity DummyChannel is
  generic 
  (
    module_id   : integer range 0 to CFG_GNSS_CHANNELS_TOTAL-1 := 0
  );
  port
  (
    inNRst      : in std_ulogic;
    inAdcClk    : in std_ulogic;
    inI					    : in std_logic_vector(1 downto 0);
    inQ					    : in std_logic_vector(1 downto 0);
    inMsPulse		 : in std_ulogic;
    c2m         : in Ctrl2Module;
    m2c         : out Module2Ctrl
  );
end;


------------------------------------------------------------------------------
architecture rtl of DummyChannel is

  type registers is record
    dbgIncr   : std_logic_vector(31 downto 0);
    accI      : std_logic_vector(31 downto 0);
    accQ      : std_logic_vector(31 downto 0);
    latchI    : std_logic_vector(31 downto 0);
    latchQ    : std_logic_vector(31 downto 0);
    rdata     : std_logic_vector(63 downto 0);
    rdata_rdy : std_ulogic;
  end record;
  
  signal r, rin : registers;
begin

  comb : process(inNRst, r, inMsPulse, c2m)
    variable v : registers;
  begin
    
    v := r;
   
    -- write control registers:
    if((conv_integer(c2m.wr_module_sel)=module_id) and (c2m.wr_ena='1')) then
      case(conv_integer(c2m.wr_field_sel)) is
        when 0 =>
          v.dbgIncr := c2m.wr_data;
        when others =>
      end case;
    end if;

    -- read data:
    v.rdata := (others => '0');
    v.rdata_rdy := '0';
    if((conv_integer(c2m.rd_module_sel)=module_id) and (c2m.rd_ena='1')) then
      v.rdata_rdy := '1';
      case(conv_integer(c2m.rd_field_sel)) is
        when 0 =>  v.rdata := conv_std_logic_vector(0,32) & r.dbgIncr;
        when 1 =>  v.rdata := r.latchQ & r.latchI;
        when 2 =>  v.rdata := r.latchI & r.latchQ;
        when others =>
      end case;
    end if;

    -- msec counter:
    if(inMsPulse='1') then v.accI := r.dbgIncr;
    else                   v.accI := r.accI + inI; end if;

    if(inMsPulse='1') then v.accQ := not r.dbgIncr;
    else                   v.accQ := r.accQ - inQ; end if;

      
    if(inMsPulse='1') then 
      v.latchI := r.accI;
      v.latchQ := r.accQ;
    end if;
 
  
    -- Reset all:
    if(inNRst='0') then
      v.accI := (others => '0');
      v.accQ := (others => '0');
      v.latchI := (others => '0');
      v.latchI := (others => '0');
      v.rdata_rdy := '0';
    end if;
  
    rin <= v;
    
  end process;
  
  m2c.rdata_rdy <= r.rdata_rdy;
  m2c.rdata     <= r.rdata;


  -- registers:
  regs : process(inAdcClk)
  begin if rising_edge(inAdcClk) then r <= rin; end if; end process;

end;




