library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
library techmap;
use techmap.gencomp.all;


package gnsspll is

  component SysPLL_all is
    generic(
      tech    : integer range 0 to NTECH := 0
    );
    port (
      CLK_IN1_P	: in std_logic;
      CLK_IN1_N	: in std_logic;
      CLK_OUT1	: out std_logic;
      RESET	: in std_logic;
      LOCKED	: out std_logic );
  end component;

  component SysPLL_v6 is 
    port (
      CLK_IN1_P	: in std_logic;
      CLK_IN1_N	: in std_logic;
      CLK_OUT1	: out std_logic;
      RESET	: in std_logic;
      LOCKED	: out std_logic );
  end component;

  component SysPLL_s6 is 
    port (
      CLK_IN1_P	: in std_logic;
      CLK_IN1_N	: in std_logic;
      CLK_OUT1	: out std_logic;
      RESET	: in std_logic;
      LOCKED	: out std_logic );
  end component;


end;
