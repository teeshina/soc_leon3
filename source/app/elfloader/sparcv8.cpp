//****************************************************************************
// Property:    GNSS Sensor Limited
// License:     GNU2
// Contact:     chief@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include <string.h>
#include <iostream>
#include <fstream>
#include "instr.h"
#include "sparcv8.h"


//****************************************************************************
void SparcV8::Disassemler(uint32 entry_adr, uint32 *data, uint32 size, std::ofstream *pFile)
{
  for (int32 i=0; i<4; i++)
  {
    instr.u.v = data[i];
    instr.format = INSTR_FORMAT_UNDEF;
  
    DisasInstr(&instr);
    PrintInstruction(entry_adr+4*i,&instr, pFile);
  }
}

//****************************************************************************
void SparcV8::DisasInstr(CInstr *p)
{
  switch(p->u.f1.op)
  {
    case 0x1:
      p->format = INSTR_F1;
      p->index  = CALL;
    break;

    case 0x0:
      switch(p->u.f2a.op2)
      {
        case 0:// UNIMP 
          p->format = INSTR_F2a;
          p->index  = UNIMP;
        break;
        case 4: // SETHI
          p->format = INSTR_F2a;
          p->index  = SETHI;
        break;
        case 2: // Bicc
          p->format = INSTR_F2b;
          p->index  = Bicc(&instr);
        break;
        case 6: // FBfcc
          p->format = INSTR_F2b;
        break;
        case 7: // CBccc
          p->format = INSTR_F2b;
        break;
        default:; // unimplemented
      }
    break;

    case 0x2:
      switch(p->u.f3a.op3)
      {
        case 0x34://FPop1
          p->format = INSTR_F3c;
        break;
        case 0x35://FPop2
          p->format = INSTR_F3c;
        break;
        case 0x3A:  // Ticc
          p->format = INSTR_F2b;
        break;
        default:
          p->format = INSTR_F3c;
          p->index  = Common(&instr);
      }
    break;

    case 0x3:
      if(p->u.f3a.i==0) p->format = INSTR_F3a;
      else                 p->format = INSTR_F3b;
      p->index  = LoadStore(&instr);
    break;
  }

  Synthetic(&instr);
}

//****************************************************************************
void SparcV8::PrintInstruction(uint32 adr, CInstr *p, std::ofstream *pFile)
{
  uint32 tmp;
  sprintf_s(chString, "0x%08x 0x%08x   ", adr, p->u.v);
  *pFile << chString;
  switch(p->index)
  {
    case SETHI: sprintf_s(chString, "sethi  %%hi(0x%08x), %s\n", (p->u.f2a.imm22<<10), chReg[p->u.f2a.rd]); break;
    case NOP: sprintf_s(chString, "nop\n"); break;
    case JMPL:
      if(p->u.f3a.i==0) 
        sprintf_s(chString, "jmpl  %0x08, %s\n", (p->u.f3a.rs1+p->u.f3a.rs2), chReg[p->u.f3a.rd]);
      else 
      {
        tmp = (p->u.f3b.simm13&0x1000) ? (p->u.f3b.simm13|0xFFFFF000) : p->u.f3b.simm13;
        sprintf_s(chString, "jmpl  %s + 0x%02x, %s\n", chReg[p->u.f3b.rs1],tmp, chReg[p->u.f3b.rd]);
      }
    break;
    case JMP:
      if(p->u.f3a.i==0) 
        sprintf_s(chString, "jmp   %0x08\n", (p->u.f3a.rs1+p->u.f3a.rs2));
      else 
      {
        tmp = (p->u.f3b.simm13&0x1000) ? (p->u.f3b.simm13|0xFFFFF000) : p->u.f3b.simm13;
        sprintf_s(chString, "jmp   %s + 0x%02x\n", chReg[p->u.f3b.rs1],tmp);
      }
    break;
    case CLR: sprintf_s(chString, "clr   %s\n", chReg[p->u.f3c.rd]); break;
  };
  
  *pFile << chString;
}

//****************************************************************************
uint32 SparcV8::Common(CInstr *p)
{
  uint32 ret;
  switch(p->u.f3c.op3)
  {
    case 0x28:
      if((p->u.f3c.rs1==15)&&(p->u.f3c.rd==0)) ret = STBAR;
      else if(p->u.f3c.rs1!=0)                 ret = RDASR;
      else                                     ret = RDY;
    break;
    case 0x30:
      if(p->u.f3c.rd!=0) ret = WRASR;
      else               ret = WRY;
    break;
    default:
      ret = Common_SHIFT + p->u.f3c.op3;
  }
  return ret;
}

//****************************************************************************
uint32 SparcV8::LoadStore(CInstr *p)
{
  uint32 ret = LoadStore_SHIFT + p->u.f3a.op3;
  return ret;
}

//****************************************************************************
uint32 SparcV8::Bicc(CInstr *p)
{
  uint32 ret = Bicc_SHIFT + p->u.f2b.cond;
  return ret;
}

//****************************************************************************
uint32 SparcV8::Ticc(CInstr *p)
{
  uint32 ret = Ticc_SHIFT + p->u.f2b.cond;
  return ret;
}

//****************************************************************************
uint32 SparcV8::Synthetic(CInstr *p)
{
  uint32 ret = p->index;
  switch(p->index)
  {
    case SETHI:
      if((p->u.f2a.rd==0)&&(p->u.f2a.imm22==0)) p->index = NOP;
    break;
    case JMPL:
      if(p->u.f3a.rd==0) p->index = JMP;
    break;
    case OR:
      if((p->u.f3c.rs1==0) && (p->u.f3c.rs2==0)) p->index = CLR;
    break;
  }
  return ret;
}

