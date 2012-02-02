------------------------------------------------------------------------------
--  This file is a part of the GRLIB VHDL IP LIBRARY
--  Copyright (C) 2003 - 2008, Gaisler Research
--  Copyright (C) 2008 - 2010, Aeroflex Gaisler
--
--  This program is free software; you can redistribute it and/or modify
--  it under the terms of the GNU General Public License as published by
--  the Free Software Foundation; either version 2 of the License, or
--  (at your option) any later version.
--
--  This program is distributed in the hope that it will be useful,
--  but WITHOUT ANY WARRANTY; without even the implied warranty of
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--  GNU General Public License for more details.
--
--  You should have received a copy of the GNU General Public License
--  along with this program; if not, write to the Free Software
--  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
-----------------------------------------------------------------------------   
-- Entity:      tap
-- File:        tap.vhd
-- Author:      Edvin Catovic - Gaisler Research
-- Description: TAP controller technology wrapper
------------------------------------------------------------------------------


library ieee;
use ieee.std_logic_1164.all;
library techmap;
use techmap.gencomp.all;
--use techmap.alltap.all;
library grlib;
use grlib.stdlib.all;

entity tap is
  generic (
    tech   : integer := 0;    
    irlen  : integer range 2 to 8 := 4;
    idcode : integer range 0 to 255 := 9;
    manf   : integer range 0 to 2047 := 804;
    part   : integer range 0 to 65535 := 0;
    ver    : integer range 0 to 15 := 0;
    trsten : integer range 0 to 1 := 1;
    scantest : integer := 0);
  port (
    inJtagRESET : in std_ulogic_vector(1 downto 0);
    inJtagTCK   : in std_ulogic_vector(1 downto 0);
    inJtagTDI   : in  std_ulogic_vector(1 downto 0);
    inJtagSel   : in std_ulogic_vector(1 downto 0);
    inJtagShift : in  std_ulogic_vector(1 downto 0);
    inJtagUpdate: in  std_ulogic_vector(1 downto 0);
    outJtagTDO  : out std_ulogic;
    outTCK      : out std_ulogic;
    outTDI      : out std_ulogic;
    tapo_inst   : out std_logic_vector(7 downto 0);
    outRESET    : out std_ulogic;--tapo_rst    : out std_ulogic;
    tapo_capt   : out std_ulogic;
    outShift    : out std_ulogic;
    outUpdate   : out std_ulogic;
    outAdrSel   : out std_ulogic;
    outDataSel  : out std_ulogic;
    tapi_en1    : in std_ulogic;
    inTDO       : in std_ulogic;
    testen      : in std_ulogic := '0';
    testrst     : in std_ulogic := '1';
    tdoen       : out std_ulogic
    );
end;


architecture rtl of tap is
  --shared variable ind : integer := 0;
begin
  outAdrSel <= inJtagSel(0);
  outDataSel <= inJtagSel(1);
  
  mux : process (inJtagRESET, inJtagTCK, inJtagTDI, inJtagSel, inJtagShift, inJtagUpdate)
    variable ind : integer := 0;
  begin
    if (inJtagSEL(0)='1') then ind := 0;
    else ind := 1;  end if;
    outRESET <= inJtagRESET(ind);
    outTCK   <= inJtagTCK(ind);
    outTDI   <= inJtagTDI(ind);
    outShift <= inJtagShift(ind);
    outUpdate <= inJtagUpdate(ind);
  end process mux;

  
  outJtagTDO <= inTDO;
  
end;  
