//****************************************************************************
//
//****************************************************************************

int main()
{
  unsigned int *adr, *status;
  adr = (unsigned int *)0x8000010c;
  *adr = 71;//scaler = 66MHz/(8*(1+rate)) = 115200 = 71
  adr = (unsigned int *)0x80000108;
  *adr = 2;
	
  adr = (unsigned int *)0x80000100;
  status = (unsigned int *)0x80000104;
  unsigned char chTmp[] = "Hello world\n";
  unsigned char *pchTmp = chTmp;
  while(true)
  {
    if(*pchTmp=='\0') pchTmp = chTmp;

    *adr = (unsigned int)(*pchTmp);
    pchTmp++;

   while((*status&0x600)!=0) {}

  }
	
  return 0;
}
