// Description: Instruction definitions according to the SPARC V8 manual.

#pragma once

#include "stdtypes.h"

// op decoding (inst(31 downto 30))
static const uint32 FMT2     = 0x0;//"00";
static const uint32 CALL     = 0x1;//"01";  // PC-relative control transfer to address PC+4*disp30
static const uint32 FMT3     = 0x2;//"10";  // op3 instruction marker
static const uint32 LDST     = 0x3;//"11";

// op2 decoding (inst(24 downto 22))
static const uint32 UNIMP    = 0x0;//"000";
static const uint32 BICC     = 0x2;//"010";
static const uint32 SETHI    = 0x4;//"100";
static const uint32 FBFCC    = 0x6;//"110";
static const uint32 CBCCC    = 0x7;//"111";

// op3 decoding (inst(24 downto 19))
static const uint32 IADD     = 0x00;//"000000"; // Add
static const uint32 IAND     = 0x01;//"000001"; // And
static const uint32 IOR      = 0x02;//"000010"; // Inclusive Or
static const uint32 IXOR     = 0x03;//"000011"; // Exclusive Or
static const uint32 ISUB     = 0x04;//"000100"; // Subtract
static const uint32 ANDN     = 0x05;//"000101"; // And Not
static const uint32 ORN      = 0x06;//"000110"; // Inclusive Or Not
static const uint32 IXNOR    = 0x07;//"000111"; // Exclusive Or Not
static const uint32 ADDX     = 0x08;//"001000"; // Add with Carry
static const uint32 UMUL     = 0x0A;//"001010"; // Unsigned Integer Multiply
static const uint32 SMUL     = 0x0B;//"001011"; // Signed Integer Multiply
static const uint32 SUBX     = 0x0C;//"001100"; // Subtract with Carry
static const uint32 UDIV     = 0x0E;//"001110"; // Unsigned Integer Divider
static const uint32 SDIV     = 0x0F;//"001111"; // Signed Integer Divider
static const uint32 ADDCC    = 0x10;//"010000"; // Add and modify icc
static const uint32 ANDCC    = 0x11;//"010001"; // And and modify icc
static const uint32 ORCC     = 0x12;//"010010"; // Or and modify icc
static const uint32 XORCC    = 0x13;//"010011"; // Xor and modify icc
static const uint32 SUBCC    = 0x14;//"010100"; // Subtract and modify icc
static const uint32 ANDNCC   = 0x15;//"010101"; // And Not and modify icc
static const uint32 ORNCC    = 0x16;//"010110"; // Or Not and modify icc
static const uint32 XNORCC   = 0x17;//"010111"; // Xor Not and modify icc
static const uint32 ADDXCC   = 0x18;//"011000"; // Add with Carry and modify icc
static const uint32 UMULCC   = 0x1A;//"011010"; // Unsigned Integer Multiply and modify icc
static const uint32 SMULCC   = 0x1B;//"011011"; // Signed Integer Multiply and modify icc
static const uint32 SUBXCC   = 0x1C;//"011100"; // Subtract with Carry and modify icc
static const uint32 UDIVCC   = 0x1E;//"011110"; // Unsigned Integer Divider and modify icc
static const uint32 SDIVCC   = 0x1F;//"011111"; // Signed Integer Divider and modify icc
static const uint32 TADDCC   = 0x20;//"100000"; // Tagged Add and modify icc
static const uint32 TSUBCC   = 0x21;//"100001"; // Tagged Subtract and modify icc
static const uint32 TADDCCTV = 0x22;//"100010"; // Tagged Add modify icc and Trap on Overflow
static const uint32 TSUBCCTV = 0x23;//"100011"; // Tagged Subtract modify icc and Trap On Overflow
static const uint32 MULSCC   = 0x24;//"100100"; // Multiply Step and modify icc
static const uint32 ISLL     = 0x25;//"100101"; // Shift Left Logical
static const uint32 ISRL     = 0x26;//"100110"; // Shift Right Logical
static const uint32 ISRA     = 0x27;//"100111"; // Shift Right Arithmetic
static const uint32 RDY      = 0x28;//"101000"; // Read Y register
static const uint32 RDPSR    = 0x29;//"101001"; // *Read Processor State Register
static const uint32 RDWIM    = 0x2A;//"101010"; // *Read Widnow Invalid Mask Register
static const uint32 RDTBR    = 0x2B;//"101011"; // *Read Trap Base Register
static const uint32 WRY      = 0x30;//"110000"; // Write Y Register
static const uint32 WRPSR    = 0x31;//"110001"; // *Write Processor State Register
static const uint32 WRWIM    = 0x32;//"110010"; // *Write Window Invalid Mask Register
static const uint32 WRTBR    = 0x33;//"110011"; // Write Trap Base Register
static const uint32 FPOP1    = 0x34;//"110100"; // Floating-point operate
static const uint32 FPOP2    = 0x35;//"110101"; // Floating-point operate
static const uint32 CPOP1    = 0x36;//"110110";
static const uint32 CPOP2    = 0x37;//"110111";
static const uint32 JMPL     = 0x38;//"111000"; // register-indirect control transfer to the address
static const uint32 RETT     = 0x39;//"111001"; // *Return from Trap. Privileged
static const uint32 TICC     = 0x3A;//"111010";
static const uint32 FLUSH    = 0x3B;//"111011"; // Flush Instruction Memory
static const uint32 SAVE     = 0x3C;//"111100"; // Save caller's window
static const uint32 RESTORE  = 0x3D;//"111101"; // Restore caller's window
static const uint32 UMAC     = 0x3E;//"111110";
static const uint32 SMAC     = 0x3F;//"111111";

