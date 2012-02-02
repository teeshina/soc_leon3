#include "headers.h"

ClockGenerator::ClockGenerator(double hz_,double step_)
{
  dF = hz_;
  dT2 = 0.5/dF;
  dTimer = 0;
  dTimeStep = step_;
}

bool ClockGenerator::Update(SClock &outClk)
{
  bHalfCycle = false;
  dTimer += dTimeStep;
  if(dTimer>=dT2)
  {
    dTimer -= dT2;
    bHalfCycle = true;
  }
  outClk.eClock_z = outClk.eClock;
  switch(outClk.eClock)
  {
    case SClock::CLK_NEGEDGE:
    case SClock::CLK_NEGATIVE:
      if(bHalfCycle)  outClk.eClock = SClock::CLK_POSEDGE;
      else            outClk.eClock = SClock::CLK_NEGATIVE;
    break;
    case SClock::CLK_POSEDGE:
    case SClock::CLK_POSITIVE:
      if(bHalfCycle)  outClk.eClock = SClock::CLK_NEGEDGE;
      else            outClk.eClock = SClock::CLK_POSITIVE;
    break;
    default:
      outClk.eClock_z = SClock::CLK_POSITIVE;
      outClk.eClock = SClock::CLK_POSITIVE;
  }
  return(outClk.eClock_z==SClock::CLK_POSEDGE);
}
