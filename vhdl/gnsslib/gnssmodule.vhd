------------------------------------------------------------------------------
--  INFORMATION:  http://www.GNSS-sensor.com
--  PROPERTY:     GNSS Sensor Ltd
--
--  E-MAIL:       gnss.sensor@gmail.com
--
--  DESCRIPTION:  This file contains GNSS DSP module description
------------------------------------------------------------------------------
--  WARNING:      
------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library grlib;
use grlib.amba.all;
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

  component gnssengine is
  generic
  (
    hindex : integer := 0;
    haddr  : integer := 0;
    hmask  : integer := 16#FFF#  
  );
  port
  (
    rst      : in  std_ulogic;
    clk      : in  std_ulogic;
    ahbsi    : in  ahb_slv_in_type;
    ahbso    : out ahb_slv_out_type;
    -- Inputs from RF
    inAdcClk : in  std_ulogic;
    inGpsI   : in  std_logic_vector(9 downto 0);
    inGpsQ   : in  std_logic_vector(9 downto 0);
    inGloI   : in  std_logic_vector(9 downto 0);
    inGloQ   : in  std_logic_vector(9 downto 0)
  );
  end component;


end;