static const uint32 LD       = 0x00;//"000000"; // Load Word
static const uint32 LDUB     = 0x01;//"000001"; // Load Unsiged Byte
static const uint32 LDUH     = 0x02;//"000010"; // Load Unsiged Halfword
static const uint32 LDD      = 0x03;//"000011"; // Load Doubleword
static const uint32 LDSB     = 0x09;//"001001"; // Load Signed Byte
static const uint32 LDSH     = 0x0A;//"001010"; // Load Signed Halfword
static const uint32 LDSTUB   = 0x0D;//"001101"; // Atomic Load-Store Unsigned Byte.
static const uint32 SWAP     = 0x0F;//"001111"; // SWAP register with memory
static const uint32 LDA      = 0x10;//"010000"; // *Load Word from Alternate space
static const uint32 LDUBA    = 0x11;//"010001"; // *Load Unsigned Byte from Alternate space
static const uint32 LDUHA    = 0x12;//"010010"; // *Load Unsiged Halfword from Alternate space
static const uint32 LDDA     = 0x13;//"010011"; // *Load Doubleword from Alternate space
static const uint32 LDSBA    = 0x19;//"011001"; // *Load Signed Byte from Alternate space
static const uint32 LDSHA    = 0x1A;//"011010"; // *Load Signed Halfword from Alternate space
static const uint32 LDSTUBA  = 0x1D;//"011101"; // *Atomic Load-Store Unsigned Byte into Alternate memory. Privileged.
static const uint32 SWAPA    = 0x1F;//"011111"; // *SWAP register with Alternate space memory
static const uint32 LDF      = 0x20;//"100000"; // Load Floating-point
static const uint32 LDFSR    = 0x21;//"100001"; // Load Floating-point State Register
static const uint32 LDDF     = 0x23;//"100011"; // Load Double Floating-point
static const uint32 LDC      = 0x30;//"110000"; // Load Coprocessor
static const uint32 LDCSR    = 0x31;//"110001"; // Load Coprocessor State Register
static const uint32 LDDC     = 0x33;//"110011"; // Load Double Coprocessor
static const uint32 ST       = 0x04;//"000100"; // Store Word
static const uint32 STB      = 0x05;//"000101"; // Store Byte
static const uint32 STH      = 0x06;//"000110"; // Store Halfword
static const uint32 ISTD     = 0x07;//"000111"; // Store Double word (int imho!)
static const uint32 STA      = 0x14;//"010100"; // *Store Word into Alternate space. Privileged instruction
static const uint32 STBA     = 0x15;//"010101"; // *Store Byte into Alternate space. Privileged instruction
static const uint32 STHA     = 0x16;//"010110"; // *Store Halfword into Alternate space. Privileged instruction
static const uint32 STDA     = 0x17;//"010111"; // *Store Doubleword into Alternate space. Privileged instruction
static const uint32 STF      = 0x24;//"100100"; // Store floating-point
static const uint32 STFSR    = 0x25;//"100101"; // Store floating-point State Register
static const uint32 STDFQ    = 0x26;//"100110"; // *Store Double floating-point deferred-trap Queue. Privileged instr.
static const uint32 STDF     = 0x27;//"100111"; // Store Double Floating-point
static const uint32 STC      = 0x34;//"110100"; // Store Coprocessor
static const uint32 STCSR    = 0x35;//"110101"; // Store Coprocessor State Register
static const uint32 STDCQ    = 0x36;//"110110"; // *Store Double Coprocessor Queue. Privileged
static const uint32 STDC     = 0x37;//"110111"; // Store Double Coprocessor
static const uint32 CASA     = 0x3C;//"111100";

// Branch on Integer Condition Codes Instructions
// bicc decoding (inst(27 downto 25))
static const uint32 BA       = 0x8;//"1000";    // Branch always

// fpop1 decoding
// [8:0]
static const uint32 FITOS    = 0x0C4;//"011000100"; // Convert Integer to Single
static const uint32 FITOD    = 0x0C8;//"011001000"; // Convert Integer to Double
static const uint32 FSTOI    = 0x0D1;//"011010001"; 
static const uint32 FDTOI    = 0x0D2;//"011010010";
static const uint32 FSTOD    = 0x0C9;//"011001001";
static const uint32 FDTOS    = 0x0C6;//"011000110";
static const uint32 FMOVS    = 0x001;//"000000001"; // Move
static const uint32 FNEGS    = 0x005;//"000000101"; // Negative
static const uint32 FABSS    = 0x009;//"000001001"; // Absolute value
static const uint32 FSQRTS   = 0x029;//"000101001";
static const uint32 FSQRTD   = 0x02A;//"000101010";
static const uint32 FADDS    = 0x041;//"001000001";
static const uint32 FADDD    = 0x042;//"001000010";
static const uint32 FSUBS    = 0x045;//"001000101";
static const uint32 FSUBD    = 0x046;//"001000110";
static const uint32 FMULS    = 0x049;//"001001001";
static const uint32 FMULD    = 0x04A;//"001001010";
static const uint32 FSMULD   = 0x069;//"001101001";
static const uint32 FDIVS    = 0x04D;//"001001101";
static const uint32 FDIVD    = 0x04E;//"001001110";

