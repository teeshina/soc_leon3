#pragma once

#include "stdtypes.h"
#include "dff.h"
#include "leon3_ml605\leon3\techmult.h"
#include "leon3_ml605\leon3\arith.h"

class mul32
{
  friend class dbg;
  private:
    struct mul_regtype
    {
      uint64 acc;//    : std_logic_vector(63 downto 0);
      uint32 state  : 2;//std_logic_vector(1 downto 0);
      uint32 start  : 1;//std_logic;
      uint32 ready  : 1;//std_logic;
      uint32 nready : 1;//std_logic;
    };

    struct mac_regtype
    {
      uint32 mmac, xmac;//    : std_logic;
      uint32 msigned, xsigned;// : std_logic;
    };

    TDFF<mul_regtype> rm;
    mul_regtype rmin;// : mul_regtype;
    mac_regtype mm, mmin;// : mac_regtype;
    uint64 ma, mb;// : std_logic_vector(32 downto 0);
    uint64 prod[2];// : std_logic_vector(65 downto 0);
    uint64 mreg;// : std_logic_vector(49 downto 0);
    uint32 vcc;// : std_logic;
    
    
 
    uint64 mop1, mop2;// : std_logic_vector(32 downto 0);
    uint64 acc, acc1, acc2;// : std_logic_vector(48 downto 0);
    uint32 zero, rsigned, rmac;// : std_logic;
    mul_regtype v;// : mul_regtype;
    mac_regtype w;// : mac_regtype;
    
    techmult m3232;

  public:
    void Update( uint32 rst,//     : in  std_ulogic;
                 SClock clk,//     : in  std_ulogic;
                 uint32 holdn,//   : in  std_ulogic;
                 mul32_in_type &muli,//    : in  mul32_in_type;
                 mul32_out_type &mulo);//    : out mul32_out_type

    void ClkUpdate()
    {
      rm.ClkUpdate();
      m3232.ClkUpdate();
    }
};


