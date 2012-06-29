#pragma once

#include "stdtypes.h"
#include "dff.h"
#include "leon3_ml605\config.h"
#include "leon3_ml605\amba\amba.h"
#include "leon3_ml605\leon3\libiu.h"
#include "leon3_ml605\leon3\libcache.h"
#include "leon3_ml605\leon3\leon3.h"

#include "leon3_ml605\leon3\proc3.h"
#include "leon3_ml605\leon3\rfile_3p.h"
#include "leon3_ml605\leon3\cachemem.h"
#include "leon3_ml605\leon3\tbufmem.h"

class leon3s
{
  friend class dbg;
  private:
  uint32 holdn;// : std_logic;
  iregfile_in_type   rfi;//   : iregfile_in_type;
  iregfile_out_type  rfo;//   : iregfile_out_type;
  cram_in_type       crami;// : cram_in_type;
  cram_out_type      cramo;// : cram_out_type;
  tracebuf_in_type   tbi;//   : tracebuf_in_type;
  tracebuf_out_type  tbo;//   : tracebuf_out_type;
  TDFF<uint32>       rst;//   : std_ulogic;
  fpc_in_type        fpi;//   : fpc_in_type;
  fpc_out_type       fpo;//   : fpc_out_type;
  fpc_in_type        cpi;//   : fpc_in_type;
  fpc_out_type       cpo;//   : fpc_out_type;
  fpc_debug_out_type cpodb;// : fpc_debug_out_type;

  uint64 rd1, rd2, wd;// : std_logic_vector(35 downto 0);
  uint32 gnd, vcc;// : std_logic;
  SClock clk_gnd;
  
  proc3      *pclProc3;
  regfile_3p clRegfile3Port;
  cachemem   clCacheMem;
  tbufmem    *pclTBufMem;

  public:
    leon3s(uint32 mst_index=AHB_MASTER_LEON3);
    ~leon3s();
    void Update(  SClock clk,//    : in  std_ulogic;
                  uint32 rstn,//   : in  std_ulogic;
                  ahb_mst_in_type &ahbi,//   : in  ahb_mst_in_type;
                  ahb_mst_out_type &ahbo,//   : out ahb_mst_out_type;
                  ahb_slv_in_type &ahbsi,//  : in  ahb_slv_in_type;
                  ahb_slv_out_vector &ahbso,//  : in  ahb_slv_out_vector;
                  l3_irq_in_type &irqi,//   : in  l3_irq_in_type;
                  l3_irq_out_type &irqo,//   : out l3_irq_out_type;
                  l3_debug_in_type &dbgi,//   : in  l3_debug_in_type;
                  l3_debug_out_type &dbgo//   : out l3_debug_out_type
                );
    
    void ClkUpdate()
    {
      rst.ClkUpdate();
      pclProc3->ClkUpdate();
      clRegfile3Port.ClkUpdate();
      clCacheMem.ClkUpdate();
      pclTBufMem->ClkUpdate();
    }
};
