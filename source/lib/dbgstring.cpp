//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "lheaders.h"

#define FILE_STRINGS_DBG "e:\\str.txt"

using namespace std;

//****************************************************************************
bool bDoStrOutput=true;
bool bSkipOutput=false;
int32 iIndS=0;
char chIndS[3*64386];
char *pchIndS = chIndS;
char chIndS2[3*64386];
char *pchIndS2 = chIndS2;

//****************************************************************************
void ResetPutStr()
{  
  pchIndS = chIndS;
  pchIndS2 = chIndS2;
  iIndS = 0;
}

//****************************************************************************
void SetSkipOutput(bool v)
{
  bSkipOutput = v;
}

//****************************************************************************
void PutWidth(int32 size, char *comment)
{
  if(!bDoStrOutput|bSkipOutput)
    return;

  bool bConvInteger=true;
  int32 iSeekCnt = 0;
  char chTemplate[]="conv_integer:";
  while((comment[iSeekCnt]!=0)&&bConvInteger&&(iSeekCnt<13))
  {
    if(chTemplate[iSeekCnt]!=comment[iSeekCnt])
    {
      bConvInteger=false;
      break;
    }
    iSeekCnt++;
  }


  int32 tmp;
  if(bConvInteger)
  {
    if(size==1) tmp = sprintf_s(pchIndS,64,"  %s <= conv_integer(S"DIG"(%i));\n", &comment[13], iIndS);
    else        tmp = sprintf_s(pchIndS,64,"  %s <= conv_integer(S"DIG"(%i downto %i));\n", &comment[13], iIndS+size-1, iIndS);
  }else
  {
    if(size==1) tmp = sprintf_s(pchIndS,64,"  %s <= S"DIG"(%i);\n", comment, iIndS);
    else        tmp = sprintf_s(pchIndS,64,"  %s <= S"DIG"(%i downto %i);\n", comment, iIndS+size-1, iIndS);
  }
  pchIndS += tmp;
  iIndS += size;

  if(size==1) tmp = sprintf_s(pchIndS2,256,"  signal %s  : std_logic;\n", comment);
  else        tmp = sprintf_s(pchIndS2,256,"  signal %s  : std_logic_vector(%i downto 0);\n", comment, size-1);
  pchIndS2 += tmp;
}

void PrintIndexStr()
{
  if(bSkipOutput) return;
  
  bDoStrOutput = false;
  std::ofstream osStr(FILE_STRINGS_DBG, ios::out);
  osStr << chIndS2;
  osStr << "\n";
  osStr << chIndS;
  osStr.close();
}

//****************************************************************************

//****************************************************************************
char* PutToStr(char *p, uint32 v, int size, char *comment, bool inv)
{
  if(comment!=NULL) PutWidth(size, comment);  

  if(!inv)
  {
    for (int i=0; i<size; i++)
    {
      if((v>>i)&0x1)  *p = '1';
      else            *p = '0';
      p++;
    }
  }else
  {
    for (int i=size-1; i>=0; i--)
    {
      if((v>>i)&0x1)  *p = '1';
      else            *p = '0';
      p++;
    }
  }
  *p = '\0';
  return p;
}

//****************************************************************************
char* PutToStr(char *p, uint64 v, int size, char *comment, bool inv)
{
  if(comment!=NULL) PutWidth(size, comment);

  if(!inv)
  {
    for (int i=0; i<size; i++)
    {
      if((v>>i)&0x1)  *p = '1';
      else            *p = '0';
      p++;
    }
  }else
  {
    for (int i=size-1; i>=0; i--)
    {
      if((v>>i)&0x1)  *p = '1';
      else            *p = '0';
      p++;
    }
  }
  *p = '\0';
  return p;
}


//****************************************************************************
char* PutToStr(char *p, uint32* bus, int size, char *comment, bool inv)
{
  if(comment!=NULL) PutWidth(size, comment);
  if(!inv)
  {
    for (int i=0; i<size; i++)
    {
      if(bus[i])  *p = '1';
      else        *p = '0';
      p++;
    }
  }else
  {
    for (int i=size-1; i>=0; i--)
    {
      if(bus[i])  *p = '1';
      else        *p = '0';
      p++;
    }
  }
  *p = '\0';
  return p;
}

//****************************************************************************
char* PutToStr(char *p, char *str)
{
  uint32 i=0;
  while(*str!='\0')
  {
    *p = *str;
    str++;
    p++;
  }
  *p = '\0';
  return p;
}


