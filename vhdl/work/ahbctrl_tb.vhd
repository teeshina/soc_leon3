library ieee;
use ieee.std_logic_1164.all;
library std;
use std.textio.all;
library grlib;
use grlib.amba.all;
use grlib.stdlib.all;
use grlib.devices.all;
library techmap;
use techmap.gencomp.all;
library gaisler;
use gaisler.misc.all;
use gaisler.libjtagcom.all;
use gaisler.jtag.all;
library work;
use work.config.all;



entity ahbctrl_tb is
end ahbctrl_tb;

architecture behavior of ahbctrl_tb is

  constant CLK_HPERIOD : time := 10 ps;
  constant STRING_SIZE : integer := 18; -- string size = index of the last element

  constant REREAD : integer := 1;
  constant REVISION : integer := REREAD;
  constant TAPSEL   : integer := has_tapsel(CFG_FABTECH);
  constant maxahbm : integer := CFG_NCPU+CFG_AHB_UART+CFG_GRETH+CFG_AHB_JTAG+CFG_SVGA_ENABLE;
  constant IOAEN : integer := 1;--CONFIG_DDRSP;--CFG_DDRSP;
  constant LEON3_VERSION : integer := 0;
  constant DSU3_VERSION : integer := 1;


  -- conversion function
  function StringToUVector(inStr: string) return std_ulogic_vector is
    variable temp: std_ulogic_vector(inStr'range) := (others => 'X');
  begin
    for i in inStr'range loop --
      if(inStr(inStr'high-i+1)='1')    then temp(i) := '1';
      elsif(inStr(inStr'high-i+1)='0') then temp(i) := '0';
      end if;
    end loop;
    return temp(inStr'high downto 1);
  end function StringToUVector;
  -- conversion function
  function StringToVector(inStr: string) return std_logic_vector is
    variable temp: std_logic_vector(inStr'range) := (others => 'X');
  begin
    for i in inStr'range loop --
      if(inStr(inStr'high-i+1)='1')    then temp(i) := '1';
      elsif(inStr(inStr'high-i+1)='0') then temp(i) := '0';
      end if;
    end loop;
    return temp(inStr'high downto 1);
  end function StringToVector;
  --
  function UnsignedToSigned(inUnsigned: std_ulogic_vector) 
    return std_logic_vector is
    variable temp: std_logic_vector(inUnsigned'length-1 downto 0) := (others => 'X');
    variable i: integer:=0;
  begin
    while i < inUnsigned'length loop
      if(inUnsigned(i)='1')    then temp(i) := '1';
      elsif(inUnsigned(i)='0') then temp(i) := '0';
      end if;
      i := i+1;
    end loop;
    return temp;
  end function UnsignedToSigned;

  -- input/output signals:
  signal inNRst       : std_logic:= '0';
  signal inClk        : std_logic:= '0';

  signal out_msti    : ahb_mst_in_type;
  signal in_msto    : ahb_mst_out_vector;
  signal out_slvi    : ahb_slv_in_type;
  signal in_slvo    : ahb_slv_out_vector;


  signal in_tapo      : tap_out_type;
  signal out_tapi     : tap_in_type;
  signal chk_TDO     : std_logic;
  signal outmst_dmao      : ahb_dma_out_type;    
  signal outcom_dmai     : ahb_dma_in_type;
  
  signal wbUSignals: std_ulogic_vector(STRING_SIZE-1 downto 0);
  signal wbSSignals: std_logic_vector(STRING_SIZE-1 downto 0);
  shared variable iClkCnt : integer := 0;

  -- leon3s -> .. -> mmu_acache
  constant leon3_hconfig : ahb_config_type := (
  0 => ahb_device_reg ( VENDOR_GAISLER, GAISLER_LEON3, 0, LEON3_VERSION, 0),
  others => zero32);

  -- UART: 
  -- hindex = NCPU
  constant uart_hconfig : ahb_config_type := (
  0 => ahb_device_reg ( VENDOR_GAISLER, GAISLER_AHBUART, 0, 0, 0),
  others => zero32);

  -- JTAG: 
  -- hindex = NCPU+CFG_AHB_UART
  constant jtag_hconfig : ahb_config_type := (
  0 => ahb_device_reg ( VENDOR_GAISLER, GAISLER_AHBJTAG, 0, REVISION, 0),
  others => zero32);

  -- slave DSU3:
  constant dsu_hconfig : ahb_config_type := (
    0 => ahb_device_reg ( VENDOR_GAISLER, GAISLER_LEON3DSU, 0, DSU3_VERSION, 0),
    --4 => ahb_membar(haddr, '0', '0', hmask),
    4 => ahb_membar(16#900#, '0', '0', 16#F00#),
    others => zero32);

  constant zero_hconfig : ahb_config_type := ( others => zero32);
  

begin

  --in_msto(0) := ahbm_none;
  in_msto(0).hbusreq <= '0';
  in_msto(0).htrans <= HTRANS_IDLE;
  in_msto(0).hlock <= '0';
  in_msto(0).hindex <= 0;
  in_msto(0).hconfig <= leon3_hconfig;

  --in_msto(CFG_NCPU) := ahbm_none;
  in_msto(CFG_NCPU).hbusreq <= '0';
  in_msto(CFG_NCPU).htrans <= HTRANS_IDLE;
  in_msto(CFG_NCPU).hlock <= '0';
  in_msto(CFG_NCPU).hindex <= CFG_NCPU;
  in_msto(CFG_NCPU).hconfig <= uart_hconfig;

  in_msto(3) <= ahbm_none;--.hconfig <= zero_hconfig;
--  in_msto(3).hbusreq <= '0';
  in_msto(4).hconfig <= zero_hconfig;
  in_msto(4).hbusreq <= '0';
  in_msto(5).hconfig <= zero_hconfig;
  in_msto(5).hbusreq <= '0';
  in_msto(6).hconfig <= zero_hconfig;
  in_msto(6).hbusreq <= '0';
  in_msto(7).hconfig <= zero_hconfig;
  in_msto(7).hbusreq <= '0';
  in_msto(8).hconfig <= zero_hconfig;
  in_msto(8).hbusreq <= '0';
  in_msto(9).hconfig <= zero_hconfig;
  in_msto(9).hbusreq <= '0';
  in_msto(10).hconfig <= zero_hconfig;
  in_msto(10).hbusreq <= '0';  
  in_msto(11).hconfig <= zero_hconfig;
  in_msto(11).hbusreq <= '0';
  in_msto(12).hconfig <= zero_hconfig;
  in_msto(12).hbusreq <= '0';
  in_msto(13).hconfig <= zero_hconfig;
  in_msto(13).hbusreq <= '0';
  in_msto(14).hconfig <= zero_hconfig;
  in_msto(14).hbusreq <= '0';
  in_msto(15).hconfig <= zero_hconfig;
  in_msto(15).hbusreq <= '0';

  in_slvo(0) <= ahbs_none;
  in_slvo(1) <= ahbs_none;
  
  in_slvo(2).hindex  <= 2;
  in_slvo(2).hready  <= '1';
  in_slvo(2).hconfig <= dsu_hconfig;
  in_slvo(2).hrdata  <= conv_std_logic_vector(16#77777777#, 32);
  in_slvo(2).hresp   <= HRESP_OKAY;
  in_slvo(2).hcache  <= '1';
  in_slvo(2).hirq    <= conv_std_logic_vector(16#0#, 32);
  in_slvo(2).hsplit  <= conv_std_logic_vector(16#0#, 16);
  
  in_slvo(3) <= ahbs_none;
  in_slvo(4) <= ahbs_none;
  in_slvo(5) <= ahbs_none;
  in_slvo(6) <= ahbs_none;
  in_slvo(7) <= ahbs_none;
  in_slvo(8) <= ahbs_none;
  in_slvo(9) <= ahbs_none;
  in_slvo(10) <= ahbs_none;
  in_slvo(11) <= ahbs_none;
  in_slvo(12) <= ahbs_none;
  in_slvo(13) <= ahbs_none;
  in_slvo(14) <= ahbs_none;
  in_slvo(15) <= ahbs_none;

  -- Process of clock generation
  procClkgen : process
  begin
      inClk <= '0' after CLK_HPERIOD, '1' after 2*CLK_HPERIOD;
      wait for 2*CLK_HPERIOD;
  end process procClkgen;

  -- Process of reading  
  procReadingFile : process
    file InputFile:TEXT is "e:/ahbctrl_tb.txt";--open read_mode file_name;
    variable rdLine: line;  
    variable strLine : string(STRING_SIZE downto 1);
  begin
    while not endfile(InputFile) loop
      readline(InputFile, rdLine);
      read(rdLine, strLine);
      wbUSignals <= StringToUVector(strLine);
      wbSSignals <= StringToVector(strLine);--UnsignedToSigned(wbSignals);
  
      wait until rising_edge(inClk);
      --wait until falling_edge(inClk);
      iClkCnt := iClkCnt + 1;
    end loop;
  end process procReadingFile;
  

  -- signal parsment and assignment
  inNRst    <= wbUSignals(0);
  -- Input signals:
  in_tapo.tck   <= wbUSignals(1);--std_ulogic;
  in_tapo.tdi   <= wbUSignals(2);--std_ulogic;
  in_tapo.inst  <= wbSSignals(10 downto 3);--std_logic_vector(7 downto 0);
  in_tapo.asel  <= wbUSignals(11);--std_ulogic;
  in_tapo.dsel  <= wbUSignals(12);--std_ulogic;
  in_tapo.reset <= wbUSignals(13);--std_ulogic;
  in_tapo.capt  <= wbUSignals(14);--std_ulogic;
  in_tapo.shift <= wbUSignals(15);--std_ulogic;
  in_tapo.upd   <= wbUSignals(16);--std_ulogic;      
  chk_TDO       <= wbUSignals(17);--std_ulogic;      


  tt_jcom : jtagcom generic map 
  (
    isel => TAPSEL,
    nsync => 2,
    ainst => 2,
    dinst => 3,
    reread => REREAD
  )
  port map
  (
    inNRst,
    inClk,
    in_tapo,
    out_tapi,
    outmst_dmao,
    outcom_dmai
  );

  tt : ahbmst generic map 
  (
    hindex => CFG_NCPU+CFG_AHB_UART,
    venid => VENDOR_GAISLER,
    devid => GAISLER_AHBJTAG,
    version => REVISION
  )port map 
  (
    inNRst,
    inClk,
    outcom_dmai,
    outmst_dmao,
    out_msti,
    in_msto(CFG_NCPU+CFG_AHB_UART)
  );


  tt_ctrl : ahbctrl generic map 
  (
    defmast => CFG_DEFMST,
    split => CFG_SPLIT, 
	  rrobin => CFG_RROBIN,
	  ioaddr => CFG_AHBIO,
	  devid => XILINX_ML401,
	  ioen => IOAEN,
	  nahbm => maxahbm,
	  nahbs => 8
	)port map 
	(
	  inNRst,
	  inClk,
	  out_msti,
	  in_msto,
	  out_slvi,
	  in_slvo
	);
  
end;
 
 