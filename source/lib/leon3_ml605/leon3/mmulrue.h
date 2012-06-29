#pragma once

#include "stdtypes.h"
#include "dff.h"
#include "leon3_ml605\leon3\mmuconfig.h"
#include "leon3_ml605\leon3\mmuiface.h"

class mmulrue
{
  friend class dbg;
  private:
    uint32 position;
    uint32 entries;
  private:
    struct lru_rtype
    {
      uint32 pos;//      : std_logic_vector(entries_log-1 downto 0);
      uint32 movetop  : 1;//std_logic;
      //-- pragma translate_off
      uint32 dummy  : 1;//std_logic;
      //-- pragma translate_on
    };

    lru_rtype c;//   : lru_rtype;
    TDFF<lru_rtype> r;//   : lru_rtype;
    lru_rtype v;// : lru_rtype;
    mmulrue_out_type ov;// : mmulrue_out_type;

  public:
    mmulrue(uint32 position_, uint32 entries_)
    {
      position = position_;
      entries  = entries_;
    }
    void Update(uint32 rst,//    : in std_logic;
                SClock clk,//    : in std_logic;
                mmulrue_in_type &lruei,//  : in mmulrue_in_type;
                mmulrue_out_type &lrueo);//  : out mmulrue_out_type );
    
    void ClkUpdate()
    {
      r.ClkUpdate();
    }
};
