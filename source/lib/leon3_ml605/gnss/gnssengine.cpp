//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      GNSS Sensor Limited
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "id.h"
#include "gnssengine.h"


static const uint32 GNSS_ENGINE_VERSION = 1;

//****************************************************************************
GnssEngine::GnssEngine(uint32 hindex_, uint32 haddr_, uint32 hmask_)
{
  hindex = hindex_;
  haddr = haddr_;
  hmask = hmask_;
}

//****************************************************************************
void GnssEngine::Update(uint32 rst,
                        SClock clk,
                        ahb_slv_in_type &ahbsi,
                        ahb_slv_out_type &ahbso,
                        SClock inAdcClk,
                        uint32 inGpsI,
                        uint32 inGpsQ,
                        uint32 inGloI,
                        uint32 inGloQ )
{
  ((ahb_device_reg*)(&ahbso.hconfig.arr[0]))->vendor  = VENDOR_GNSSSENSOR;
  ((ahb_device_reg*)(&ahbso.hconfig.arr[0]))->device  = GNSSSENSOR_ENGINE;
  ((ahb_device_reg*)(&ahbso.hconfig.arr[0]))->version = GNSS_ENGINE_VERSION;
  ((ahb_device_reg*)(&ahbso.hconfig.arr[0]))->cfgver  = 0;
  ((ahb_device_reg*)(&ahbso.hconfig.arr[0]))->interrupt = IRQ_GNSS_ENGINE;
  ((ahb_membar_type*)(&ahbso.hconfig.arr[4]))->memaddr  = haddr;
  ((ahb_membar_type*)(&ahbso.hconfig.arr[4]))->addrmask = hmask;
  ((ahb_membar_type*)(&ahbso.hconfig.arr[4]))->area_id  = CFGAREA_TYPE_AHB_IO;
  ((ahb_membar_type*)(&ahbso.hconfig.arr[4]))->prefetch = 0;
  ((ahb_membar_type*)(&ahbso.hconfig.arr[4]))->cache    = 0;


  wRdEna = ahbsi.hready & !ahbsi.hwrite & BIT32(ahbsi.hsel,hindex) & BIT32(ahbsi.htrans,1);
  wWrEna = ahbsi.hready & ahbsi.hwrite & BIT32(ahbsi.hsel,hindex) & BIT32(ahbsi.htrans,1);

  wbAdr = BITS32(ahbsi.haddr, CFG_GNSS_ADDR_WIDTH-1, 0);

  clGnssTop.Update(rst,
                    clk,
                    wbAdr,
                    wRdEna,
                    wbGnssOutRdData,
                    wbAdr,
                    wWrEna,
                    ahbsi.hwdata,
                    wGnssOutIrqPulse,
                    inAdcClk,
                    inGpsI,
                    inGpsQ,
                    inGloI,
                    inGloQ
                     );

  ahbso.hrdata  = wbGnssOutRdData;
  ahbso.hready  = 1;//
  ahbso.hresp   = HRESP_OKAY; 
  ahbso.hsplit  = 0; 
  ahbso.hirq    = wGnssOutIrqPulse << IRQ_GNSS_ENGINE;
  ahbso.hcache  = 1;
  ahbso.hindex  = hindex;

  
}
