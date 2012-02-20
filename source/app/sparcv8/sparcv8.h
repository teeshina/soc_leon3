//****************************************************************************
// Property:    GNSS Sensor Limited
// License:     GNU2
// Contact:     chief@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "stdtypes.h"

class SparcV8
{
  friend class dbg;
  private:
  public:
    void Put(uint32 word);
};
