
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
