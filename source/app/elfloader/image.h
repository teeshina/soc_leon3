//****************************************************************************
// Property:    GNSS Sensor Limited
// License:     GNU2
// Contact:     chief@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#pragma once 

#include <fstream>
#include "stdtypes.h"

static const int32 ELF_IMAGE_MAXSIZE = 1<<16; // words


class SrcElement
{
  friend class dbg;
  public:
    uint32 adr;
    uint32 val;
    uint8 *pDataName;    // 0=no name
    uint8 *pFuncName;
    uint8 *pSectionName;
    uint8 *pFileName;
    bool  bInit;
};

class SrcImage
{
  friend class dbg;
  public:
    uint32 entry;
    int32  iSizeWords;
    SrcElement arr[ELF_IMAGE_MAXSIZE];
};
