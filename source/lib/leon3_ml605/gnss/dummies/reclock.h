//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GPL
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#pragma once

class Reclock
{
  friend class dbg;
  private:
  
  public:
    void Update(uint32 inNRst,
                SClock inClkBus,
                SClock inClkAdc,
                // bus clock:
                GnssMuxBus &inCtrlMuxBus,
                GnssMuxBus &outCtrlMuxBus,
                // adc clock:
                uint32 inMsReady,
                uint32 &outMsReady,
                uint64 inSelData,
                uint64 &outSelData,
                uint32 &outSelDataRdy );
    
    void ClkUpdate()
    {
    }
};
