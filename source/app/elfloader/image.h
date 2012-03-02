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
    bool bIsName;
    char *pName;
};

class SrcImage
{
  friend class dbg;
  public:
    uint32 entry;
    int32  iSizeBytes;
    SrcElement arr[ELF_IMAGE_MAXSIZE];
};
