//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GPL
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#pragma once

struct GnssMuxBus
{
  uint32 wRdEna         : 1;
  uint32 wbRdModuleSel;   // : CFG_GNSS_ADDR_WIDTH-1
  uint32 wbRdFieldSel   : 3;
  uint32 wWrEna         : 1;
  uint32 wbWrModuleSel; // : CFG_GNSS_ADDR_WIDTH-1
  uint32 wbWrFieldSel   : 4;
  uint32 wbWrData;
};

