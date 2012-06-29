#pragma once

#include "stdtypes.h"
#include "dff.h"
#include "leon3_ml605\leon3\mem_unisim.h"


class regfile_3p
{
  friend class dbg;
  private:
    #define regfile_3p_inferred
#ifdef regfile_3p_inferred
    generic_regfile_3p *pclMem3p;
#else
    unisim_syncram_2p *x0;  // WARNING!! unisim, not a syncram_2p
    unisim_syncram_2p *x1;
#endif
  public:
    regfile_3p();
    ~regfile_3p();
    
    void Update( SClock wclk,//   : in  std_ulogic;
                uint32 waddr,//  : in  std_logic_vector((abits -1) downto 0);
                uint32 wdata,//  : in  std_logic_vector((dbits -1) downto 0);
                uint32 we,//     : in  std_ulogic;
                SClock rclk,//   : in  std_ulogic;
                uint32 raddr1,// : in  std_logic_vector((abits -1) downto 0);
                uint32 re1,//    : in  std_ulogic;
                uint32 &rdata1,// : out std_logic_vector((dbits -1) downto 0);
                uint32 raddr2,// : in  std_logic_vector((abits -1) downto 0);
                uint32 re2,//    : in  std_ulogic;
                uint32 &rdata2,// : out std_logic_vector((dbits -1) downto 0);
                uint32 testin//   : in std_logic_vector(3 downto 0) := "0000");
               );
    
    void ClkUpdate()
    {
#ifdef regfile_3p_inferred
      pclMem3p->ClkUpdate();
#else
      x0->ClkUpdate();
      x1->ClkUpdate();
#endif
    }
};
