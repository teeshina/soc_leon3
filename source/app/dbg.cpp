#include "headers.h"

extern int32 iClkCnt;
extern const JTagTestInput TEST[];

extern SystemOnChipIO ioSoC;
extern jtag_port      portJTAG;
extern uart_port      portUART1;

//****************************************************************************

//****************************************************************************
dbg::dbg()
{
  memset(this, 0, sizeof(dbg));
}

//****************************************************************************
void dbg::GenerateReset(int32 cnt, uint32 outNRst)
{
#if 1// button simulation
  if(cnt < 26)  ioSoC.inNRst = 1;
  else          ioSoC.inNRst = 0;
#else
  if(cnt < 12)  ioSoC.inNRst = 0;
  else          ioSoC.inNRst = 1;
#endif
}

//****************************************************************************
void dbg::Output()
{
  // Check UART port:
  if(portUART1.IsRdDataRdy())
  {
    printf_s("%6i:{UART1}R \"%s\"\n", iClkCnt, portUART1.GetpDataString());
  }

  // Check JTAG port:
  if(portJTAG.IsRdDataRdy())
  {
    uint32 adr=0,rd=0;
    adr = portJTAG.GetRdAdr();
    rd = portJTAG.GetRdData();
    if(false==TEST[iJtagInstrCnt-1].bWrite)
      printf_s("%6i:{JTag}R [0x%08x] => 0x%08x\n", iClkCnt,adr, rd);
  }
  
  
  if(portJTAG.IsEmpty()&&(iJtagInstrCnt<JTAG_TEST_TOTAL))
  {
    const JTagTestInput *t = &TEST[iJtagInstrCnt];
    portJTAG.Put(t);
    if(t->bWrite)
      printf_s("%6i:{JTag}W [0x%08x] <= 0x%08x //%s\n", iClkCnt, t->uiAddr,t->uiWrData,t->pchComment);
      
    iJtagInstrCnt++;
  }else if(portJTAG.IsEmpty())
    bEnd=true;
}


