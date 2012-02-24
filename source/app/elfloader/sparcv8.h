//****************************************************************************
// Property:    GNSS Sensor Limited
// License:     GNU2
// Contact:     chief@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#pragma once 

#include <fstream>
#include "stdtypes.h"

struct Format1
{
  uint32 disp30 : 30;
  uint32 op     : 2;
};
struct Format2a
{
  uint32 imm22 : 22;
  uint32 op2    : 3;
  uint32 rd     : 5;
  uint32 op     : 2;
};
struct Format2b
{
  uint32 disp22 : 22;
  uint32 op2    : 3;
  uint32 cond   : 4;
  uint32 a      : 1;
  uint32 op     : 2;
};
struct Format3a
{
  uint32 rs2    : 5;
  uint32 asi    : 8;
  uint32 i      : 1;
  uint32 rs1    : 5;
  uint32 op3    : 6;
  uint32 rd     : 5;
  uint32 op     : 2;
};
struct Format3b
{
  uint32 simm13 : 13;
  uint32 i      : 1;
  uint32 rs1    : 5;
  uint32 op3    : 6;
  uint32 rd     : 5;
  uint32 op     : 2;
};
struct Format3c
{
  uint32 rs2    : 5;
  uint32 opf    : 9;
  uint32 rs1    : 5;
  uint32 op3    : 6;
  uint32 rd     : 5;
  uint32 op     : 2;
};

class CInstr
{
  public:
  union U
  {
    uint32   v;
    Format1  f1;
    Format2a f2a;
    Format2b f2b;
    Format3a f3a;
    Format3b f3b;
    Format3c f3c;
  }u;
  int32 format;
  uint32 index;
};

static const int32 INSTR_FORMAT_UNDEF = -1;
static const int32 INSTR_F1    = 0;
static const int32 INSTR_F2a   = 1;
static const int32 INSTR_F2b   = 2;
static const int32 INSTR_F3a   = 3;
static const int32 INSTR_F3b   = 4;
static const int32 INSTR_F3c   = 5;


class SparcV8
{
  friend class dbg;
    static const int32 NWINDOWS = 8;
  private:
    // Control/status registers:
    struct ProcessorStateRegister
    {
      uint32 cwp  : 5;  //[4:0] Current window pointer
      uint32 et   : 1;  //[5] 1=traps enable; 0=disable
      uint32 ps   : 1;  //[6] contains the value of the S bit at the time of the most recent trap.
      uint32 s    : 1;  //[7] 1=supervisor mode; 0-user mode
      uint32 pil  : 4;  //[11:8] Interrupt leevel above which proceesor will accpet irq
      uint32 ef   : 1;  //[12] 1=FPU enabled; 0=disabled (wr into it ignored)
      uint32 ec   : 1;  //[13] 1=co-processor enable; 0=disable (writting into it ignored)
      uint32 reserved : 6; //[19:14]
      uint32 icc_c    : 1; //[20] 1=carry; 0=no carry
      uint32 icc_v    : 1; //[21] 1=overflow; 0=non-overflow
      uint32 icc_z    : 1; //[22] 1=zero; 0=non-zero
      uint32 icc_n    : 1; //[23] 1=negative; 0=not negative
      uint32 ver      : 4; //[27:24]
      uint32 impl     : 4; //[31:28]. Unique ID in response to a WRPSR instruction
    };
    uint32 PSR; // processor State Register
    uint32 WIM; // Window Invalid Mask
    uint32 TBR; // Trap Vase Register
    uint32 Y;   // Multiply/divide register
    uint32 PC;  // Program counter
    uint32 nPC; // Next Program counter
    //uint32 ASR; // Implementation dependent Ancillary State Registers
    
    // General-purpose registers:
    uint32 rGlobal[8];              // r[0] - r[7]
    uint32 rLocal[8+8*NWINDOWS+8];  // out, locals, ins: r[8] - r[31]
    
    CInstr instr;
    char chString[256];

  public:
    void Disassemler(uint32 entry_adr, uint32 *data, uint32 size, std::ofstream *pFile);
    void DisasInstr(CInstr *p);
    
    uint32 Common(CInstr *p);
    uint32 LoadStore(CInstr *p);
    uint32 Bicc(CInstr *p);
    uint32 Ticc(CInstr *p);
    uint32 Synthetic(CInstr *p);
    
    void PrintInstruction(uint32 adr, CInstr *p, std::ofstream *pFile);
};




