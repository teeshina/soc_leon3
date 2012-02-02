#pragma once

class leon3mp
{
  friend class dbg;
  private:
    ahb_mst_in_type   stCtrl2Mst;// : ahb_dma_in_type;
    ahb_mst_out_type  stMst2Ctrl[AHB_MASTERS_MAX];// : ahb_dma_out_type;
    ahb_slv_in_type   stCtrl2Slv;
    ahb_slv_out_type  stSlv2Ctrl[AHB_SLAVES_MAX];
    ahb_slv_out_vector  in_slvo;//    : in  ahb_slv_out_vector;

    AhbMasterJtag clAhbMasterJtag;
    AhbSlaveMem   *pclAhbSlaveMem;
    AhbControl    clAhbControl;
    
    //
    l3_irq_in_type irqi;//   : in  l3_irq_in_type;
    l3_irq_out_type irqo;//   : out l3_irq_out_type;
    l3_debug_in_type dbgi;//   : in  l3_debug_in_type;
    l3_debug_out_type dbgo;
    leon3s clLeon3s;

  public:
    leon3mp();
    ~leon3mp();

    void Update
    (
      uint32 inNRst,
      SClock inClk,
      // JTAG interface:
      uint32 nTRST,
      SClock TCK,
      uint32 TMS,
      uint32 TDI,
      uint32 &TDO
    );
    
    void ClkUpdate()
    {
      clAhbMasterJtag.ClkUpdate();
      clAhbControl.ClkUpdate();
      pclAhbSlaveMem->ClkUpdate();
      
      clLeon3s.ClkUpdate();
    }
};

