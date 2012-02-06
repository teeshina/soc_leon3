#include "lheaders.h"

uint32 iClkCnt = 0;

using namespace std;

//****************************************************************************
dbg::dbg()
{
#ifdef DBG_mmulrue
  ptst_mmulrue = new mmulrue(0,CFG_DTLBNUM); // index in a range CFG_ITLBNUM or CFG_DTLBNUM
#endif
#ifdef DBG_mmulru
  ptst_mmulru = new mmulru(CFG_ITLBNUM); // index in a range CFG_ITLBNUM or CFG_DTLBNUM
#endif
#ifdef DBG_ahbram
  ptst_ahbram = new ahbram(AHB_SLAVE_RAM, 0x300, 0xfff);
#endif
}

dbg::~dbg()
{
#ifdef DBG_mmulrue
  free(ptst_mmulrue);
#endif
#ifdef DBG_mmulru
  free(ptst_mmulru);
#endif
#ifdef DBG_ahbram
  free(ptst_ahbram);
#endif
}

//****************************************************************************
void dbg::Init(LibInitData *p)
{
  sLibInitData = *p;
  
  char chName[1024];
  for (int32 i=0; i<TB_TOTAL; i++)
  {
    if(PRINT_TESTBENCH_ENABLE==p->uiBenchEna[i])
    {
      sprintf_s(chName,"%s%s",chDirOut,chBenchFile[i]);
      posBench[i] = new ofstream(chName);
    }
  }
}

//****************************************************************************
void dbg::Close()
{
  for (int32 i=0; i<TB_TOTAL; i++)
  {
    if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[i]) 
    { posBench[i]->close(); free(posBench[i]); } 
  }
}

//****************************************************************************
void dbg::Update(SystemOnChipIO &io)
{
#ifdef DBG_mul32
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_mul32]) mul32_tb(io);
#endif
#ifdef DBG_div32
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_div32]) div32_tb(io);
#endif
#ifdef DBG_mmutlbcam
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_mmutlbcam]) mmutlbcam_tb(io);
#endif
#ifdef DBG_mmutlb
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_mmutlb]) mmutlb_tb(io);
#endif
#ifdef DBG_mmu_icache
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_mmu_icache]) mmu_icache_tb(io);
#endif
#ifdef DBG_mmu_dcache
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_mmu_dcache]) mmu_dcache_tb(io);
#endif
#ifdef DBG_mmu_acache
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_mmu_acache]) mmu_acache_tb(io);
#endif
#ifdef DBG_mmutw
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_mmutw]) mmutw_tb(io);
#endif
#ifdef DBG_mmulrue
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_mmulrue]) mmulrue_tb(io);
#endif
#ifdef DBG_mmulru
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_mmulru]) mmulru_tb(io);
#endif
#ifdef DBG_mmu
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_mmu]) mmu_tb(io);
#endif
#ifdef DBG_mmu_cache
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_mmu_cache]) mmu_cache_tb(io);
#endif
#ifdef DBG_iu3
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_iu3]) iu3_tb(io);
#endif
#ifdef DBG_cachemem
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_cachemem]) cachemem_tb(io);
#endif
#ifdef DBG_regfile_3p
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_regfile_3p]) regfile_3p_tb(io);
#endif
#ifdef DBG_tbufmem
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_tbufmem]) tbufmem_tb(io);
#endif

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_jtagcom]) jtagcom_tb(io);

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_AHBMASTER]) ahbmst_tb(io);  

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_AHBJTAG]) ahbjtag_tb(io);   // (jtagcom + ahbmst)
  
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_AHBCTRL]) ahbctrl_tb(io);
  
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_leon3s]) leon3s_tb(io);
  
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_dsu3x]) dsu3x_tb(io);

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_ahbram]) ahbram_tb(io);

  if(io.inClk.eClock==SClock::CLK_POSEDGE)
    iClkCnt++;
}


//****************************************************************************
bool bDoStrOutput=true;
int32 iIndS=0;
char chIndS[3*64386];
char *pchIndS = chIndS;
char chIndS2[3*64386];
char *pchIndS2 = chIndS2;

void ResetPutStr()
{  
  pchIndS = chIndS;
  pchIndS2 = chIndS2;
  iIndS = 0;
}
void PutWidth(int32 size, char *comment)
{
  if(!bDoStrOutput)
    return;

  int32 tmp;
  if(size==1) tmp = sprintf_s(pchIndS,64,"  %s <= S(%i);\n", comment, iIndS);
  else        tmp = sprintf_s(pchIndS,64,"  %s <= S(%i downto %i);\n", comment, iIndS+size-1, iIndS);
  pchIndS += tmp;
  iIndS += size;

  if(size==1) tmp = sprintf_s(pchIndS2,256,"  signal %s  : std_logic;\n", comment);
  else        tmp = sprintf_s(pchIndS2,256,"  signal %s  : std_logic_vector(%i downto 0);\n", comment, size-1);
  pchIndS2 += tmp;
}

void PrintIndexStr()
{
  bDoStrOutput = false;
  std::ofstream osStr("e:\\str.txt",ios::out);
  osStr << chIndS2;
  osStr << "\n";
  osStr << chIndS;
  osStr.close();
}

//****************************************************************************

//****************************************************************************
char* PutToStr(char *p, uint32 v, int size, char *comment, bool inv)
{
  if(comment!=NULL) PutWidth(size, comment);  

  if(!inv)
  {
    for (int i=0; i<size; i++)
    {
      if((v>>i)&0x1)  *p = '1';
      else            *p = '0';
      p++;
    }
  }else
  {
    for (int i=size-1; i>=0; i--)
    {
      if((v>>i)&0x1)  *p = '1';
      else            *p = '0';
      p++;
    }
  }
  *p = '\0';
  return p;
}

//****************************************************************************
char* PutToStr(char *p, uint64 v, int size, char *comment, bool inv)
{
  if(comment!=NULL) PutWidth(size, comment);

  if(!inv)
  {
    for (int i=0; i<size; i++)
    {
      if((v>>i)&0x1)  *p = '1';
      else            *p = '0';
      p++;
    }
  }else
  {
    for (int i=size-1; i>=0; i--)
    {
      if((v>>i)&0x1)  *p = '1';
      else            *p = '0';
      p++;
    }
  }
  *p = '\0';
  return p;
}


//****************************************************************************
char* PutToStr(char *p, uint32* bus, int size, char *comment, bool inv)
{
  if(comment!=NULL) PutWidth(size, comment);
  if(!inv)
  {
    for (int i=0; i<size; i++)
    {
      if(bus[i])  *p = '1';
      else        *p = '0';
      p++;
    }
  }else
  {
    for (int i=size-1; i>=0; i--)
    {
      if(bus[i])  *p = '1';
      else        *p = '0';
      p++;
    }
  }
  *p = '\0';
  return p;
}

//****************************************************************************
char* PutToStr(char *p, char *str)
{
  uint32 i=0;
  while(*str!='\0')
  {
    *p = *str;
    str++;
    p++;
  }
  *p = '\0';
  return p;
}


