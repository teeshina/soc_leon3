//****************************************************************************
//
//****************************************************************************
#include "sysctrl.h"
#include "uart.h"

//#define RUN_TEST_ACCELEROMETER

SysControl clSysControl;
ApbUart    clApbUart;
void *__gxx_personality_v0;

//****************************************************************************
int main()
{
  clSysControl.Init();

#ifdef RUN_TEST_ACCELEROMETER
  int32 testnum=0;
  uint32 *adr;
#endif


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

#ifdef RUN_TEST_ACCELEROMETER
    switch(testnum++)
    {
      // accelerometer SPI interface reading
      case 2:
        adr = (uint32*)(0x80000900); // scaler of accelerometer
        *adr = 8;
        
        adr = (uint32*)(0x80000908);
        *adr = 0x00000008;	// enable reading by writting into adr
      break;
      case 3:
        adr = (uint32*)(0x8000090C);
	  		*adr = 0;	    // run spi reading

        // test of writting
        adr = (uint32*)(0x800008F8);
  			*adr = 15;	    // run spi reading
        *(++adr) = 31;
      break;
    	default:;
    }
#endif
  }
  
  return 0;
}

