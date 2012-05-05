------------------------------------------------------------------------------
--  INFORMATION:  http://www.GNSS-sensor.com
--  PROPERTY:     GNSS Sensor Ltd
--  E-MAIL:       alex.kosin@gnss-sensor.com
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
use gnsslib.dummies.all;

------------------------------------------------------------------------------
entity gnsstop is
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
end;

architecture rtl of gnsstop is

  signal FifoOutDataRdy : std_ulogic;
  signal FifoOutAdr     : std_logic_vector(CFG_GNSS_ADDR_WIDTH-1 downto 0);
  signal FifoOutData    : std_logic_vector(31 downto 0);
 
  signal m2c              : m2c_tot;
  signal c2m              : Ctrl2Module;
  signal CtrlOutIrqPulse  : std_ulogic;
  signal CtrlOutMemWrEna  : std_ulogic;
  signal CtrlOutMemWrAdr  : std_logic_vector(CFG_GNSS_ADDR_WIDTH-4 downto 0);
  signal CtrlOutMemWrData : std_logic_vector(63 downto 0);

  signal GlbTmrOutMsReady : std_ulogic;
  
  signal word_sel : std_ulogic;
  signal rbIrqPulse : std_logic_vector(2 downto 0);
  signal rd_val : std_logic_vector(63 downto 0);

begin


  clFifo : wrfifo port map
  (
    inNRst,
    inBusClk,
    inAdcClk,
    inWrEna,
    inWrAdr,
    inWrData,
    FifoOutDataRdy,
    FifoOutAdr,
    FifoOutData
  );

  clGnssControl : GnssControl port map
  (
    inNRst,
    inAdcClk,
    FifoOutAdr,
    FifoOutDataRdy,
    FifoOutData,
    m2c,
    c2m,
    GlbTmrOutMsReady,
    CtrlOutIrqPulse,
    CtrlOutMemWrEna,
    CtrlOutMemWrAdr,
    CtrlOutMemWrData
  );

  clGlobalTimer : GlobalTimer port map
  (
    inNRst,
    inAdcClk,
    c2m,
    m2c(MODULE_ID_GLB_TIMER),
    GlbTmrOutMsReady
  );

  -- output reading interface via dual-port RAM
  clSnapRam : dp_snapram port map
  (
    inAdcClk,
    CtrlOutMemWrEna,
    CtrlOutMemWrAdr,
    CtrlOutMemWrData,
    inBusClk,
    inRdEna,
    inRdAdr(CFG_GNSS_ADDR_WIDTH-1 downto 3),
    rd_val
  );


  -- GNSS channels:
  clChannels : ChannelsPack port map
  (
    inNRst,
    inAdcClk,
    inGpsI,
    inGpsQ,
    inGloI,
    inGloQ,
    GlbTmrOutMsReady,
    c2m,
    m2c(CFG_GNSS_CHANNELS_TOTAL-1 downto 0)
  );


  outIrqPulse <= rbIrqPulse(2) and (not rbIrqPulse(1));
  
  -- Reading Data multiplexer
  mux : process(inRdAdr(2), word_sel,rd_val) 
    variable rdata : std_logic_vector(31 downto 0);
  begin
    if(word_sel='0') then rdata := rd_val(31 downto 0);
    else                  rdata := rd_val(63 downto 32); end if;
    outRdData <= rdata;
  end process;
  

  -- ADC to AMBA clock transition for interrupt signal:
  regs : process(inBusClk) begin 
    if rising_edge(inBusClk) then 
      rbIrqPulse <= rbIrqPulse(1 downto 0) & CtrlOutIrqPulse;
      word_sel <= inRdAdr(2);
    end if;
  end process;


end;




