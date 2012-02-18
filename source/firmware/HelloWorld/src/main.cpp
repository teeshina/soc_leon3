//****************************************************************************
//
//****************************************************************************

int main()
{
	unsigned int *adr;
	adr = (unsigned int *)0x8000010c;
  *adr = 4;
  adr = (unsigned int *)0x80000108;
  *adr = 2;
	
	adr = (unsigned int *)0x80000100;
	unsigned char chTmp[] = "T\n";
	unsigned char *pchTmp = chTmp;
	while(*pchTmp!='\0')
	{
		*adr = (unsigned int)(*pchTmp);
		pchTmp++;
	}
	

	int a = 0;
	for(int i=0; i<10; i++)
	{
		a += 3;
	}

	return a;
}
