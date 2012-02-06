//****************************************************************************
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "lheaders.h"

leon3mp topLeon3mp;
dbg     clDbg;

//****************************************************************************
DECLSPEC_TYPE void __stdcall LibInit(LibInitData *p)
{
  clDbg.Init(p);
}

//****************************************************************************
DECLSPEC_TYPE void __stdcall LibClose(void)
{
  clDbg.Close();
}

//****************************************************************************
DECLSPEC_TYPE void __stdcall LibLeonUpdate(SystemOnChipIO &ioData)
{
  topLeon3mp.Update(
    ioData.inNRst,  // system reset
    ioData.inClk,   // system clock 66 MHz
    ioData.jtag.nTRST,
    ioData.jtag.TCK,
    ioData.jtag.TMS,
    ioData.jtag.TDI,
    ioData.jtag.TDO
  );

  clDbg.Update(ioData);
}

//****************************************************************************
DECLSPEC_TYPE void __stdcall LibLeonClkUpdate(void)
{ 
  topLeon3mp.ClkUpdate();
}

