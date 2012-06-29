#pragma once

#include "stdtypes.h"
#include "dff.h"

class JTagPad
{
  friend class dbg;
  private:
    enum {IR, DR, R_TOTAL};  // Data/Instruction register
    
  public:
    TDFF<uint32> rResetLogic;
    TDFF<uint32> rRunTestIdle;
    TDFF<uint32> rSelectScan[R_TOTAL];
    TDFF<uint32> rCapture[R_TOTAL];
    TDFF<uint32> rShift[R_TOTAL];
    TDFF<uint32> rExit1[R_TOTAL];
    TDFF<uint32> rPause[R_TOTAL];
    TDFF<uint32> rExit2[R_TOTAL];
    TDFF<uint32> rUpdate[R_TOTAL];
    
    TDFF<uint32> rbIR;  // 10 bits for Virtex6 (document "ug360.pdf", page 178")
    TDFF<uint64> rbDR;  // 35 bits
    
    // BSCAN_VIRTEX6 signal:
    uint32 wTCK;  // convert SClock to simple wire
    TDFF<uint32> rSelUser1;
    TDFF<uint32> rSelUser2;
    
    void Update(uint32 nTRST,
                SClock TCK,
                uint32 TMS,
                uint32 TDI,
                uint32 &TDO,        //TDO input driven user's logic. Sampled by falling edge.

                uint32 &outSelUser1,//sel=1 when JTAG instruction register holds USER instruction
                uint32 &outSelUser2,
                uint32 &outTDI,     //fed through directly from FPGA TDI pin
                uint32 &outDRCK,    //DRCK is same as TCK. if interface is not selected it remains high
                uint32 &outCapture, //Active-high pulse indicating Capture_DR state
                uint32 &outShift,   //Active-high. Indicate Shift_DR state
                uint32 &outUpdate,  //Active-high indicating the Update_DR state
                uint32 &outReset,   //=1 reset output
                uint32 in_tdo1      //2-nd register doesn't use
                );
                
   void ClkUpdate()
   {
      rResetLogic.ClkUpdate();
      rRunTestIdle.ClkUpdate();
      for (int32 i=0; i<R_TOTAL; i++)
      {
        rSelectScan[i].ClkUpdate();
        rCapture[i].ClkUpdate();
        rShift[i].ClkUpdate();
        rExit1[i].ClkUpdate();
        rPause[i].ClkUpdate();
        rExit2[i].ClkUpdate();
        rUpdate[i].ClkUpdate();
      }
      rbIR.ClkUpdate();
      rbDR.ClkUpdate();
      
      rSelUser1.ClkUpdate();
      rSelUser2.ClkUpdate();
   }
};
