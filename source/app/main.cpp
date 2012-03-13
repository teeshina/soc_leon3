//****************************************************************************
// Property:    GNSS Sensor Limited
// License:     GNU2
// Contact:     chief@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "headers.h"


int32 iClkCnt = 0;

Settings clSettings;

SystemOnChipIO  ioSoC;
ElfFile         *pclElfFile;
ClockGenerator  *pclkSys;
jtag_port       *portJTAG;
uart_port       *portUART1;
dbg             clDbg;

//*******************************************************************
int _tmain(int argc, _TCHAR* argv[])
{
  clSettings.LoadFromFile();

  
  pclElfFile = new ElfFile( clSettings.GetpElfFileName() );
  pclkSys = new ClockGenerator( clSettings.GetSysClockHz(), clSettings.GetTimescale() );
  portJTAG = new jtag_port( clSettings.GetJtagClockHz(), clSettings.GetTimescale() );
  portUART1 = new uart_port( clSettings.GetUartClkScale() );
  

  LibInit(clSettings.GetpLibInitData());  // Library init procedure:
  pclElfFile->Load();                     // load execution code from *.elf file into SoC Internal RAM
  

  while( iClkCnt < clSettings.GetClkTotal() )
  {
    clDbg.GenerateReset(iClkCnt, ioSoC.inNRst);
    pclkSys->Update(ioSoC.inClk);
    
    if(clSettings.IsJtagEna())
    {
      portJTAG->Update(ioSoC.inNRst,
                       ioSoC.jtag.TDO,
                       ioSoC.jtag.TCK,
                       ioSoC.jtag.nTRST,
                       ioSoC.jtag.TMS,
                       ioSoC.jtag.TDI );
    }
    
    if(clSettings.IsUartEna())
    {
      portUART1->Update(ioSoC.inNRst,
                        ioSoC.inClk,
                        ioSoC.uart1.TD,
                        ioSoC.uart1.RTS,
                        ioSoC.uart1.RD,
                        ioSoC.uart1.CTS );
    }

    LibLeonUpdate(ioSoC);
    
    clDbg.Output();

    LibLeonClkUpdate();
    if(ioSoC.inClk.eClock==SClock::CLK_POSEDGE) iClkCnt++;
  }

  clDbg.PrintByeScreen(&clSettings);
  
  free(portUART1);
  free(portJTAG);
  free(pclkSys);
  free(pclElfFile);
  LibClose();
	return 0;
}



