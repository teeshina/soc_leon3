#pragma once

class ClockGenerator
{
  friend class dbg;
  private:
    double dF;  // clock frequency [Hz]
    double dT2;  // half of clock period [sec]
    bool bHalfCycle;
    double dTimer;    // 1 period timer
    double dTimeStep; // time incrementer
  public:
    ClockGenerator(double hz,double step);
    
    bool Update(SClock &outClk);
};
