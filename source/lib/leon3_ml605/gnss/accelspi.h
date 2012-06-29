//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "stdtypes.h"
#include "dff.h"
#include "leon3_ml605\config.h"
#include "leon3_ml605\amba\amba.h"

class accelspi
{
  friend class dbg;
  private:
    uint32 pindex;
    uint32 paddr;
    uint32 pmask;
  private:
    static const int32 FIFO_SIZE = 16;
    struct regs
    {
      uint32 WHO_AM_I : 8;
      uint32 CTRL_REG1 : 8;
      uint32 CTRL_REG2 : 8;
      uint32 CTRL_REG3 : 8;
      uint32 HP_FILTER_RESET : 8;
      uint32 STATUS_REG : 8;
      uint32 OUT_X : 8;
      uint32 OUT_Y : 8;
      uint32 OUT_Z : 8;
      uint32 FF_WU_CFG_1 : 8;
      uint32 FF_WU_SRC_1 : 8;
      uint32 FF_WU_THS_1 : 8;
      uint32 FF_WU_DURATION_1 : 8;
      uint32 FF_WU_CFG_2 : 8;
      uint32 FF_WU_SRC_2 : 8;
      uint32 FF_WU_THS_2 : 8;
      uint32 FF_WU_DURATION_2 : 8;
      uint32 CLICK_CFG : 8;
      uint32 CLICK_SRC : 8;
      uint32 CLICK_THSY_X : 8;
      uint32 CLICK_THSZ : 8;
      uint32 CLICK_TimeLimit : 8;
      uint32 CLICK_Latency : 8;
      uint32 CLICK_Window : 8;
      
      uint32 scale;
      uint32 cfg_icnt_ena : 1;    // run reading using internal counter
      uint32 cfg_int1_ena : 1;    // run reading using gyroscope interrpupt 1
      uint32 cfg_int2_ena : 1;    // run reading using gyroscope interrpupt 1
      uint32 cfg_wrword_ena : 1;  // run reading by writting into certain address
      
      uint32 rd_interval;
      uint32 rd_intervalcnt;
      uint32 rd_wordcnt : 5;
      
      uint32 wr_run      : 1;
      uint32 rd_word_run : 1;
      uint32 rd_all      : 1;
      uint32 ShiftTx     : 16;
      uint32 ScaleCnt;
      uint32 ClkPosedge  : 1;
      uint32 ClkNegedge  : 1;
      uint32 SPC         : 1;
      uint32 BitCnt       : 6;
      uint32 nCS          : 1;
      uint32 WriteWord    : 16;
      uint32 RxAdr  : 8;
      uint32 RxData : 8;

      uint64 FifoData[FIFO_SIZE];      //32+7 bits
      uint32 FifoCnt : 5;
      uint32 Writting  : 1;
      uint32 Reading   : 1;
      uint32 WrFifo  : 1;
      
      uint32 Int1 : 1;
      uint32 Int2 : 1;
    };

    regs v;
    TDFF<regs> r;//

    
    uint32 readdata;
    uint32 wWrFifo : 1;
    uint32 wRdFifo : 1;
    uint32 wr_run : 1;
    uint32 rd_possible : 1;
    uint32 rd_run_icnt : 1;
    uint32 rd_run_int1 : 1;
    uint32 rd_run_int2 : 1;
    uint32 rd_run_wrword : 1;
    uint32 rd_all_run : 1;
    uint32 rd_word_run : 1;
    uint32 rd_word_rdy : 1;
    uint32 rd_adr : 6;


  public:
    accelspi(uint32 pindex_=APB_ACCELEROMETER, uint32 paddr_=0x8, uint32 pmask_=0xffe);
    
    void Update(  uint32 rst,//    : in  std_ulogic;
                  SClock clk,//    : in  std_ulogic;
                  apb_slv_in_type &in_apbi,//   : in  apb_slv_in_type;
                  apb_slv_out_type &out_apbo,//   : out apb_slv_out_type;
                  uint32 inInt1,
                  uint32 inInt2,
                  uint32 inSDI,
                  uint32 &outSPC,
                  uint32 &outSDO,
                  uint32 &outCSn);

    void ClkUpdate()
    {
      r.ClkUpdate();
    }
};
