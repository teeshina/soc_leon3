-----------------------------------------------------------------------------
-- LEON3 Demonstration design test bench configuration
-- Copyright (C) 2009 Aeroflex Gaisler
------------------------------------------------------------------------------


library techmap;
use techmap.gencomp.all;
library grlib;
use grlib.amba.all;
use grlib.stdlib.all;

package config is
-- Technology and synthesis options
  constant CFG_FABTECH : integer := virtex6;
  constant CFG_MEMTECH : integer := virtex6;
  constant CFG_PADTECH : integer := virtex6;
  constant CFG_NOASYNC : integer := 0;
  constant CFG_SCAN : integer := 0;
-- LEON3 processor core
  constant CFG_LEON3 : integer := 1;
  constant CFG_NCPU : integer := (1);
  constant CFG_NWIN : integer := (8);
  constant CFG_V8 : integer := 16#32# + 4*0;
  constant CFG_MAC : integer := 0;
  constant CFG_BP : integer := 1;
  constant CFG_SVT : integer := 1;
  constant CFG_RSTADDR : integer := 16#00000#;
  constant CFG_LDDEL : integer := (1);
  constant CFG_NOTAG : integer := 0;
  constant CFG_NWP : integer := (2);
  constant CFG_PWD : integer := 1*2;
  constant CFG_FPU : integer := 0 + 16*0;
  constant CFG_GRFPUSH : integer := 0;
  constant CFG_ICEN : integer := 1;
  constant CFG_ISETS : integer := 2;
  constant CFG_ISETSZ : integer := 8;
  constant CFG_ILINE : integer := 8;
  constant CFG_IREPL : integer := 0;
  constant CFG_ILOCK : integer := 0;
  constant CFG_ILRAMEN : integer := 0;
  constant CFG_ILRAMADDR: integer := 16#8E#;
  constant CFG_ILRAMSZ : integer := 1;
  constant CFG_DCEN : integer := 1;
  constant CFG_DSETS : integer := 2;
  constant CFG_DSETSZ : integer := 4;
  constant CFG_DLINE : integer := 4;
  constant CFG_DREPL : integer := 0;
  constant CFG_DLOCK : integer := 0;
  constant CFG_DSNOOP : integer := 1 + 1 + 4*1;
  constant CFG_DFIXED : integer := 16#0#;
  constant CFG_DLRAMEN : integer := 0;
  constant CFG_DLRAMADDR: integer := 16#8F#;
  constant CFG_DLRAMSZ : integer := 1;
  constant CFG_MMUEN : integer := 1;
  constant CFG_ITLBNUM : integer := 8;
  constant CFG_DTLBNUM : integer := 8;
  constant CFG_TLB_TYPE : integer := 0 + 1*2;
  constant CFG_TLB_REP : integer := 0;
  constant CFG_MMU_PAGE : integer := 0;
  constant CFG_DSU : integer := 1;
  constant CFG_ITBSZ : integer := 4;
  constant CFG_ATBSZ : integer := 4;
  constant CFG_LEON3FT_EN : integer := 0;
  constant CFG_IUFT_EN : integer := 0;
  constant CFG_FPUFT_EN : integer := 0;
  constant CFG_RF_ERRINJ : integer := 0;
  constant CFG_CACHE_FT_EN : integer := 0;
  constant CFG_CACHE_ERRINJ : integer := 0;
  constant CFG_LEON3_NETLIST: integer := 0;
  constant CFG_DISAS : integer := 0 + 0;
  constant CFG_PCLOW : integer := 2;
-- AMBA settings
  constant CFG_AHBCTRL_DEFMST : integer := (0);
  constant CFG_AHBCTRL_RROBIN : integer := 1;
  constant CFG_AHBCTRL_SPLIT : integer := 1;
  constant CFG_AHBCTRL_AHBIO : integer := 16#FFF#;
  constant CFG_APBADDR : integer := 16#800#;
  constant CFG_AHB_MON : integer := 0;
  constant CFG_AHB_MONERR : integer := 0;
  constant CFG_AHB_MONWAR : integer := 0;
  constant CFG_AHB_DTRACE : integer := 0;
-- Ethernet DSU
  constant CFG_DSU_ETH : integer := 1 + 0 + 0;
  constant CFG_ETH_BUF : integer := 16;
  constant CFG_ETH_IPM : integer := 16#C0A8#;
  constant CFG_ETH_IPL : integer := 16#0033#;
  constant CFG_ETH_ENM : integer := 16#020789#;
  constant CFG_ETH_ENL : integer := 16#000123#;
