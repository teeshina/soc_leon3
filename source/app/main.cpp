// leon3.cpp : Defines the entry point for the console application.
//

#include "headers.h"

int32 iClkCnt = 0;
SystemOnChipIO ioSoC;

ClockGenerator clkSys(SYS_CLOCK_F, timescale);
jtag_port portJTAG;
dbg clDbg;


int _tmain(int argc, _TCHAR* argv[])
{
  int *const a = (int*const)5;
  LibInitData sLibInitData;
  // Select testbench to print:
  for (int i=0; i<TB_TOTAL; i++)
    sLibInitData.uiBenchEna[i] = PRINT_TESTBENCH_DISABLE;
  
  //sLibInitData.uiBenchEna[TB_jtagcom] = PRINT_TESTBENCH_ENABLE;
  //sLibInitData.uiBenchEna[TB_AHBMASTER] = PRINT_TESTBENCH_ENABLE;
  //sLibInitData.uiBenchEna[TB_AHBJTAG] = PRINT_TESTBENCH_ENABLE;
  //sLibInitData.uiBenchEna[TB_AHBCTRL] = PRINT_TESTBENCH_ENABLE;
  //sLibInitData.uiBenchEna[TB_mmutlbcam] = PRINT_TESTBENCH_ENABLE;
  //sLibInitData.uiBenchEna[TB_mmutlb] = PRINT_TESTBENCH_ENABLE;
  //sLibInitData.uiBenchEna[TB_mul32] = PRINT_TESTBENCH_ENABLE;
  //sLibInitData.uiBenchEna[TB_div32] = PRINT_TESTBENCH_ENABLE;
  //sLibInitData.uiBenchEna[TB_mmu_icache] = PRINT_TESTBENCH_ENABLE;
  //sLibInitData.uiBenchEna[TB_mmu_dcache] = PRINT_TESTBENCH_ENABLE;
  //sLibInitData.uiBenchEna[TB_mmu_acache] = PRINT_TESTBENCH_ENABLE;
  //sLibInitData.uiBenchEna[TB_iu3] = PRINT_TESTBENCH_ENABLE;
  //sLibInitData.uiBenchEna[TB_mmutw] = PRINT_TESTBENCH_ENABLE;
  //sLibInitData.uiBenchEna[TB_mmulrue] = PRINT_TESTBENCH_ENABLE;
  //sLibInitData.uiBenchEna[TB_mmulru] = PRINT_TESTBENCH_ENABLE;
  //sLibInitData.uiBenchEna[TB_mmu] = PRINT_TESTBENCH_ENABLE;
  //sLibInitData.uiBenchEna[TB_mmu_cache] = PRINT_TESTBENCH_ENABLE;
  //sLibInitData.uiBenchEna[TB_cachemem] = PRINT_TESTBENCH_ENABLE;
  //sLibInitData.uiBenchEna[TB_regfile_3p] = PRINT_TESTBENCH_ENABLE;
  //sLibInitData.uiBenchEna[TB_tbufmem] = PRINT_TESTBENCH_ENABLE;
  //sLibInitData.uiBenchEna[TB_leon3s] = PRINT_TESTBENCH_ENABLE;
  //sLibInitData.uiBenchEna[TB_dsu3x] = PRINT_TESTBENCH_ENABLE;
  //sLibInitData.uiBenchEna[TB_ahbram] = PRINT_TESTBENCH_ENABLE;
  
  // Library init procedure:
  LibInit(&sLibInitData);
  
  while (!clDbg.IsEnd())
  {
    clDbg.GenerateReset(iClkCnt, ioSoC.inNRst);
    clkSys.Update(ioSoC.inClk);
    
    portJTAG.Update(ioSoC.inNRst,
                    ioSoC.jtag.TDO,
                    ioSoC.jtag.TCK,
                    ioSoC.jtag.nTRST,
                    ioSoC.jtag.TMS,
                    ioSoC.jtag.TDI );
    

    LibLeonUpdate(ioSoC);
    
    clDbg.Output();

    LibLeonClkUpdate();
    if(ioSoC.inClk.eClock==SClock::CLK_POSEDGE) iClkCnt++;
  }
  LibClose();
	return 0;
}


