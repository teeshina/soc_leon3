#pragma once

class AhbMasterJtag
{
  friend class dbg;
  private:
    uint32 wPadOutAdrSel;
    uint32 wPadOutDataSel;
    uint32 wPadOutTDI;
    uint32 wPadOutDRCK;
    uint32 wPadOutCapture;
    uint32 wPadOutShift;
    uint32 wPadOutUpdate;
    uint32 wPadOutReset;
    JTagPad clJTagPad;

    uint32          wComOutTDO;
    ahb_dma_in_type jcomout_dmai;// : ahb_dma_in_type;
    jtagcom jtagcom0;

    ahb_dma_out_type  ahbmstout_dmao;// : ahb_dma_out_type;
    AhbMaster *ahbmst0;


  public:
    AhbMasterJtag()
    {
      ahbmst0 = new AhbMaster(VENDOR_GAISLER, GAISLER_AHBJTAG, AHB_MASTER_JTAG, 1, 0, 0, 3);
    }
    ~AhbMasterJtag()
    {
      free(ahbmst0);
    }
  
    void Update(
        uint32 inNRst,//         : in  std_ulogic;
        SClock inClk,//         : in  std_ulogic;
        //JTAG interface:
        uint32 nTRST,
        SClock TCK,
        uint32 TMS,
        uint32 TDI,
        uint32 &TDO,
        // AMBA interface:
        ahb_mst_in_type ahbi,//        : in  ahb_mst_in_type;
        ahb_mst_out_type& ahbo//        : out ahb_mst_out_type;
      );

    void ClkUpdate()
    {
      clJTagPad.ClkUpdate();
      ahbmst0->ClkUpdate();
      jtagcom0.ClkUpdate();
    }
};


