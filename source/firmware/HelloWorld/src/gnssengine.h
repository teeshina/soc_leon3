#ifndef __GNSSTEST_H__
#define __GNSSTEST_H__

#include "stdtypes.h"
#include "soconfig.h"

static const uint32 CFG_GNSS_CHANNELS_TOTAL = 32;

// Dummy channel implementation:
struct dumchannel_fields {
    volatile uint32 rw_dbgIncr;	  // 0x0
    volatile uint32 unused_1;	    // 0x4
    volatile uint32 r_QI;	        // 0x8
    volatile uint32 r_IQ;         // 0xC
    volatile uint32 unused_2[12]; // 0x10..0x3C
};

struct dumtimer_fields {
    volatile uint32 rw_MsLength;	// 0x0
    volatile uint32 r_MsCnt;	    // 0x4
    volatile uint32 unused_2[14]; // 0x18..0x3C
};

struct GnssEngine_fields {
    dumchannel_fields chan[CFG_GNSS_CHANNELS_TOTAL];
    dumtimer_fields   gtmr;
};


class GnssEngine
{
  friend class dbg;
  private:
    GnssEngine_fields *gnss;
  public:
    void Init();
};

#endif
