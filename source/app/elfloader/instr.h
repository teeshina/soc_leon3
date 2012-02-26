//****************************************************************************
// Property:    GNSS Sensor Limited
// License:     GNU2
// Contact:     chief@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#pragma once

#include "stdtypes.h"

const char chReg[][16] = 
{
  "%g0",//r0
  "%g1",//r1
  "%g2",//r2
  "%g3",//r3
  "%g4",//r4
  "%g5",//r5
  "%g6",//r6
  "%g7",//r7
  "%o0",//r8
  "%o1",//r9
  "%o2",//r10
  "%o3",//r11
  "%o4",//r12
  "%o5",//r13
  "%sp",//r14. stack pointer
  "%o7",//r15
  "%l0",
  "%l1",
  "%l2",
  "%l3",
  "%l4",
  "%l5",
  "%l6",
  "%l7",
  "%i0",
  "%i1",
  "%i2",
  "%i3",
  "%i4",
  "%i5",
  "%fp", // frame pointer
  "%i7"  // r31
};

static const uint32 UNKNOWN = 0;
static const uint32 CALL    = 1;
static const uint32 UNIMP   = 2;
static const uint32 SETHI   = 3;

static const uint32 Common_SHIFT = 4;
static const uint32 ADD        = Common_SHIFT + 0x0;
static const uint32 AND        = Common_SHIFT + 0x1;
static const uint32 OR         = Common_SHIFT + 0x2;
static const uint32 XOR        = Common_SHIFT + 0x3;
static const uint32 SUB        = Common_SHIFT + 0x4;
static const uint32 ANDN       = Common_SHIFT + 0x5;
static const uint32 ORN        = Common_SHIFT + 0x6;
static const uint32 XNOR       = Common_SHIFT + 0x7;
static const uint32 ADDX       = Common_SHIFT + 0x8;
//static const uint32 unused   = Common_SHIFT + 0x9;
static const uint32 UMUL       = Common_SHIFT + 0xA;
static const uint32 SMUL       = Common_SHIFT + 0xB;
static const uint32 SUBX       = Common_SHIFT + 0xC;
//static const uint32 unused   = Common_SHIFT + 0xD;
static const uint32 UDIV       = Common_SHIFT + 0xE;
static const uint32 SDIV       = Common_SHIFT + 0xF;
static const uint32 ADDcc      = Common_SHIFT + 0x10;
static const uint32 ANDcc      = Common_SHIFT + 0x11;
static const uint32 ORcc       = Common_SHIFT + 0x12;
static const uint32 XORcc      = Common_SHIFT + 0x13;
static const uint32 SUBcc      = Common_SHIFT + 0x14;
static const uint32 ANDNcc     = Common_SHIFT + 0x15;
static const uint32 ORNcc      = Common_SHIFT + 0x16;
static const uint32 XNORcc     = Common_SHIFT + 0x17;
static const uint32 ADDXcc     = Common_SHIFT + 0x18;
//static const uint32 unused   = Common_SHIFT + 0x19;
static const uint32 UMULcc     = Common_SHIFT + 0x1A;
static const uint32 SMULcc     = Common_SHIFT + 0x1B;
static const uint32 SUBXcc     = Common_SHIFT + 0x1C;
//static const uint32 unused   = Common_SHIFT + 0x1D;
static const uint32 UDIVcc     = Common_SHIFT + 0x1E;
static const uint32 SDIVcc     = Common_SHIFT + 0x1F;
static const uint32 TADDcc     = Common_SHIFT + 0x20;
static const uint32 TSUBcc     = Common_SHIFT + 0x21;
static const uint32 TADDccTV   = Common_SHIFT + 0x22;
static const uint32 TSUBccTV   = Common_SHIFT + 0x23;
static const uint32 MULScc     = Common_SHIFT + 0x24;
static const uint32 SLL        = Common_SHIFT + 0x25;
static const uint32 SRL        = Common_SHIFT + 0x26;
static const uint32 SRA        = Common_SHIFT + 0x27;
static const uint32 RDASR      = Common_SHIFT + 0x28; // rs1!=0
static const uint32 RDY        = Common_SHIFT + 0x2C; // rs1==0
static const uint32 STBAR      = Common_SHIFT + 0x2D; // rs1==15; rd=0
static const uint32 RDPSR      = Common_SHIFT + 0x29;
static const uint32 RDWIM      = Common_SHIFT + 0x2A;
static const uint32 RDTBR      = Common_SHIFT + 0x2B;
//static const uint32 unused   = Common_SHIFT + 0x2C; // RDY
//static const uint32 unused   = Common_SHIFT + 0x2D; // STBAR
//static const uint32 unused   = Common_SHIFT + 0x2E;
//static const uint32 unused   = Common_SHIFT + 0x2F;
static const uint32 WRASR      = Common_SHIFT + 0x30; // rd!=0
static const uint32 WRY        = Common_SHIFT + 0x3E; // rd==0
static const uint32 WRPSR      = Common_SHIFT + 0x31;
static const uint32 WRWIM      = Common_SHIFT + 0x32;
static const uint32 WRTBR      = Common_SHIFT + 0x33;
//static const uint32 FPop1      = Common_SHIFT + 0x34;
//static const uint32 FPop2      = Common_SHIFT + 0x35;
static const uint32 CPop1      = Common_SHIFT + 0x36;
static const uint32 CPop2      = Common_SHIFT + 0x37;
static const uint32 JMPL       = Common_SHIFT + 0x38;
static const uint32 RETT       = Common_SHIFT + 0x39; // return from a trap handler
//static const uint32 Ticc      = Common_SHIFT + 0x3A;
static const uint32 FLUSH      = Common_SHIFT + 0x3B;
static const uint32 SAVE       = Common_SHIFT + 0x3C;
static const uint32 RESTORE    = Common_SHIFT + 0x3D;

