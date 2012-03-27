//****************************************************************************
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#pragma once

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#define DECLSPEC_TYPE __declspec(dllexport) 

#include <stdarg.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

#include <memory>
#define _USE_MATH_DEFINES       // math defines
#include <math.h>

#include "stdtypes.h"
#include "dff.h"
#include "converter.h"
#include "libface.h"

#include "id.h"
#include "const.h"
#include "sparc.h"
#include "leon3_ml605\config.h"
#include "leon3_ml605\stdlib\stdlib.h"

#include "leon3_ml605\leon3\mem_unisim.h"

#include "leon3_ml605\amba\amba.h"
#include "leon3_ml605\amba\ahbmst_v.h"
#include "leon3_ml605\amba\ahbctrl.h"
#include "leon3_ml605\amba\ahbctrl_v.h"
#include "leon3_ml605\amba\apbctrl.h"
#include "leon3_ml605\amba\gptimer.h"
#include "leon3_ml605\jtag\jtagpad.h"
#include "leon3_ml605\jtag\jtagcom_v.h"
#include "leon3_ml605\jtag\ahbjtag.h"
#include "leon3_ml605\mem\ahbram.h"
#include "leon3_ml605\mem\ahbmem_v.h"
#include "leon3_ml605\mem\romdata.h"
#include "leon3_ml605\mem\ahbrom.h"
#include "leon3_ml605\uart\uart.h"
#include "leon3_ml605\uart\apbuart.h"

#include "leon3_ml605\leon3\arith.h"
#include "leon3_ml605\leon3\leon3.h"
#include "leon3_ml605\leon3\libiu.h"
#include "leon3_ml605\leon3\iu3.h"
#include "leon3_ml605\leon3\libcache.h"
#include "leon3_ml605\leon3\techmult.h"
#include "leon3_ml605\leon3\mul32.h"
#include "leon3_ml605\leon3\div32.h"
#include "leon3_ml605\leon3\mmuconfig.h"
#include "leon3_ml605\leon3\libmmu.h"
#include "leon3_ml605\leon3\mmuiface.h"
#include "leon3_ml605\leon3\mmu_icache.h"
#include "leon3_ml605\leon3\mmu_dcache.h"
#include "leon3_ml605\leon3\mmu_acache.h"
#include "leon3_ml605\leon3\mmutlbcam.h"
#include "leon3_ml605\leon3\mmulrue.h"
#include "leon3_ml605\leon3\mmulru.h"
#include "leon3_ml605\leon3\mmutlb.h"
#include "leon3_ml605\leon3\mmutw.h"
#include "leon3_ml605\leon3\mmu.h"
#include "leon3_ml605\leon3\mmu_cache.h"
#include "leon3_ml605\leon3\proc3.h"
#include "leon3_ml605\leon3\rfile_3p.h"
#include "leon3_ml605\leon3\cachemem.h"
#include "leon3_ml605\leon3\tbufmem.h"
#include "leon3_ml605\leon3\leon3s.h"
#include "leon3_ml605\leon3\dsu3x.h"
#include "leon3_ml605\leon3\irqmp.h"


#include "leon3_ml605\gnss\rfctrl.h"
#include "leon3_ml605\gnss\topgnss.h"

#include "leon3_ml605\leon3mp.h"
#include "dbgstring.h"
#include "ldbg.h"

extern uint32 iClkCnt;



