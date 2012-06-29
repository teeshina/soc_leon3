#pragma once

#include "stdtypes.h"
#include "dff.h"
#include "leon3_ml605\config.h"


//******************************************************************
class gen_mult_pipe
{
  friend class dbg;
  private:

    struct pipet
    {
      uint64 arr2[CFG_MULPIPE+1][2]; // is array (num_stages-1 downto 1) of resw;
    };
    TDFF<pipet> p_i;// : pipet;
    uint64 prod[2];// :  resw;
 
 public:
    void Update( SClock clk,//     : in  std_logic;          -- register clock
                  uint32 en,//      : in  std_logic;          -- register enable
                  uint32 tc,//      : in  std_logic;          -- '0' : unsigned, '1' : signed
                  uint64 a,//       : in  std_logic_vector(a_width-1 downto 0);  -- multiplier
                  uint64 b,//       : in  std_logic_vector(b_width-1 downto 0);  -- multiplicand
                  uint64 *product);// : out std_logic_vector(a_width+b_width-1 downto 0));  -- product
                  
    void ClkUpdate()
    { p_i.ClkUpdate(); }

};

//******************************************************************
class techmult
{
  friend class dbg;
  private:
     uint32 gnd, vcc;//       : std_ulogic;
    uint32 pres;// : std_ulogic := '0';
    uint32 sonly;// : std_ulogic := '0';

    gen_mult_pipe dwm;
  public:
  
    void Update(uint64 a,//       : in std_logic_vector(a_width-1 downto 0);  
                uint64 b,//       : in std_logic_vector(b_width-1 downto 0);
                SClock clk,//     : in std_logic;     
                uint32 en,//      : in std_logic;     
                uint32 sign,//    : in std_logic;     
                uint64 *product);// : out std_logic_vector(a_width+b_width-1 downto 0));
    
    void ClkUpdate()
    {
      dwm.ClkUpdate();
    }
    
};
    

