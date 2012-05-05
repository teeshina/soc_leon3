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
use gnsslib.gnssmodule.all;

library work;
use work.all;
use work.config.all;

entity soc_noram is Port 
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
end soc_noram;

architecture rtl of soc_noram is
  component tap_rstn
  port (
    inSysReset  : in std_ulogic;
    inSysClk    : in std_ulogic;
    inPllLock   : in std_ulogic;
    outNRst     : out std_ulogic );
  end component;
  component SysPLL
  port (
    CLK_IN1_P	: in std_logic;
    CLK_IN1_N	: in std_logic;
    CLK_OUT1	: out std_logic;
    RESET		: in std_logic;
    LOCKED	: out std_logic );
  end component;



  signal wNRst : std_ulogic;
  signal wClkBus : std_ulogic;
  signal wPllLocked : std_ulogic;
  signal wSysReset : std_ulogic;
  
  signal msti : ahb_mst_in_type;
  signal msto : ahb_mst_out_vector := (others => ahbm_none);
  signal slvi : ahb_slv_in_type;
  signal slvo : ahb_slv_out_vector := (others => ahbs_none);
  signal apbi : apb_slv_in_type;
  signal apbo : apb_slv_out_vector := (others => apb_none);

  signal irqi : irq_in_vector(0 to CFG_NCPU-1);
  signal irqo : irq_out_vector(0 to CFG_NCPU-1);

  signal dbgi : l3_debug_in_vector(0 to CFG_NCPU-1);
  signal dbgo : l3_debug_out_vector(0 to CFG_NCPU-1);
  
  signal dsui : dsu_in_type;
  signal dsuo : dsu_out_type; 

  signal uart1i : uart_in_type;
  signal uart1o : uart_out_type;

  signal gpti : gptimer_in_type;

