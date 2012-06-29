#pragma once

#include "stdtypes.h"
#include "dff.h"
#include "leon3_ml605\leon3\mmuconfig.h"
#include "leon3_ml605\leon3\mmuiface.h"

class mmutlbcam
{
  friend class dbg;
  private:
    uint32 tlb_type;
  private:
    struct tlbcam_rtype
    {
      tlbcam_reg btag;//     : tlbcam_reg;
    };
    TDFF<tlbcam_rtype> r;
    //tlbcam_rtype c;// : tlbcam_rtype;

    tlbcam_rtype v;// : tlbcam_rtype;
    uint32 hm : 1;
    uint32 hf : 1;//               : std_logic;
    uint32 h_i1 : 1;
    uint32 h_i2 : 1;
    uint32 h_i3 : 1;
    uint32 h_c : 1;//std_logic;
    uint32 h_l2 : 1;
    uint32 h_l3 : 1;//           : std_logic;
    uint32 h_su_cnt              : 1;//std_logic;
    uint32 blvl                  : 2;//std_logic_vector(1 downto 0);
    uint32 bet                   : 2;//std_logic_vector(1 downto 0);
    uint32 bsu                   : 1;//std_logic;
    uint32 blvl_decode           : 4;//std_logic_vector(3 downto 0);
    uint32 bet_decode            : 4;//std_logic_vector(3 downto 0);
    uint32 ref : 1;
    uint32 modified         : 1;//std_logic;
    uint32 tlbcamo_pteout;//        : std_logic_vector(31 downto 0);
    uint32 tlbcamo_LVL           : 2;//std_logic_vector(1 downto 0);
    uint32 tlbcamo_NEEDSYNC      : 1;//std_logic;
    uint32 tlbcamo_WBNEEDSYNC    : 1;//std_logic;
    uint32 vaddr_r;//                 : std_logic_vector(31 downto 12);
    uint32 vaddr_i;//                 : std_logic_vector(31 downto 12);
    uint32 pagesize;// : integer range 0 to 3;

  public:
    mmutlbcam(uint32 tlb_type_=1);
    
    void Update(uint32 rst,//     : in std_logic;
                SClock clk,//     : in std_logic;
                mmutlbcam_in_type &tlbcami,// : in mmutlbcam_in_type;
                mmutlbcam_out_type &tlbcamo);// : out mmutlbcam_out_type
    
    void ClkUpdate()
    {
      r.ClkUpdate();
    }
};

