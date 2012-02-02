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
use work.util_tb.all;



entity ahbmst_tb is
end ahbmst_tb;

architecture behavior of ahbmst_tb is

  constant CLK_HPERIOD : time := 10 ps;
  constant STRING_SIZE : integer := 330; -- string size = index of the last element

  constant REREAD : integer := 1;
  constant TAPSEL   : integer := has_tapsel(CFG_FABTECH);
  constant REVISION : integer := REREAD;

  -- input/output signals:
  signal inNRst       : std_logic:= '0';
  signal inClk        : std_logic:= '0';
  signal in_dmai      : ahb_dma_in_type;
  signal out_dmao     : ahb_dma_out_type;
  signal chk_dmao     : ahb_dma_out_type;    
  signal in_ahbi      : ahb_mst_in_type;
  signal out_ahbo     : ahb_mst_out_type;
  signal chk_ahbo     : ahb_mst_out_type;

  signal wbSignals: std_ulogic_vector(STRING_SIZE-1 downto 0);
  signal wbSSignals: std_logic_vector(STRING_SIZE-1 downto 0);
  shared variable iClkCnt : integer := 0;
  
  type reg_type is record
    start   : std_ulogic;
    retry   : std_ulogic;
    grant   : std_ulogic;
    active  : std_ulogic;
  end record;
  signal chk_r : reg_type;

begin


  -- Process of clock generation
  procClkgen : process
  begin
      inClk <= '0' after CLK_HPERIOD, '1' after 2*CLK_HPERIOD;
      wait for 2*CLK_HPERIOD;
  end process procClkgen;

  -- Process of reading  
  procReadingFile : process
    file InputFile:TEXT is "e:/ahbmaster_tb.txt";--open read_mode file_name;
    variable rdLine: line;  
    variable strLine : string(STRING_SIZE downto 1);
  begin
    while not endfile(InputFile) loop
      readline(InputFile, rdLine);
      read(rdLine, strLine);
      wbSignals <= StringToUVector(strLine);
      wbSSignals <= StringToSVector(strLine);--UnsignedToSigned(wbSignals);
  
      wait until rising_edge(inClk);
      --wait until falling_edge(inClk);
      iClkCnt := iClkCnt + 1;
    end loop;
  end process procReadingFile;
  

-- signal parsment and assignment
  inNRst    <= wbSignals(0);
  
  -- Input signals:
  in_dmai.address <= wbSSignals(32 downto 1);
  in_dmai.wdata   <= wbSSignals(64 downto 33);-- std_logic_vector(AHBDW-1 downto 0);
  in_dmai.start   <= wbSignals(65);--std_ulogic;
  in_dmai.burst   <= wbSignals(66);--std_ulogic;
  in_dmai.write   <= wbSignals(67);-- std_ulogic;
  in_dmai.busy    <= wbSignals(68);--std_ulogic;
  in_dmai.irq     <= wbSignals(69);--std_ulogic;
  in_dmai.size    <= wbSSignals(72 downto 70);--std_logic_vector(2 downto 0);
  
  chk_ahbo.hbusreq  <= wbSignals(73);
  chk_ahbo.hlock    <= wbSignals(74);
  chk_ahbo.htrans   <= wbSSignals(76 downto 75);
  chk_ahbo.haddr    <= wbSSignals(108 downto 77);
  chk_ahbo.hwrite   <= wbSignals(109);
  chk_ahbo.hsize    <= wbSSignals(112 downto 110);
  chk_ahbo.hburst   <= wbSSignals(115 downto 113);
  chk_ahbo.hprot    <= wbSSignals(119 downto 116);
  chk_ahbo.hwdata   <= wbSSignals(151 downto 120);
  chk_ahbo.hirq     <= wbSSignals(183 downto 152);
  chk_ahbo.hconfig(0)   <= wbSSignals(215 downto 184);
--  chk_ahbo.hindex   <= wbSSignals(231 downto 216);


  in_ahbi.hgrant  <= wbSSignals(247 downto 232);--: std_logic_vector(0 to NAHBMST-1);     -- bus grant
  in_ahbi.hready  <= wbSignals(248);--: std_ulogic;                           -- transfer done
  in_ahbi.hresp   <= wbSSignals(250 downto 249);-- : std_logic_vector(1 downto 0);   -- response type
  in_ahbi.hrdata  <= wbSSignals(282 downto 251);--  : std_logic_vector(AHBDW-1 downto 0);   -- read data bus
  in_ahbi.hcache  <= '0';--: std_ulogic;                           -- cacheable
  in_ahbi.hirq    <= "00000000000000000000000000000000";--: std_logic_vector(NAHBIRQ-1 downto 0); -- interrupt result bus
  in_ahbi.testen  <= '0';--: std_ulogic;                           -- scan test enable
  in_ahbi.testrst <= '0';--: std_ulogic;                           -- scan test reset
  in_ahbi.scanen  <= '0';--: std_ulogic;                           -- scan enable
  in_ahbi.testoen <= '0';--  : std_ulogic;                           -- test output enable 

  -- Check signals:
  chk_dmao.start  <= wbSSignals(283);
  chk_dmao.active <= wbSSignals(284);
  chk_dmao.ready  <= wbSSignals(285);
  chk_dmao.retry  <= wbSSignals(286);
  chk_dmao.mexc   <= wbSSignals(287);

  chk_dmao.haddr  <= wbSSignals(297 downto 288);
  chk_dmao.rdata  <= wbSSignals(329 downto 298);

  
  tt : ahbmst generic map 
  (
    hindex => 3,
    venid => VENDOR_GAISLER,
    devid => GAISLER_AHBJTAG,
    version => REVISION
  )port map 
  (
    inNRst,
    inClk,
    in_dmai,
    out_dmao,
    in_ahbi,
    out_ahbo
  );


--procCheck : process (inClk)
--begin
  --if(rising_edge(inClk)) then
    --if(dma_data /= (tt.dmao.rdata)) then
      --print("Debug output example: " & tost(chk_dmai.address));
      --print("Debug output example: " & tost(tt/r.addr);
    --end if
  --end if;
--end process procCheck;


end;
 
 