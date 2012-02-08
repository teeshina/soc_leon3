//****************************************************************************
// Author:      Khabarov Sergey
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
  {false,   0x50000ffc,           0, "wrong addr" },
  {false,   0x50000000,           0, "wrong addr" },
  {false,   0x50000004,           0, "wrong addr" },
  {false,   0x40000010,           0, "slave RAM" },
  {true,    0x40000ff8,  0x56700022, "Slave RAM writting" },
  {true,    0x40000ffc,  0x11112222, "Slave RAM writting" },
  {true,    0x40000000,  0x33334444, "Slave RAM writting" },
  {true,    0x40000004,  0x88889999, "Slave RAM writting" },
  {false,   0x40000ffc,           0, "Slave RAM reading" },
  {false,   0x40000000,           0, "Slave RAM reading" },
  {false,   0x40000004,           0, "Slave RAM reading" },
  {false,   0x40000008,           0, "Slave RAM reading" },
};
const int32 JTAG_TEST_TOTAL = sizeof(TEST)/sizeof(JTagTestInput);

