#pragma once

#include "stdtypes.h"

struct div32_in_type
{
  uint64 y;//                : std_logic_vector(32 downto 0); -- Y (MSB divident)
  uint64 op1;//              : std_logic_vector(32 downto 0); -- operand 1 (LSB divident)
  uint64 op2;//              : std_logic_vector(32 downto 0); -- operand 2 (divisor)
  uint32 flush;//            : std_logic;
  uint32 Signed;//           : std_logic;
  uint32 start;//            : std_logic;
};

struct div32_out_type
{
  uint32 ready;//           : std_logic;
  uint32 nready;//          : std_logic;
  uint32 icc;//             : std_logic_vector(3 downto 0); -- ICC
  uint32 result;//          : std_logic_vector(31 downto 0); -- div result
};

struct mul32_in_type
{
  uint64 op1;//              : std_logic_vector(32 downto 0); -- operand 1
  uint64 op2;//              : std_logic_vector(32 downto 0); -- operand 2
  uint32 flush;//            : std_logic;
  uint32 Signed;//           : std_logic;
  uint32 start;//            : std_logic;
  uint32 mac;//              : std_logic;
  uint64 acc;//              : std_logic_vector(39 downto 0);
  //--y                : std_logic_vector(7 downto 0); -- Y (MSB MAC register)
  //--asr18           : std_logic_vector(31 downto 0); -- LSB MAC register
};

struct mul32_out_type
{
  uint32 ready;//           : std_logic;
  uint32 nready;//          : std_logic;
  uint32 icc;//             : std_logic_vector(3 downto 0); -- ICC
  uint64 result;//          : std_logic_vector(63 downto 0); -- mul result
};

