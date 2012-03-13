//****************************************************************************
// Author:      GNSS Sensor Limited
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#pragma once

#include "stdtypes.h"

//****************************************************************************
struct JTagTestInput
{
  bool   bWrite;
  uint32 uiAddr;
  uint32 uiWrData;
  char   *pchComment;
};

#define MST_ID(n)(0xFFFFF000+((8*n)<<2))
#define MST_ADR(n)(0xFFFFF000+((8*n+4)<<2))

static const JTagTestInput TEST[] =
{
  //write       Addr        WrData         Comment
  {false,   0xFFFFFFF0,           0, "Device,LibVersion" },
  {false,   MST_ID(0),            0, "msto[0].hconfig[0]" },
  {false,   MST_ID(1),            0, "msto[1].hconfig[0]" },
  {false,   0x90000000,           0, "DSU3 control register" },
  {false,   0x50000ffc,           0, "wrong addr" },
  {false,   0x40000000,           0, "RAM Entry point" },
  {true,    0x4003ffe0,  0x56700022, "RAM writting (end of 256 kBytes)" },
  {true,    0x4003ffe4,  0x11112222, "RAM writting" },
  {true,    0x4003ffe8,  0x33334444, "RAM writting" },
  {true,    0x4003ffec,  0x88889999, "RAM writting" },
  {false,   0x4003ffe4,           0, "Slave RAM reading" },
  {false,   0x4003ffe8,           0, "Slave RAM reading" },
  {false,   0x4003ffec,           0, "Slave RAM reading" },
  {false,   0x4003fff0,           0, "Slave RAM reading" },
};
const int32 JTAG_TEST_TOTAL = sizeof(TEST)/sizeof(JTagTestInput);

