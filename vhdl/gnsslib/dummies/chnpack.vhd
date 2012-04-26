------------------------------------------------------------------------------
--  INFORMATION:  http://www.GNSS-sensor.com
--  PROPERTY:     GNSS Sensor Ltd
--  E-MAIL:       alex.kosin@gnss-sensor.com
--  DESCRIPTION:  Pack of Single channels.
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
entity ChannelsPack is
  port
  (
    inNRst      : in std_ulogic;
    inAdcClk    : in std_ulogic;
    inGpsI			   : in std_logic_vector(1 downto 0);
    inGpsQ			   : in std_logic_vector(1 downto 0);
    inGloI			   : in std_logic_vector(1 downto 0);
    inGloQ			   : in std_logic_vector(1 downto 0);
    inMsPulse		 : in std_ulogic;
    c2m         : in Ctrl2Module;
    m2c         : out m2c_chn
  );
end;

architecture rtl of ChannelsPack is

begin

  -- GPS L1-CA pack:
  gpscal1: for i in 0 to CFG_GNSS_GPS_L1CA_NUM-1 generate
    ch_gps : DummyChannel generic map (i)
    port map (inNRst, inAdcClk, inGpsI, inGpsQ, inMsPulse, c2m, m2c(i));
  end generate gpscal1;

  -- SBAS L1 pack:
  sbasl1: for i in CFG_GNSS_GPS_L1CA_NUM 
                to (CFG_GNSS_GPS_L1CA_NUM+CFG_GNSS_SBAS_L1_NUM-1) generate
    ch_sbas : DummyChannel generic map (i)
    port map (inNRst, inAdcClk, inGpsI, inGpsQ, inMsPulse, c2m, m2c(i));
  end generate sbasl1;

  -- Galileo E1 pack:
  gale1: for i in (CFG_GNSS_GPS_L1CA_NUM+CFG_GNSS_SBAS_L1_NUM) 
                to (CFG_GNSS_GPS_L1CA_NUM+CFG_GNSS_SBAS_L1_NUM+CFG_GNSS_GALILEO_E1_NUM-1) generate
    ch_gal : DummyChannel generic map (i)
    port map (inNRst, inAdcClk, inGpsI, inGpsQ, inMsPulse, c2m, m2c(i));
  end generate gale1;

  -- GLONASS L1-CA pack:
  glncal1: for i in (CFG_GNSS_GPS_L1CA_NUM+CFG_GNSS_SBAS_L1_NUM+CFG_GNSS_GALILEO_E1_NUM) 
                to (CFG_GNSS_GPS_L1CA_NUM+CFG_GNSS_SBAS_L1_NUM+CFG_GNSS_GALILEO_E1_NUM+CFG_GNSS_GLONASS_L1_NUM-1) generate
    ch_gln : DummyChannel generic map (i)
    port map (inNRst, inAdcClk, inGloI, inGloQ, inMsPulse, c2m, m2c(i));
  end generate glncal1;


end;




