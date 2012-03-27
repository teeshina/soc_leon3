library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.std_logic_arith.all;
use ieee.numeric_std.all;

library techmap;
use techmap.gencomp.all;
library gnsslib;
use gnsslib.gnssmem.all;


entity InitRam is
    port ( clk : in std_logic;
           ena : in std_logic_vector(3 downto 0);
           we : in std_logic_vector(3 downto 0);
           addr : in std_logic_vector(15 downto 0);
           di : in std_logic_vector(31 downto 0);
           do : out std_logic_vector(31 downto 0) );
end InitRam;

architecture rtl of InitRam is
  signal RAM00 : ram_type := ram00;
  signal RAM01 : ram_type := ram01;
  signal RAM02 : ram_type := ram02;
  signal RAM03 : ram_type := ram03;
  signal rbAdr : std_logic_vector(15 downto 0);
 
begin

  
  reg : process(clk)
    variable n : integer;
  begin
    n := conv_integer(addr);
    if rising_edge(clk) then
      if(we(0)='1') then
        RAM00(n) <= di(7 downto 0);
      end if;
      if(we(1)='1') then
        RAM01(n) <= di(15 downto 8);
      end if;
      if(we(2)='1') then
        RAM02(n) <= di(23 downto 16);
      end if;
      if(we(3)='1') then
        RAM03(n) <= di(31 downto 24);
      end if;
      
      rbAdr <= addr; 
    end if;
  end process;

  do <= RAM03(conv_integer(addr))
   & RAM02(conv_integer(addr))
   & RAM01(conv_integer(addr))
   & RAM00(conv_integer(addr));
end;
