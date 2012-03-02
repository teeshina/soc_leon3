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

#include "..\..\lib\leon3_ml605\mem\romdata.h"

//****************************************************************************
void SparcV8::Disassemler(SrcImage *pImage, std::ofstream *pFile)
{
#if 0
  for (int32 i=0; i<=190; i++)
  {
    instr.u.v  = RomData[i].data;
    entry_adr=0;
#else
  for (int32 i=0; i<=(pImage->iSizeBytes>>2); i++)
  {
    instr.u.v  = pImage->arr[i].val;
#endif
    instr.format = INSTR_FORMAT_UNDEF;
  
    DisasInstr(&instr);
    PrintInstruction(pImage->arr[i].adr, &instr, pFile);
  }
}

//****************************************************************************
void SparcV8::DisasInstr(CInstr *p)
{
  // parse operation:
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
          p->index  = Ticc(&instr);
        break;
        default:
          p->format = INSTR_F3c;
          p->index  = Common(&instr);
      }
    break;

    case 0x3:
      if(p->u.f3a.i==0) p->format = INSTR_F3a;
      else              p->format = INSTR_F3b;
      p->index  = LoadStore(&instr);
    break;
  }

  Synthetic(&instr);
}

//****************************************************************************
void SparcV8::PrintInstruction(uint32 adr, CInstr *p, std::ofstream *pFile)
{
  sprintf_s(chString, "%08x %08x   ", adr, p->u.v);
  *pFile << chString;

  uint32 o   = p->index;
  char *cho  = stInstr[o].chText;

  uint32 rd  = p->u.f3a.rd;
  uint32 rs1 = p->u.f3a.rs1;
  uint32 rs2 = p->u.f3a.rs2;

  uint32 i = p->u.f3a.i;

  int32 disp30 = int32(p->u.f1.disp30);

  int32 disp22 = int32(p->u.f2b.disp22);
  if(disp22&0x200000) disp22 |= 0xffe00000;

  uint32 imm22 = p->u.f2a.imm22;

  int32  simm13 = int32(p->u.f3b.simm13);
  if(simm13&0x1000) simm13 |= 0xfffff000;

  int32 imm7 = int32(p->u.f3b.simm13&0xff);
  if(imm7&0x80) imm7 |= 0xFFFFFF00;

  uint32 asi = p->u.f3a.asi;

  sprintf_s(chString,"%s !!! undefined\n", cho);
  if(o==CALL)
  {
    sprintf_s(chString, "call  0x%08x\n",int32(adr) + 4*disp30);
  }else if(o==UNIMP)
  {
    sprintf_s(chString, "unimp\n");
  }else if(o==SETHI)
  {
    sprintf_s(chString, "%s  %%hi(0x%x), %s\n", cho, (imm22<<10), chReg[rd]);
  }else if( (o==OR)||(o==ORcc)||(o==XOR)||(o==XORcc) )
  {
    if(i==0)           sprintf_s(chString, "%s  %s, %s, %s\n", cho, chReg[rs1], chReg[rs2], chReg[rd]);
    else if(simm13==0) sprintf_s(chString, "%s  %s, %s\n", cho, chReg[rs1], chReg[rd]);
    else               sprintf_s(chString, "%s  %s, 0x%x, %s\n", cho, chReg[rs1], simm13, chReg[rd]);
  }else if( (o==AND)||(o==ANDN)||(o==ORN)||(o==XNOR)
          ||(o==ANDcc)||(o==ANDNcc)||(o==ORNcc)||(o==XNORcc) )
  {
    if(i==0)          sprintf_s(chString, "%s  %s, %s, %s\n", cho, chReg[rs1], chReg[rs2], chReg[rd]);
    else if(simm13<0) sprintf_s(chString, "%s  %s, 0x%x, %s\n", cho, chReg[rs1], simm13, chReg[rd]);
    else              sprintf_s(chString, "%s  %s, 0x%x, %s\n", cho, chReg[rs1], simm13, chReg[rd]);

  }else if( (o==ADD)||(o==SUB)||(o==ADDX)||(o==UMUL)||(o==SMUL)||(o==SUBX)||(o==UDIV)||(o==SDIV)
          ||(o==ADDcc)||(o==SUBcc)||(o==ADDXcc)||(o==UMULcc)||(o==SMULcc)||(o==SUBXcc)||(o==UDIVcc)||(o==SDIVcc)
          ||(o==TADDcc)||(o==TSUBcc)||(o==TADDccTV)||(o==TSUBccTV)||(o==MULScc) )
  {
    // if rs1==0, GRMON doesn't show rs1.
    if(i==0)          sprintf_s(chString, "%s  %s, %s, %s\n", cho, chReg[rs1], chReg[rs2], chReg[rd]);
    else if(simm13<0) sprintf_s(chString, "%s  %s, %d, %s\n", cho, chReg[rs1], simm13, chReg[rd]);
    else              sprintf_s(chString, "%s  %s, %d, %s\n", cho, chReg[rs1], simm13, chReg[rd]);
  }else if((o==SLL)||(o==SRL)||(o==SRA))
  {
    if(i==0) sprintf_s(chString, "%s  %s, %s, %s\n", cho, chReg[rs1], chReg[rs2], chReg[rd]);
    else     sprintf_s(chString, "%s  %s, %d, %s\n", cho, chReg[rs1], (simm13&0x1f), chReg[rd]);
  }else if(o==RDASR)
  {
    sprintf_s(chString, "mov  %%asr%i, %s\n", rs1, chReg[rd]);
  }else if(o==STBAR)
  {
  //TODO:
  }else if((o==RDY)||(o==RDPSR)||(o==RDWIM)||(o==RDTBR))
  {
    sprintf_s(chString, "mov  %s, %s\n", cho, chReg[rd]);
  }else if(o==WRASR)
  {
    if(rs1==0)
    {
      if(i==0) sprintf_s(chString, "mov  %s, %%asr%i\n", chReg[rs2], rd);
      else     sprintf_s(chString, "mov  0x%x, %%asr%i\n", simm13, rd);
    }else
    {
      if(i==0) sprintf_s(chString, "wr  %s ^ %s, %%asr%i\n", chReg[rs1], chReg[rs2], rd);
      else     sprintf_s(chString, "wr  %s ^ 0x%x, %%asr%i\n", chReg[rs1], simm13, rd);
    }
  }else if((o==WRPSR)||(o==WRTBR)||(o==WRWIM)||(o==WRY))
  {
    if(rs1==0)
    {
      if(i==0) sprintf_s(chString, "mov  %s, %s\n", chReg[rs2], cho);
      else     sprintf_s(chString, "mov  0x%x, %p\n", simm13, cho);
    }else
    {
      if(i==0) sprintf_s(chString, "wr  %s ^ %s, %s\n", chReg[rs1], chReg[rs2], cho);
      else     sprintf_s(chString, "wr  %s ^ 0x%x, %s\n", chReg[rs1], simm13, cho);
    }
  }else if(o==CPop1)
  {
  //TODO:
  }else if(o==CPop2)
  {
  //TODO:
  }else if(o==JMPL)
  {
    if(i==0)          sprintf_s(chString, "jmpl  %s + %s, %s\n", chReg[rs1], chReg[rs2], chReg[rd]);
    else if(simm13<0) sprintf_s(chString, "jmpl  %s - 0x%x, %s\n", chReg[rs1], -simm13, chReg[rd]);
    else              sprintf_s(chString, "jmpl  %s + 0x%x, %s\n", chReg[rs1], simm13, chReg[rd]);
  }else if((o==RETT)||(o==FLUSH))
  {
    if(i==0)          sprintf_s(chString,"%s %s + %s\n", cho, chReg[rs1],chReg[rs2]);
    else if(simm13<0) sprintf_s(chString,"%s %s - %d\n", cho, chReg[rs1],-simm13);
    else              sprintf_s(chString,"%s %s + %d\n", cho, chReg[rs1],simm13);
  }else if((o==SAVE)||(o==RESTORE))
  {
    if((i==0)&&(rs1==0)&&(rs2==0)&&(rd==0)) sprintf_s(chString, "%s\n",cho);
    else if(i==0) sprintf_s(chString, "%s  %s, %s, %s\n", cho, chReg[rs1], chReg[rs2], chReg[rd]);
    else          sprintf_s(chString, "%s  %s, %d, %s\n", cho, chReg[rs1], simm13, chReg[rd]);
  }else if((o==LD)||(o==LDUB)||(o==LDUH)||(o==LDD))
  {
    if((i==0)&&(rs2==0)) sprintf_s(chString, "%s  [%s], %s\n", cho, chReg[rs1], chReg[rd]);
    else if(i==0)     sprintf_s(chString, "%s  [%s + %s], %s\n", cho, chReg[rs1], chReg[rs2], chReg[rd]);
    else if(simm13<0) sprintf_s(chString, "%s  [%s - 0x%x], %s\n", cho, chReg[rs1], -simm13, chReg[rd]);
    else              sprintf_s(chString, "%s  [%s + 0x%x], %s\n", cho, chReg[rs1], simm13, chReg[rd]);
  }else if((o==ST)||(o==STB)||(o==STH)||(o==STD))
  {
    if((i==0)&&(rs2==0))sprintf_s(chString, "%s  %s, [%s]\n", cho, chReg[rd], chReg[rs1]);
    else if(i==0)     sprintf_s(chString, "%s  %s, [%s + %s]\n", cho, chReg[rd], chReg[rs1], chReg[rs2]);
    else if(simm13<0) sprintf_s(chString, "%s  %s, [%s - 0x%x]\n", cho, chReg[rd], chReg[rs1], -simm13);// checked!
    else              sprintf_s(chString, "%s  %s, [%s + 0x%x]\n", cho, chReg[rd], chReg[rs1], simm13);
  }else if(o==LDSB)
  {
  //TODO:
  }else if(o==LDSH)
  {
  //TODO:
  }else if(o==LDSTUB)
  {
  //TODO:
  }else if(o==SWAP)
  {
  //TODO:
  }else if(o==LDA)
  {
  //TODO:
  }else if(o==LDUBA)
  {
  //TODO:
  }else if(o==LDUHA)
  {
  //TODO:
  }else if(o==LDDA)
  {
  //TODO:
  }else if((o==STA)||(o==STBA)||(o==STHA)||(o==STDA))
  {
    if((i==0)&&(rs2==0))sprintf_s(chString, "%s  %s, [%s]%i\n", cho, chReg[rd], chReg[rs1],asi);
    else if(i==0)     sprintf_s(chString, "%s  %s, [%s + %s]%i\n", cho, chReg[rd], chReg[rs1], chReg[rs2],asi);
  }else if(o==LDSBA)
  {
  //TODO:
  }else if(o==LDSHA)
  {
  //TODO:
  }else if(o==LDSTUBA)
  {
  //TODO:
  }else if(o==SWAPA)
  {
  //TODO:
  }else if(o==LDF)
  {
  //TODO:
  }else if(o==LDFSR)
  {
  //TODO:
  }else if(o==LDDF)
  {
  //TODO:
  }else if(o==STF)
  {
  //TODO:
  }else if(o==STFSR)
  {
  //TODO:
  }else if(o==STDFQ)
  {
  //TODO:
  }else if(o==STDF)
  {
  //TODO:
  }else if(o==LDC)
  {
  //TODO:
  }else if(o==LDCSR)
  {
  //TODO:
  }else if(o==LDDC)
  {
  //TODO:
  }else if(o==STC)
  {
  //TODO:
  }else if(o==STCSR)
  {
  //TODO:
  }else if(o==STDCQ)
  {
  //TODO:
  }else if(o==STDC)
  {
  //TODO:
  }else if((o>=BN)&&(o<=BVC))
  {
    if(p->u.f2b.a) sprintf_s(chString, "%s,a  0x%08x\n",cho , adr + (4*disp22));
    else           sprintf_s(chString, "%s  0x%08x\n",cho , adr + (4*disp22));
  }else if((o>=TN)&&(o<=TVC))
  {
    if(i==0) sprintf_s(chString, "%s  %s + %s\n",cho, chReg[rs1], chReg[rs2]);
    else     sprintf_s(chString, "%s  0x%x\n",cho , imm7);
  }else if(o==CLR)
  {
    sprintf_s(chString, "clr  %s\n", chReg[rd]);
  }else if((o==CLRMEM)||(o==CLRMEMB)||(o==CLRMEMH))
  {
    if((i==0)&&(rs2==0)) sprintf_s(chString, "%s  [%s]\n", cho, chReg[rs1]);
    else if(i==0)        sprintf_s(chString, "%s  [%s + %s]\n", cho, chReg[rs1], chReg[rs2]);
    else if(simm13<0)    sprintf_s(chString, "%s  [%s - 0x%x]\n", cho, chReg[rs1], -simm13);
    else                 sprintf_s(chString, "%s  [%s + 0x%x]\n", cho, chReg[rs1], simm13);
  }else if((o==JMP))
  {
    if(simm13<0)      sprintf_s(chString, "jmp  %s - 0x%x\n", chReg[rs1],-simm13);
    else if(simm13>0) sprintf_s(chString, "jmp  %s + 0x%x\n", chReg[rs1], simm13);
    else              sprintf_s(chString, "jmp  %s\n", chReg[rs1]);
  }else if(o==NOP)
  {
    sprintf_s(chString, "nop\n");
  }else if(o==MOV)
  {
    if(i==0) sprintf_s(chString, "mov  %s, %s\n", chReg[rs2], chReg[rd]);
    else     sprintf_s(chString, "mov  %i, %s\n", simm13, chReg[rd]);
  }else if((o==RET)||(o==RETL))
  {
    sprintf_s(chString, "%s\n",cho);
  }else if(o==CMP)
  {
    if((i==0)&&(rs1==0)) sprintf_s(chString, "%s  %s\n",cho, chReg[rs2]);
    else if(i==0)        sprintf_s(chString, "%s  %s, %s\n",cho, chReg[rs1], chReg[rs2]);
    else if(simm13==0) sprintf_s(chString, "%s  %s\n",cho, chReg[rs1]);
    else               sprintf_s(chString, "%s  %s, %d\n",cho, chReg[rs1], simm13);
  }else if(o==CALLREG)
  {
    if((i==0)&&(rs2==0)) sprintf_s(chString, "call  %s\n",chReg[rs1]);
    else if(i==0)        sprintf_s(chString, "call  %s + %s\n",chReg[rs1],chReg[rs2]);
    else if(simm13<0)    sprintf_s(chString, "call  %s - %d\n",chReg[rs1],-simm13);
    else                 sprintf_s(chString, "call  %s + %d\n",chReg[rs1],simm13);
  }
    

  
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

  uint32 rd  = p->u.f3a.rd;
  uint32 rs1 = p->u.f3a.rs1;
  uint32 rs2 = p->u.f3a.rs2;
  uint32 i = p->u.f3a.i;

  switch(p->index)
  {
    case SETHI:
      if((rd==0)&&(p->u.f2a.imm22==0)) p->index = NOP;
    break;
    case JMPL:
      if((rd==0)&&(i==1)&&(rs1==31)&&(p->u.f3b.simm13==8)) p->index = RET;
      else if((rd==0)&&(i==1)&&(rs1==15)&&(p->u.f3b.simm13==8)) p->index = RETL;
      else if(rd==0) p->index = JMP;
      else if(rd==15) p->index = CALLREG;
    break;
    case OR:
      if((i==0)&&(rs1==0) && (rs2==0)) p->index = CLR;
      else if(rs1==0)          p->index = MOV;
    break;
    case ST:
      if(rd==0) p->index = CLRMEM;
    break;
    case STB:
      if(rd==0) p->index = CLRMEMB;
    break;
    case STH:
      if(rd==0) p->index = CLRMEMH;
    break;
    case SUBcc:
      if(rd==0) p->index = CMP;
    break;
  }
  return ret;
}

