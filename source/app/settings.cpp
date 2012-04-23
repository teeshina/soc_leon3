//****************************************************************************
// Property:    GNSS Sensor Limited
// License:     GNU2
// Contact:     chief@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "headers.h"

#define SETTINGS_FILENAME "settings.txt"
#define DEFAULT_ELF_FILE "..\\source\\firmware\\HelloWorld\\target_VIRTEX_ML605\\elf\\HelloWorld.elf"
#define DEFAULT_OUT_PATH "e:\\"

#define DEFAULT_CLK_DURATION 30000


using namespace std;

//****************************************************************************
Settings::Settings()
{
  memset(this, 0, sizeof(Settings));
}

//****************************************************************************
void Settings::LoadFromFile()
{
  pisSettings = new ifstream(SETTINGS_FILENAME, std::ios::in);
  if(pisSettings->is_open())
  {
    char chTmp[64];
    size_t szStrLength;
    EStringName eString;
    
    while(!pisSettings->eof())
    {
      pisSettings->getline(chLine, STRING_LENGTH_MAX);
      szStrLength = strlen(chLine);
      if(0==szStrLength) continue;
      if((chLine[0]=='/')&&(chLine[0]=='/')) continue;
      
      eString=STR_UNKNOWN;
      for (int32 i=0; i<STR_UNKNOWN; i++)
      {
        memcpy(chTmp, chLine, strlen(AllStrings[i].name));
        chTmp[strlen(AllStrings[i].name)] = '\0';
        if(!strcmp(chTmp, AllStrings[i].name)) eString = AllStrings[i].eName;
      }
      
      ParseLine(chLine, eString);
    }
    
    pisSettings->close();
  }else
    GenerateDefaultSettings();
    
  free(pisSettings);

  SaveToFile();
}

//****************************************************************************
void Settings::ParseLine(char *src, EStringName name)
{
  size_t length=strlen(AllStrings[name].name);
  switch(name)
  {
    case STR_ELF_FILE:        GetFieldString(chElfFile, src, length );   break;
    case STR_OUT_PATH:        
      GetFieldString(sLibInitData.chDirOut, src, length);
      CheckOutPathExist(sLibInitData.chDirOut);
    break;
    case STR_SYSCLK_HZ:       dSysClockHz = GetFieldDouble(src, length);  break;
    case STR_TIMESCALE_RATE:  
      dTimeScaleRate = GetFieldDouble(src, length);
      timescale=1.0/dSysClockHz/dTimeScaleRate;
    break;
    case STR_UART_ENA:        iUartEna = GetFieldInt(src, length); break;
    case STR_UART_CLKSCALE:   uiUartClkScale = (uint32)GetFieldInt(src, length); break;
    case STR_JTAG_ENA:        iJtagEna = GetFieldInt(src, length); break;
    case STR_JTAGCLK_HZ:      dJtagClockHz = GetFieldDouble(src, length);  break;
    case STR_RF_FRONTEND_ENA: iRfFrontEndEna = GetFieldInt(src, length); break;
    case STR_GYRO_ENA:        iGyroEna = GetFieldInt(src, length); break;
    case STR_ACCELER_ENA:     iAccelerEna = GetFieldInt(src, length); break;
    case STR_GNSSENGINE_ENA:  iGnssEngineEna = GetFieldInt(src, length); break;
    case STR_CLK_DURATION:    iClkSimTotal = GetFieldInt(src, length);  break;
    case STR_PRINT_VHDLDATA:  sLibInitData.iPrintVhdlData = GetFieldInt(src, length); break;
    default:;
  }
  if((name>=STR_TB_jtagcom)&&(name<STR_PRINT_VHDLDATA))
    sLibInitData.uiBenchEna[name-STR_TB_jtagcom] = GetFieldInt(src, length);
}

//****************************************************************************
void Settings::CheckOutPathExist(char *path)
{
  // TODO: add using <mscorlib.dll>
}

//****************************************************************************
double Settings::GetFieldDouble(char *src, size_t namelength)
{
  double ret=0;
  char chTmp[64]={0};
  char *pcur = &src[namelength];
  for(int32 i=0; i<STRING_LENGTH_MAX; i++)
  {
    if( ((*pcur<'0')||(*pcur>'9')) && (*pcur!='.') ) break;
    chTmp[i] = *pcur;
    chTmp[i+1] = '\0';
    pcur++;
  }
  
  if(strlen(chTmp)) ret = atof(chTmp);
  return ret;
}

//****************************************************************************
int32 Settings::GetFieldInt(char *src, size_t namelength)
{
  int32 ret=0;
  char chTmp[32]={0};
  char *pcur = &src[namelength];
  for(int32 i=0; i<STRING_LENGTH_MAX; i++)
  {
    if( ((*pcur)<'0') || ((*pcur)>'9') ) break;
    chTmp[i] = *pcur;
    chTmp[i+1] = '\0';
    pcur++;
  }
  
  if(strlen(chTmp)) ret = atoi(chTmp);
  return ret;
}

