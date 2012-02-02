#include "lheaders.h"

void proc3::Update( SClock clk,//    : in  std_ulogic;
                    uint32 rstn,//   : in  std_ulogic;
                    uint32 &holdn,//  : out std_ulogic;
                    ahb_mst_in_type &ahbi,//   : in  ahb_mst_in_type;
                    ahb_mst_out_type &ahbo,//   : out ahb_mst_out_type;
                    ahb_slv_in_type &ahbsi,//  : in  ahb_slv_in_type;
                    ahb_slv_out_vector &ahbso,//  : in  ahb_slv_out_vector;
                    iregfile_in_type &rfi,//    : out iregfile_in_type;
                    iregfile_out_type &rfo,//    : in  iregfile_out_type;
                    cram_in_type &crami,//  : out cram_in_type;
                    cram_out_type &cramo,//  : in  cram_out_type;
                    tracebuf_in_type &tbi,//    : out tracebuf_in_type;
                    tracebuf_out_type &tbo,//    : in  tracebuf_out_type;
                    fpc_in_type &fpi,//    : out fpc_in_type;
                    fpc_out_type &fpo,//    : in  fpc_out_type;
                    fpc_in_type &cpi,//    : out fpc_in_type;
                    fpc_out_type &cpo,//    : in  fpc_out_type;
                    l3_irq_in_type &irqi,//   : in  l3_irq_in_type;
                    l3_irq_out_type &irqo,//   : out l3_irq_out_type;
                    l3_debug_in_type &dbgi,//   : in  l3_debug_in_type;
                    l3_debug_out_type &dbgo,//   : out l3_debug_out_type;
                    SClock hclk,//
                    SClock sclk,//   : in std_ulogic;
                    uint32 hclken//   : in std_ulogic
                )
{
  holdnx = ico.hold & dco.hold & fpo.holdn;
  holdn  = holdnx;
  pholdn = fpo.holdn;

  // integer unit
  clIU3.Update(clk, rstn, holdnx, ici, ico, dci, dco, rfi, rfo, irqi, irqo,
                 dbgi, dbgo, muli, mulo, divi, divo, fpo, fpi, cpo, cpi, tbo, tbi, sclk);

  // multiply and divide units

#if (CFG_V8 != 0)
  clMul32.Update(rstn, clk, holdnx, muli, mulo);
  clDiv32.Update(rstn, clk, holdnx, divi, divo);
#else
  divo <= ('0', '0', "0000", zero32);
  mulo <= ('0', '0', "0000", zero32&zero32);
#endif

//-- cache controller

  clCacheMMU.Update(rstn, clk, ici, ico, dci, dco,
    ahbi, ahbo, ahbsi, ahbso, crami, cramo, pholdn, hclk, sclk, hclken);
}
