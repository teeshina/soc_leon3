//****************************************************************************
// Property:    GNSS Sensor Limited
// License:     GPL
// Contact:     alex.kosin@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "headers.h"

//****************************************************************************
GnssRfSim::GnssRfSim(double hz,double step)
{
  pAdcClk = new ClockGenerator(hz,step);
}

GnssRfSim::~GnssRfSim()
{
  free(pAdcClk);
}

//****************************************************************************
void GnssRfSim::Update( uint32 inAntEna,
                        uint32 &outAntStatus,
                        uint32 &outAntDetected,
                        SClock &outAdcClk,
                        uint32 &outIa,
                        uint32 &outQa,
                        uint32 &outIb,
                        uint32 &outQb )
{
  pAdcClk->Update(outAdcClk);
  
  outAntStatus   = 0;
  outAntDetected = 0;
  
  // ADC simulator:
  outIa = rand()&0x3;
  outQa = rand()&0x3;
  outIb = rand()&0x3;
  outQb = rand()&0x3;
}
