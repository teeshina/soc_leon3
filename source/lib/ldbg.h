//****************************************************************************
// Property:    GNSS Sensor Limited
// License:     GNU2
// Contact:     chief@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#pragma once

#include <fstream>
#include "stdtypes.h"
#include "libface.h"
#include "dbgstring.h"


extern uint32 iClkCnt;

class dbg
{
  private:
    char chStr[2*64386];
    char *pStr;
    char chArr[256];
    DbgString clVhdl;

    LibInitData sLibInitData;
    std::ofstream *posBench[TB_TOTAL];
  public:
    
  public:
    dbg();
    ~dbg();
    
    void Update(SystemOnChipIO &ioData);
    void BackDoorLoadRAM(uint32 adr, uint32 v);
    // library:
    void Init(LibInitData *p);
    void Close();

    // testbench generators:
    void mul32_tb(SystemOnChipIO &io);
    void div32_tb(SystemOnChipIO &io);
    void mmu_icache_tb(SystemOnChipIO &io);
    void mmu_dcache_tb(SystemOnChipIO &io);
    void mmu_acache_tb(SystemOnChipIO &io);
    void mmu_cache_tb(SystemOnChipIO &io);
    void jtagcom_tb(SystemOnChipIO &io);
    void ahbmst_tb(SystemOnChipIO &io);
    void jtagpad_tb(SystemOnChipIO &io);
    void ahbjtag_tb(SystemOnChipIO &io);
    void ahbctrl_tb(SystemOnChipIO &io);
    void leon3s_tb(SystemOnChipIO &io);
    void dsu3x_tb(SystemOnChipIO &io);
    void ahbram_tb(SystemOnChipIO &io);
    void apbctrl_tb(SystemOnChipIO &io);
    void apbuart_tb(SystemOnChipIO &io);
    void irqmp_tb(SystemOnChipIO &io);
    void gptimer_tb(SystemOnChipIO &io);
    void iu3_tb(SystemOnChipIO &io);
    void mmutlbcam_tb(SystemOnChipIO &io);
    void mmutlb_tb(SystemOnChipIO &io);
    void mmutw_tb(SystemOnChipIO &io);
    void mmulrue_tb(SystemOnChipIO &io);
    void mmulru_tb(SystemOnChipIO &io);
    void mmu_tb(SystemOnChipIO &io);
    void cachemem_tb(SystemOnChipIO &io);
    void regfile_3p_tb(SystemOnChipIO &io);
    void tbufmem_tb(SystemOnChipIO &io);
    void finderr_tb(SystemOnChipIO &io);
    void soc_leon3_tb(SystemOnChipIO &io);
    void rfctrl_tb(SystemOnChipIO &io);
    void gyrospi_tb(SystemOnChipIO &io);
    void accelspi_tb(SystemOnChipIO &io);
    void GnssEngine_tb(SystemOnChipIO &io);
    
    void NCOCarrIF_tb(SystemOnChipIO &io);
    void PrnGenerator_tb(SystemOnChipIO &io);
    void SymbSync_tb(SystemOnChipIO &io);
    void ChannelTop_tb(SystemOnChipIO &io);
    
    // 
    void ResetPutStr(){ clVhdl.ResetPutStr(); }
    bool CheckModificator(char*ref, char*comment){ clVhdl.CheckModificator(ref, comment); }
    void PutWidth(int32 size, char *comment) { clVhdl.PutWidth(size, comment); }
    void PrintIndexStr() {clVhdl.PrintIndexStr(); }
    char* PutToStr(char *p, uint32 v, int size, char *comment=NULL, bool inv=false){ return clVhdl.Put(p,v,size,comment,inv); }
    char* PutToStr(char *p, uint64 v, int size, char *comment=NULL, bool inv=false){ return clVhdl.Put(p,v,size,comment,inv); }
    char* PutToStr(char *p, uint32* bus, int size, char *comment=NULL, bool inv=false){ return clVhdl.Put(p,bus,size,comment,inv); }
    char* PutToStr(char *p, char *str){ return clVhdl.Put(p,str); }
};

