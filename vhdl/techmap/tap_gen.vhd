  -----------------------------------------------------------------------------   
-- Entity:      tap_gen
-- File:        tap_gen.vhd
-- Author:      Sergey Khabarov - GNSS sensor ltd
-- Description: TAP controller technology wrapper
------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
library techmap;
use techmap.gencomp.all;
library grlib;
use grlib.stdlib.all;

entity tap_gen is
  generic ( 
    irlen  : integer range 2 to 8 := 2;
    idcode : integer range 0 to 255 := 9;
    manf   : integer range 0 to 2047 := 804;
    part   : integer range 0 to 65535 := 0;
    ver    : integer range 0 to 15 := 0;
    trsten : integer range 0 to 1 := 1;    
    scantest : integer := 0);
  port (
    trst        : in std_ulogic;
    tckp        : in std_ulogic;
    tckn        : in std_ulogic;
    tms         : in std_ulogic;
    tdi         : in std_ulogic;
    tdo         : out std_ulogic;
    tapi_en1    : in std_ulogic;
    tapi_tdo1   : in std_ulogic;
    tapi_tdo2   : in std_ulogic;
    tapo_tck    : out std_ulogic;
    tapo_tdi    : out std_ulogic;
    tapo_inst   : out std_logic_vector(7 downto 0);
    tapo_rst    : out std_ulogic;
    tapo_capt   : out std_ulogic;
    tapo_shft   : out std_ulogic;
    tapo_upd    : out std_ulogic;
    tapo_xsel1  : out std_ulogic;
    tapo_xsel2  : out std_ulogic;
    testen      : in std_ulogic := '0';
    testrst     : in std_ulogic := '1';
    tdoen       : out std_ulogic
    );
end;


architecture rtl of tap_gen is
  constant IR  : integer  := 0;
  constant DR  : integer  := 1;
  
  constant JTAG_INSTRUCTION_WIDTH : integer := 10;
  constant JTAG_INSTRUCTION_USER1 : std_ulogic_vector(JTAG_INSTRUCTION_WIDTH-1 downto 0) := "1111000010";--16#3C2#;  -- instruction to accept address (35 bits)
  constant JTAG_INSTRUCTION_USER2 : std_ulogic_vector(JTAG_INSTRUCTION_WIDTH-1 downto 0) := "1111000011";--16#3C3#;  -- instruction to accept data (33 bits)

  type reg_type is record
    ResetLogic  : std_ulogic;
    RunTestIdle : std_ulogic;
    SelectScan : std_ulogic_vector(1 downto 0);
    Capture    : std_ulogic_vector(1 downto 0);
    Shift      : std_ulogic_vector(1 downto 0);
    Exit1      : std_ulogic_vector(1 downto 0);
    Pause      : std_ulogic_vector(1 downto 0);
    Exit2      : std_ulogic_vector(1 downto 0);
    Update     : std_ulogic_vector(1 downto 0);
    IR         : std_ulogic_vector(JTAG_INSTRUCTION_WIDTH-1 downto 0);
    SelUser1   : std_ulogic;
    SelUser2   : std_ulogic;
  end record;

  constant r_zero : reg_type := ( '0', '0', "00","00","00","00","00","00","00","0000000000",'0','0');

  signal r, rin : reg_type;
  

