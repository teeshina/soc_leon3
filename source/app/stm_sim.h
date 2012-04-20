//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GPL
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************
//              Texas Instruments gyroscope SPI simulator
//****************************************************************************

#pragma once

class STMicroelectronics
{
  friend class dbg;
  protected:
    uint32 clk_z;
    uint32 mem[64];
    uint32 wshift;
    uint32 rshift;
    uint32 iBitCnt;
    
    uint32 ModeRW;
    uint32 ModeIncr;
    uint32 addr;
  public:
   STMicroelectronics() : clk_z(0),rshift(0),iBitCnt(0) {}
   
   void Update(uint32 nCS, uint32 clk, uint32 idata, uint32 &odata, uint32 &oint1, uint32 &oint2 );
   virtual void GenerateRegs()=0;
};


class StGyroscopeSim : public STMicroelectronics
{
  friend class dbg;
  public:
    StGyroscopeSim() : STMicroelectronics()
    {
      memset(mem,0,sizeof(uint64)*sizeof(64));
      mem[0x0f] = 0xD4;//who_am_i default value
      mem[0x20] = 0x07;
    }
    
    void GenerateRegs(){}
};

class StAccelerometerSim : public STMicroelectronics
{
  friend class dbg;
  public:
    StAccelerometerSim() : STMicroelectronics()
    {
      memset(mem,0,sizeof(uint64)*sizeof(64));
      mem[0x0f] = 0x3B;//who_am_i default value
      mem[0x20] = 0x07;
    }
    
    void GenerateRegs(){}
};

