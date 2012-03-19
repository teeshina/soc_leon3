#ifndef __SYSCTRL_H__
#define __SYSCTRL_H__

#include "stdtypes.h"
#include "uart.h"
#include "irqctrl.h"
#include "timer.h"


class SysControl
{
  friend class dbg;
  private:
   IrqControl clIrqControl;
   Timer      clTimer;

  public:
    void Init();
};

#endif
