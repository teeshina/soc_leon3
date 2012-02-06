-----------------------------------------------------------------------------
--  LEON3 Demonstration design
--  Copyright (C) 2004 Jiri Gaisler, Gaisler Research
------------------------------------------------------------------------------
--  This file is a part of the GRLIB VHDL IP LIBRARY
--  Copyright (C) 2003 - 2008, Gaisler Research
--  Copyright (C) 2008 - 2010, Aeroflex Gaisler
--
--  This program is free software; you can redistribute it and/or modify
--  it under the terms of the GNU General Public License as published by
--  the Free Software Foundation; either version 2 of the License, or
--  (at your option) any later version.
--
--  This program is distributed in the hope that it will be useful,
--  but WITHOUT ANY WARRANTY; without even the implied warranty of
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--  GNU General Public License for more details.
--
--  You should have received a copy of the GNU General Public License
--  along with this program; if not, write to the Free Software
--  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
------------------------------------------------------------------------------


library ieee;
use ieee.std_logic_1164.all;
library grlib, techmap;
use grlib.amba.all;
use grlib.devices.all;
use grlib.stdlib.all;
use techmap.gencomp.all;
use techmap.allclkgen.all;
library gaisler;
use gaisler.memctrl.all;
use gaisler.leon3.all;
use gaisler.uart.all;
use gaisler.misc.all;
use gaisler.net.all;
use gaisler.jtag.all;
use gaisler.spacewire.all;

library esa;
use esa.memoryctrl.all;
use work.config.all;

entity leon3mp is
  generic (
    fabtech   : integer := CFG_FABTECH;
    memtech   : integer := CFG_MEMTECH;
    padtech   : integer := CFG_PADTECH;
    ncpu      : integer := CFG_NCPU;
    disas     : integer := CFG_DISAS;	-- Enable disassembly to console
    dbguart   : integer := CFG_DUART;	-- Print UART on console
    pclow     : integer := CFG_PCLOW
  );
  port (
    sys_rst_in	: in  std_ulogic;
    sys_clk	: in  std_ulogic; 	-- 100 MHz main clock
    sysace_clk_in  : in  std_ulogic;   -- System ACE clock
    plb_error	: out std_logic;	-- IU error mode
    opb_error	: out std_logic;	-- DSU active
    flash_a23	: out std_ulogic;
    sram_flash_addr : out std_logic_vector(22 downto 0);
    sram_flash_data : inout std_logic_vector(31 downto 0);
    sram_cen  	: out std_logic;
    sram_bw   	: out std_logic_vector (0 to 3);
    sram_flash_oe_n : out std_ulogic;
    sram_flash_we_n 	: out std_ulogic;
    flash_ce  	: out std_logic;
    sram_clk  	: out std_ulogic;
    sram_clk_fb	: in  std_ulogic; 
    sram_mode 	: out std_ulogic;
    sram_adv_ld_n : out std_ulogic;


    txd1   	: out std_ulogic; 			-- UART1 tx data
    rxd1   	: in  std_ulogic;  			-- UART1 rx data

    gpio        : inout std_logic_vector(26 downto 0); 	-- I/O port



    -- SPI
    spi_data_out     : in  std_logic;
    spi_data_in      : out std_ulogic;
    spi_data_cs_b    : out std_ulogic;
    spi_clk          : out std_ulogic;
    
	 maddr   : in std_logic_vector(16 downto 0);
	 mdatai  : in std_logic_vector(31 downto 0);
	 mdatao  : out std_logic_vector(31 downto 0);
	 mce     : in  std_ulogic;
	 mwe     : in  std_ulogic;
	 moe     : in  std_ulogic;
	 mode    : in  std_ulogic
	 );
end;

architecture rtl of leon3mp is


constant blength : integer := 12;
constant fifodepth : integer := 8;
constant maxahbm : integer := NCPU+CFG_AHB_UART
	+CFG_GRETH+CFG_AHB_JTAG+CFG_SVGA_ENABLE;

