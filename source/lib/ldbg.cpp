//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "lheaders.h"

uint32 iClkCnt = 0;

using namespace std;

//****************************************************************************
dbg::dbg()
{
#ifdef DBG_mmulrue
  tst_mmulrue = new mmulrue(0,CFG_DTLBNUM); // index in a range CFG_ITLBNUM or CFG_DTLBNUM
#endif
#ifdef DBG_mmulru
  tst_mmulru = new mmulru(CFG_ITLBNUM); // index in a range CFG_ITLBNUM or CFG_DTLBNUM
#endif
#ifdef DBG_ahbram
  ptst_ahbram = new ahbram(AHB_SLAVE_RAM, 0x300, 0xfff);
#endif
#ifdef DBG_apbctrl
  ptst_apbctrl = new apbctrl(AHB_SLAVE_APBBRIDGE, 0x800, 0xfff);
#endif
#ifdef DBG_apbuart
  ptst_apbuart = new apbuart(APB_UART_CFG,0x1,0xfff); // total address 0x800001xx  = 256 bytes
#endif
#ifdef DBG_irqmp
  ptst_irqmp = new irqmp(APB_IRQ_CONTROL,0x2,0xfff);
#endif
}

dbg::~dbg()
{
#ifdef DBG_mmulrue
  free(tst_mmulrue);
#endif
#ifdef DBG_mmulru
  free(tst_mmulru);
#endif
#ifdef DBG_ahbram
  free(ptst_ahbram);
#endif
#ifdef DBG_apbctrl
  free(ptst_apbctrl);
#endif
#ifdef DBG_apbuart
  free(ptst_apbuart);
#endif
#ifdef DBG_irqmp
  free(ptst_irqmp);
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
      sprintf_s(chName,"%s%s",p->chDirOut,chBenchFile[i]);
      posBench[i] = new ofstream(chName,ios::out);
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
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_jtagcom]) jtagcom_tb(io);

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_ahbmaster]) ahbmst_tb(io);  

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_ahbjtag]) ahbjtag_tb(io);   // (jtagcom + ahbmst)
  
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_ahbctrl]) ahbctrl_tb(io);
  
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_leon3s]) leon3s_tb(io);
  
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_dsu3x]) dsu3x_tb(io);

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_ahbram]) ahbram_tb(io);

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_apbctrl]) apbctrl_tb(io);

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_apbuart]) apbuart_tb(io);
  
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_irqmp]) irqmp_tb(io);

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_mul32]) mul32_tb(io);

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_div32]) div32_tb(io);

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_mmu_icache]) mmu_icache_tb(io);

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_mmu_dcache]) mmu_dcache_tb(io);

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_mmu_acache]) mmu_acache_tb(io);

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_mmu_cache]) mmu_cache_tb(io);

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_mmutlbcam]) mmutlbcam_tb(io);

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_mmutlb]) mmutlb_tb(io);

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_mmutw]) mmutw_tb(io);

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_mmulrue]) mmulrue_tb(io);

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_mmulru]) mmulru_tb(io);

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_mmu]) mmu_tb(io);

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_iu3]) iu3_tb(io);

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_cachemem]) cachemem_tb(io);

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_regfile_3p]) regfile_3p_tb(io);

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_tbufmem]) tbufmem_tb(io);

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_finderr]) finderr_tb(io);

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_soc_leon3]) soc_leon3_tb(io);

  if(io.inClk.eClock==SClock::CLK_POSEDGE)
    iClkCnt++;
}

//****************************************************************************
void dbg::BackDoorLoadRAM(uint32 adr, uint32 v)
{
  // check address:
  if( topLeon3mp.pclAhbRAM->addr != (BITS32(adr,31,20)&topLeon3mp.pclAhbRAM->hmask) )
    return;

  // check size overflow:
  uint32 ulRamAdr = adr & ~(topLeon3mp.pclAhbRAM->hmask<<20);
  if(ulRamAdr>=(CFG_AHBRAMSZ*1024)) 
    return;

  ulRamAdr>>=2;
  topLeon3mp.pclAhbRAM->ppSyncram[0]->memarr[ulRamAdr] = v&0xFF;
  topLeon3mp.pclAhbRAM->ppSyncram[1]->memarr[ulRamAdr] = (v>>8)&0xFF;
  topLeon3mp.pclAhbRAM->ppSyncram[2]->memarr[ulRamAdr] = (v>>16)&0xFF;
  topLeon3mp.pclAhbRAM->ppSyncram[3]->memarr[ulRamAdr] = (v>>24)&0xFF;
}