static const uint32 LoadStore_SHIFT = Common_SHIFT + 0x40;
static const uint32 LD         = LoadStore_SHIFT + 0x0;
static const uint32 LDUB       = LoadStore_SHIFT + 0x1;
static const uint32 LDUH       = LoadStore_SHIFT + 0x2;
static const uint32 LDD        = LoadStore_SHIFT + 0x3;
static const uint32 ST         = LoadStore_SHIFT + 0x4;
static const uint32 STB        = LoadStore_SHIFT + 0x5;
static const uint32 STH        = LoadStore_SHIFT + 0x6;
static const uint32 STD        = LoadStore_SHIFT + 0x7;
//static const uint32 none     = LoadStore_SHIFT + 0x8;
static const uint32 LDSB       = LoadStore_SHIFT + 0x9;
static const uint32 LDSH       = LoadStore_SHIFT + 0xA;
//static const uint32 none     = LoadStore_SHIFT + 0xB;
//static const uint32 none     = LoadStore_SHIFT + 0xC;
static const uint32 LDSTUB     = LoadStore_SHIFT + 0xD;
//static const uint32 none     = LoadStore_SHIFT + 0xE;
static const uint32 SWAP       = LoadStore_SHIFT + 0xF;
static const uint32 LDA        = LoadStore_SHIFT + 0x10;
static const uint32 LDUBA      = LoadStore_SHIFT + 0x11;
static const uint32 LDUHA      = LoadStore_SHIFT + 0x12;
static const uint32 LDDA       = LoadStore_SHIFT + 0x13;
static const uint32 STA        = LoadStore_SHIFT + 0x14;
static const uint32 STBA       = LoadStore_SHIFT + 0x15;
static const uint32 STHA       = LoadStore_SHIFT + 0x16;
static const uint32 STDA       = LoadStore_SHIFT + 0x17;
//static const uint32 none     = LoadStore_SHIFT + 0x18;
static const uint32 LDSBA      = LoadStore_SHIFT + 0x19;
static const uint32 LDSHA      = LoadStore_SHIFT + 0x1A;
//static const uint32 none     = LoadStore_SHIFT + 0x1B;
//static const uint32 none     = LoadStore_SHIFT + 0x1C;
static const uint32 LDSTUBA    = LoadStore_SHIFT + 0x1D;
//static const uint32 none     = LoadStore_SHIFT + 0x1E;
static const uint32 SWAPA      = LoadStore_SHIFT + 0x1F;
static const uint32 LDF        = LoadStore_SHIFT + 0x20;
static const uint32 LDFSR      = LoadStore_SHIFT + 0x21;
//static const uint32 none     = LoadStore_SHIFT + 0x22;
static const uint32 LDDF       = LoadStore_SHIFT + 0x23;
static const uint32 STF        = LoadStore_SHIFT + 0x24;
static const uint32 STFSR      = LoadStore_SHIFT + 0x25;
static const uint32 STDFQ      = LoadStore_SHIFT + 0x26;
static const uint32 STDF       = LoadStore_SHIFT + 0x27;
//static const uint32 none     = LoadStore_SHIFT + 0x28;
//static const uint32 none     = LoadStore_SHIFT + 0x29;
//static const uint32 none     = LoadStore_SHIFT + 0x2A;
//static const uint32 none     = LoadStore_SHIFT + 0x2B;
//static const uint32 none     = LoadStore_SHIFT + 0x2C;
//static const uint32 none     = LoadStore_SHIFT + 0x2D;
//static const uint32 none     = LoadStore_SHIFT + 0x2E;
//static const uint32 none     = LoadStore_SHIFT + 0x2F;
static const uint32 LDC        = LoadStore_SHIFT + 0x30;
static const uint32 LDCSR      = LoadStore_SHIFT + 0x31;
//static const uint32 none     = LoadStore_SHIFT + 0x32;
static const uint32 LDDC       = LoadStore_SHIFT + 0x33;
static const uint32 STC        = LoadStore_SHIFT + 0x34;
static const uint32 STCSR      = LoadStore_SHIFT + 0x35;
static const uint32 STDCQ      = LoadStore_SHIFT + 0x36;
static const uint32 STDC       = LoadStore_SHIFT + 0x37;
//static const uint32 none     = LoadStore_SHIFT + 0x38;
//static const uint32 none     = LoadStore_SHIFT + 0x39;
//static const uint32 none     = LoadStore_SHIFT + 0x3A;
//static const uint32 none     = LoadStore_SHIFT + 0x3B;
//static const uint32 none     = LoadStore_SHIFT + 0x3C;
//static const uint32 none     = LoadStore_SHIFT + 0x3D;
//static const uint32 none     = LoadStore_SHIFT + 0x3E;
//static const uint32 none     = LoadStore_SHIFT + 0x3F;

