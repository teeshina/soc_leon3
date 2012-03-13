//****************************************************************************
//
//****************************************************************************

#include <stdio.h>

extern "C" void *catch_interrupt(void func(), int irq);
int tmp=0;
int tmp2=1;

//****************************************************************************
void IrqHandler()
{
	while(true){}
}

//****************************************************************************
void UartInit()
{
	unsigned int *adr;
  adr = (unsigned int *)0x8000010c;

  // Set scaler:
  //		 scaler = 66MHz/(8*(1+rate)) = 115200 = 71
#if 1
  *adr = 4;	 // for the fast simulation. It is the same as in file "settings.txt"
#else
  *adr = 71; // 115200 baudrate
#endif

  // Init port for the transmition:
  adr = (unsigned int *)0x80000108;
  *adr = 2;
}

//****************************************************************************
void SendStringToUart(char *str)
{
	unsigned int *adr, *status;
	adr = (unsigned int *)0x80000100; 		// APB UART1 write data address
	status = (unsigned int *)0x80000104;  // APB UART1 status address
	char *tmp = str;

  while(*tmp!='\0')
  {
    while((*status&0x600)!=0) {}

    *adr = (unsigned int)(*tmp);
    tmp++;

  }
}

//****************************************************************************
int main()
{
  char chStr[32];
  tmp += 1;


	catch_interrupt(IrqHandler, 0x10);

	UartInit();


  while(true)
  {
    sprintf(chStr,"%s: %i\n", "Hi, World", tmp2);
		SendStringToUart(chStr);

    tmp2 += 1;
  }
	
  return 0;
}
