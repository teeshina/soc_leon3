#include "lheaders.h"


//****************************************************************************
void ahbrom::Update(uint32 rst,//     : in  std_ulogic;
                    SClock clk,//     : in  std_ulogic;
                    ahb_slv_in_type &ahbsi,//   : in  ahb_slv_in_type;
                    ahb_slv_out_type &ahbso)//   : out ahb_slv_out_type
{
  ((ahb_device_reg*)(&ahbso.hconfig.arr[0]))->vendor  = VENDOR_GAISLER;
  ((ahb_device_reg*)(&ahbso.hconfig.arr[0]))->device  = GAISLER_AHBROM;
  ((ahb_device_reg*)(&ahbso.hconfig.arr[0]))->version = 0;
  ((ahb_device_reg*)(&ahbso.hconfig.arr[0]))->cfgver  = 0;
  ((ahb_device_reg*)(&ahbso.hconfig.arr[0]))->interrupt = 0;
  ((ahb_membar_type*)(&ahbso.hconfig.arr[4]))->memaddr  = haddr;
  ((ahb_membar_type*)(&ahbso.hconfig.arr[4]))->addrmask = hmask;
  ((ahb_membar_type*)(&ahbso.hconfig.arr[4]))->area_id  = CFGAREA_TYPE_AHB_MEM;
  ((ahb_membar_type*)(&ahbso.hconfig.arr[4]))->prefetch = 1;
  ((ahb_membar_type*)(&ahbso.hconfig.arr[4]))->cache    = 1;

  addr.CLK = clk;
  addr.D = ahbsi.haddr & MSK32(9,2);

  ahbso.hresp   = HRESP_OKAY;
  ahbso.hsplit  = 0; 
  ahbso.hirq    = 0;
  ahbso.hcache  = 1;
  //ahbso.hconfig = hconfig;
  ahbso.hindex  = hindex;

  ahbso.hrdata  = RomData[addr.Q>>2].data;
  ahbso.hready  = 1;

}
