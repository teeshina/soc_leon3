------------------------------------------------------------------------------
--  INFORMATION:  http://www.GNSS-sensor.com
--  PROPERTY:     GNSS Sensor Ltd
--
--  E-MAIL:       gnss.sensor@gmail.com
--
--  DESCRIPTION:  This file contains GNSS Engine top level description
------------------------------------------------------------------------------
--  WARNING:      
------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library grlib;
use grlib.amba.all;
use grlib.stdlib.all;

library gaisler;
use gaisler.misc.all;

library techmap;
use techmap.gencomp.all;

library gnsslib;
use gnsslib.gnssmodule.all;

------------------------------------------------------------------------------
entity gnssengine is
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
end;


------------------------------------------------------------------------------
architecture rtl of gnssengine is

  constant VENDOR_GNSSSENSOR   : integer := 1;
  constant GNSSSENSOR_ENGINE   : integer := 1;
  constant GNSS_ENGINE_VERSION : integer := 1;
  constant IRQ_GNSS_ENGINE     : integer := 1;
  constant CFGAREA_TYPE_AHB_MEM: integer := 1;
  
  constant hconfig : ahb_config_type := 
  (
    0 => ahb_device_reg ( VENDOR_GNSSSENSOR, GNSSSENSOR_ENGINE, GNSS_ENGINE_VERSION, 0, IRQ_GNSS_ENGINE),
    4 => ahb_membar(haddr, '1', '1', hmask),
    others => zero32
  );
  
  constant CFG_GNSS_CHANNELS_TOTAL :  integer := 32;
  constant CFG_GNSS_MEMORY_SIZE64  :  integer := 8*CFG_GNSS_CHANNELS_TOTAL+8;
  constant CFG_GNSS_ADDR_WIDTH     :  integer := log2x(CFG_GNSS_MEMORY_SIZE64/8)+6;
  constant CFG_GNSS_ADDR_WIDTH_DEC :  integer := CFG_GNSS_ADDR_WIDTH - 1;
  
begin

  update : process(rst, clk)
    variable wbAdr           : std_logic_vector(CFG_GNSS_ADDR_WIDTH_DEC downto 0);
    variable wRdEna          : std_ulogic ;
    variable wWrEna          : std_ulogic ;
    variable wGnssOutIrqPulse: std_ulogic ;
    variable wbGnssOutRdData : std_ulogic_vector (31 downto 0);
  begin

    wRdEna := ahbsi.hready and (not ahbsi.hwrite) and ahbsi.hsel(hindex) and ahbsi.htrans(1);
    wWrEna := ahbsi.hready and      ahbsi.hwrite  and ahbsi.hsel(hindex) and ahbsi.htrans(1);
    wbAdr  := ahbsi.haddr (CFG_GNSS_ADDR_WIDTH_DEC downto 0);

  end process;
  ahbso.hconfig <= hconfig;
end;




