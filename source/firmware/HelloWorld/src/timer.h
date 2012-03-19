#ifndef __TIMERTEST_H__
#define __TIMERTEST_H__

#include "stdtypes.h"

#define TIMERS_TOTAL 2

struct timer_fields {
    volatile unsigned int counter;		/* 0x0 */
    volatile unsigned int reload;		/* 0x4 */
    volatile unsigned int control;		/* 0x8 */
    volatile unsigned int dummy;		/* 0xC */
};

struct gptimer_fields {
    volatile unsigned int scalercnt;		/* 0x00 */
    volatile unsigned int scalerload;		/* 0x04 */
    volatile unsigned int configreg;		/* 0x08 */
    volatile unsigned int dummy1;		/* 0x0C */
    struct timer_fields timer[TIMERS_TOTAL];
};


class Timer
{
  friend class dbg;
  private:
    gptimer_fields *gptimer;
  public:
    void Init();
};

#endif
