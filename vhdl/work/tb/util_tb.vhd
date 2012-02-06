library ieee;
use ieee.std_logic_1164.all;
library std;
use std.textio.all;

package util_tb is

function StringToUVector(inStr: string) return std_ulogic_vector;
function StringToSVector(inStr: string) return std_logic_vector;
function UnsignedToSigned(inUnsigned: std_ulogic_vector) return std_logic_vector;

end;

package body util_tb is

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
  
  function StringToSVector(inStr: string) return std_logic_vector is
    variable temp: std_logic_vector(inStr'range) := (others => 'X');
  begin
    for i in inStr'range loop --
      if(inStr(inStr'high-i+1)='1')    then temp(i) := '1';
      elsif(inStr(inStr'high-i+1)='0') then temp(i) := '0';
      end if;
    end loop;
    return temp(inStr'high downto 1);
  end function StringToSVector;
  

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

end;
