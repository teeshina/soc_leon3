//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "lheaders.h"

using namespace std;

//****************************************************************************
DbgString::DbgString()
{
  bDoStrOutput=true;    // Generate file just after first string has been formed
  bSkipOutput=false;    // 
}

//****************************************************************************
void DbgString::SetOutputPath(int32 ena, char *path)
{
  if(ena) bDoStrOutput = true;
  else    bDoStrOutput = false;
  
  strcpy_s(chVhdlNames, 1024, path);
  strcat_s(chVhdlNames, 1024, "strdbg.txt");
}

//****************************************************************************
void DbgString::ResetPutStr()
{
  pchIndS = chIndS;
  pchIndS2 = chIndS2;
  iIndS = 0;
}

//****************************************************************************
bool DbgString::CheckModificator(char*ref, char*comment)
{
  bool bRet = true;
  size_t iCnt=0;
  size_t length = strlen(ref);
  while((comment[iCnt]!=0)&&bRet&&(iCnt<length))
  {
    if(ref[iCnt]!=comment[iCnt])
    {
      bRet=false;
      break;
    }
    iCnt++;
  }
  return bRet;
}

//****************************************************************************
void DbgString::PutWidth(int32 size, char *comment)
{
  if(!bDoStrOutput|bSkipOutput)
    return;

  int32 tmp;
  if(CheckModificator("conv_integer:", comment))
  {
    if(size==1) tmp = sprintf_s(pchIndS,64,"  %s <= conv_integer(S"DIG"(%i));\n", &comment[13], iIndS);
    else        tmp = sprintf_s(pchIndS,64,"  %s <= conv_integer(S"DIG"(%i downto %i));\n", &comment[13], iIndS+size-1, iIndS);
  }else if(CheckModificator("unsigned:", comment))
  {
    if(size==1) tmp = sprintf_s(pchIndS,64,"  %s <= U"DIG"(%i);\n", &comment[9], iIndS);
    else        tmp = sprintf_s(pchIndS,64,"  %s <= U"DIG"(%i downto %i);\n", &comment[9], iIndS+size-1, iIndS);
  }else if(CheckModificator("vector:", comment))
  {
    if(size==1) tmp = sprintf_s(pchIndS,64,"  %s <= S"DIG"(%i downto %i);\n", &comment[7], iIndS, iIndS);
    else        tmp = sprintf_s(pchIndS,64,"  %s <= S"DIG"(%i downto %i);\n", &comment[7], iIndS+size-1, iIndS);
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

//****************************************************************************
void DbgString::PrintIndexStr()
{
  if(bSkipOutput) return;
  if(!bDoStrOutput) return;
  bDoStrOutput = false;   // Print only once!!!!
  
  posVhdlNames = new std::ofstream(chVhdlNames, ios::out);
  if(posVhdlNames->is_open())
  {
    *posVhdlNames << "//*************************************************\n";
    *posVhdlNames << "//VHDL Strings with declaration: \n";
    *posVhdlNames << "//*************************************************\n\n";
    *posVhdlNames << chIndS2;
    *posVhdlNames << "\n";
    
    *posVhdlNames << "//*************************************************\n";
    *posVhdlNames << "//VHDL Strings with assignments: \n";
    *posVhdlNames << "//*************************************************\n\n";
    *posVhdlNames << chIndS;
    posVhdlNames->close();
  }else
  {
    printf_s("Error: can't create file \"%s\"",chVhdlNames);
  }
  free(posVhdlNames);
}


//****************************************************************************
char* DbgString::Put(char *p, uint32 v, int size, char *comment, bool inv)
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
char* DbgString::Put(char *p, uint64 v, int size, char *comment, bool inv)
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
char* DbgString::Put(char *p, uint32* bus, int size, char *comment, bool inv)
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
char* DbgString::Put(char *p, char *str)
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


