#pragma once

class AhbMaster
{
  friend class dbg;
  private:
    uint32 HMST_INDEX;
    uint32 IRQ_BIT_INDEX;
    uint32 VENDOR_ID;
    uint32 DEVICE_ID;
    uint32 VERSION;
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
      VERSION        = version;
      IRQ_BIT_INDEX  = irqindex;
      ADDR_INC       = adrinc;
      PROTECT_BITS   = protect;

      memset((void*)&hconfig, 0, sizeof(ahb_config_type));
      ((ahb_device_reg*)(&hconfig.arr[0]))->vendor  = VENDOR_ID;
      ((ahb_device_reg*)(&hconfig.arr[0]))->device  = DEVICE_ID;
      ((ahb_device_reg*)(&hconfig.arr[0]))->version = VERSION;
    }
  
    void Update(
        uint32 inNRst,
        SClock inClk,
        uint32 address,
        uint32 wdata,//           : std_logic_vector(AHBDW-1 downto 0);
        uint32 start,//           : std_ulogic;
        uint32 burst,//           : std_ulogic;
        uint32 write,//           : std_ulogic;
        uint32 busy,//            : std_ulogic;
        uint32 irq,//             : std_ulogic;
        uint32 size,//            : std_logic_vector(2 downto 0);
        // DMA response:
        uint32& outDmaStart,//           : std_ulogic;
        uint32& outDmaActive,//          : std_ulogic;
        uint32& outDmaReady,//           : std_ulogic;
        uint32& outDmaRetry,//           : std_ulogic;
        uint32& outDmaMexc,//            : std_ulogic;
        uint32& outDmaAdr,//           : std_logic_vector(9 downto 0);
        uint32& outDmaRdData,//           : std_logic_vector(AHBDW-1 downto 0);
        // AHB bus response:
        uint32 inAhbGrant,//[0:15]  : (0 to AHB_MASTERS_MAX-1);     -- bus grant
        uint32 inAhbReady,//  ;                           -- transfer done
        uint32 inAhbResponse,//[1:0] : (1 downto 0);   -- response type
        uint32 inAhbRdData,//[31:0]  : (AHBDW-1 downto 0);   -- read data bus
        // Request to AHB
        ahb_mst_out_type& ahbo
      );
    void ClkUpdate()
    {
      rStart.ClkUpdate();
      rRetry.ClkUpdate();
      rGrant.ClkUpdate();
      rActive.ClkUpdate();
    }
};      

