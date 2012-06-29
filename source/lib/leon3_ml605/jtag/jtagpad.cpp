#include "id.h"
#include "jtagpad.h"

void JTagPad::Update( uint32 nTRST,
                      SClock TCK,
                      uint32 TMS,
                      uint32 TDI,
                      uint32 &TDO,
                      
                      uint32 &outSelUser1,  //sel=1 when JTAG instruction register holds USER instruction
                      uint32 &outSelUser2,
                      uint32 &outTDI,       //fed through directly from FPGA TDI pin
                      uint32 &outDRCK,      //DRCK is same as TCK. if interface is not selected it remains high
                      uint32 &outCapture,   //Active-high pulse indicating Capture_DR state
                      uint32 &outShift,     //Active-high. Indicate Shift_DR state
                      uint32 &outUpdate,    //Active-high indicating the Update_DR state
                      uint32 &outReset,     //=1 reset output
                      uint32 in_tdo1        //2-nd register doesn't use
                      )
{
  // data/instruction loop select:
  rResetLogic.CLK = TCK;
  if(!nTRST)                      rResetLogic.D = 1;
  else if(rResetLogic.Q&TMS)      rResetLogic.D = 1;
  else if(rSelectScan[IR].Q&TMS)  rResetLogic.D = 1;
  else                            rResetLogic.D = 0;
  
  rRunTestIdle.CLK = TCK;
  if(!nTRST)                                    rRunTestIdle.D = 0;
  else if(rResetLogic.Q & !TMS)                 rRunTestIdle.D = 1;
  else if(rRunTestIdle.Q &!TMS)                 rRunTestIdle.D = 1;
  else if((rUpdate[DR].Q|rUpdate[IR].Q) & !TMS) rRunTestIdle.D = 1;
  else                                          rRunTestIdle.D = 0;
  
  rSelectScan[DR].CLK = TCK;
  if(!nTRST)                                    rSelectScan[DR].D = 0;
  else if(rRunTestIdle.Q & TMS)                 rSelectScan[DR].D = 1;
  else if((rUpdate[DR].Q|rUpdate[IR].Q) & TMS)  rSelectScan[DR].D = 1;
  else                                          rSelectScan[DR].D = 0;

  rSelectScan[IR].CLK = TCK;
  if(!nTRST)                       rSelectScan[IR].D = 0;
  else if(rSelectScan[DR].Q & TMS) rSelectScan[IR].D = 1;
  else                             rSelectScan[IR].D = 0;


  // Data/Instruction transmition:
  for (int32 i=0; i<R_TOTAL; i++)
  {
    rCapture[i].CLK = TCK;
    if(!nTRST)                       rCapture[i].D = 0;
    else if(rSelectScan[i].Q & !TMS) rCapture[i].D = 1;
    else                             rCapture[i].D = 0;
    
    rShift[i].CLK = TCK;
    if(!nTRST)                        rShift[i].D = 0;
    else if(rCapture[i].Q & !TMS)     rShift[i].D = 1;
    else if(rShift[i].Q & !TMS)       rShift[i].D = 1;
    else if(rExit2[i].Q & !TMS)       rShift[i].D = 1;
    else                              rShift[i].D = 0;

    rExit1[i].CLK = TCK;
    if(!nTRST)                   rExit1[i].D = 0;
    else if(rShift[i].Q & TMS)   rExit1[i].D = 1;
    else if(rCapture[i].Q & TMS) rExit1[i].D = 1;
    else                         rExit1[i].D = 0;
    
    rPause[i].CLK = TCK;
    if(!nTRST)                  rPause[i].D = 0;
    else if(rExit1[i].Q & !TMS) rPause[i].D = 1;
    else if(rPause[i].Q & !TMS) rPause[i].D = 1;
    else                        rPause[i].D = 0;
    
    rExit2[i].CLK = TCK;
    if(!nTRST)                  rExit2[i].D = 0;
    else if(rPause[i].Q & TMS)  rExit2[i].D = 1;
    else                        rExit2[i].D = 0;
    
    rUpdate[i].CLK = TCK;
    if(!nTRST)                 rUpdate[i].D = 0;
    else if(rExit2[i].Q & TMS) rUpdate[i].D = 1;
    else if(rExit1[i].Q & TMS) rUpdate[i].D = 1;
    else                       rUpdate[i].D = 0;
  }
  
  // Fill Instruction register:
  rbIR.CLK = TCK;
  if(!nTRST)              rbIR.D = 0;
  else if(rCapture[IR].Q) rbIR.D = 0;
  else if(rShift[IR].Q)   rbIR.D = (rbIR.Q>>1) | (TDI<<(JTAG_INSTRUCTION_WIDTH-1));
  
  rSelUser1.CLK = TCK;
  if(!nTRST)                                               rSelUser1.D = 0;
  else if(rUpdate[IR].Q&&(rbIR.Q==JTAG_INSTRUCTION_USER1)) rSelUser1.D = 1;
  else if(rUpdate[IR].Q|rCapture[IR].Q)                    rSelUser1.D = 0;
  outSelUser1 = rSelUser1.Q;

  rSelUser2.CLK = TCK;
  if(!nTRST)                                               rSelUser2.D = 0;
  else if(rUpdate[IR].Q&&(rbIR.Q==JTAG_INSTRUCTION_USER2)) rSelUser2.D = 1;
  else if(rUpdate[IR].Q|rCapture[IR].Q)                    rSelUser2.D = 0;
  outSelUser2 = rSelUser2.Q;

  wTCK = ((TCK.eClock==SClock::CLK_POSEDGE)||(TCK.eClock==SClock::CLK_POSITIVE)) ? 1: 0;
  
  // other BSCAN_VIRTEX6 primitive outputs:
  outDRCK     = (rSelUser1.Q|rSelUser2.Q) ? wTCK: 1;
  outCapture  = rCapture[DR].Q;
  outTDI      = TDI;
  outShift    = rShift[DR].Q;
  outUpdate   = rUpdate[DR].Q;
  outReset    = rResetLogic.Q;
  
  TDO = in_tdo1;
}
