------------------------------------------------------------------------------
--  INFORMATION:  http://www.GNSS-sensor.com
--  PROPERTY:     GNSS Sensor Ltd
--
--  E-MAIL:       gnss.sensor@gmail.com
--
--  DESCRIPTION:  DUMMY version of the GNSS top level: interrupt generator
--                and simple accumulators.
------------------------------------------------------------------------------
--  WARNING:      
------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library gnsslib;
use gnsslib.gnssmodule.all;

------------------------------------------------------------------------------
entity gnsstop is
  port
  (
    rst      : in  std_ulogic;
    inBusClk : in  std_ulogic;
    inRdAdr  : in std_logic_vector(CFG_GNSS_ADDR_WIDTH-1 downto 0);
    inRdEna  : in std_ulogic;
    outRdData : out std_logic_vector(31 downto 0);
    inWrAdr  : in std_logic_vector(CFG_GNSS_ADDR_WIDTH-1 downto 0);
    inWrEna  : in std_ulogic;
    inWrData : in std_logic_vector(31 downto 0);
    outIrqPulse : out std_ulogic;
    -- Inputs from RF
    inAdcClk : in  std_ulogic;
    inGpsI   : in  std_logic_vector(1 downto 0);
    inGpsQ   : in  std_logic_vector(1 downto 0);
    inGloI   : in  std_logic_vector(1 downto 0);
    inGloQ   : in  std_logic_vector(1 downto 0)
  );
end;


------------------------------------------------------------------------------
architecture rtl of gnsstop is

  signal wbGlbTimerOutData : std_logic_vector(63 downto 0);

  signal wbChnOutData : std_logic_vector(63 downto 0);

  signal wbSelData : std_logic_vector(63 downto 0);
 
begin

  wbSelData <= wbChnOutData or wbGlbTimerOutData;


  outIrqPulse <= '0';
  outRdData <= (others => '0');


end;




