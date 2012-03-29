//****************************************************************************
//
//****************************************************************************
#include "sysctrl.h"
#include "uart.h"

SysControl clSysControl;
ApbUart    clApbUart;
void *__gxx_personality_v0;

//****************************************************************************
int main()
{
  clSysControl.Init();

  double res,f = 3.14;
  int a=21;
  while(true) 
  {
#ifdef PC_SIMULATION
    res = f*3.3;
//    printf_uart("%.1f\n",res);
    a = res*2;
    printf_uart("%i\n",a);
#endif
  }
  
  return 0;
}

