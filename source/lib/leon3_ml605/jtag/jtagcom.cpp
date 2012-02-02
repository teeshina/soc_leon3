#include "lheaders.h"

//****************************************************************************
void jtagcom::Update( uint32            inNRst,//  : in std_ulogic;
                      SClock            inClk,//  : in std_ulogic;
                      // tap output:
                      uint32            inTCK,  // JTAG clock
                      uint32            inTDI,
                      uint32            inAdrSel,
                      uint32            inDataSel,
                      uint32            inShift,
                      uint32            inUpdate,
                      uint32            inRESET,
                      uint32&           outTDO,// JTAG signal
                      // dma output:
                      ahb_dma_out_type  inDMA,// : in  ahb_dma_out_type;    
                      ahb_dma_in_type&  outDMA// : out ahb_dma_in_type
                    )
{
  const int  ADDBITS = 10;

  write = (uint32)((rbAdr.Q>>34)&0x1);// write or read operation

  
  rTCK[0].CLK = inClk;
  rTCK[0].D = inTCK;
  rTCK[1].CLK = inClk;
  rTCK[1].D = rTCK[0].Q;
  rTCK[2].CLK = inClk;
  rTCK[2].D = rTCK[1].Q;
  wEdgeTCK = !rTCK[2].Q & rTCK[1].Q;
 
  rTDI[0].CLK = inClk;
  rTDI[0].D = inTDI;
  rTDI[1].CLK = inClk;
  rTDI[1].D = rTDI[0].Q; 
  rTDI[2].CLK = inClk;
  rTDI[2].D = rTDI[1].Q; 

  rReset[0].CLK = inClk;
  rReset[0].D = inRESET;
  rReset[1].CLK = inClk;
  rReset[1].D = rReset[0].Q;
  
  rShift[0].CLK = inClk;
  rShift[0].D = inShift;
  rShift[1].CLK = inClk;
  rShift[1].D = rShift[0].Q;
  rShift[2].CLK = inClk;
  rShift[2].D = rShift[1].Q;
  
  rUpdate[0].CLK = inClk;
  rUpdate[0].D = inUpdate;
  rUpdate[1].CLK = inClk;
  rUpdate[1].D = rUpdate[0].Q;
  rUpdate[2].CLK = inClk;
  rUpdate[2].D = rUpdate[1].Q;

  rAdrSel[0].CLK = inClk;
  rAdrSel[0].D   = inAdrSel;
  rAdrSel[1].CLK = inClk;
  rAdrSel[1].D   = rAdrSel[0].Q;
  rDataSel[0].CLK = inClk;
  rDataSel[0].D = inDataSel;
  rDataSel[1].CLK = inClk;
  rDataSel[1].D = rDataSel[0].Q;
  

  //tapi.en = rAdrSel.Q | rDataSel.Q;  // isn't support in Virtex6
  if (rAdrSel[1].Q)                        // send address bit value
    outTDO = (uint32)rbAdr.Q&0x1;
#ifdef REREAD
  else
    outTDO = ((uint32)(rbData.Q&0x1)) & rHoldn.Q;
#else
  else
    outTDO = ((uint32)(rbData.Q&0x1));
#endif

  uint32 wShift = rbState.Q==JTAG_SHIFT ? 1: 0;
  uint32 wAhb = rbState.Q==JTAG_AHB ? 1: 0;
  uint32 wNext = rbState.Q==JTAG_NEXT_SHIFT ? 1: 0;

  rbState.CLK = inClk;
  if( !inNRst | rReset[1].Q )
    rbState.D = JTAG_SHIFT;
  else if(wShift&!rShift[2].Q&rUpdate[2].Q & ((rAdrSel[1].Q & !write)|(rDataSel[1].Q & (write | (!write&rSeq.Q)))) )
    rbState.D = JTAG_AHB;
  else if(wAhb & inDMA.active & inDMA.ready)
    rbState.D = JTAG_NEXT_SHIFT;
#ifdef REREAD
  else if( wNext & (!rUpdate[2].Q & !rShift[2].Q & rHoldn.Q))
#else
  else if( wNext & !rUpdate[2].Q)
#endif
    rbState.D = JTAG_SHIFT;

  // signal to granting the bus:
  uint32 wStartDMA = wAhb & !inDMA.active;

  // reading address:
  rbAdr.CLK   = inClk;
  if( !inNRst | rReset[1].Q )                                      rbAdr.D = 0;
  else if(wShift & rAdrSel[1].Q & wEdgeTCK & rShift[2].Q)          rbAdr.D = (((uint64)rTDI[2].Q)<<34)|(rbAdr.Q>>1);
  else if(wShift & rDataSel[1].Q & rUpdate[2].Q & (rSeq.Q&!write)) rbAdr.D = ((rbAdr.Q>>10)<<10) + BITS64((rbAdr.Q+4),11,0);
  else if(wAhb & inDMA.active & inDMA.ready & (rSeq.Q & write))    rbAdr.D = ((rbAdr.Q>>10)<<10) + BITS64((rbAdr.Q+4),11,0);

  rbData.CLK   = inClk;
  if(!inNRst)                                               rbData.D = 0;
  else if(wShift & rDataSel[1].Q & wEdgeTCK & rShift[2].Q)  rbData.D = (((uint64)rTDI[2].Q)<<32)|(rbData.Q>>1);
  else if(wShift & rUpdate[2].Q)                            rbData.D = BITS64(rbData.Q,31,0);// transfer not done. clear bit 32
#ifdef REREAD
  else if(wAhb & inDMA.active & inDMA.ready)                rbData.D = MSK64(32,32) | inDMA.rdata; // transfer done. bit 32
#else
  else if(wAhb & inDMA.active & inDMA.ready)                rbData.D = inDMA.rdata;
#endif

  // sequential data flag:
  rSeq.CLK    = inClk;
  if(!inNRst | rReset[1].Q)                                 rSeq.D = 0;
  else if(wShift & rDataSel[1].Q & wEdgeTCK & rShift[2].Q)  rSeq.D = rTDI[2].Q;


#ifdef REREAD
  // We skip one JTAG sequence if rHoldn=0, This happens only if there's 
  //    shift command during AHB wait state occurs. In this case we doesn't switch into SHIFT state
  rHoldn.CLK = inClk;
  if(!inNRst)               rHoldn.D = 0;
  else if(wShift)           rHoldn.D = 1;
  else if(wAhb&rShift[2].Q) rHoldn.D = 0;
  else if(wNext)            rHoldn.D = (rHoldn.Q|rUpdate[2].Q) & !rShift[2].Q;
#else
  rHoldn.D = 0;
#endif


  outDMA.address = (uint32)BITS64(rbAdr.Q,31,0);
  outDMA.wdata = (uint32)BITS64(rbData.Q,31,0);
  outDMA.start = wStartDMA;
  outDMA.burst = 0;
  outDMA.write = write;
  outDMA.busy = 0;
  outDMA.irq = 0;
  outDMA.size = (rbState.Q==JTAG_SHIFT) ? 0x0: (uint32)BITS64(rbAdr.Q,33,32);
}
