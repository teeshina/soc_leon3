------------------------------------------------------------------------------
--  INFORMATION:  http://www.GNSS-sensor.com
--  PROPERTY:     GNSS Sensor Ltd
--  E-MAIL:       alex.kosin@gnss-sensor.com
--  DESCRIPTION:  DUMMY version of the Channels Data snapper and Control block.
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
use gnsslib.dummies.all;

------------------------------------------------------------------------------
entity GnssControl is
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
    outMemWrAdr  : out std_logic_vector(CFG_GNSS_ADDR_WIDTH-4 downto 0);
    outMemWrData : out std_logic_vector(63 downto 0)
  );
end;

architecture rtl of GnssControl is
  constant last_field : std_logic_vector(log2x(CFG_GNSS_DWORD_PER_MODULE)-1 downto 0) 
    := conv_std_logic_vector(CFG_GNSS_DWORD_PER_MODULE-1, log2x(CFG_GNSS_DWORD_PER_MODULE));
  constant last_module : std_logic_vector(log2x(CFG_GNSS_MODULES_TOTAL)-1 downto 0) 
    := conv_std_logic_vector(CFG_GNSS_MODULES_TOTAL-1, log2x(CFG_GNSS_MODULES_TOTAL));
  
  type registers is record
    wr_ena        : std_ulogic;
    wr_module_sel : std_logic_vector(CFG_GNSS_ADDR_WIDTH-log2x(CFG_GNSS_DWORD_PER_MODULE)-4 downto 0);
    wr_field_sel  : std_logic_vector(3 downto 0);
    wr_data       : std_logic_vector(31 downto 0);
    SnapEna   : std_ulogic;
    SnapModuleCnt   : std_logic_vector(log2x(CFG_GNSS_MODULES_TOTAL)-1 downto 0);
    SnapFieldCnt   : std_logic_vector(log2x(CFG_GNSS_DWORD_PER_MODULE)-1 downto 0);
    SnapEna_z : std_ulogic;
    SnapModuleCnt_z   : std_logic_vector(log2x(CFG_GNSS_MODULES_TOTAL)-1 downto 0);
    SnapFieldCnt_z  : std_logic_vector(log2x(CFG_GNSS_DWORD_PER_MODULE)-1 downto 0);
  end record;

  signal r, rin : registers;

begin

  comb : process(inNRst, r, inWrAdr, inWrEna, inWrData, inMsReady, m2c)
    variable v : registers;
    variable v_m2c : Module2Ctrl;
    variable snap_end : std_ulogic;
  begin
    
    v := r;
  
    v.wr_ena := '0';
    snap_end := '0';
  
    if(inWrEna='1') then
      v.wr_ena        := '1';
      v.wr_module_sel := inWrAdr(CFG_GNSS_ADDR_WIDTH-1 downto 6);
      v.wr_field_sel  := inWrAdr(5 downto 2);
      v.wr_data       := inWrData;
    end if;
  

    -- Reading is fully synchronous with all modules:
    if( (r.SnapModuleCnt=last_module)
      and (r.SnapFieldCnt=last_field) ) then snap_end := '1'; end if;

    if(inMsReady='1')   then v.SnapEna := '1';
    elsif(snap_end='1') then v.SnapEna := '0'; end if;
    v.SnapEna_z := r.SnapEna;

    if((inMsReady='1')or(r.SnapEna='0')or(snap_end='1')) then v.SnapFieldCnt := (others=>'0');
    elsif(r.SnapEna='1') then v.SnapFieldCnt := r.SnapFieldCnt+1; end if;
    v.SnapFieldCnt_z := r.SnapFieldCnt;

    if((inMsReady='1')or(r.SnapEna='0')or(snap_end='1')) then v.SnapModuleCnt := (others=>'0');
    elsif((r.SnapEna='1')and(r.SnapFieldCnt=last_field)) then v.SnapModuleCnt := r.SnapModuleCnt+1; end if;
    v.SnapModuleCnt_z := r.SnapModuleCnt;

    -- mux data for DP writting:  
    v_m2c := m2c(conv_integer(r.SnapModuleCnt_z));

    -- reset:
    if(inNRst='0') then
      v.SnapEna := '0';
      v.SnapFieldCnt := (others =>'0');
      v.SnapModuleCnt := (others =>'0');
      v.SnapEna_z := '0';
      v.SnapFieldCnt_z := (others =>'0');
      v.SnapModuleCnt_z := (others =>'0');
      v.wr_ena := '0';
    end if;

    outMemWrEna <= r.SnapEna_z;
    outMemWrAdr <= r.SnapModuleCnt & r.SnapFieldCnt_z;
    outMemWrData <= v_m2c.rdata;

    c2m.wr_ena        <= r.wr_ena;
    c2m.wr_module_sel <= r.wr_module_sel;
    c2m.wr_field_sel  <= r.wr_field_sel;
    c2m.wr_data       <= r.wr_data;

    c2m.rd_ena        <= r.SnapEna;
    c2m.rd_module_sel <= r.SnapModuleCnt;
    c2m.rd_field_sel  <= r.SnapFieldCnt;
  
    outIrqPulse <= r.SnapEna_z and (not r.SnapEna);

    rin <= v;

  end process;

  -- registers:
  regs : process(inAdcClk)
  begin if rising_edge(inAdcClk) then r <= rin; end if; end process;

end;




