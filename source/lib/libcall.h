//****************************************************************************
// Property:    GNSS Sensor Limited
// License:     GNU2
// Contact:     chief@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#pragma once

#include "stdtypes.h"
#include "libface.h"

#if !defined(DECLSPEC_TYPE)
  #error Not defined IMPORT/EXPORT
#endif


//****************************************************************************
extern "C" DECLSPEC_TYPE void __stdcall LibInit(LibInitData *);
extern "C" DECLSPEC_TYPE void __stdcall LibClose(void);
extern "C" DECLSPEC_TYPE void __stdcall LibLeonUpdate(SystemOnChipIO &ioData);
extern "C" DECLSPEC_TYPE void __stdcall LibLeonClkUpdate(void);
extern "C" DECLSPEC_TYPE void __stdcall LibBackDoorLoadRAM(uint32 adr, uint32 v);