static const uint32 Bicc_SHIFT = LoadStore_SHIFT + 0x40;
static const uint32 BN         = Bicc_SHIFT + 0x0;
static const uint32 BE         = Bicc_SHIFT + 0x1;
static const uint32 BLE        = Bicc_SHIFT + 0x2;
static const uint32 BL         = Bicc_SHIFT + 0x3;
static const uint32 BLEU       = Bicc_SHIFT + 0x4;
static const uint32 BCS        = Bicc_SHIFT + 0x5;
static const uint32 BNEG       = Bicc_SHIFT + 0x6;
static const uint32 BVS        = Bicc_SHIFT + 0x7;
static const uint32 BA         = Bicc_SHIFT + 0x8;
static const uint32 BNE        = Bicc_SHIFT + 0x9;
static const uint32 BG         = Bicc_SHIFT + 0xA;
static const uint32 BGE        = Bicc_SHIFT + 0xB;
static const uint32 BGU        = Bicc_SHIFT + 0xC;
static const uint32 BCC        = Bicc_SHIFT + 0xD;
static const uint32 BPOS       = Bicc_SHIFT + 0xE;
static const uint32 BVC        = Bicc_SHIFT + 0xF;

static const uint32 Ticc_SHIFT = Bicc_SHIFT + 0x10;
static const uint32 TN         = Ticc_SHIFT + 0x0;
static const uint32 TE         = Ticc_SHIFT + 0x1;
static const uint32 TLE        = Ticc_SHIFT + 0x2;
static const uint32 TL         = Ticc_SHIFT + 0x3;
static const uint32 TLEU       = Ticc_SHIFT + 0x4;
static const uint32 TCS        = Ticc_SHIFT + 0x5;
static const uint32 TNEG       = Ticc_SHIFT + 0x6;
static const uint32 TVS        = Ticc_SHIFT + 0x7;
static const uint32 TA         = Ticc_SHIFT + 0x8;
static const uint32 TNE        = Ticc_SHIFT + 0x9;
static const uint32 TG         = Ticc_SHIFT + 0xA;
static const uint32 TGE        = Ticc_SHIFT + 0xB;
static const uint32 TGU        = Ticc_SHIFT + 0xC;
static const uint32 TCC        = Ticc_SHIFT + 0xD;
static const uint32 TPOS       = Ticc_SHIFT + 0xE;
static const uint32 TVC        = Ticc_SHIFT + 0xF;