-- LEON2 memory controller
  constant CFG_MCTRL_LEON2 : integer := 1;
  constant CFG_MCTRL_RAM8BIT : integer := 1;
  constant CFG_MCTRL_RAM16BIT : integer := 1;
  constant CFG_MCTRL_5CS : integer := 0;
  constant CFG_MCTRL_SDEN : integer := 0;
  constant CFG_MCTRL_SEPBUS : integer := 0;
  constant CFG_MCTRL_INVCLK : integer := 0;
  constant CFG_MCTRL_SD64 : integer := 0;
  constant CFG_MCTRL_PAGE : integer := 0 + 0;
-- AHB ROM
  constant CFG_AHBROPIP : integer := 0;
-- AHB RAM
  constant CFG_AHBRSZ : integer := 256;
  constant CFG_SRAM_ADRBITS : integer := log2(CFG_AHBRSZ) + 8 - AHBDW/64;
-- Gaisler Ethernet core
  constant CFG_GRETH : integer := 1;
  constant CFG_GRETH1G : integer := 0;
  constant CFG_ETH_FIFO : integer := 16;
-- UART 1
  constant CFG_UART1_FIFO : integer := 16;
-- Modular timer
  constant CFG_GPT_ENABLE : integer := 1;
  constant CFG_GPT_NTIM : integer := (2);
  constant CFG_GPT_SW : integer := (16);
  constant CFG_GPT_TW : integer := (32);
  constant CFG_GPT_SEPIRQ : integer := 1;
  constant CFG_GPT_WDOGEN : integer := 0;
  constant CFG_GPT_WDOG : integer := 16#0#;
-- GPIO port
  constant CFG_GRGPIO_ENABLE : integer := 1;
  constant CFG_GRGPIO_IMASK : integer := 16#0000#;
  constant CFG_GRGPIO_WIDTH : integer := (8);
-- I2C master
  constant CFG_I2C_ENABLE : integer := 1;

-- VGA and PS2/ interface
  constant CFG_KBD_ENABLE : integer := 0;
  constant CFG_VGA_ENABLE : integer := 0;
  constant CFG_SVGA_ENABLE : integer := 1;

-- AMBA System ACE Interface Controller
  constant CFG_GRACECTRL : integer := 0;

-- GRLIB debugging
  constant CFG_DUART : integer := 0;
-- Xilinx MIG DDR2 controller
  constant CFG_MIG_DDR2 : integer := 1;
  constant CFG_MIG_CLK4 : integer := 16;
  
  constant CFG_AHBCTRL_IOEN : integer := 1;
  
  constant CFG_FWROM_ENABLE : integer := 1;

  constant LEON3_CORES_NUM  : integer := CFG_NCPU;
  constant AHB_MASTER_LEON3 : integer := 0;
  constant AHB_MASTER_JTAG  : integer := AHB_MASTER_LEON3+LEON3_CORES_NUM;
  constant AHB_MASTER_TOTAL : integer := AHB_MASTER_JTAG+1;

  
  constant AHB_SLAVE_ROM        : integer := 0;
  constant AHB_SLAVE_DSU        : integer := AHB_SLAVE_ROM+1;
  constant AHB_SLAVE_RAM        : integer := AHB_SLAVE_DSU+1;
  constant AHB_SLAVE_APBBRIDGE  : integer := AHB_SLAVE_RAM+1;
  constant AHB_SLAVE_GNSSENGINE : integer := AHB_SLAVE_APBBRIDGE+1;
  constant AHB_SLAVE_FW_IMAGE   : integer := AHB_SLAVE_GNSSENGINE+1;
  constant AHB_SLAVE_TOTAL      : integer := AHB_SLAVE_FW_IMAGE+1;

  constant IRQ_IRQ_CONTROL      : integer := 0;
  constant IRQ_DSU              : integer := IRQ_IRQ_CONTROL+1;
  constant IRQ_GNSS_ENGINE      : integer := IRQ_DSU+1;
  constant IRQ_UART_1           : integer := IRQ_GNSS_ENGINE+1;
  constant IRQ_TIMER            : integer := IRQ_UART_1+1;
  constant IRQ_TOTAL            : integer := IRQ_TIMER+1+((CFG_GPT_NTIM-1)*CFG_GPT_SEPIRQ);


  constant APB_UART_1           : integer := 0;
  constant APB_IRQ_CONTROL      : integer := APB_UART_1+1;
  constant APB_TIMER            : integer := APB_IRQ_CONTROL+1;
  constant APB_RF_CONTROL       : integer := APB_TIMER+1;
  constant APB_GYROSCOPE        : integer := APB_RF_CONTROL+1;
  constant APB_ACCELEROMETER    : integer := APB_GYROSCOPE+1;
  constant APB_TOTAL            : integer := APB_ACCELEROMETER+1;

end;
