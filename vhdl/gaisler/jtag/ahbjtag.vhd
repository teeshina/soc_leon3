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
-- Entity:      ahbjtag
-- File:        ahbjtag.vhd
-- Author:      Edvin Catovic, Jiri Gaisler - Gaisler Research
-- Description: JTAG communication link with AHB master interface
------------------------------------------------------------------------------  
-- GRLIB2 CORE
-- VENDOR:      VENDOR_GAISLER
-- DEVICE:      GAISLER_AHBJTAG
-- VERSION:     1
-- AHBMASTER:   0
-------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
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

entity ahbjtag is
  generic (
    tech    : integer range 0 to NTECH := 0;
    hindex  : integer := 0;
    nsync : integer range 1 to 2 := 1;
    idcode : integer range 0 to 255 := 9;    
    manf   : integer range 0 to 2047 := 804;
    part   : integer range 0 to 65535 := 0;
    ver    : integer range 0 to 15 := 0;
    ainst   : integer range 0 to 255 := 2;
    dinst   : integer range 0 to 255 := 3;
    scantest : integer := 0);
  port (
    rst         : in  std_ulogic;
    clk         : in  std_ulogic;
    inJtagTCK   : in  std_ulogic_vector(1 downto 0);
    inJtagTDI   : in  std_ulogic_vector(1 downto 0);
    inJtagSel   : in  std_ulogic_vector(1 downto 0);
    inJtagShift : in  std_ulogic_vector(1 downto 0);
    inJtagUpdate: in  std_ulogic_vector(1 downto 0);
    outJtagTDO  : out std_ulogic;
    ahbi        : in  ahb_mst_in_type;
    ahbo        : out ahb_mst_out_type;
    inJtagRESET : in std_ulogic_vector(1 downto 0);
    tdoen       : out std_ulogic
    );
end;      

architecture struct of ahbjtag is

-- Set REREAD to 1 to include support for re-read operation when host reads
-- out data register before jtagcom has completed the current AMBA access and
-- returned to state 'shft'.
constant REREAD : integer := 1;

constant REVISION : integer := REREAD;
constant TAPSEL   : integer := has_tapsel(tech);

signal dmai : ahb_dma_in_type;
signal dmao : ahb_dma_out_type;
signal ltapi : tap_in_type;
signal ltapo : tap_out_type;

begin

  ahbmst0 : ahbmst 
    generic map (hindex => hindex, venid => VENDOR_GAISLER,
                 devid => GAISLER_AHBJTAG, version => REVISION)
    port map (rst, clk, dmai, dmao, ahbi, ahbo);

  tap0 : tap generic map (tech => tech, irlen => 6, idcode => idcode, 
	manf => manf, part => part, ver => ver, scantest => scantest)
    port map (inJtagRESET, 
              inJtagTCK, 
              inJtagTDI, 
              inJtagSel,
              inJtagShift,
              inJtagUpdate,
              outJtagTDO, 
              ltapo.tck, ltapo.tdi, ltapo.inst, ltapo.reset, ltapo.capt,
              ltapo.shift, ltapo.upd, ltapo.asel, ltapo.dsel, ltapi.en, ltapi.tdo,
              ahbi.testen,
              ahbi.testrst,
              tdoen);
  
  jtagcom0 : jtagcom generic map (isel => TAPSEL, nsync => nsync, ainst => ainst, dinst => dinst, reread => REREAD)
    port map (rst, clk, ltapo, ltapi, dmao, dmai);

  
-- pragma translate_off
    bootmsg : report_version 
    generic map ("ahbjtag AHB Debug JTAG rev " & tost(REVISION));
-- pragma translate_on

end;
