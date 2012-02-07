#pragma once


#define NEW_JTAGCOM
//****************************************************************************
class jtagcom
{
  friend class dbg;
  private:
    enum EJtagState {JTAG_SHIFT, JTAG_AHB, JTAG_NEXT_SHIFT};


    TDFF<uint32>rTCK;  // clock edge detector
    TDFF<uint32>rTDI;
    TDFF<uint32>rAdrSel[2];
    TDFF<uint32>rDataSel[2];
    TDFF<uint32>rShift[3];
    TDFF<uint32>rUpdate[3];
    TDFF<uint32>rReset;
    
    TDFF<uint64>rbAdr;
    TDFF<uint64>rbData;
    TDFF<uint32>rSeq;
    TDFF<uint32>rbState;
#ifdef REREAD
    TDFF<uint32>rHoldn;
#endif

  uint32 wEdgeTCK;
  uint32 write;// : std_ulogic;



  public:
    void Update(
      uint32            inNRst,//  : in std_ulogic;
      SClock            inClk,//  : in std_ulogic;
      //tap output:
      uint32            inTCK,  // JTAG clock
      uint32            inTDI,
      uint32            inAdrSel,
      uint32            inDataSel,
      uint32            inShift,
      uint32            inUpdate,
      uint32            inRESET,
      uint32&           outTDO,
      // dma output:
      ahb_dma_out_type  in_dmao,// : in  ahb_dma_out_type;    
      ahb_dma_in_type&  out_dmai// : out ahb_dma_in_type
    );
    void ClkUpdate()
    {
      rTCK.ClkUpdate();
      rTDI.ClkUpdate();
      rAdrSel[0].ClkUpdate();
      rAdrSel[1].ClkUpdate();
      rDataSel[0].ClkUpdate();
      rDataSel[1].ClkUpdate();
      rShift[0].ClkUpdate();
      rShift[1].ClkUpdate();
      rShift[2].ClkUpdate();
      rUpdate[0].ClkUpdate();
      rUpdate[1].ClkUpdate();
      rUpdate[2].ClkUpdate();
      rReset.ClkUpdate();
      rbAdr.ClkUpdate();
      rbData.ClkUpdate();
      rSeq.ClkUpdate();
      rbState.ClkUpdate();
#ifdef REREAD
      rHoldn.ClkUpdate();
#endif
    }
};

