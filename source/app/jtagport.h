#pragma once

//****************************************************************************
class jtag_port//TODO: rename it a bit later
{
  friend class dbg;
  private:
    enum EState
    {
      STATE_RESET,
      STATE_IDLE,
      STATE_SELECT_DR,
      STATE_SELECT_IR,
      STATE_CAPTURE,
      STATE_SHIFT,
      STATE_EXIT1,
      STATE_PAUSE,
      STATE_EXIT2,
      STATE_UPDATE
    };
    EState eState;
    int32 iBitCnt;
    enum ESelector {IR,DR};
    ESelector eR;
    uint32 ulSelIR;  // 10 bits for Virtex6 compatibility
    uint64 ulSelDR;
    uint32 uiSeqCnt;

    bool   bRdDataRdy;
    uint32 ulRdShift;
    uint32 ulRdData;
    uint32 ulRdAdr;
    uint64 ulWrShift;

    bool bEmpty;
    JTagTestInput stCommand;
    
    ClockGenerator *pclkTck;
  public:
    jtag_port();
    ~jtag_port();
  
    void Update(uint32 inNRst,
                uint32 inTDO,
                SClock &outTCK,
                uint32 &outnTRST,
                uint32 &outTMS,
                uint32 &outTDI
                );
    bool IsEmpty() { return bEmpty&&(eState!=STATE_RESET); }
    void Put(const JTagTestInput *p) { stCommand=*p; bEmpty=false; }
    bool IsRdDataRdy(){return bRdDataRdy;}
    
    uint32 GetRdAdr() { return ulRdAdr; }
    uint32 GetRdData() {bRdDataRdy=false; return ulRdData;}
};


