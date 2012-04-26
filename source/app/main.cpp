//****************************************************************************
// Property:    GNSS Sensor Limited
// License:     GNU2
// Contact:     chief@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************
//
// GPTIMER VHDL behavior.
// Question 1:  vtimer.arr[i].value should be reset during (!rst), otherwise
//              signal "z" always =0, cause value="XXXXXXXX" not equals to zero.
//              Such behavior doesn't match to real situation.
//

#include "headers.h"


int32 iClkCnt = 0;

Settings clSettings;

SystemOnChipIO  ioSoC;
ElfFile         *pclElfFile;
ClockGenerator  *pclkSys;
jtag_port       *portJTAG;
uart_port       *portUART1;
GnssRfSim       *pGnssRfSim;
StGyroscopeSim  clGyroSim;
StAccelerometerSim  clAccelSim;
dbg             clDbg;

//*******************************************************************
int _tmain(int argc, _TCHAR* argv[])
{
  clSettings.LoadFromFile();

  
  pclElfFile = new ElfFile( clSettings.GetpElfFileName() );
  pclkSys = new ClockGenerator( clSettings.GetSysClockHz(), clSettings.GetTimescale() );
  portJTAG = new jtag_port( clSettings.GetJtagClockHz(), clSettings.GetTimescale() );
  portUART1 = new uart_port( clSettings.GetUartClkScale() );
  pGnssRfSim = new GnssRfSim( clSettings.GetSysClockHz(), clSettings.GetTimescale() );
  

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
    
    if(clSettings.IsGyroEna())
    {
      clGyroSim.Update(ioSoC.gyro.nCS,
                       ioSoC.gyro.SPC,
                       ioSoC.gyro.SDI,
                       ioSoC.gyro.SDO,
                       ioSoC.gyro.Int1,
                       ioSoC.gyro.Int2 );
    }

    if(clSettings.IsAccelerEna())
    {
      clAccelSim.Update(ioSoC.acceler.nCS,
                        ioSoC.acceler.SPC,
                        ioSoC.acceler.SDI,
                        ioSoC.acceler.SDO,
                        ioSoC.acceler.Int1,
                        ioSoC.acceler.Int2 );
    }
    
    if(clSettings.IsRfFrontEndEna())
    {
      pGnssRfSim->Update(ioSoC.antctrl.ExtAntEna,
                         ioSoC.antctrl.ExtAntStat,
                         ioSoC.antctrl.ExtAntDetect,
                         ioSoC.gnss.adc_clk,
                         ioSoC.gnss.I[0],
                         ioSoC.gnss.Q[0],
                         ioSoC.gnss.I[1],
                         ioSoC.gnss.Q[1] );
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
  free(pGnssRfSim);
  LibClose();
	return 0;
}



