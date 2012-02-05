#pragma once

#if !defined(DECLSPEC_TYPE)
  #error Not defined IMPORT/EXPORT
#endif

#define PRINT_TESTBENCH_ENABLE  1
#define PRINT_TESTBENCH_DISABLE 0

//****************************************************************************
enum ETestBenchName
{
  TB_jtagcom,
  TB_AHBMASTER,
  TB_AHBJTAG,
  TB_AHBCTRL,
  
  TB_mmutlbcam,
  TB_mmutlb,
  TB_mul32,
  TB_div32,
  TB_mmu_icache,
  TB_iu3,
  TB_mmu_dcache,
  TB_mmu_acache,
  TB_mmutw,
  TB_mmulrue,
  TB_mmulru,
  TB_mmu,
  TB_mmu_cache,
  TB_cachemem,
  TB_regfile_3p,
  TB_tbufmem,
  TB_leon3s,
  TB_dsu3x,
  TB_ahbram,
  
  TB_TOTAL
};

const char chDirOut[] = "e:\\";
const char chBenchFile[TB_TOTAL][64]=
{
  "jtagcom_tb.txt",
  "ahbmaster_tb.txt",
  "ahbjtag_tb.txt",
  "ahbctrl_tb.txt",
  "mmutlbcam_tb.txt",
  "mmutlb_tb.txt",
  "mul32_tb.txt",
  "div32_tb.txt",
  "mmu_icache_tb.txt",
  "iu3_tb.txt",
  "mmu_dcache_tb.txt",
  "mmu_acache_tb.txt",
  "mmutw_tb.txt",
  "mmulrue_tb.txt",
  "mmulru_tb.txt",
  "mmu_tb.txt",
  "mmu_cache_tb.txt",
  "cachemem_tb.txt",
  "regfile_3p_tb.txt",
  "tbufmem_tb.txt",
  "leon3s_tb.txt",
  "dsu3x_tb.txt",
  "ahbram_tb.txt",
};


//****************************************************************************
struct LibInitData
{
  uint32 uiBenchEna[TB_TOTAL];
};

//****************************************************************************
// Virtex6 compatible: see page 179 of ug350.pdf document
const uint32 JTAG_INSTRUCTION_WIDTH = 10;
const uint32 JTAG_INSTRUCTION_USER1 = 0x3C2;  // instruction to accept address (35 bits)
const uint32 JTAG_INSTRUCTION_USER2 = 0x3C3;  // instruction to accept data (33 bits)


//****************************************************************************
struct SystemOnChipIO
{
  uint32 inNRst;  // in: common reset
  SClock inClk;   // in: System clock: (default 66 MHz)
  // JTAG interface:
  struct SJTag
  {
    uint32 nTRST; // in: Test Reset
    SClock TCK;   // in: Test Clock
    uint32 TMS;   // in: Test Mode State
    uint32 TDI;   // in: Test Data Input
    uint32 TDO;   // out: Test Data Output
  } jtag;
  // UART interface:
  struct SUart
  {
    uint32 TD;  // Transmit Data
    uint32 RD;  // Receive Data
    uint32 RTS; // Request To Send
    uint32 CTS; // Clear To Send
  } uart;
};


//****************************************************************************
extern "C" DECLSPEC_TYPE void __stdcall LibInit(LibInitData *);
extern "C" DECLSPEC_TYPE void __stdcall LibClose(void);
extern "C" DECLSPEC_TYPE void __stdcall LibLeonUpdate(SystemOnChipIO &ioData);
extern "C" DECLSPEC_TYPE void __stdcall LibLeonClkUpdate(void);


