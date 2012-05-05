//****************************************************************************
// Author:	    Jiri Gaisler - Gaisler Research
// C++ version: GNSS Sensor Limited
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#pragma once

struct RomValue
{
  uint32 adr;
  uint32 data;
};

static const uint32 ROM_SIZE = 0x1<<8;
const RomValue RomData[ROM_SIZE] = 
{
    {0x00000, 0x81D82000},// flush %g0 + 0
    {0x00001, 0x033CC000},// sethi %hi(0xf3000000), %g1
    {0x00002, 0x821060C0},// or %g1, 0xc0, %g1; 
    {0x00003, 0x81884000},// wr %g1 ^ %g0, %psr
    {0x00004, 0x81900000},// mov %g0, %wim
    {0x00005, 0x81980000},// mov %g0, %tbr
    {0x00006, 0x81800000},// mov %g0, %y0
    {0x00007, 0x01000000},// nop
    {0x00008, 0x03002040},// sethi  %hi(0x810000), %g1
    {0x00009, 0x8210600F},// or  %g1, 0xf, %g1
    {0x0000A, 0xC2A00040},// sta %g1, [%g0]2
    {0x0000B, 0x87444000},// mov %asr17, %g3
    {0x0000C, 0x8608E01F},// and %g3, 0x1f, %g3
    {0x0000D, 0x88100000},// clr  %g4
    {0x0000E, 0x8A100000},// clr  %g5
    {0x0000F, 0x8C100000},// clr  %g6
    {0x00010, 0x8E100000},// clr  %g7
    {0x00011, 0xA0100000},// clr  %l0
    {0x00012, 0xA2100000},// clr  %l1
    {0x00013, 0xA4100000},// clr  %l2
    {0x00014, 0xA6100000},// clr  %l3
    {0x00015, 0xA8100000},// clr  %l4
    {0x00016, 0xAA100000},// clr  %l5
    {0x00017, 0xAC100000},// clr  %l6
    {0x00018, 0xAE100000},// clr  %l7
    {0x00019, 0x90100000},// clr  %o0
    {0x0001A, 0x92100000},// clr  %o1
    {0x0001B, 0x94100000},// clr  %o2
    {0x0001C, 0x96100000},// clr  %o3
    {0x0001D, 0x98100000},// clr  %o4
    {0x0001E, 0x9A100000},// clr  %o5
    {0x0001F, 0x9C100000},// clr  %sp
    {0x00020, 0x9E100000},// clr  %o7
    {0x00021, 0x86A0E001},// subcc  %g3, 1, %g3
    {0x00022, 0x16BFFFEF},// bge 0x00000044
    {0x00023, 0x81E00000},// save; // All registers cleared
    {0x00024, 0x82102002},// mov 2, %g1
    {0x00025, 0x81904000},// wr %g1 ^ %g0, %wim
    {0x00026, 0x033CC000},// sethi %hi(0xf3000000), %g1
    {0x00027, 0x821060E0},// or %g1, 0xe0, %g1; 
    {0x00028, 0x81884000},// wr %g1 ^ %g0, %psr
    {0x00029, 0x01000000},// nop
    {0x0002A, 0x01000000},// nop
    {0x0002B, 0x01000000},// nop.
    {0x0002C, 0x87444000},// mov  %asr17, %g3
    {0x0002D, 0x8730E01C},// srl  %g3, 28, %g3; //bit[31:28]=hindex of cpu
    {0x0002E, 0x8688E00F},// andcc  %g3, 0xf, %g3
    {0x0002F, 0x3D1000FF},// sethi  %hi(0x4003fc00), %fp
    {0x00030, 0xBC17A3E0},// or  %fp, 0x3e0, %fp; // fp = 0x4003ffe0 = 262 112
    {0x00031, 0x9C27A060},// sub  %fp, 96, %sp
    // jump to entry point if ROM image disabled:
#if(CFG_FWROM_ENABLE!=1)
    {0x00032, 0x03100000},// sethi  %hi(0x40000000), %g1
    {0x00033, 0x81C04000},// jmp  %g1
#else
    {0x00032, 0x01000000},// nop
    {0x00033, 0x01000000},// nop
#endif
    {0x00034, 0x01000000},// nop
    {0x00035, 0x01000000},// nop
    {0x00036, 0x01000000},// nop

    // copy ROM image to base address if no flag:
    {0x00037, 0x03200001},//   sethi  %hi(0x80000400), %g1
    {0x00038, 0x05021561},//   sethi  %hi(0x8558400), %g2
    {0x00039, 0x821060fc},//   or  %g1, 0xfc, %g1
    {0x0003a, 0x8410a288},//   or  %g2, 0x288, %g2
    {0x0003b, 0xc6004000},//   ld  [%g1], %g3
    {0x0003c, 0x80a0c002},//   cmp  %g3, %g2
    {0x0003d, 0x0280000b},//   be  0x40001534(#00048#)
    {0x0003e, 0x093c0000},//   sethi  %hi(0xf0000000), %g4
    {0x0003f, 0xc4204000},//   st  %g2, [%g1]
    {0x00040, 0x03140000},//   sethi  %hi(0x50000000), %g1
    {0x00041, 0x07140080},//   sethi  %hi(0x50020000), %g3
    {0x00042, 0xc4004000},//   ld  [%g1], %g2
    {0x00043, 0xc4204004},//   st  %g2, [%g1 + %g4]
    {0x00044, 0x82006004},//   add  %g1, 4, %g1
    {0x00045, 0x80a04003},//   cmp  %g1, %g3
    {0x00046, 0x32bffffd},//   bne,a  0x40001520(#00043#)
    {0x00047, 0xc4004000},//   ld  [%g1], %g2
    // end of coping goto entry point:
    {0x00048, 0x03100000},// sethi  %hi(0x40000000), %g1
    {0x00049, 0x81C04000},// jmp  %g1
    {0x0004a, 0x01000000},// nop
    {0x0004b, 0x01000000},// nop
    {0x0004c, 0x01000000},// nop
    {0x0004d, 0x01000000},
};

