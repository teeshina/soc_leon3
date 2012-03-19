#include "timer.h"

void Timer::Init()
{
  gptimer = (gptimer_fields*)0x80000300;

	gptimer->scalerload = 66;
	gptimer->scalercnt = 66;
	for (int32 i=0; i<TIMERS_TOTAL; i++) gptimer->timer[i].control = 0; // halt all timers

  gptimer->timer[0].reload = 70;
  gptimer->timer[0].control = 0xf;


}
