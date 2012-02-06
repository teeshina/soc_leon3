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
  constant CLK_HPERIOD : time := 10 ps;
  constant STRING_SIZE : integer := 542; -- string size = index of the last element

  constant REREAD : integer := 1;
  constant TAPSEL   : integer := has_tapsel(CFG_FABTECH);
  constant REVISION : integer := REREAD;
end ahbmst_tb;

architecture behavior of ahbmst_tb is


  -- input/output signals:
  signal inNRst       : std_logic:= '0';
  signal inClk        : std_logic:= '0';
  signal in_dmai      : ahb_dma_in_type;
  signal dmao     : ahb_dma_out_type;
  signal ch_dmao     : ahb_dma_out_type;    
  signal in_ahbi      : ahb_mst_in_type;
  signal ahbo     : ahb_mst_out_type;
  signal ch_ahbo     : ahb_mst_out_type;

  signal U: std_ulogic_vector(STRING_SIZE-1 downto 0);
  signal S: std_logic_vector(STRING_SIZE-1 downto 0);
  shared variable iClkCnt : integer := 0;
  shared variable iErrCnt : integer := 0;
  
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
      U <= StringToUVector(strLine);
      S <= StringToSVector(strLine);
  
      wait until rising_edge(inClk);
      --wait until falling_edge(inClk);
      iClkCnt := iClkCnt + 1;
    end loop;
  end process procReadingFile;
  

-- signal parsment and assignment
  inNRst <= S(0);
  in_dmai.address <= S(32 downto 1);
  in_dmai.wdata <= S(64 downto 33);
  in_dmai.start <= S(65);
  in_dmai.burst <= S(66);
  in_dmai.write <= S(67);
  in_dmai.busy <= S(68);
  in_dmai.irq <= S(69);
  in_dmai.size <= S(72 downto 70);
  in_ahbi.hgrant <= S(88 downto 73);
  in_ahbi.hready <= S(89);
  in_ahbi.hresp <= S(91 downto 90);
  in_ahbi.hrdata <= S(123 downto 92);
  ch_dmao.start <= S(124);
  ch_dmao.active <= S(125);
  ch_dmao.ready <= S(126);
  ch_dmao.retry <= S(127);
  ch_dmao.mexc <= S(128);
  ch_dmao.haddr <= S(138 downto 129);
  ch_dmao.rdata <= S(170 downto 139);
  ch_ahbo.hbusreq <= S(171);
  ch_ahbo.hlock <= S(172);
  ch_ahbo.htrans <= S(174 downto 173);
  ch_ahbo.haddr <= S(206 downto 175);
  ch_ahbo.hwrite <= S(207);
  ch_ahbo.hsize <= S(210 downto 208);
  ch_ahbo.hburst <= S(213 downto 211);
  ch_ahbo.hprot <= S(217 downto 214);
  ch_ahbo.hwdata <= S(249 downto 218);
  ch_ahbo.hirq <= S(281 downto 250);
  ch_ahbo.hconfig(0) <= S(313 downto 282);
  ch_ahbo.hconfig(1) <= S(345 downto 314);
  ch_ahbo.hconfig(2) <= S(377 downto 346);
  ch_ahbo.hconfig(3) <= S(409 downto 378);
  ch_ahbo.hconfig(4) <= S(441 downto 410);
  ch_ahbo.hconfig(5) <= S(473 downto 442);
  ch_ahbo.hconfig(6) <= S(505 downto 474);
  ch_ahbo.hconfig(7) <= S(537 downto 506);
  ch_ahbo.hindex <= conv_integer(S(541 downto 538));

  
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
    dmao,
    in_ahbi,
    ahbo
  );


procCheck : process (inClk,ch_dmao,ch_ahbo)
begin
  if(rising_edge(inClk)and(iClkCnt>2)) then
    if(ch_ahbo/=ahbo) then print("Err: ahbo");  iErrCnt:=iErrCnt+1; end if;
  end if;
end process procCheck;


end;
 
 