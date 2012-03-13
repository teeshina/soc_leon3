//****************************************************************************
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "headers.h"

//****************************************************************************
jtag_port::jtag_port(double hz,double step)
{
  pclkTck = new ClockGenerator(hz,step);
  eState     = STATE_RESET;
  bEmpty     = true;
  bRdDataRdy = false;
}
jtag_port::~jtag_port()
{
  free(pclkTck);
}


//****************************************************************************
void jtag_port::Update(uint32 inRst,
                        uint32 inTDO,
                        SClock &outTCK,
                        uint32 &outnTRST,
                        uint32 &outTMS,
                        uint32 &outTDI
                        )
{
  if(inRst) // button has active high level
  {
    eState   = STATE_RESET;
    outnTRST = 0;
    outTMS   = 1;
    outTDI   = 1;
    bEmpty   = true;
    bRdDataRdy = false;
  }
  if((!pclkTck->Update(outTCK)) || inRst )
    return;
  
  outnTRST = 1;
  
  // output data bit:
  if(eState==STATE_CAPTURE)
  {
    ulRdAdr = stCommand.uiAddr;
    if(eR==IR)                               ulWrShift = uint64(ulSelIR);
    else if(ulSelIR==JTAG_INSTRUCTION_USER1) ulWrShift = (uint64(stCommand.bWrite)<<34)|(uint64(0x2)<<32)|uint64(ulRdAdr);
    else if(ulSelIR==JTAG_INSTRUCTION_USER2) ulWrShift = uint64(stCommand.uiWrData) | 0x100000000;
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

