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

  clSRAM : syncrambw generic map 
  (
    CFG_MEMTECH,
    CFG_SRAM_ADRBITS,
    AHBDW
  )port map
  (
    wClkBus,
    ramaddr,
    hwdata,
    ramdata,
    ramsel,
    write
  ); 


end Behavioral;

