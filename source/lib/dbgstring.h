//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#pragma once


class DbgString
{
  friend class dbg;
  private:
    static const int32 DBGSTRING_MAX_LENGTH = 3*64386;
    bool bDoStrOutput;
    bool bSkipOutput;
    
    int32 iIndS;
    char chIndS[DBGSTRING_MAX_LENGTH];
    char *pchIndS;
    
    char chIndS2[DBGSTRING_MAX_LENGTH];
    char *pchIndS2;

    std::ofstream *posVhdlNames;
    char chVhdlNames[1024];
    
  public:
    DbgString();
    
    void SetOutputPath(int32 ena, char *path);
    
    void ResetPutStr();
    void SetSkipOutput(bool v){ bSkipOutput = v; }
    
    bool CheckModificator(char*ref, char*comment);
    void PutWidth(int32 size, char *comment);
    void PrintIndexStr();


    char* Put(char *p, uint32 v, int size, char *comment, bool inv);
    char* Put(char *p, uint64 v, int size, char *comment, bool inv);
    char* Put(char *p, uint32* bus, int size, char *comment, bool inv);
    char* Put(char *p, char *str);
    
};
