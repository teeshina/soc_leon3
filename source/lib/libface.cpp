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
#ifdef USE_GNSSLTD_DUMMIES
  ioData.gnss.adc_clk = ioData.inClk;
#endif

  topLeon3mp.Update(
    ioData.inNRst,  // system reset
    ioData.inClk,   // system clock 66 MHz
    ioData.jtag.nTRST,
    ioData.jtag.TCK,
    ioData.jtag.TMS,
    ioData.jtag.TDI,
    ioData.jtag.TDO,
    ioData.uart1.CTS,
    ioData.uart1.RD,
    ioData.uart1.RTS,
    ioData.uart1.TD,
    ioData.spimax2769.LD,
    ioData.spimax2769.SCLK,
    ioData.spimax2769.nSDATA,
    ioData.spimax2769.nCS,
    ioData.antctrl.ExtAntStat,
    ioData.antctrl.ExtAntDetect,
    ioData.antctrl.ExtAntEna,
    ioData.gnss.adc_clk,
    ioData.gnss.I[0],
    ioData.gnss.Q[0],
    ioData.gnss.I[1],
    ioData.gnss.Q[1]
  );

  clDbg.Update(ioData);
}

//****************************************************************************
DECLSPEC_TYPE void __stdcall LibLeonClkUpdate(void)
{ 
  topLeon3mp.ClkUpdate();
}

//****************************************************************************
DECLSPEC_TYPE void __stdcall LibBackDoorLoadRAM(uint32 adr, uint32 v)
{
  clDbg.BackDoorLoadRAM(adr, v);
}
