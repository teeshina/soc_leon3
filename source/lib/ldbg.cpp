//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "lheaders.h"
#include "dbgstring.h"

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
#ifdef DBG_gptimer
  ptst_gptimer = new gptimer(APB_TIMER, 0x3, 0xfff);
#endif
#ifdef DBG_ChannelTop
  ptst_ChannelTop = new ChannelTop(0,0);
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
#ifdef DBG_gptimer
  free(ptst_gptimer);
#endif
#ifdef DBG_ChannelTop
  free(ptst_ChannelTop);
#endif
}

//****************************************************************************
void dbg::Init(LibInitData *p)
{
  
  char chName[1024];
  for (int32 i=0; i<TB_TOTAL; i++)
  {
    if(PRINT_TESTBENCH_ENABLE==p->uiBenchEna[i])
    {
      sprintf_s(chName,"%s%s",p->chDirOut,chBenchFile[i]);
      posBench[i] = new ofstream(chName,ios::out);
      if(!posBench[i]->is_open())
      {
        printf_s("ERROR: can't create file \"%s%s\"\n",p->chDirOut,chBenchFile[i]);
        p->uiBenchEna[i] = PRINT_TESTBENCH_DISABLE;
        free(posBench[i]);
      }
    }
  }
  clVhdl.SetOutputPath(p->iPrintVhdlData, p->chDirOut);
  sLibInitData = *p;
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

#define HIST_SIZE 64
uint32 hist_adr[HIST_SIZE]={};
//****************************************************************************
void dbg::Update(SystemOnChipIO &io)
{
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_jtagcom]) jtagcom_tb(io);

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_ahbmaster]) ahbmst_tb(io);  

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_jtagpad]) jtagpad_tb(io);

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_ahbjtag]) ahbjtag_tb(io);   // (jtagcom + ahbmst)
  
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_ahbctrl]) ahbctrl_tb(io);
  
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_leon3s]) leon3s_tb(io);
  
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_dsu3x]) dsu3x_tb(io);

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_ahbram]) ahbram_tb(io);

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_apbctrl]) apbctrl_tb(io);

  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_apbuart]) apbuart_tb(io);
  
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_irqmp]) irqmp_tb(io);
  
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_gptimer]) gptimer_tb(io);

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
  
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_rfctrl]) rfctrl_tb(io);
  
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_gyrospi]) gyrospi_tb(io);
  
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_accelspi]) accelspi_tb(io);
  
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_GnssEngine]) GnssEngine_tb(io);
  
#if!defined(USE_GNSSLTD_DUMMIES)
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_CarrNcoIF]) CarrNcoIF_tb(io);
    
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_PrnGenerator]) PrnGenerator_tb(io);
  
  if(PRINT_TESTBENCH_ENABLE==sLibInitData.uiBenchEna[TB_ChannelTop]) ChannelTop_tb(io);
#endif

  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    iClkCnt++;
#if 0
    #define adr_chBuf       0x4000bca0
    #define adr_pchBuf      0x4000bea0
    #define adr_iBufCnt     0x4000bea4
    #define adr_tmpUartStr  0x4000bea8
    #define adr_iIrqCnt     0x4000bfa8
    
    #define adr_TickHandler 0x400013c0
    #define adr_HandlerIrq  0x40009754
    #define adr_call_initcalls 0x40009cd8

    if(topLeon3mp.apbo.arr[APB_TIMER].pirq)
    bool st = true;
    
    //if(topLeon3mp.stCtrl2Slv.haddr==adr_TickHandler)
    if(topLeon3mp.stCtrl2Slv.haddr==0x400012f4)
    {
      uint32 adr = 0xbfd0>>2;
      uint32 mm = topLeon3mp.pclAhbRAM->ppSyncram[0]->memarr[adr];
      mm |= (topLeon3mp.pclAhbRAM->ppSyncram[1]->memarr[adr]<<8);
      mm |= (topLeon3mp.pclAhbRAM->ppSyncram[2]->memarr[adr]<<16);
      mm |= (topLeon3mp.pclAhbRAM->ppSyncram[3]->memarr[adr]<<24);
      bool st = true;
    }
    
    if(topLeon3mp.stCtrl2Slv.hsel==0x4)
    {
      for(int32 i=(HIST_SIZE-1); i>0; i--)
        hist_adr[i] = hist_adr[i-1];
      hist_adr[0] = topLeon3mp.stCtrl2Slv.haddr;
    }
#endif
  }
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


