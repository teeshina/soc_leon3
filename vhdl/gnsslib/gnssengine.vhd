------------------------------------------------------------------------------
--  INFORMATION:  http://www.GNSS-sensor.com
--  PROPERTY:     GNSS Sensor Ltd
--  E-MAIL:       alex.kosin@gnss-sensor.com
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
use gnsslib.dummies.all;  -- This is an open source implementation
--use gnsslib.products.all; -- This is an GNSS Sensor Ltd proprietary implementation

------------------------------------------------------------------------------
entity gnssengine is
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
end;


------------------------------------------------------------------------------
architecture rtl of gnssengine is

  constant VENDOR_GNSSSENSOR   : integer := 16#F1#;
  constant GNSSSENSOR_ENGINE   : integer := 16#078#;
  constant GNSS_ENGINE_VERSION : integer := 1;
  
  constant hconfig : ahb_config_type := 
  (
    0 => ahb_device_reg ( VENDOR_GNSSSENSOR, GNSSSENSOR_ENGINE, 0, GNSS_ENGINE_VERSION, irqind),
    4 => ahb_membar(haddr, '0', '0', hmask),
    others => zero32
  );
  
  signal wbAdr           : std_logic_vector(CFG_GNSS_ADDR_WIDTH-1 downto 0);
  signal wRdEna          : std_ulogic;
  signal wWrEna          : std_ulogic;
  signal wGnssOutIrqPulse: std_ulogic;
  signal wbGnssOutRdData : std_logic_vector (AHBDW-1 downto 0);
 
begin

  wRdEna <= ahbsi.hready and (not ahbsi.hwrite) and ahbsi.hsel(hindex) and ahbsi.htrans(1);
  wWrEna <= ahbsi.hready and      ahbsi.hwrite  and ahbsi.hsel(hindex) and ahbsi.htrans(1);
  wbAdr  <= ahbsi.haddr(CFG_GNSS_ADDR_WIDTH-1 downto 0);

  comb : process(rst, wGnssOutIrqPulse)
    variable hirq : std_logic_vector(NAHBIRQ-1 downto 0);
  begin
    hirq := (others => '0');
    hirq(irqind) := wGnssOutIrqPulse;
    ahbso.hirq    <= hirq;
  end process;

  clGnssTop : gnsstop port map
  (
    rst,
    clk,
    wbAdr,
    wRdEna,
    wbGnssOutRdData,
    wbAdr,
    wWrEna,
    ahbsi.hwdata,
    wGnssOutIrqPulse,
    -- Inputs from RF
    inAdcClk,
    inGpsI,
    inGpsQ,
    inGloI,
    inGloQ
  );


  ahbso.hconfig <= hconfig;
  ahbso.hrdata  <= wbGnssOutRdData;
  ahbso.hready  <= '1';
  ahbso.hresp   <= HRESP_OKAY; 
  ahbso.hsplit  <= (others => '0'); 
  ahbso.hcache  <= '1';
  ahbso.hindex  <= hindex;

end;