begin

  ------------------------------------
  -- PLL: 
  clPll : SysPLL_all generic map
  (
    tech => CFG_FABTECH
  )port map 
  (
    CLK_IN1_P	=> inSysClk_p,
    CLK_IN1_N	=> inSysClk_n,
    CLK_OUT1	=> wClkBus,
    RESET		=> inRst,
    LOCKED	=> wPllLocked
  );

  ------------------------------------
  -- System Reset: 
  wSysReset <= inRst or not wPllLocked;
  clTapRst : tap_rstn port map
  (
    inSysReset  => wSysReset,
    inSysClk    => wClkBus,
    inPllLock   => wPllLocked,
    outNRst     => wNRst
  );

  ------------------------------------
  -- AHB Controller: 
  clAhbCtrl : ahbctrl  generic map
  (
    defmast => CFG_AHBCTRL_DEFMST,
    split   => CFG_AHBCTRL_SPLIT, 
    rrobin  => CFG_AHBCTRL_RROBIN,
    ioaddr  => CFG_AHBCTRL_AHBIO,
    devid   => XILINX_ML605,
    ioen    => CFG_AHBCTRL_IOEN,
    nahbm   => AHB_MASTER_TOTAL,
    nahbs   => AHB_SLAVE_TOTAL
  )port map 
  (
    wNRst,
    wClkBus,
    msti,
    msto,
    slvi,
    slvo
  );
  
  ------------------------------------
  -- JTAG module:
  clAhbJTAG : ahbjtag generic map
  (
    tech   => CFG_FABTECH,
    hindex => AHB_MASTER_JTAG
  )port map
  (
    wNRst,
    wClkBus,
    TCK, TMS, TDI, TDO,--tck, tms, tdi, tdo,
    msti, msto(AHB_MASTER_JTAG),
    open, open, open, open, open, open, open, '0', nTRST, open
  );


  ------------------------------------
  -- LEON3 processor
  cpu : for i in 0 to CFG_NCPU-1 generate
    clLeon3s : leon3s generic map (
		AHB_MASTER_LEON3+i,
		CFG_FABTECH,
		CFG_MEMTECH,
		CFG_NWIN,
		CFG_DSU,
		CFG_FPU,
		CFG_V8,
		0,
		CFG_MAC,
		CFG_PCLOW,
		CFG_NOTAG,
		CFG_NWP,
		CFG_ICEN,
		CFG_IREPL,
		CFG_ISETS,
		CFG_ILINE,
		CFG_ISETSZ,
		CFG_ILOCK,
		CFG_DCEN,
		CFG_DREPL,
		CFG_DSETS,
		CFG_DLINE,
		CFG_DSETSZ,
		CFG_DLOCK,
		CFG_DSNOOP,
		CFG_ILRAMEN,
		CFG_ILRAMSZ,
		CFG_ILRAMADDR,
		CFG_DLRAMEN,
		CFG_DLRAMSZ,
		CFG_DLRAMADDR,
		CFG_MMUEN,
		CFG_ITLBNUM,
		CFG_DTLBNUM,
		CFG_TLB_TYPE,
		CFG_TLB_REP,
		CFG_LDDEL,
		CFG_DISAS,
		CFG_ITBSZ,
		CFG_PWD,
		CFG_SVT,
		CFG_RSTADDR,
		CFG_NCPU-1,
		CFG_DFIXED,
		CFG_SCAN,
		CFG_MMU_PAGE
	)port map (
		wClkBus,
		wNRst,
		msti,
		msto(AHB_MASTER_LEON3+i),
		slvi,
		slvo,
		irqi(i),
		irqo(i),
		dbgi(i),
		dbgo(i)
	);
  end generate;

  ------------------------------------
  -- Debug Support Unit: 
  clDsu3 : dsu3 generic map (
    hindex => AHB_SLAVE_DSU,
    haddr  => 16#900#,
	  hmask  => 16#F00#,
    ncpu   => CFG_NCPU,
	  tbits  => 30,
	  tech   => CFG_MEMTECH,
	  irq    => IRQ_DSU,
	  kbytes => CFG_ATBSZ
  )port map (
    wNRst,
	  wClkBus,
	  msti,
	  slvi,
	  slvo(AHB_SLAVE_DSU),
	  dbgo,
	  dbgi,
	  dsui,
	  dsuo
 );
  dsui.enable <= '1';
  dsui.break  <= inDsuBreak;


  ------------------------------------
  -- AHB/APB bridge:
  clApbCtrl : apbctrl generic map
  (
    hindex  => AHB_SLAVE_APBBRIDGE,
    haddr   => 16#800#,--CFG_APBADDR,
    nslaves => 16
  )
  port map 
  (
    wNRst,
    wClkBus,
    slvi,
    slvo(AHB_SLAVE_APBBRIDGE),
    apbi,
    apbo
  );

  ------------------------------------
  -- UART 1
  uart1i.extclk <= '0';
  uart1i.ctsn   <= inCTS;
  uart1i.rxd    <= inRX;

  clApbUART1 : apbuart generic map
  (
    pindex    => APB_UART_1,
    paddr => 16#001#,
    pmask => 16#FFF#,
    pirq      => IRQ_UART_1,
    console   => CFG_DUART,
    fifosize  => CFG_UART1_FIFO
  )port map
  (
    wNRst, 
    wClkBus, 
    apbi, 
    apbo(APB_UART_1), 
    uart1i, 
    uart1o
  );
  outTX  <= uart1o.txd;
  outRTS <= uart1o.rtsn;

  ------------------------------------
  -- Multi-processor IRQ controller:
  clIrqCtrl : irqmp generic map 
  (
    pindex => APB_IRQ_CONTROL,
    paddr => 16#002#,
    pmask => 16#FFF#,
    ncpu => CFG_NCPU
  )port map
  (
    wNRst,
    wClkBus,
    apbi,
    apbo(APB_IRQ_CONTROL),
    irqo,
    irqi
  );

  ------------------------------------
  -- Timer Unit:
  gpti.dhalt  <= dsuo.tstop;
  gpti.extclk <= '0';

  clTimer : gptimer generic map 
  (
    pindex  => APB_TIMER,
    paddr   => 16#003#,
    pmask   => 16#FFF#,
    pirq    => IRQ_TIMER,
    sepirq  => CFG_GPT_SEPIRQ,
    sbits   => CFG_GPT_SW,
    ntimers => CFG_GPT_NTIM,
    nbits   => CFG_GPT_TW
  )port map 
  (
    wNRst,
    wClkBus,
    apbi,
    apbo(APB_TIMER),
    gpti,
    open
  );

  ------------------------------------
  -- RF front-end control module:
  clRfControl : rfctrl generic map
  (
    pindex  => APB_RF_CONTROL,
    paddr   => 16#004#,
    pmask   => 16#FFF#
  )port map
  (
    wNRst,
    wClkBus,
    apbi,
    apbo(APB_RF_CONTROL),
    inLD,
    outSCLK,
    outSDATA,
    outCSn,
    inExtAntStat,
    inExtAntDetect,
    outExtAntEna
  );

  ------------------------------------
  -- STMicroelectronics 3-axis Gyroscope controller
  clGyroSpi : gyrospi generic map
  (
    pindex => APB_GYROSCOPE,
    paddr  => 16#006#, -- 6..7 addr fields
    pmask  => 16#ffe#
  ) port map
  (
    wNRst,
    wClkBus,
    apbi,
    apbo(APB_GYROSCOPE),
    inGyroInt1,
    inGyroInt2,
    inGyroSDI,
    outGyroSPC,
    outGyroSDO,
    outGyroCSn
  );

  ------------------------------------
  -- STMicroelectronics 3-axis Accelerometer controller
  clAccelerSpi : accelspi generic map
  (
    pindex => APB_ACCELEROMETER,
    paddr  => 16#008#, -- 8..9 addr fields
    pmask  => 16#ffe#
  ) port map
  (
    wNRst,
    wClkBus,
    apbi,
    apbo(APB_ACCELEROMETER),
    inAccelerInt1,
    inAccelerInt2,
    inAccelerSDI,
    outAccelerSPC,
    outAccelerSDO,
    outAccelerCSn
  );
  

  ------------------------------------
  -- AHB ROM:
  clBootROM : entity work.ahbrom generic map 
  (
    hindex => AHB_SLAVE_ROM,
    haddr => 16#000#,
    hmask => 16#fff#,
    pipe => CFG_AHBROPIP
  )port map 
  (
    wNRst,
    wClkBus,
    slvi,
    slvo(AHB_SLAVE_ROM)
  );

  ------------------------------------
  -- ROM image with "Hello World" firmware:
  romimg_gen : if(CFG_FWROM_ENABLE=1) generate
    clFwRomImage : entity FwRomImage generic map
    (
      hindex  => AHB_SLAVE_FW_IMAGE,
      haddr   => 16#500#,
      hmask   => 16#fff#
    )port map
    (
      wNRst,
      wClkBus,
      slvi,
      slvo(AHB_SLAVE_FW_IMAGE)
    );
  end generate;


  ------------------------------------
  -- AHB RAM:
  clAhbRAM : ahbram_p generic map 
  (
    hindex => AHB_SLAVE_RAM,
    haddr  => 16#400#, 
    hmask  => 16#FFF#,
    tech   => CFG_MEMTECH,
    abits  => CFG_SRAM_ADRBITS,
    kbytes => CFG_AHBRSZ
  )port map 
  (
    wNRst,
    wClkBus,
    slvi,
    slvo(AHB_SLAVE_RAM),
    ramaddr,
    hwdata,
    ramdata,
    ramsel,
    write
  );

  ------------------------------------
  -- GPS/GLONASS/Galileo/SBAS Navigation engine:
  clGnssEngine : GnssEngine generic map 
  (
    hindex => AHB_SLAVE_GNSSENGINE,
    haddr  => 16#D00#, 
    hmask  => 16#FFF#,
    irqind => IRQ_GNSS_ENGINE
  )port map 
  (
    wNRst,
    wClkBus,
    slvi,
    slvo(AHB_SLAVE_GNSSENGINE),
    --inAdcClk,-- <== RIGHT
    wClkBus,-- <== WRONG: debug purpose only!!!
    inIa,
    inQa,
    inIb,
    inQb
  );



  outClkBus <= wClkBus;
  outLED(0) <= wPllLocked;
  outLED(1) <= dbgo(0).error and wNRst;
  outLED(2) <= dsuo.active;
  outLED(3) <= inDsuBreak;

end rtl;

