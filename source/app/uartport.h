#pragma once

//****************************************************************************
class uart_port
{
  friend class dbg;
  private:
    SClock clk;
    ClockGenerator *pclkGen;
    bool bEmpty;
    bool bCharReady;
    bool bRdDataRdy;
    
    enum EState {UART_IDLE, UART_WAIT, UART_SHIFT, UART_STOPBIT};
    EState eState;
    uint32 uiShiftCnt;
    uint8 uchString[1024];
    uint32 uiSymbCnt;
  public:
    uart_port();
    ~uart_port();
    
    void Update( uint32 inNRst,
                 uint32 inTD,
                 uint32 inRTS,
                 uint32 &outRD,
                 uint32 &outCTS );
};