signal vcc, gnd   : std_logic_vector(4 downto 0);
signal memi  : memory_in_type;
signal memo  : memory_out_type;
signal wpo   : wprot_out_type;
signal sdi   : sdctrl_in_type;
signal sdo   : sdctrl_out_type;
signal sdo2, sdo3 : sdctrl_out_type;

signal apbi  : apb_slv_in_type;
signal apbo  : apb_slv_out_vector := (others => apb_none);
signal ahbsi : ahb_slv_in_type;
signal ahbso : ahb_slv_out_vector := (others => ahbs_none);
signal ahbmi : ahb_mst_in_type;
signal ahbmo : ahb_mst_out_vector := (others => ahbm_none);

signal clkm, rstn, rstraw, srclkl : std_ulogic;
signal clkm_90, clkm_180, clkm_270 : std_ulogic;

signal cgi, cgi2   : clkgen_in_type;
signal cgo, cgo2   : clkgen_out_type;
signal u1i, u2i, dui : uart_in_type;
signal u1o, u2o, duo : uart_out_type;

signal irqi : irq_in_vector(0 to NCPU-1);
signal irqo : irq_out_vector(0 to NCPU-1);

signal dbgi : l3_debug_in_vector(0 to NCPU-1);
signal dbgo : l3_debug_out_vector(0 to NCPU-1);

signal dsui : dsu_in_type;
signal dsuo : dsu_out_type; 

signal ethi, ethi1, ethi2 : eth_in_type;
signal etho, etho1, etho2 : eth_out_type;

signal gpti : gptimer_in_type;

signal gpioi : gpio_in_type;
signal gpioo : gpio_out_type;

signal clklock, lock, lclk, clkml, rst, ndsuact : std_ulogic;
signal tck, tckn, tms, tdi, tdo : std_ulogic;
signal ddrclk, ddrrst : std_ulogic;


signal ethclk, egtx_clk_fb : std_ulogic;
signal egtx_clk, legtx_clk, l2egtx_clk : std_ulogic;

signal kbdi  : ps2_in_type;
signal kbdo  : ps2_out_type;
signal moui  : ps2_in_type;
signal mouo  : ps2_out_type;
signal vgao  : apbvga_out_type;
signal clk_sel : std_logic_vector(1 downto 0);
signal clkval : std_logic_vector(1 downto 0);
signal clkvga, clk1x, video_clk, dac_clk : std_ulogic;

signal i2ci : i2c_in_type;
signal i2co : i2c_out_type;

signal spii : spi_in_type;
signal spio : spi_out_type;
signal slvsel : std_logic_vector(CFG_SPICTRL_SLVS-1 downto 0);

constant BOARD_FREQ : integer := 100000;   -- input frequency in KHz
constant CPU_FREQ : integer := BOARD_FREQ * CFG_CLKMUL / CFG_CLKDIV;  -- cpu frequency in KHz
constant I2C_FILTER : integer := (CPU_FREQ*5+50000)/100000+1;
constant IOAEN : integer := CFG_DDRSP;

signal stati : ahbstat_in_type;

signal ddsi  : ddrmem_in_type;
signal ddso  : ddrmem_out_type;

signal ddrclkfb, ssrclkfb, ddr_clkl, ddr_clk90l, ddr_clknl, ddr_clk270l : std_ulogic;
signal ddr_clkv 	: std_logic_vector(2 downto 0);
signal ddr_clkbv	: std_logic_vector(2 downto 0);
signal ddr_ckev  	: std_logic_vector(1 downto 0);
signal ddr_csbv  	: std_logic_vector(1 downto 0);
signal ddr_adl      	: std_logic_vector (13 downto 0);

signal clkace : std_ulogic;
signal acei   : gracectrl_in_type;
signal aceo   : gracectrl_out_type;

--testlogic-----------------------------------
signal tcnt1 : std_logic_vector(26 downto 0);
signal tcnt2 : std_logic_vector(26 downto 0);
signal testrst : std_logic;
----------------------------------------------

