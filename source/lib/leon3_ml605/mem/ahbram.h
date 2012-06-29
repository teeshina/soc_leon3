#pragma once

#include "stdtypes.h"
#include "dff.h"
#include "leon3_ml605\config.h"
#include "leon3_ml605\amba\amba.h"
#include "leon3_ml605\leon3\mem_unisim.h"

class ahbram
{
  friend class dbg;
    uint32 hindex;
    uint32 addr;
    uint32 hmask;
    uint32 kbytes;
  private:
    struct reg_type
    {
      uint32 hwrite : 1;//std_ulogic;
      uint32 hready : 1;//std_ulogic;
      uint32 hsel   : 1;//std_ulogic;
      uint32 addr;//   : std_logic_vector(abits-1+log2(dw/8) downto 0);
      uint32 size;//   : std_logic_vector(2 downto 0);
      uint32 prdata;// : std_logic_vector((dw-1)*pipe downto 0);
    };

    reg_type c;//     : reg_type;
    TDFF<reg_type> r;
    uint32 ramsel   : 1;//std_ulogic;
    uint32 write;//    : std_logic_vector(dw/8-1 downto 0);
    uint32 ramaddr;//  : std_logic_vector(abits-1 downto 0);
    uint32 ramdata;//  : std_logic_vector(dw-1 downto 0);
    uint32 hwdata;//   : std_logic_vector(dw-1 downto 0);

    uint32 bs;//      : std_logic_vector(dw/8-1 downto 0);
    reg_type v;//       : reg_type;
    uint32 haddr;//   : std_logic_vector(abits-1 downto 0);
    uint32 hrdata;//  : std_logic_vector(dw-1 downto 0);
    uint32 seldata;// : std_logic_vector(dw-1 downto 0);
    uint32 raddr;//   : std_logic_vector(3 downto 2);
    syncram **ppSyncram;

  public:
    ahbram(uint32 hindex_,
           uint32 addr_=CFG_AHBRAMADDR,
           uint32 hmask_=CFG_AHBRAMMASK);
    ~ahbram();

    void Update( uint32 rst,//     : in  std_ulogic;
                SClock clk,//     : in  std_ulogic;
                ahb_slv_in_type &ahbsi,//   : in  ahb_slv_in_type;
                ahb_slv_out_type &ahbso//   : out ahb_slv_out_type
              );

    void ClkUpdate()
    {
      r.ClkUpdate();
      for (uint32 i=0; i<=(AHBDW/8-1); i++)
        ppSyncram[i]->ClkUpdate();
    }
};