//****************************************************************************
void Settings::GetFieldString(char *dst, char *src, size_t namelength)
{
  for(int32 i=0; i<STRING_LENGTH_MAX; i++)
  {
    if(src[namelength+1+i]=='"') break;  // "+1" cause path "...."
    dst[i] = src[namelength+1+i];
    dst[i+1] = '\0';
  }
}

//****************************************************************************
void Settings::GenerateDefaultSettings()
{

  strcpy_s(chElfFile, STRING_LENGTH_MAX, DEFAULT_ELF_FILE);
  strcpy_s(sLibInitData.chDirOut, PATH_LENGTH_MAX, DEFAULT_OUT_PATH);

  dSysClockHz     = 66000000.0;
  dTimeScaleRate  = 4.0;
  timescale       = 1.0/dSysClockHz/dTimeScaleRate;
  iClkSimTotal    = DEFAULT_CLK_DURATION;
  
  iUartEna       = 1;
  uiUartClkScale = 4; // at SysClk=66MHz: 4=1 650 000 baud; 71=114583 baud.
  iJtagEna       = 0;
  dJtagClockHz   = 20000000.0;

  
  for (int32 i=0; i<TB_TOTAL; i++)
    sLibInitData.uiBenchEna[i] = PRINT_TESTBENCH_DISABLE;
    
  sLibInitData.iPrintVhdlData = PRINT_TESTBENCH_DISABLE;
}

//****************************************************************************
void Settings::SaveToFile()
{
  ofstream osSettings(SETTINGS_FILENAME);
  osSettings << "//****************************************************************************\n";
  osSettings << "// Property:    GNSS Sensor Limited\n";
  osSettings << "// License:     GPL\n";
  osSettings << "// Contact:     chief@gnss-sensor.com\n";
  osSettings << "// Repository:  git@github.com:teeshina/soc_leon3.git\n";
  osSettings << "//****************************************************************************\n";
  osSettings << "// WARNING:     Don't insert spaces.\n";
  osSettings << "//****************************************************************************\n\n";

  sprintf_s(chLine, "%s\"%s\"\n",AllStrings[STR_ELF_FILE].name, chElfFile);
  osSettings << chLine;

  sprintf_s(chLine, "%s\"%s\"\n",AllStrings[STR_OUT_PATH].name, sLibInitData.chDirOut);
  osSettings << chLine;
  osSettings << "\n";

  sprintf_s(chLine, "%s%.1f\n",AllStrings[STR_SYSCLK_HZ].name, dSysClockHz);
  osSettings << chLine;

  sprintf_s(chLine, "%s%.3f\n",AllStrings[STR_TIMESCALE_RATE].name, dTimeScaleRate);
  osSettings << chLine;

  sprintf_s(chLine, "%s%i\n",AllStrings[STR_CLK_DURATION].name, iClkSimTotal);
  osSettings << chLine;
  osSettings << "\n";


  osSettings << "// UART settings (use the same clkscale at FW):\n";
  osSettings << "//   at SysClk=66MHz: 4  = 1 650 000 baud. It is used for fast simulation.\n";
  osSettings << "//                    71 = 114583 baud.\n";
  sprintf_s(chLine, "%s%i\n",AllStrings[STR_UART_ENA].name, iUartEna);
  osSettings << chLine;
  
  sprintf_s(chLine, "%s%i\n",AllStrings[STR_UART_CLKSCALE].name, uiUartClkScale);
  osSettings << chLine;

  sprintf_s(chLine, "%s%i\n",AllStrings[STR_JTAG_ENA].name, iJtagEna);
  osSettings << chLine;

  sprintf_s(chLine, "%s%.1f\n",AllStrings[STR_JTAGCLK_HZ].name, dJtagClockHz);
  osSettings << chLine;

  sprintf_s(chLine, "%s%i\n",AllStrings[STR_RF_FRONTEND_ENA].name, iRfFrontEndEna);
  osSettings << chLine;

  sprintf_s(chLine, "%s%i\n",AllStrings[STR_GYRO_ENA].name, iGyroEna);
  osSettings << chLine;

  sprintf_s(chLine, "%s%i\n",AllStrings[STR_ACCELER_ENA].name, iAccelerEna);
  osSettings << chLine;

  sprintf_s(chLine, "%s%i\n",AllStrings[STR_GNSSENGINE_ENA].name, iGnssEngineEna);
  osSettings << chLine;

  osSettings << "\n";
  
  
  osSettings << "// Enable/disable pattern file generation for the certain VHDL testbench file:\n";
  for (int32 i=0; i<TB_TOTAL; i++)
  {
    sprintf_s(chLine, "%s%i\n",AllStrings[STR_TB_jtagcom+i].name, sLibInitData.uiBenchEna[i]);
    osSettings << chLine;
  }

  sprintf_s(chLine, "%s%i\n",AllStrings[STR_PRINT_VHDLDATA].name, sLibInitData.iPrintVhdlData);
  osSettings << chLine;
}

