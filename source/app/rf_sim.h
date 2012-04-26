//****************************************************************************
// Property:    GNSS Sensor Limited
// License:     GPL
// Contact:     alex.kosin@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#pragma once

//****************************************************************************
class GnssRfSim
{
  friend class dbg;
  private:
    
    ClockGenerator *pAdcClk;
  public:
    GnssRfSim(double hz,double step);
    ~GnssRfSim();
  
    void Update(uint32 inAntEna,
                uint32 &outAntStatus,
                uint32 &outAntDetected,
                SClock &outAdcClk,
                uint32 &outIa,
                uint32 &outQa,
                uint32 &outIb,
                uint32 &outQb );
};




