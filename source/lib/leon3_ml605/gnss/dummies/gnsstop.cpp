//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GPL
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************
// WARNING:     DUMMY version of the GNSS top level: interrupt generator
//              and simple accumulators.
//****************************************************************************

#include "lheaders.h"



//****************************************************************************
void GnssTop::Update( uint32 inNRst,
                      SClock inClk,
                      uint32 inRdAdr,
                      uint32 inRdEna,
                      uint32 &outRdData,
                      uint32 inWrAdr,
                      uint32 inWrEna,
                      uint32 inWrData,
                      uint32 &outIrqPulse,
                      SClock inAdcClk,
                      uint32 inGpsI,
                      uint32 inGpsQ,
                      uint32 inGloI,
                      uint32 inGloQ )
{
  // multiplexer:
  wbSelData = wbChnOutData | wbGlbTimerOutData;
  
  clReclock.Update( inNRst, inClk, inAdcClk,
                    stCtrlOutMuxBus,
                    stReclkOutMuxBus, 
                    wGlbTmrOutMsReady, wReclkOutMsReady,
                    wbSelData, wbReclkOutSelData, wReclkOutSelDataRdy );

  clGnssControl.Update(inNRst, inClk, inRdAdr, inRdEna, outRdData, inWrAdr, inWrEna, inWrData,
                       stCtrlOutMuxBus,
                       wbReclkOutSelData, wReclkOutSelDataRdy,
                       wReclkOutMsReady, outIrqPulse);

  clGlobalTimer.Update( inNRst,
                        inAdcClk,
                        stReclkOutMuxBus,
                        wbGlbTimerOutData,
                        wGlbTmrOutMsReady );
                        
  clChannelsPack.Update(inNRst, inAdcClk, inGpsI, inGpsQ, inGloI, inGloQ,
                        wGlbTmrOutMsReady, stReclkOutMuxBus, wbChnOutData);
}
