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
use grlib.stdlib.all;

library gnsslib;
use gnsslib.gnssmodule.all;

package dummies is

  ------------------------------------------------------------------------------
  -- GNNS module Internal data interface
  type Ctrl2Module is record
    rd_ena         : std_ulogic;
    rd_module_sel  : std_logic_vector(CFG_GNSS_ADDR_WIDTH-log2x(CFG_GNSS_DWORD_PER_MODULE)-4 downto 0);
    rd_field_sel   : std_logic_vector(2 downto 0);
    wr_ena         : std_ulogic;
    wr_module_sel  : std_logic_vector(CFG_GNSS_ADDR_WIDTH-log2x(CFG_GNSS_DWORD_PER_MODULE)-4 downto 0);
    wr_field_sel   : std_logic_vector(3 downto 0);
    wr_data        : std_logic_vector(31 downto 0);
  end record;

  type Module2Ctrl is record
    rdata     : std_logic_vector(63 downto 0);
    rdata_rdy : std_ulogic;
  end record;
  
  type m2c_vector_type is array (natural range <>) of Module2Ctrl;
  subtype m2c_tot is m2c_vector_type(CFG_GNSS_MODULES_TOTAL-1 downto 0);
  subtype m2c_chn is m2c_vector_type(CFG_GNSS_CHANNELS_TOTAL-1 downto 0);
  

  ------------------------------------------------------------------------------
  -- Navigation module top level implementation
  component gnsstop is
  port
  (
    inNRst   : in  std_ulogic;
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
  end component;


  ------------------------------------------------------------------------------
  component wrfifo is
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
  end component;
  

  ------------------------------------------------------------------------------
  component GnssControl is
  port
  (
    inNRst   : in std_ulogic;
    inAdcClk : in std_ulogic;
    inWrAdr  : in std_logic_vector(CFG_GNSS_ADDR_WIDTH-1 downto 0);
    inWrEna  : in std_ulogic;
    inWrData : in std_logic_vector(31 downto 0);
    -- internal interface with channels
    m2c      : in m2c_tot;
    c2m      : out Ctrl2Module;
    -- snapper run:
    inMsReady    : in std_ulogic;
    outIrqPulse  : out std_ulogic;
    -- DP memory interface
    outMemWrEna  : out std_ulogic;
    outMemWrAdr  : out std_logic_vector(CFG_GNSS_ADDR_WIDTH-4 downto 0); -- 64-bits words
    outMemWrData : out std_logic_vector(63 downto 0)
  );
  end component;


  ------------------------------------------------------------------------------
  component GlobalTimer is
  port
  (
    inNRst      : in std_ulogic;
    inAdcClk    : in std_ulogic;
    c2m         : in Ctrl2Module;
    m2c         : out Module2Ctrl;
    outMsPulse  : out std_ulogic
  );
  end component;

  ------------------------------------------------------------------------------
  component dp_snapram is
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
  end component;

  ------------------------------------------------------------------------------
  component DummyChannel is generic 
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
  end component;

  ------------------------------------------------------------------------------
  component ChannelsPack is
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
  end component;



end;
