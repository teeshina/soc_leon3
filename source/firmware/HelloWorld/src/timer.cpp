#include "timer.h"

void Timer::Init()
{
  gptimer = (gptimer_fields*)ADR_GPTIMER_BASE;

  gptimer->scalerload = 66;
  gptimer->scalercnt = 66;
  for (int32 i=0; i<TIMERS_TOTAL; i++) gptimer->timer[i].control = 0; // halt all timers

  gptimer->timer[0].reload = 70;
  gptimer->timer[0].control = TIMER_CTRL_ENABLE|TIMER_CTRL_RESTART|TIMER_CTRL_LOAD|TIMER_CTRL_IRQENA;


}
