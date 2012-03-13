#pragma once

//****************************************************************************
class dbg
{
  private:
  public:
    int32 iJtagInstrCnt;
    char chJTag[1024];
    
  public:
    dbg();
    void GenerateReset(int32 cnt, uint32 outNRst);
    void Output();
    void PrintByeScreen(Settings *p);

  public:
};