static const uint32 Synth_SHIFT = Ticc_SHIFT + 0x10;
static const uint32 CLR        = Synth_SHIFT + 0x0;
static const uint32 CLRMEMB    = Synth_SHIFT + 0x1;
static const uint32 CLRMEMH    = Synth_SHIFT + 0x2;
static const uint32 CLRMEM     = Synth_SHIFT + 0x3;
static const uint32 JMP        = Synth_SHIFT + 0x4;
static const uint32 NOP        = Synth_SHIFT + 0x5;
static const uint32 MOV        = Synth_SHIFT + 0x6;
static const uint32 RET        = Synth_SHIFT + 0x7;
static const uint32 RETL       = Synth_SHIFT + 0x8;
static const uint32 CMP        = Synth_SHIFT + 0x9;
static const uint32 CALLREG    = Synth_SHIFT + 0xA;

struct SInstruction
{
  uint32 instr;
  char   chText[256];
};

SInstruction stInstr[] = 
{
  {UNKNOWN,"Unknown"},
  {CALL, "call"},
  {UNIMP, "unimp"},
  {SETHI, "sethi"},
  
  {ADD, "add"},
  {AND, "and"},
  {OR, "or"},
  {XOR, "xor"},
  {SUB, "sub"},
  {ANDN, "andn"},
  {ORN, "orn"},
  {XNOR, "xnor"},
  {ADDX, "addx"},
  {Common_SHIFT + 0x9, "none"},
  {UMUL, "umul"},
  {SMUL, "smul"},
  {SUBX, "subx"},
  {Common_SHIFT + 0xD, "none"},
  {UDIV, "udiv"},
  {SDIV, "sdiv"},
  {ADDcc, "addcc"},
  {ANDcc, "andcc"},
  {ORcc, "orcc"},
  {XORcc, "xorcc"},
  {SUBcc, "subcc"},
  {ANDNcc, "andnxx"},
  {ORNcc, "orncc"},
  {XNORcc, "xnorcc"},
  {ADDXcc, "addxcc"},
  {Common_SHIFT + 0x19, "none"},
  {UMULcc, "umulcc"},
  {SMULcc, "smulcc"},
  {SUBXcc, "subxcc"},
  {Common_SHIFT + 0x1D, "none"},
  {UDIVcc, "udivcc"},
  {SDIVcc, "sdivcc"},
  {TADDcc, "taddcc"},
  {TSUBcc, "tsubcc"},
  {TADDccTV, "taddcctv"},
  {TSUBccTV, "tsubcctv"},
  {MULScc, "mulscc"},
  {SLL, "sll"},
  {SRL, "srl"},
  {SRA, "sra"},
  {RDASR, "rdasr"},
  {RDPSR, "%psr"},
  {RDWIM, "%wim"},
  {RDTBR, "%tbr"},
  {RDY, "%y"},
  {STBAR, "stbar"},
  {Common_SHIFT + 0x2E, "none"},
  {Common_SHIFT + 0x2F, "none"},
  {WRASR, "WRASR"}, // rd!=0
  {WRPSR, "%psr"},
  {WRWIM, "%wim"},
  {WRTBR, "%tbr"},
  {Common_SHIFT + 0x34, "FPop1"},
  {Common_SHIFT + 0x35, "FPop2"},
  {CPop1, "CPop1"},
  {CPop2, "CPop2"},
  {JMPL, "JMPL"},
  {RETT, "rett"},
  {Common_SHIFT + 0x3A, "Ticc"},
  {FLUSH, "flush"},
  {SAVE, "save"},
  {RESTORE, "restore"},
  {WRY, "%y"}, // rd==0
  {Common_SHIFT + 0x3F, "none"},

  {LD,"ld"},
  {LDUB,"ldub"},
  {LDUH,"lduh"},
  {LDD,"ldd"},
  {ST,"st"},
  {STB,"stb"},
  {STH,"sth"},
  {STD,"std"},
  {LoadStore_SHIFT + 0x8,"none"},
  {LDSB,"LDSB"},
  {LDSH,"LDSH"},
  {LoadStore_SHIFT + 0xB,"none"},
  {LoadStore_SHIFT + 0xC,"none"},
  {LDSTUB,"LDSTUB"},
  {LoadStore_SHIFT + 0xE,"none"},
  {SWAP,"SWAP"},
  {LDA,"LDA"},
  {LDUBA,"LDUBA"},
  {LDUHA,"LDUHA"},
  {LDDA,"LDDA"},
  {STA,"sta"},
  {STBA,"stba"},
  {STHA,"stha"},
  {STDA,"stda"},
  {LoadStore_SHIFT + 0x18,"none"},
  {LDSBA,"LDSBA"},
  {LDSHA,"LDSHA"},
  {LoadStore_SHIFT + 0x1B,"none"},
  {LoadStore_SHIFT + 0x1C,"none"},
  {LDSTUBA,"LDSTUBA"},
  {LoadStore_SHIFT + 0x1E,"none"},
  {SWAPA,"SWAPA"},
  {LDF,"LDF"},
  {LDFSR,"LDFSR"},
  {LoadStore_SHIFT + 0x22,"none"},
  {LDDF,"LDDF"},
  {STF,"STF"},
  {STFSR,"STFSR"},
  {STDFQ,"STDFQ"},
  {STDF,"STDF"},
  {LoadStore_SHIFT + 0x28,"none"},
  {LoadStore_SHIFT + 0x29,"none"},
  {LoadStore_SHIFT + 0x2A,"none"},
  {LoadStore_SHIFT + 0x2B,"none"},
  {LoadStore_SHIFT + 0x2C,"none"},
  {LoadStore_SHIFT + 0x2D,"none"},
  {LoadStore_SHIFT + 0x2E,"none"},
  {LoadStore_SHIFT + 0x2F,"none"},
  {LDC,"LDC"},
  {LDCSR,"LDCSR"},
  {LoadStore_SHIFT + 0x32,"none"},
  {LDDC,"LDDC"},
  {STC,"STC"},
  {STCSR,"STCSR"},
  {STDCQ,"STDCQ"},
  {STDC,"STDC"},
  {LoadStore_SHIFT + 0x38,"none"},
  {LoadStore_SHIFT + 0x39,"none"},
  {LoadStore_SHIFT + 0x3A,"none"},
  {LoadStore_SHIFT + 0x3B,"none"},
  {LoadStore_SHIFT + 0x3C,"none"},
  {LoadStore_SHIFT + 0x3D,"none"},
  {LoadStore_SHIFT + 0x3E,"none"},
  {LoadStore_SHIFT + 0x3F,"none"},
    
  {BN,"bn"},
  {BE,"be"},
  {BLE,"ble"},
  {BL,"bl"},
  {BLEU,"bleu"},
  {BCS,"bcs"},
  {BNEG,"bneg"},
  {BVS,"bvs"},
  {BA,"ba"},
  {BNE,"bne"},
  {BG,"bg"},
  {BGE,"bge"},
  {BGU,"bgu"},
  {BCC,"bcc"},
  {BPOS,"bpos"},
  {BVC,"bvc"},

  {TN,"tn"},
  {TE,"te"},
  {TLE,"tle"},
  {TL,"tl"},
  {TLEU,"tleu"},
  {TCS,"tcs"},
  {TNEG,"tneg"},
  {TVS,"tvs"},
  {TA,"ta"},
  {TNE,"tne"},
  {TG,"tg"},
  {TGE,"tge"},
  {TGU,"tgu"},
  {TCC,"tcc"},
  {TPOS,"tpos"},
  {TVC,"tvc"},
  
  {CLR, "clr"},
  {CLRMEMB, "clrb"},//clear byte
  {CLRMEMH, "clrh"},//clear halfword
  {CLRMEM, "clr"},//clear word
  {JMP, "jmp"},
  {NOP, "nop"},
  {MOV, "mov"},
  {RET, "ret"},
  {RETL, "retl"},
  {CMP, "cmp"},
  {CALLREG, "call"},
};

