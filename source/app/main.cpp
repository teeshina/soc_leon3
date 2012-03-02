//****************************************************************************
// Property:    GNSS Sensor Limited
// License:     GNU2
// Contact:     chief@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

// Question 1:
//            Why when IU executes bne instruction from ROM, pc register is 
//            jumping into right position, but IU proceeds execution of all
//            afterward instructions?? And as result there's 0x2b trap occur.
#include "headers.h"

static const double SYS_CLOCK_F  = 66000000; //Hz
static const double JTAG_CLOCK_F = 20000000; //Hz

static const double timescale = 1.0/SYS_CLOCK_F/4.0;


int32 iClkCnt = 0;

#define ELF_FILE "..\\source\\firmware\\HelloWorld\\target_VIRTEX_ML605\\elf\\HelloWorld.elf"
ElfFile         clElfFile(ELF_FILE);
SystemOnChipIO  ioSoC;
ClockGenerator  clkSys(SYS_CLOCK_F, timescale);
jtag_port       portJTAG;
uart_port       portUART1;
dbg             clDbg;

void GetInitSettings(LibInitData *p);

//*******************************************************************
int _tmain(int argc, _TCHAR* argv[])
{
  LibInitData sLibInitData;
  GetInitSettings(&sLibInitData);
  
  // Library init procedure:
  LibInit(&sLibInitData);

  clElfFile.Load();   // load execution code from *.elf file into SoC Internal RAM
  
  //while (!clDbg.IsEnd())
  while(iClkCnt<3000)
  {
    clDbg.GenerateReset(iClkCnt, ioSoC.inNRst);
    clkSys.Update(ioSoC.inClk);
#if 0
    portJTAG.Update(ioSoC.inNRst,
                    ioSoC.jtag.TDO,
                    ioSoC.jtag.TCK,
                    ioSoC.jtag.nTRST,
                    ioSoC.jtag.TMS,
                    ioSoC.jtag.TDI );
#endif
    
    portUART1.Update(ioSoC.inNRst,
                     ioSoC.inClk,
                     ioSoC.uart1.TD,
                     ioSoC.uart1.RTS,
                     ioSoC.uart1.RD,
                     ioSoC.uart1.CTS );

    LibLeonUpdate(ioSoC);
    
    clDbg.Output();

    LibLeonClkUpdate();
    if(ioSoC.inClk.eClock==SClock::CLK_POSEDGE) iClkCnt++;
  }
  LibClose();
	return 0;
}

//*******************************************************************
void GetInitSettings(LibInitData *p)
{
  // Select testbench to print:
  for (int i=0; i<TB_TOTAL; i++)
    p->uiBenchEna[i] = PRINT_TESTBENCH_DISABLE;
  
  //p->uiBenchEna[TB_jtagcom] = PRINT_TESTBENCH_ENABLE;
  //p->uiBenchEna[TB_ahbmaster] = PRINT_TESTBENCH_ENABLE;
  //p->uiBenchEna[TB_ahbjtag] = PRINT_TESTBENCH_ENABLE;
  //p->uiBenchEna[TB_ahbctrl] = PRINT_TESTBENCH_ENABLE;
  //p->uiBenchEna[TB_mmutlbcam] = PRINT_TESTBENCH_ENABLE;
  //p->uiBenchEna[TB_mmutlb] = PRINT_TESTBENCH_ENABLE;//!
  //p->uiBenchEna[TB_mul32] = PRINT_TESTBENCH_ENABLE;
  //p->uiBenchEna[TB_div32] = PRINT_TESTBENCH_ENABLE;
  //p->uiBenchEna[TB_mmu_icache] = PRINT_TESTBENCH_ENABLE;
  //p->uiBenchEna[TB_iu3] = PRINT_TESTBENCH_ENABLE;
  //p->uiBenchEna[TB_mmu_dcache] = PRINT_TESTBENCH_ENABLE;
  //p->uiBenchEna[TB_mmu_acache] = PRINT_TESTBENCH_ENABLE;
  //p->uiBenchEna[TB_mmutw] = PRINT_TESTBENCH_ENABLE;
  //p->uiBenchEna[TB_mmulrue] = PRINT_TESTBENCH_ENABLE;
  //p->uiBenchEna[TB_mmulru] = PRINT_TESTBENCH_ENABLE;
  //p->uiBenchEna[TB_mmu] = PRINT_TESTBENCH_ENABLE;
  //p->uiBenchEna[TB_mmu_cache] = PRINT_TESTBENCH_ENABLE;
  //p->uiBenchEna[TB_cachemem] = PRINT_TESTBENCH_ENABLE;
  //p->uiBenchEna[TB_regfile_3p] = PRINT_TESTBENCH_ENABLE;
  //p->uiBenchEna[TB_tbufmem] = PRINT_TESTBENCH_ENABLE;
  //p->uiBenchEna[TB_leon3s] = PRINT_TESTBENCH_ENABLE;
  //p->uiBenchEna[TB_dsu3x] = PRINT_TESTBENCH_ENABLE;
  //p->uiBenchEna[TB_ahbram] = PRINT_TESTBENCH_ENABLE;
  //p->uiBenchEna[TB_apbctrl] = PRINT_TESTBENCH_ENABLE;
  //p->uiBenchEna[TB_apbuart] = PRINT_TESTBENCH_ENABLE;
  //p->uiBenchEna[TB_finderr] = PRINT_TESTBENCH_ENABLE;
  //p->uiBenchEna[TB_soc_leon3] = PRINT_TESTBENCH_ENABLE;
}


