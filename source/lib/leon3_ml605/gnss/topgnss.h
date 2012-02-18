//****************************************************************************
// Property:    GNSS Sensor Limited
// License:     GNU2
// Contact:     chief@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#pragma once

class GnssChannels
{
  friend class dbg;
  private:
  public:
    void Update(uint32 inNRst,
                SClock inClk);
};
