//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GPL
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "lheaders.h"

//****************************************************************************
ChannelsPack::ChannelsPack()
{
  for (uint32 i=0; i<CFG_GNSS_CHANNELS_TOTAL; i++)
    pDummyChn[i] = new DummyChannel(i);
}

ChannelsPack::~ChannelsPack()
{
  for (uint32 i=0; i<CFG_GNSS_CHANNELS_TOTAL; i++)
    free(pDummyChn[i]);
}

//****************************************************************************
void ChannelsPack::Update( uint32 inNRst,
                           SClock inAdcClk,
                           uint32 inGpsI,
                           uint32 inGpsQ,
                           uint32 inGloI,
                           uint32 inGloQ,
                           uint32 inMsPulse,
                           Ctrl2Module &c2m,
                           Module2Ctrl *m2c )
{
  uint32 shft;
  
  // GPS L1-CA pack:
  shft=0;
  for(uint32 i=shft; i<(shft+CFG_GNSS_GPS_L1CA_NUM); i++)
    pDummyChn[i]->Update(inNRst, inAdcClk, inGpsI, inGpsQ, inMsPulse, c2m, m2c[i]);

  // SBAS L1 pack:
  shft = CFG_GNSS_GPS_L1CA_NUM;
  for(uint32 i=shft; i<(shft+CFG_GNSS_SBAS_L1_NUM); i++)
    pDummyChn[i]->Update(inNRst, inAdcClk, inGpsI, inGpsQ, inMsPulse, c2m, m2c[i]);
  
  // GALILEO E1 pack
  shft = CFG_GNSS_GPS_L1CA_NUM+CFG_GNSS_SBAS_L1_NUM;
  for(uint32 i=shft; i<(shft+CFG_GNSS_GALILEO_E1_NUM); i++)
    pDummyChn[i]->Update(inNRst, inAdcClk, inGpsI, inGpsQ, inMsPulse, c2m, m2c[i]);
  
  // GLONASS L1-CA pack
  shft = CFG_GNSS_GPS_L1CA_NUM+CFG_GNSS_SBAS_L1_NUM+CFG_GNSS_GALILEO_E1_NUM;
  for(uint32 i=shft; i<(shft+CFG_GNSS_GLONASS_L1_NUM); i++)
    pDummyChn[i]->Update(inNRst, inAdcClk, inGloI, inGloQ, inMsPulse, c2m, m2c[i]);
}
