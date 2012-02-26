library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.std_logic_arith.all;
use ieee.numeric_std.all;

library unisim;
use unisim.vcomponents.all;
library techmap;
use techmap.gencomp.all;
library gnsslib;
use gnsslib.gnsspll.all;


entity SysPLL_all is
  generic (
    tech    : integer range 0 to NTECH := 0
  );
  port
  (
    CLK_IN1_P         : in     std_logic;
    CLK_IN1_N         : in     std_logic;
    CLK_OUT1          : out    std_logic;
    RESET             : in     std_logic;
    LOCKED            : out    std_logic
  );
end SysPLL_all;

architecture rtl of SysPLL_all is
begin

   xv6 : if tech = virtex6 generate
     pll0 : SysPLL_v6 port map (CLK_IN1_P, CLK_IN1_N, CLK_OUT1, RESET, LOCKED);
   end generate;

   xs6 : if tech = spartan6 generate
     pll0 : SysPLL_s6 port map (CLK_IN1_P, CLK_IN1_N, CLK_OUT1, RESET, LOCKED);
   end generate;

   xun : if ((tech/=virtex6)and(tech/=spartan6)) generate
     CLK_OUT1 <= CLK_IN1_P;
     LOCKED   <= '1';
   end generate;

end;
