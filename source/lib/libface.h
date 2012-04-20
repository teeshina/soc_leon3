//****************************************************************************
// Property:    GNSS Sensor Limited
// License:     GNU2
// Contact:     chief@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#pragma once

#if !defined(DECLSPEC_TYPE)
  #error Not defined IMPORT/EXPORT
#endif

#define PRINT_TESTBENCH_ENABLE  1
#define PRINT_TESTBENCH_DISABLE 0

#define DIG ""

static const int32 PATH_LENGTH_MAX = 1024;

//****************************************************************************
enum ETestBenchName
{
  TB_jtagcom,
  TB_ahbmaster,
  TB_jtagpad,
  TB_ahbjtag,
  TB_ahbctrl,
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
  TB_apbctrl,
  TB_apbuart,
  TB_irqmp,
  TB_gptimer,
  TB_finderr,
  TB_soc_leon3,
  TB_rfctrl,
  TB_gyrospi,
  TB_accelspi,
  TB_GnssEngine,
  
  TB_TOTAL
};

const char chBenchFile[TB_TOTAL][64]=
{
  "jtagcom_tb.txt",
  "ahbmaster_tb.txt",
  "jtagpad_tb.txt",
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
  "apbctrl_tb.txt",
  "apbuart_tb.txt",
  "irqmp_tb.txt",
  "gptimer_tb.txt",
  "finderr1_tb.txt",
  "soc_leon3_tb.txt",
  "rfctrl_tb.txt",
  "gyrospi_tb.txt",
  "accelspi_tb.txt",
  "gnssengine_tb.txt"
};

//****************************************************************************
struct LibInitData
{
  char chDirOut[PATH_LENGTH_MAX];
  uint32 uiBenchEna[TB_TOTAL];    // print data patterns
  int32  iPrintVhdlData;          // print VHDL variables assignemt for fast insertion
};

//****************************************************************************
// Virtex6 compatible: see page 179 of ug350.pdf document
const uint32 JTAG_INSTRUCTION_WIDTH = 10;
const uint32 JTAG_INSTRUCTION_USER1 = 0x3C2;  // instruction to accept address (35 bits)
const uint32 JTAG_INSTRUCTION_USER2 = 0x3C3;  // instruction to accept data (33 bits)

//****************************************************************************

//****************************************************************************
struct SystemOnChipIO
{
  uint32 inNRst;  // in: common reset !!WARNING!! "button" has active high level
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
  } uart1;
  // DDR2/3 interface (ML605 = 512 MB)
  struct SDDR
  {
  } ddr;

  //GNSS inputs:
  static const int32 TOTAL_MAXIM2769 = 2;
  struct SGnss
  {
    uint32 I[TOTAL_MAXIM2769];  // [0]=Adc Re GPS/GALILEO L1; [1]=Adc Re GLONASS L1
    uint32 Q[TOTAL_MAXIM2769];  // [0]=Adc Im GPS/GALILEO L1; [1]=Adc Im GLONASS L1
    SClock adc_clk;             // ADC clock ~16..32 MHz
  } gnss;
  // MAX2769 synthesizer SPI interface:
  struct SMax2769
  {
    uint32 LD[TOTAL_MAXIM2769];      // in: PLL locked
    uint32 nSDATA;  // spi interface data
    uint32 SCLK;    // spi interface clock
    uint32 nCS[TOTAL_MAXIM2769];     // spi interface ncs
  }spimax2769;
  struct SAntennaControl
  {
    uint32 ExtAntStat;  // in:
    uint32 ExtAntDetect;// in:
    uint32 ExtAntEna;   // out:
  }antctrl;
  struct SGyroscope
  {
    uint32 SDI;  // gyroscope input  <- SoC output (SDO)
    uint32 SDO;  // gyroscope output -> SoC input (SDI)
    uint32 nCS;  // gyroscope input  <- SoC output
    uint32 SPC;  // gyroscope input  -> SoC ouptut
    uint32 Int1; // gyroscope output -> SoC input
    uint32 Int2; // gyroscope output -> SoC input
  }gyro;
  struct SAccelerometer
  {
    uint32 SDI;  // accelerometer input  <- SoC output (SDO)
    uint32 SDO;  // accelerometer output -> SoC input (SDI)
    uint32 nCS;  // accelerometer input  <- SoC output
    uint32 SPC;  // accelerometer input  -> SoC ouptut
    uint32 Int1; // accelerometer output -> SoC input
    uint32 Int2; // accelerometer output -> SoC input
  }acceler;
};


//****************************************************************************
extern "C" DECLSPEC_TYPE void __stdcall LibInit(LibInitData *);
extern "C" DECLSPEC_TYPE void __stdcall LibClose(void);
extern "C" DECLSPEC_TYPE void __stdcall LibLeonUpdate(SystemOnChipIO &ioData);
extern "C" DECLSPEC_TYPE void __stdcall LibLeonClkUpdate(void);
extern "C" DECLSPEC_TYPE void __stdcall LibBackDoorLoadRAM(uint32 adr, uint32 v);


