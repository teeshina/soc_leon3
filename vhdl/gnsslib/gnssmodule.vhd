------------------------------------------------------------------------------
--  INFORMATION:  http://www.GNSS-sensor.com
--  PROPERTY:     GNSS Sensor Ltd
--  E-MAIL:       alex.kosin@gnss-sensor.com
--  DESCRIPTION:  This file contains GNSS DSP module description
------------------------------------------------------------------------------
--  WARNING:      
------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library grlib;
use grlib.amba.all;
use grlib.stdlib.all;
library techmap;
use techmap.gencomp.all;

package gnssmodule is

------------------------------------------------------------------------------
-- RF front-end controller module

  component rfctrl is
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
  end component; 

------------------------------------------------------------------------------
-- 3-axis STMicroelectronics Gyroscope SPI controller (4-wires mode)

  component gyrospi is
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
  end component; 

------------------------------------------------------------------------------
-- 3-axis STMicroelectronics Accelerometer SPI controller (4-wires mode)

  component accelspi is
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
  end component; 


  ------------------------------------------------------------------------------
  -- GNSS Engine, top level


  constant CFG_GNSS_GPS_L1CA_NUM     : integer := 12;
  constant CFG_GNSS_SBAS_L1_NUM      : integer := 2; 
  constant CFG_GNSS_GALILEO_E1_NUM   : integer := 6;
  constant CFG_GNSS_GLONASS_L1_NUM   : integer := 12;
  constant CFG_GNSS_CHANNELS_TOTAL   : integer := (CFG_GNSS_GPS_L1CA_NUM+CFG_GNSS_SBAS_L1_NUM+CFG_GNSS_GALILEO_E1_NUM+CFG_GNSS_GLONASS_L1_NUM);

  constant CFG_GNSS_TIMERS_TOTAL     : integer := 1;
  constant MODULE_ID_GLB_TIMER       : integer := 0+CFG_GNSS_CHANNELS_TOTAL;

  constant CFG_GNSS_MODULES_TOTAL    : integer := CFG_GNSS_CHANNELS_TOTAL+CFG_GNSS_TIMERS_TOTAL;
  constant CFG_GNSS_DWORD_PER_MODULE : integer := 8;
  constant CFG_GNSS_MEMORY_SIZE64    : integer := CFG_GNSS_DWORD_PER_MODULE*CFG_GNSS_MODULES_TOTAL;
  constant CFG_GNSS_ADDR_WIDTH       : integer := log2x(CFG_GNSS_MEMORY_SIZE64/8)+6;
  

  
  component gnssengine is
  generic
  (
    hindex : integer := 0;
    haddr  : integer := 0;
    hmask  : integer := 16#FFF#;
    irqind : integer := 0
  );
  port
  (
    rst      : in  std_ulogic;
    clk      : in  std_ulogic;
    ahbsi    : in  ahb_slv_in_type;
    ahbso    : out ahb_slv_out_type;
    -- Inputs from RF
    inAdcClk : in  std_ulogic;
    inGpsI   : in  std_logic_vector(1 downto 0);
    inGpsQ   : in  std_logic_vector(1 downto 0);
    inGloI   : in  std_logic_vector(1 downto 0);
    inGloQ   : in  std_logic_vector(1 downto 0)
  );
  end component;

end;
