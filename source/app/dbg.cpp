#include "headers.h"

extern int32 iClkCnt;
extern SystemOnChipIO ioSoC;

extern const JTagTestInput TEST[];
extern jtag_port portJTAG;

//****************************************************************************

//****************************************************************************
dbg::dbg()
{
  memset(this, 0, sizeof(dbg));
}

//****************************************************************************
void dbg::GenerateReset(int32 cnt, uint32 outNRst)
{
  if(cnt < 12)  ioSoC.inNRst = 0;
  else          ioSoC.inNRst = 1;
}

//****************************************************************************
void dbg::Output()
{
  if(portJTAG.IsRdDataRdy())
  {
    uint32 adr=0,rd=0;
    adr = portJTAG.GetRdAdr();
    rd = portJTAG.GetRdData();
    if(false==TEST[iJtagInstrCnt-1].bWrite)
      printf_s("\t{JTag}: [0x%08x] = 0x%08x\n", adr, rd);
  }
  
  
  if(portJTAG.IsEmpty()&&(iJtagInstrCnt<JTAG_TEST_TOTAL))
  {
    const JTagTestInput *t = &TEST[iJtagInstrCnt];
    portJTAG.Put(t);
    printf_s("%i: %s\n",iClkCnt,t->pchComment);
    if(t->bWrite)
    {
      printf_s("  Writting %i to [0x%08x] <= 0x%08x, 0x%08x, 0x%08x, 0x%08x;\n",
        t->uiSize, t->uiAddr,
        t->uiWrData[0], t->uiWrData[1], t->uiWrData[2], t->uiWrData[3]);
    }else
      printf_s("  Read %i from [0x%08x];\n",t->uiSize,t->uiAddr);
      
    iJtagInstrCnt++;
  }else if(portJTAG.IsEmpty())
    bEnd=true;
}


