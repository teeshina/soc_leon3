

library ieee;
use ieee.std_logic_1164.all;
library grlib;
use grlib.stdlib.all;

entity tap_rstn is
  port (
    inSysReset  : in std_ulogic;
    inSysClk    : in std_ulogic;
    inPllLock   : in std_ulogic;
    outNRst     : out std_ulogic
    );
end;


architecture rtl of tap_rstn is
  signal rbPllLock : std_logic_vector(4 downto 0);
  signal rReset    : std_ulogic;

begin


  proc_rst : process (inSysClk) begin
    if rising_edge(inSysClk) then 
      rbPllLock <= (rbPllLock(3 downto 0) & '1') and (not inSysReset & not inSysReset & not inSysReset & not inSysReset & not inSysReset); 
      rReset <= rbPllLock(4) and rbPllLock(3) and rbPllLock(2);
    end if;
  end process;
  outNRst <= rReset and not inSysReset;
  
end;  
