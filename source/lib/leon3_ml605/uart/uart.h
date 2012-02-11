#pragma once

struct uart_in_type
{
  uint32 rxd;//   	: std_ulogic;
  uint32 ctsn;//   	: std_ulogic;
  uint32 extclk;//	: std_ulogic;
};

struct uart_out_type
{
  uint32 rtsn;//   	: std_ulogic;
  uint32 txd;//   	: std_ulogic;
  uint32 scaler;//	: std_logic_vector(31 downto 0);
  uint32 txen;//     	: std_ulogic;
  uint32 flow;//   	: std_ulogic;
  uint32 rxen;//     	: std_ulogic;
};