// fpop2 decoding
static const uint32 FCMPS    = 0x051;//"001010001";
static const uint32 FCMPD    = 0x052;//"001010010";
static const uint32 FCMPES   = 0x055;//"001010101";
static const uint32 FCMPED   = 0x056;//"001010110";

// trap type decoding
//[5:0]
static const uint32 TT_IAEX   = 0x01;//"000001";
static const uint32 TT_IINST  = 0x02;//"000010";
static const uint32 TT_PRIV   = 0x03;//"000011";
static const uint32 TT_FPDIS  = 0x04;//"000100";
static const uint32 TT_WINOF  = 0x05;//"000101";
static const uint32 TT_WINUF  = 0x06;//"000110";
static const uint32 TT_UNALA  = 0x07;//"000111";
static const uint32 TT_FPEXC  = 0x08;//"001000";
static const uint32 TT_DAEX   = 0x09;//"001001";
static const uint32 TT_TAG    = 0x0A;//"001010";
static const uint32 TT_WATCH  = 0x0B;//"001011";

static const uint32 TT_DSU    = 0x10;//"010000";
static const uint32 TT_PWD    = 0x11;//"010001";

static const uint32 TT_RFERR  = 0x20;//"100000";
static const uint32 TT_IAERR  = 0x21;//"100001";
static const uint32 TT_CPDIS  = 0x24;//"100100";
static const uint32 TT_CPEXC  = 0x28;//"101000";
static const uint32 TT_DIV    = 0x2A;//"101010";
static const uint32 TT_DSEX   = 0x2B;//"101011";
static const uint32 TT_TICC   = 0x3F;//"111111";
static const uint32 TT_ICERR  = 0x30;//"110000"; // icache correctable error
static const uint32 TT_DCERR  = 0x34;//"110100"; // dcache correctable error
static const uint32 TT_RCERR  = 0x38;//"111000"; // RF correctable error

// Alternate address space identifiers (only 5 lsb bist are used)
// [4:0]
static const uint32 ASI_SYSR    = 0x02;//"00010"; -- 0x02
static const uint32 ASI_UINST   = 0x08;//"01000"; -- 0x08
static const uint32 ASI_SINST   = 0x09;//"01001"; -- 0x09
static const uint32 ASI_UDATA   = 0x0A;//"01010"; -- 0x0A
static const uint32 ASI_SDATA   = 0x0B;//"01011"; -- 0x0B
static const uint32 ASI_ITAG    = 0x0C;//"01100"; -- 0x0C
static const uint32 ASI_IDATA   = 0x0D;//"01101"; -- 0x0D
static const uint32 ASI_DTAG    = 0x0E;//"01110"; -- 0x0E
static const uint32 ASI_DDATA   = 0x0F;//"01111"; -- 0x0F
static const uint32 ASI_IFLUSH  = 0x10;//"10000"; -- 0x10
static const uint32 ASI_DFLUSH  = 0x11;//"10001"; -- 0x11

static const uint32 ASI_FLUSH_PAGE     = 0x10;//"10000";  -- 0x10 i/dcache flush page
static const uint32 ASI_FLUSH_CTX      = 0x13;//"10011";  -- 0x13 i/dcache flush ctx

static const uint32 ASI_DCTX           = 0x14;//"10100";  -- 0x14 dcache ctx
static const uint32 ASI_ICTX           = 0x15;//"10101";  -- 0x15 icache ctx

static const uint32 ASI_MMUFLUSHPROBE  = 0x18;//"11000";  -- 0x18 i/dtlb flush/(probe)
static const uint32 ASI_MMUREGS        = 0x19;//"11001";  -- 0x19 mmu regs access
static const uint32 ASI_MMU_BP         = 0x1C;//"11100";  -- 0x1c mmu Bypass 
static const uint32 ASI_MMU_DIAG       = 0x1D;//"11101";  -- 0x1d mmu diagnostic 
//static const uint32 ASI_MMU_DSU        = "11111";  -- 0x1f mmu diagnostic 

static const uint32 ASI_MMUSNOOP_DTAG  = 0x1E;//"11110";  -- 0x1e mmusnoop physical dtag 

// ftt decoding
//[2:0]
static const uint32 FPIEEE_ERR  = 0x1;//"001";
static const uint32 FPSEQ_ERR   = 0x4;//"100";
static const uint32 FPHW_ERR    = 0x5;//"101";



