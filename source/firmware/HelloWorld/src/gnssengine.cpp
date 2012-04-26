#include "gnssengine.h"

void GnssEngine::Init()
{
  gnss = (GnssEngine_fields*)ADR_GNSS_ENGINE_BASE;

  gnss->gtmr.rw_MsLength = 7000;
  for (int32 i=0; i<CFG_GNSS_CHANNELS_TOTAL; i++) gnss->chan[i].rw_dbgIncr = 0xff123400+i; // 
}
