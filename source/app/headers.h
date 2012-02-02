// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#include <stdio.h>
#include <tchar.h>
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

#include "dbg.h"

const double SYS_CLOCK_F = 66000000; //Hz
const double JTAG_CLOCK_F = 20000000; //Hz

const double timescale = 1.0/SYS_CLOCK_F/4.0;



