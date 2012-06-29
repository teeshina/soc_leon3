//****************************************************************************
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************
//              Verilog styled AHB master controller
//****************************************************************************

#pragma once

#include "stdtypes.h"
#include "dff.h"
#include "id.h"
#include "leon3_ml605\amba\amba.h"

class AhbMaster
{
  friend class dbg;
  private:
    uint32 HMST_INDEX;
    uint32 IRQ_BIT_INDEX;
    uint32 VENDOR_ID;
    uint32 DEVICE_ID;
    uint32 Version;
    uint32 PROTECT_BITS;
    uint32 ADDR_INC;

    ahb_config_type hconfig;

    TDFF<uint32>rStart;
    TDFF<uint32>rRetry;
    TDFF<uint32>rGrant;
    TDFF<uint32>rActive;

    uint32 wReady;//   : std_ulogic;
    uint32 wRetry;//   : std_ulogic;
    uint32 wErrResponse;//    : std_ulogic;
    uint32 wbAdrInc;//     : std_logic_vector(5 downto 0);    -- address increment ?? SH: Maybe [7:0] width???
    uint32 wRespEnd;

    uint32 wbCurAdr;//   : std_logic_vector(31 downto 0);   -- AHB address
    uint32 wbTransferType;//  : std_logic_vector(1 downto 0);    -- transfer type
    uint32 wbNextAdr;// : std_logic_vector(9 downto 0); -- next sequential address


  private:
    //AhbMaster();//prohibited constructor
  public:
    AhbMaster(uint32 vendor=VENDOR_GAISLER, 
              uint32 device=0,
              uint32 mstindex=0, 
              uint32 version=0,
              uint32 irqindex=0,
              uint32 adrinc=0,
              uint32 protect=3 )
    {
      VENDOR_ID      = vendor;
      DEVICE_ID      = device;
      HMST_INDEX     = mstindex;
      Version        = version;
      IRQ_BIT_INDEX  = irqindex;
      ADDR_INC       = adrinc;
      PROTECT_BITS   = protect;

      memset((void*)&hconfig, 0, sizeof(ahb_config_type));
      ((ahb_device_reg*)(&hconfig.arr[0]))->vendor  = VENDOR_ID;
      ((ahb_device_reg*)(&hconfig.arr[0]))->device  = DEVICE_ID;
      ((ahb_device_reg*)(&hconfig.arr[0]))->version = Version;
    }
  
    void Update(
        uint32 inNRst,
        SClock inClk,
        ahb_dma_in_type &in_dmai,
        ahb_dma_out_type &out_dmao,
        ahb_mst_in_type &in_ahbi,
        ahb_mst_out_type &out_ahbo
      );

    void ClkUpdate()
    {
      rStart.ClkUpdate();
      rRetry.ClkUpdate();
      rGrant.ClkUpdate();
      rActive.ClkUpdate();
    }
};      

