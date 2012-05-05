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
entity GlobalTimer is
  port
  (
    inNRst      : in std_ulogic;
    inAdcClk    : in std_ulogic;
    c2m         : in Ctrl2Module;
    m2c         : out Module2Ctrl;
    outMsPulse  : out std_ulogic
  );
end;

architecture rtl of GlobalTimer is

  type registers is record
    MsLength : std_logic_vector(31 downto 0);
    SubMsCnt : integer;
    MsCnt    : integer;
    rdata    : std_logic_vector(63 downto 0);
    rdata_rdy : std_ulogic;
  end record;
  
  signal r, rin : registers;
begin

  comb : process(inNRst, r, c2m)
    variable v : registers;
    variable MsPulse : std_ulogic;
  begin
    
    v := r;
   
    -- write control registers:
    if((conv_integer(c2m.wr_module_sel)=MODULE_ID_GLB_TIMER) and (c2m.wr_ena='1')) then
      case(conv_integer(c2m.wr_field_sel)) is
        when 0 =>  v.MsLength := c2m.wr_data;
        when others =>
      end case;
    end if;

    -- read data:
    v.rdata := (others => '0');
    v.rdata_rdy := '0';
    if((conv_integer(c2m.rd_module_sel)=MODULE_ID_GLB_TIMER) and (c2m.rd_ena='1')) then
      v.rdata_rdy := '1';
      case(conv_integer(c2m.rd_field_sel)) is
        when 0 =>
          v.rdata := conv_std_logic_vector(r.MsCnt,32) & r.MsLength;
        when others =>
      end case;
    end if;

    -- msec counter:
    MsPulse := '0';
    if((r.SubMsCnt+1)=conv_integer(r.MsLength)) then MsPulse := '1'; end if;
  
    if(MsPulse='1') then v.SubMsCnt := 0;
    elsif(v.MsLength/=zero32) then v.SubMsCnt := r.SubMsCnt+1; end if;
  
    if(MsPulse='1') then v.MsCnt := r.MsCnt+1; end if;

  
    -- Reset all:
    if(inNRst='0') then
      v.MsLength := (others => '0');
      v.SubMsCnt := 0;
      v.MsCnt    := 0;
    end if;
  
    outMsPulse <= MsPulse;

    rin <= v;
    
  end process;
  
  m2c.rdata_rdy <= r.rdata_rdy;
  m2c.rdata     <= r.rdata;


  -- registers:
  regs : process(inAdcClk)
  begin if rising_edge(inAdcClk) then r <= rin; end if; end process;

end;




