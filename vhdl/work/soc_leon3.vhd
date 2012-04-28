----------------------------------------------------------------------------------
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library grlib;
use grlib.amba.all;
use grlib.stdlib.all;
use grlib.devices.all;
library techmap;
use techmap.gencomp.all;
library gaisler;
use gaisler.misc.all;
use gaisler.jtag.all;
use gaisler.leon3.all;
use gaisler.uart.all;

library gnsslib;
use gnsslib.gnsspll.all;

library work;
use work.all;
use work.config.all;

entity soc_leon3 is Port 
( 
  inRst       : in std_logic; -- button "Center"
  inDsuBreak  : in std_logic; -- button "North
  -- Differential system clock (ML605=200 MHz)
  inSysClk_p  : in std_logic;
  inSysClk_n  : in std_logic;
  -- UART
  inCTS   : in std_logic;
  inRX    : in std_logic;
  outRTS  : out std_logic;
  outTX   : out std_logic;
  -- JTAG:
  nTRST : in std_logic; -- in: Test Reset
  TCK   : in std_logic;   -- in: Test Clock
  TMS   : in std_logic;   -- in: Test Mode State
  TDI   : in std_logic;   -- in: Test Data Input
  TDO   : out std_logic;   -- out: Test Data Output
  -- GNSS RF inputs:
  inAdcClk : in std_logic;
  inIa     : in std_logic_vector(1 downto 0);
  inQa     : in std_logic_vector(1 downto 0);
  inIb     : in std_logic_vector(1 downto 0);
  inQb     : in std_logic_vector(1 downto 0);
  -- MAX2769 SPIs and antenna controls signals:
  inLD     : in std_logic_vector(1 downto 0);
  outSCLK  : out std_ulogic;
  outSDATA : out std_ulogic;
  outCSn   : out std_logic_vector(1 downto 0);
  inExtAntStat   : in std_ulogic;
  inExtAntDetect : in std_ulogic;
  outExtAntEna   : out std_ulogic;
  -- Gyroscope SPI interface
  inGyroSDI   : in std_logic;
  inGyroInt1  : in std_logic; -- interrupt 1
  inGyroInt2  : in std_logic; -- interrupt 2
  outGyroSDO  : out std_logic;
  outGyroCSn  : out std_logic;
  outGyroSPC  : out std_logic;
  -- Accelerometer SPI interface
  inAccelerSDI   : in std_logic;
  inAccelerInt1  : in std_logic; -- interrupt 1
  inAccelerInt2  : in std_logic; -- interrupt 2
  outAccelerSDO  : out std_logic;
  outAccelerCSn  : out std_logic;
  outAccelerSPC  : out std_logic;
  -- User pins
  inDIP   : in std_ulogic_vector(7 downto 0);
  outLED  : out std_ulogic_vector(7 downto 0)
);
end soc_leon3;



architecture Behavioral of soc_leon3 is
  component soc_noram 
  port 
  ( 
    inRst       : in std_logic; -- button "Center"
    inDsuBreak  : in std_logic; -- button "North 
    inSysClk_p  : in std_logic;
    inSysClk_n  : in std_logic;
    -- UART
    inCTS   : in std_logic;
    inRX    : in std_logic;
    outRTS  : out std_logic;
    outTX   : out std_logic;
    -- JTAG:
    nTRST : in std_logic; -- in: Test Reset
    TCK   : in std_logic;   -- in: Test Clock
    TMS   : in std_logic;   -- in: Test Mode State
    TDI   : in std_logic;   -- in: Test Data Input
    TDO   : out std_logic;   -- out: Test Data Output
    -- GNSS RF inputs:
    inAdcClk : in std_logic;
    inIa     : in std_logic_vector(1 downto 0);
    inQa     : in std_logic_vector(1 downto 0);
    inIb     : in std_logic_vector(1 downto 0);
    inQb     : in std_logic_vector(1 downto 0);
    -- MAX2769 SPIs and antenna controls signals:
    inLD     : in std_logic_vector(1 downto 0);
    outSCLK  : out std_ulogic;
    outSDATA : out std_ulogic;
    outCSn   : out std_logic_vector(1 downto 0);
    inExtAntStat   : in std_ulogic;
    inExtAntDetect : in std_ulogic;
    outExtAntEna   : out std_ulogic;
    -- Gyroscope SPI interface
    inGyroSDI   : in std_logic;
    inGyroInt1  : in std_logic; -- interrupt 1
    inGyroInt2  : in std_logic; -- interrupt 2
    outGyroSDO  : out std_logic;
    outGyroCSn  : out std_logic;
    outGyroSPC  : out std_logic;
    -- Accelerometer SPI interface
    inAccelerSDI   : in std_logic;
    inAccelerInt1  : in std_logic; -- interrupt 1
    inAccelerInt2  : in std_logic; -- interrupt 2
    outAccelerSDO  : out std_logic;
    outAccelerCSn  : out std_logic;
    outAccelerSPC  : out std_logic;
    -- User pins
    inDIP   : in std_ulogic_vector(7 downto 0);
    outLED  : out std_ulogic_vector(7 downto 0);
    -- SRAM signals
    outClkBus : out std_logic;
    ramaddr : out std_logic_vector(CFG_SRAM_ADRBITS-1 downto 0);
    hwdata  : out std_logic_vector(AHBDW-1 downto 0);
    ramdata : in std_logic_vector(AHBDW-1 downto 0);
    ramsel  : out std_logic_vector(AHBDW/8-1 downto 0);
    write   : out std_logic_vector(AHBDW/8-1 downto 0)
  );
  end component;

  signal wClkBus : std_ulogic;
  signal ramaddr : std_logic_vector(CFG_SRAM_ADRBITS-1 downto 0);
  signal hwdata  : std_logic_vector(AHBDW-1 downto 0);
  signal ramdata : std_logic_vector(AHBDW-1 downto 0);
  signal ramsel  : std_logic_vector(AHBDW/8-1 downto 0);
  signal write   : std_logic_vector(AHBDW/8-1 downto 0);


begin

  clSocNoRAM : soc_noram port map
  ( 
    inRst,
    inDsuBreak,
    inSysClk_p,
    inSysClk_n,
    -- UART
    inCTS,
    inRX,
    outRTS,
    outTX,
    -- JTAG:
    nTRST,
    TCK,
    TMS,
    TDI,
    TDO,
    -- ADC samples:
    inAdcClk,
    inIa,
    inQa,
    inIb,
    inQb,
    -- RF front-end control:
    inLD,
    outSCLK,
    outSDATA,
    outCSn,
    inExtAntStat,
    inExtAntDetect,
    outExtAntEna,
    -- Gyroscope
    inGyroSDI,
    inGyroInt1,
    inGyroInt2,
    outGyroSDO,
    outGyroCSn,
    outGyroSPC,
    -- Accelerometer
    inAccelerSDI,
    inAccelerInt1,
    inAccelerInt2,
    outAccelerSDO,
    outAccelerCSn,
    outAccelerSPC,
    -- User pins
    inDIP,
    outLED,
    -- SRAM signals
    wClkBus,
    ramaddr,
    hwdata,
    ramdata,
    ramsel,
    write
  );

  -- SRAM.
  clSRAM : syncrambw generic map 
  (
    CFG_MEMTECH,
    CFG_SRAM_ADRBITS,
    AHBDW
  )port map
  ( wClkBus, ramaddr, hwdata, ramdata, ramsel, write );

end Behavioral;

