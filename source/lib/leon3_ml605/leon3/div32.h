#pragma once
#include "stdtypes.h"
#include "dff.h"
#include "leon3_ml605\leon3\arith.h"


class div32
{
  friend class dbg;
  private:
    struct div_regtype
    {
      uint64 x[2];//      : std_logic_vector(64 downto 0);
      uint32 state  : 3;//std_logic_vector(2 downto 0);
      uint32 zero   : 1;//std_logic;
      uint32 zero2  : 1;//std_logic;
      uint32 qcorr  : 1;//std_logic;
      uint32 zcorr  : 1;//std_logic;
      uint32 qzero  : 1;//std_logic;
      uint32 qmsb   : 1;//std_logic;
      uint32 ovf    : 1;//std_logic;
      uint32 neg    : 1;//std_logic;
      uint32 cnt    : 5;// std_logic_vector(4 downto 0);
    };

    TDFF<div_regtype> r;
    div_regtype rin;// : div_regtype;
    uint64 addin1, addin2, addout;//: std_logic_vector(32 downto 0);
    uint32 addsub;// : std_logic;
    
    div_regtype v;// : div_regtype;
    uint32 vready, vnready;// : std_logic;
    uint64 vaddin1, vaddin2;// : std_logic_vector(32 downto 0);
    uint32 vaddsub, ymsb;// : std_logic;


  public:
    void Update(uint32 rst,//     : in  std_ulogic;
                    SClock clk,//     : in  std_ulogic;
                    uint32 holdn,//   : in  std_ulogic;
                    div32_in_type &divi,//    : in  div32_in_type;
                    div32_out_type &divo);//    : out div32_out_type
    
    void ClkUpdate()
    {
      r.ClkUpdate();
    }
};
