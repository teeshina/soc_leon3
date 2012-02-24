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
Library UNISIM;
use UNISIM.vcomponents.all;
library techmap;
use techmap.gencomp.all;
library grlib;
use grlib.stdlib.all;

entity tap is
  generic ( tech   : integer := 0);
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
  signal drck1, drck2, sel1, sel2 : std_ulogic;
  signal capt1, capt2, rst1, rst2 : std_ulogic;
  signal shift1, shift2, tdi1, tdi2 : std_ulogic;
  signal update1, update2 : std_ulogic;

begin
  
  -- FPGA image:
  jtagtap_fpga : if tech=virtex6 generate
    u0 : BSCAN_VIRTEX6 
      generic map (JTAG_CHAIN => 1) 
      port map (
        CAPTURE => capt1, 
        DRCK => drck1,
        RESET => rst1, 
        SEL => sel1, 
        SHIFT => shift1, 
        TDI => tdi1, 
        UPDATE => update1,
        TDO => inTDO
        );

    u1 : BSCAN_VIRTEX6 
      generic map (JTAG_CHAIN => 2) 
      port map (
        CAPTURE => capt2, 
        DRCK => drck2,
        RESET => rst2, 
        SEL => sel2, 
        SHIFT => shift2, 
        TDI => tdi2, 
        UPDATE => update2,
        TDO => inTDO
        );  

    outTCK  <= drck1 when sel1 = '1' else drck2;
    outRESET  <= rst1 when sel1 = '1' else rst2;
    outShift <= shift1 when sel1 = '1' else shift2;
    outTDI  <= tdi1  when sel1 = '1' else tdi2;
    outUpdate  <= update1 when sel1 ='1' else update2;
    outAdrSel <= sel1; 
    outDataSel <= sel2;
  end generate;
  
  -- Model sim RTL image:
  jtagtap_rtl : if tech/=virtex6 generate
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

    outAdrSel  <= inJtagSel(0);
    outDataSel <= inJtagSel(1);
    outJtagTDO <= inTDO;
  end generate;
  
end;  
