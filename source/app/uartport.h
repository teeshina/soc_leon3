#pragma once

//****************************************************************************
class uart_port
{
  friend class dbg;
  private:
    SClock clk;
    int32 iClkDivider;
    ClockGenerator *pclkGen;
    bool bEmpty;
    bool bCharReady;
    bool bRdDataRdy;  // string is ready. '\n' used to mark end of sending sequence
    
    enum EState {UART_IDLE, UART_SHIFT, UART_STOPBIT};
    EState eState;
    uint32 uiShiftCnt;
    uint8 uchString[1024];
    uint32 uiSymbCnt;
  public:
    uart_port();
    ~uart_port();
    
    void Update( uint32 inRst,
                 SClock inSysClk,
                 uint32 inTD,
                 uint32 inRTS,
                 uint32 &outRD,
                 uint32 &outCTS );

    bool IsRdDataRdy(){return bRdDataRdy;}
    uint8 *GetpDataString() { bRdDataRdy=false; return uchString; }
};
