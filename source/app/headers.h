//****************************************************************************
// Property:    GNSS Sensor Limited
// License:     GNU2
// Contact:     chief@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#include <stdio.h>
#include <tchar.h>
#include <stdarg.h>
#include <memory>
#include <iostream>
#include <fstream>

#include "stdtypes.h"
#include "dff.h"

#define DECLSPEC_TYPE __declspec(dllimport) 
#pragma comment(lib, "libleon.lib")
#include "..\lib\libface.h"

#include "tests.h"
#include "converter.h"
#include "id.h"
#include "clkgen.h"
#include "jtagport.h"
#include "uartport.h"

#include "elfloader\elfloader.h"
#include "dbg.h"

extern const double SYS_CLOCK_F;
extern const double JTAG_CLOCK_F;
extern const double timescale;



