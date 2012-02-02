#pragma once

//****************************************************************************
class dbg
{
  private:
  public:
    int32 iJtagInstrCnt;
    char chJTag[1024];

    bool bEnd;

  public:
    dbg();
    void GenerateReset(int32 cnt, uint32 outNRst);
    void Output();
    
    bool IsEnd() { return bEnd; }

  public:
    //SystemOnChipIO ioLeonData;

};
