//****************************************************************************
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "headers.h"

//****************************************************************************
jtag_port::jtag_port()
{
  pclkTck = new ClockGenerator(JTAG_CLOCK_F,timescale);
  eState     = STATE_RESET;
  bEmpty     = true;
  bRdDataRdy = false;
}
jtag_port::~jtag_port()
{
  free(pclkTck);
}


//****************************************************************************
void jtag_port::Update(uint32 inNRst,
                        uint32 inTDO,
                        SClock &outTCK,
                        uint32 &outnTRST,
                        uint32 &outTMS,
                        uint32 &outTDI
                        )
{
  if(!inNRst)
  {
    eState   = STATE_RESET;
    outnTRST = 0;
    outTMS   = 1;
    outTDI   = 1;
    bEmpty   = true;
    uiSeqCnt  = 0;
    bRdDataRdy = false;
    return;
  }
  if(!pclkTck->Update(outTCK))
    return;
  
  outnTRST = 1;
  
  // output data bit:
  if(eState==STATE_CAPTURE)
  {
    ulRdAdr = (stCommand.uiAddr&0xFFFFF000) | ((stCommand.uiAddr + 4*uiSeqCnt)&0xFFF);
    if(eR==IR)                                               ulWrShift = uint64(ulSelIR);
    else if(ulSelIR==JTAG_INSTRUCTION_USER1)                 ulWrShift = (uint64(stCommand.bWrite)<<34)|(uint64(0x2)<<32)|uint64(ulRdAdr);
    else if((ulSelIR==JTAG_INSTRUCTION_USER2)&&(uiSeqCnt<3)) ulWrShift = uint64(stCommand.uiWrData[uiSeqCnt]) | 0x100000000;
    else if(ulSelIR==JTAG_INSTRUCTION_USER2)                 ulWrShift = uint64(stCommand.uiWrData[uiSeqCnt]);
  }

  if(eState==STATE_SHIFT)
  {
    outTDI = uint32(ulWrShift)&0x1;
    ulWrShift>>=1;
  }else
    outTDI = 1;

  // input data bit:
  if((eR==DR)&&(ulSelIR==JTAG_INSTRUCTION_USER2)&&(eState==STATE_SHIFT))
    ulRdShift = (ulRdShift>>1) | (inTDO<<31);

  if((eR==DR)&&(ulSelIR==JTAG_INSTRUCTION_USER2)&&(eState==STATE_UPDATE))
  {
    bRdDataRdy = true;
    ulRdData   = ulRdShift;
  }

  // State machine of bits:
  switch(eState)
  {
    case STATE_RESET:
      outTMS   = 0;
      eState = STATE_IDLE;
    break;
    case STATE_IDLE:
      if(!bEmpty)
      {
        outTMS  = 1;
        eState  = STATE_SELECT_DR;
        ulSelIR = JTAG_INSTRUCTION_USER1;
        eR      = IR;
        uiSeqCnt = 0;
      }else
        outTMS   = 0;
    break;
    case STATE_SELECT_DR:
      if(eR==IR)
      {
        outTMS = 1;
        eState = STATE_SELECT_IR;
      }else
      {
        outTMS = 0;
        eState = STATE_CAPTURE;
      }
    break;
    case STATE_SELECT_IR:
      outTMS = 0;
      eState = STATE_CAPTURE;
    break;
    case STATE_CAPTURE:
      outTMS = 0;
      eState = STATE_SHIFT;
      iBitCnt = eR==IR ? JTAG_INSTRUCTION_WIDTH 
                       : (ulSelIR==JTAG_INSTRUCTION_USER1) ? 35 : 33;
    break;
    case STATE_SHIFT:
      if((--iBitCnt)>0) outTMS = 0;
      else
      {
        outTMS = 1;
        eState = STATE_EXIT1;
      }
    break;
    case STATE_EXIT1:
      outTMS = 1;
      eState = STATE_UPDATE;
    break;
    case STATE_UPDATE:
      if(eR==IR)
      {
        eR = DR;
        outTMS = 1;
        eState = STATE_SELECT_DR;
      }else if(ulSelIR==JTAG_INSTRUCTION_USER1)
      {
        eR = IR;
        outTMS = 1;
        eState = STATE_SELECT_DR;
        ulSelIR = JTAG_INSTRUCTION_USER2;
      }else if(uiSeqCnt<stCommand.uiSize)
      {
        eR = IR;
        outTMS = 1;
        uiSeqCnt++;
        eState = STATE_SELECT_DR;
        ulSelIR = JTAG_INSTRUCTION_USER1;
      }else
      {
        outTMS = 0;
        eState = STATE_IDLE;
        bEmpty = true;
      }
    break;
    default:;
  }
}