attribute syn_keep : boolean;
attribute syn_preserve : boolean;
attribute syn_keep of lock : signal is true;
attribute syn_preserve of lock : signal is true;
attribute syn_keep of clkml : signal is true;
attribute syn_preserve of clkml : signal is true;
attribute syn_keep of egtx_clk : signal is true;
attribute syn_preserve of egtx_clk : signal is true;
attribute keep : boolean;
attribute keep of lock : signal is true;
attribute keep of clkml : signal is true;
attribute keep of clkm : signal is true;
attribute keep of egtx_clk : signal is true;

--attribute syn_noprune : boolean;
--attribute syn_noprune of sysace_clk_in_pad : label is true;

signal romsn   : std_ulogic;
constant SPW_LOOP_BACK : integer := 0;

-- ahbmo(0)
constant leon3_hconfig : ahb_config_type := (
  0 => ahb_device_reg ( VENDOR_GAISLER, GAISLER_LEON3, 0, LEON3_VERSION, 0),
  others => zero32);


-- ahbso(1)
constant apbctrl_hconfig : ahb_config_type := (
  0 => ahb_device_reg ( 1, 6, 0, 0, 0),
  4 => ahb_membar(CFG_APBADDR, '0', '0', 16#fff#),
  others => zero32);

-- ahbso(2)
constant dsu3_hconfig : ahb_config_type := (
    0 => ahb_device_reg ( VENDOR_GAISLER, GAISLER_LEON3DSU, 0, 1, 0),
    4 => ahb_membar(16#900#, '0', '0', 16#F00#),
    others => zero32);


-- ahbso(3)
constant mctrl_hconfig : ahb_config_type := (
  0 => ahb_device_reg ( VENDOR_ESA, ESA_MCTRL, 0, 1, 0),
  4 => ahb_membar(16#000#, '1', '1', 16#E00#),
  5 => ahb_membar(16#200#,  '0', '0', 16#E00#),
  6 => ahb_membar(16#C00#, '1', '1', 16#FF0#),
  others => zero32);

-- ahbso(4)
constant grace_hconfig : ahb_config_type := (
    0 => ahb_device_reg(VENDOR_GAISLER, GAISLER_GRACECTRL, 0, 0, 13),
    4 => ahb_iobar(16#002#, 16#fff#), others => zero32);

-- ahbso(6)
constant ahbrom_hconfig : ahb_config_type := (
  0 => ahb_device_reg ( VENDOR_GAISLER, GAISLER_AHBROM, 0, 0, 0),
  4 => ahb_membar(CFG_AHBRODDR, '1', '1', 16#fff#), others => zero32);

-- ahbso(7)  
constant ram_hconfig : ahb_config_type := (
  0 => ahb_device_reg ( VENDOR_GAISLER, GAISLER_AHBRAM, 0, log2(CFG_AHBRSZ)+10, 0),
  4 => ahb_membar(CFG_AHBRADDR, '1', '1', 16#fff#),
  others => zero32);


--apbo(0)
constant mctrl_pconfig : apb_config_type := (
  0 => ahb_device_reg ( VENDOR_ESA, ESA_MCTRL, 0, 1, 0),
  1 => apb_iobar(0, 16#fff#));
  
-- apbo(1)
constant uart_pconfig : apb_config_type := (
  0 => ahb_device_reg ( VENDOR_GAISLER, GAISLER_APBUART, 0, 1, 2),
  1 => apb_iobar(1, 16#fff#));

-- apbo(2)
constant irqctrl_pconfig : apb_config_type := (
  0 => ahb_device_reg ( VENDOR_GAISLER, GAISLER_IRQMP, 0, 3, 0),
  1 => apb_iobar(2, 16#fff#));

-- apbo(3)
constant gpt_pconfig : apb_config_type := (
  0 => ahb_device_reg ( VENDOR_GAISLER, GAISLER_GPTIMER, 0, 0, CFG_GPT_IRQ),
  1 => apb_iobar(3, 16#fff#));

-- apbo(8)
constant gpio_pconfig : apb_config_type := (
  0 => ahb_device_reg ( VENDOR_GAISLER, GAISLER_GPIO, 0, 1, 0),
  1 => apb_iobar(8, 16#fff#));

-- apbo(10)
constant spictrl_pconfig : apb_config_type := (
  0 => ahb_device_reg(VENDOR_GAISLER, GAISLER_SPICTRL, 0, 2, 12),
  1 => apb_iobar(10, 16#fff#));

-- apbo(15)
constant ahbstat_pconfig : apb_config_type := (
  0 => ahb_device_reg (VENDOR_GAISLER, GAISLER_AHBSTAT, 0, 0, 7),
  1 => apb_iobar(15, 16#FFF#));

signal rbPllLock : std_logic_vector(4 downto 0);
signal rReset : std_ulogic;

begin

  lclk <= sys_clk;
  clkm <= sys_clk;

  rst <= sys_rst_in;
  proc_rst : process (sys_clk) begin
	 if rising_edge(sys_clk) then 
	   rbPllLock <= (rbPllLock(3 downto 0) & '1') and (not sys_rst_in & not sys_rst_in & not sys_rst_in & not sys_rst_in & not sys_rst_in); 
	   rReset <= rbPllLock(4) and rbPllLock(3) and rbPllLock(2);
	 end if;
  end process;
  rstn <= rReset and not sys_rst_in;

----------------------------------------------------------------------
---  AHB CONTROLLER --------------------------------------------------
----------------------------------------------------------------------

  ahb0 : ahbctrl 		-- AHB arbiter/multiplexer
  generic map (defmast => CFG_DEFMST, split => CFG_SPLIT, 
	rrobin => CFG_RROBIN, ioaddr => CFG_AHBIO, devid => XILINX_ML401,
	ioen => IOAEN, nahbm => maxahbm, nahbs => 8)
  port map (rstn, clkm, ahbmi, ahbmo, ahbsi, ahbso);

----------------------------------------------------------------------
---  LEON3 processor and DSU -----------------------------------------
----------------------------------------------------------------------

  l3 : if CFG_LEON3 = 1 generate
    cpu : for i in 0 to NCPU-1 generate
      u0 : leon3s			-- LEON3 processor      
      generic map (i, fabtech, memtech, CFG_NWIN, CFG_DSU, CFG_FPU, CFG_V8, 
	0, CFG_MAC, pclow, 0, CFG_NWP, CFG_ICEN, CFG_IREPL, CFG_ISETS, CFG_ILINE, 
	CFG_ISETSZ, CFG_ILOCK, CFG_DCEN, CFG_DREPL, CFG_DSETS, CFG_DLINE, CFG_DSETSZ,
	CFG_DLOCK, CFG_DSNOOP, CFG_ILRAMEN, CFG_ILRAMSZ, CFG_ILRAMADDR, CFG_DLRAMEN,
        CFG_DLRAMSZ, CFG_DLRAMADDR, CFG_MMUEN, CFG_ITLBNUM, CFG_DTLBNUM, CFG_TLB_TYPE, CFG_TLB_REP, 
        CFG_LDDEL, disas, CFG_ITBSZ, CFG_PWD, CFG_SVT, CFG_RSTADDR, NCPU-1)
      port map (clkm, rstn, ahbmi, ahbmo(i), ahbsi, ahbso, 
    		irqi(i), irqo(i), dbgi(i), dbgo(i));
    end generate;
--  ahbmo(0).hindex <= 0;
--  ahbmo(0).hconfig <= leon3_hconfig;

--    errorn_pad : odpad generic map (tech => padtech) port map (plb_error, dbgo(0).error);
  
    dsugen : if CFG_DSU = 1 generate
      dsu0 : dsu3			-- LEON3 Debug Support Unit
      generic map (hindex => 2, haddr => 16#900#, hmask => 16#F00#, 
         ncpu => NCPU, tbits => 30, tech => memtech, irq => 0, kbytes => CFG_ATBSZ)
      port map (rstn, clkm, ahbmi, ahbsi, ahbso(2), dbgo, dbgi, dsui, dsuo);
      dsui.enable <= '1';
       dsui.break <= gpioo.val(11); --  South Button
      dsuact_pad : outpad generic map (tech => padtech) port map (opb_error, ndsuact);
      ndsuact <= not dsuo.active;
    end generate;
--  ahbso(2).hindex <= 2;
--  ahbso(2).hconfig <= dsu3_hconfig;

  end generate;


  ahbjtaggen0 :if CFG_AHB_JTAG = 1 generate
    ahbjtag0 : ahbjtag generic map(tech => fabtech, hindex => NCPU+CFG_AHB_UART)
      port map(rstn, clkm, tck, tms, tdi, tdo, ahbmi, ahbmo(NCPU+CFG_AHB_UART),
               open, open, open, open, open, open, open, gnd(0));
  end generate;
  
----------------------------------------------------------------------
---  Memory controllers ----------------------------------------------
----------------------------------------------------------------------

  memi.writen <= '1'; memi.wrn <= "1111"; memi.bwidth <= "10";
  memi.brdyn <= '1'; memi.bexcn <= '1';

--  mctrl0 : if CFG_MCTRL_LEON2 = 1 generate
--    mctrl0 : mctrl generic map (hindex => 3, pindex => 0, 
--	ramaddr => 16#C00#, rammask => 16#FF0#,
--	paddr => 0, srbanks => 1, ram8 => CFG_MCTRL_RAM8BIT, 
--	ram16 => CFG_MCTRL_RAM16BIT, sden => CFG_MCTRL_SDEN, 
--	invclk => CFG_MCTRL_INVCLK, sepbus => CFG_MCTRL_SEPBUS)
--    port map (rstn, clkm, memi, memo, ahbsi, ahbso(3), apbi, apbo(0), wpo, open);
--  end generate;
  ahbso(3).hindex <= 3;
  ahbso(3).hconfig <= mctrl_hconfig;
  apbo(0).pindex <= 0;
  apbo(0).pconfig <= mctrl_pconfig;



--  bpromgen : if CFG_AHBROMEN /= 0 generate
--    brom : entity work.ahbrom
--      generic map (hindex => 6, haddr => CFG_AHBRODDR, pipe => CFG_AHBROPIP)
--      port map ( rstn, clkm, ahbsi, ahbso(6));
--  end generate;
  ahbso(6).hindex <= 6;
  ahbso(6).hconfig <= ahbrom_hconfig;


----------------------------------------------------------------------
---  APB Bridge and various periherals -------------------------------
----------------------------------------------------------------------

  apb0 : apbctrl				-- AHB/APB bridge
  generic map (hindex => 1, haddr => CFG_APBADDR, nslaves => 16)
  port map (rstn, clkm, ahbsi, ahbso(1), apbi, apbo );
--  ahbso(1).hindex <= 1;
--  ahbso(1).hconfig <= apbctrl_hconfig;


  apbo(1).pindex <= 1;
  apbo(1).pconfig <= uart_pconfig;

  apbo(2).pindex <= 2;
  apbo(2).pconfig <= irqctrl_pconfig;

  apbo(3).pindex <= 3;
  apbo(3).pconfig <= gpt_pconfig;

  apbo(8).pindex <= 8;
  apbo(8).pconfig <= gpio_pconfig;

  apbo(15).pindex <= 15;
  apbo(15).pconfig <= ahbstat_pconfig;
  
  apbo(10).pindex <= 10;
  apbo(10).pconfig <= spictrl_pconfig;
  
  
-----------------------------------------------------------------------
---  AHB RAM ----------------------------------------------------------
-----------------------------------------------------------------------

--  ocram : if CFG_AHBRAMEN = 1 generate 
--    ahbram0 : ahbram generic map (hindex => 7, haddr => CFG_AHBRADDR, 
--	tech => CFG_MEMTECH, kbytes => CFG_AHBRSZ)
--    port map ( rstn, clkm, ahbsi, ahbso(7));
--  end generate;
  ahbso(7).hindex <= 7;
  ahbso(7).hconfig <= ram_hconfig;


end;
