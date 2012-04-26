//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GPL
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#pragma once

class ChannelsPack
{
  friend class dbg;
  private:
    DummyChannel *pDummyChn[CFG_GNSS_CHANNELS_TOTAL];
    
  public:
  
    ChannelsPack();
    ~ChannelsPack();
  
    void Update(uint32 inNRst,
                SClock inAdcClk,
                uint32 inGpsI,
                uint32 inGpsQ,
                uint32 inGloI,
                uint32 inGloQ,
                uint32 inMsPulse,
                Ctrl2Module &c2m,
                Module2Ctrl *m2c );
  
    void ClkUpdate()
    {
      for (int32 i=0; i<CFG_GNSS_CHANNELS_TOTAL; i++)
        pDummyChn[i]->ClkUpdate();
    }
};
