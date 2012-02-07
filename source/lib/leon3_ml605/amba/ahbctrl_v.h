//****************************************************************************
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************
//              This is AHB controller module. It is specially changed to 
//              match Verilog style
//****************************************************************************
#pragma once

class AhbControl
{
  friend class dbg;
  private:
    uint32 uiAdr;
    uint32 uiWrite;
    uint32 uiWrData;
    bool  bAdrAccepted;
    
    TDFF<int32> rbWaitGrant;
    TDFF<int32> rbWaitAdrAccept;
    TDFF<int32> rbWaitDataReady;
    TDFF<uint32>rbRdData;
    TDFF<uint32>rbCfgData;

    enum EState
    {
      STATE_IDLE,
      STATE_WAIT_GRANT,
      STATE_GRANT,
      STATE_WAIT_ADDR_ACCEPT,
      STATE_ADDR_ACCEPT,
      STATE_WAIT_RD_DATA,
      STATE_RD_DATA_READY
    };
    TDFF<EState> rbState;

    uint32 wbMstReqAll;     // all request bits in one bits
    uint32 wbMstReqMask;
    uint32 wbMstSelNext;
    uint32 wbMstSelHighPriority;
    uint32 wbMstSelLowPriority;
    uint32 wbMstSel;
    TDFF<uint32>rbMstSel[2];
    uint32 wNewBurst;
    uint32 wGuardNewBurst;
    TDFF<uint32>rbLastBurstMst; // last master index which use BURST transaction
    uint32 wbSlvSel;
    TDFF<uint32>rbSlvSel;
    uint32 wCfgSel;
    TDFF<uint32>rCfgSel;

  public:
    AhbControl();
  
    void Update( uint32 inNRst,
                 SClock inClk,
                 ahb_mst_out_type *inMstToCtrl,
                 ahb_mst_in_type  &outCtrl2Mst,
                 ahb_slv_out_vector &inSlv2Ctrl,
                 ahb_slv_in_type  &outCtrl2Slv
                 );
    
    void ClkUpdate()
    {
      rbState.ClkUpdate();
      rbWaitGrant.ClkUpdate();
      rbWaitAdrAccept.ClkUpdate();
      rbWaitDataReady.ClkUpdate();
      rbRdData.ClkUpdate();
      rbMstSel[0].ClkUpdate();
      rbMstSel[1].ClkUpdate();
      rbLastBurstMst.ClkUpdate();
      rbSlvSel.ClkUpdate();
      rCfgSel.ClkUpdate();
      rbCfgData.ClkUpdate();
    }
};