begin
  
  -- data/instruction loop select:
  
  comb : process (trst, tckp, tckn, tms, tdi, tapi_tdo1, r)
    variable v : reg_type;
  begin
    v := r;
    
    if((trst = '0') ) then          v.ResetLogic := '1';
    elsif((r.ResetLogic and tms) = '1')     then v.ResetLogic := '1';
    elsif((r.SelectScan(IR) and tms) = '1') then v.ResetLogic := '1';
    else                                         v.ResetLogic := '0'; end if;

    if(trst = '0') then                                             v.RunTestIdle := '0';
    elsif((r.ResetLogic='1') and (tms='0')) then                    v.RunTestIdle := '1';
    elsif((r.RunTestIdle='1') and (tms='0')) then                   v.RunTestIdle := '1';
    elsif(((r.Update(DR) or r.Update(IR))='1') and (tms='0')) then  v.RunTestIdle := '1';
    else                                                            v.RunTestIdle := '0'; end if;
      
    if(trst = '0') then                                         v.SelectScan(DR) := '0';
    elsif((r.RunTestIdle and tms)='1') then                     v.SelectScan(DR) := '1';
    elsif(((r.Update(DR) or r.Update(IR)) and tms) = '1') then  v.SelectScan(DR) := '1';
    else                                                        v.SelectScan(DR) := '0'; end if;

    if(trst = '0') then                          v.SelectScan(IR) := '0';
    elsif((r.SelectScan(DR) and tms) = '1') then v.SelectScan(IR) := '1';
    else                                         v.SelectScan(IR) := '0'; end if;
      
      
    -- Data/Instruction transmition:
    for i in 0 to 1 loop
      if(trst = '0') then                             v.Capture(i) := '0';
      elsif((r.SelectScan(i)='1') and (tms='0')) then v.Capture(i) := '1';
      else                                            v.Capture(i) := '0'; end if;
        
      if(trst = '0') then                          v.Shift(i) := '0';
      elsif((r.Capture(i)='1') and (tms='0')) then v.Shift(i) := '1';
      elsif((r.Shift(i)='1') and (tms='0')) then   v.Shift(i) := '1';
      elsif((r.Exit2(i)='1') and (tms='0')) then   v.Shift(i) := '1';
      else                                         v.Shift(i) := '0'; end if;

      if(trst = '0') then                    v.Exit1(i) := '0';
      elsif((r.Shift(i) and tms)='1') then   v.Exit1(i) := '1';
      elsif((r.Capture(i) and tms)='1') then v.Exit1(i) := '1';
      else                                   v.Exit1(i) := '0'; end if;
        
      if(trst = '0') then                      v.Pause(i) := '0';
      elsif((r.Exit1(i)='1') and tms='0') then v.Pause(i) := '1';
      elsif((r.Pause(i)='1') and tms='0') then v.Pause(i) := '1';
      else                                     v.Pause(i) := '0'; end if;
        
      if(trst = '0') then                  v.Exit2(i) := '0';
      elsif((r.Pause(i) and tms)='1') then v.Exit2(i) := '1';
      else                                 v.Exit2(i) := '0'; end if;
        
      if(trst = '0') then                  v.Update(i) := '0';
      elsif((r.Exit2(i) and tms)='1') then v.Update(i) := '1';
      elsif((r.Exit1(i) and tms)='1') then v.Update(i) := '1';
      else                                 v.Update(i) := '0'; end if;
    end loop;

    -- Fill Instruction register:
    if(trst='0') then             v.IR := (others => '0');
    elsif(r.Capture(IR)='1') then v.IR := (others => '0');
    elsif(r.Shift(IR)='1')   then v.IR := (tdi & r.IR((JTAG_INSTRUCTION_WIDTH-1) downto 1)); end if;
      
    if(trst = '0') then                                            v.SelUser1 := '0';
    elsif((r.Update(IR)='1')and(r.IR=JTAG_INSTRUCTION_USER1)) then v.SelUser1 := '1';
    elsif((r.Update(IR) or r.Capture(IR)) = '1') then              v.SelUser1 := '0'; end if;

    if(trst = '0') then                                            v.SelUser2 := '0';
    elsif((r.Update(IR)='1')and(r.IR=JTAG_INSTRUCTION_USER2)) then v.SelUser2 := '1';
    elsif((r.Update(IR) or r.Capture(IR)) = '1') then              v.SelUser2 := '0'; end if;

    rin <= v;

    if((r.SelUser1 or r.SelUser2)='1') then tapo_tck <= tckp;
    else                                    tapo_tck <= '1'; end if;

  end process;
  
  reg : process(tckp)
  begin 
    if rising_edge(tckp) then r <= rin; end if; 
  end process;


  tapo_xsel1 <= r.SelUser1;
  tapo_xsel2 <= r.SelUser2;
  -- other BSCAN_VIRTEX6 primitive outputs:
  tapo_capt  <= r.Capture(DR);
  tapo_tdi   <= TDI;
  tapo_shft  <= r.Shift(DR);
  tapo_upd   <= r.Update(DR);
  tapo_rst   <= r.ResetLogic;

  tdo <= tapi_tdo1;
  
end;  
