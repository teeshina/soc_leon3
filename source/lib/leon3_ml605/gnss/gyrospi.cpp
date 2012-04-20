//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "lheaders.h"


//****************************************************************************
gyrospi::gyrospi(uint32 pindex_, uint32 paddr_, uint32 pmask_)
{
  pindex = pindex_;
  paddr  = paddr_;
  pmask  = pmask_;
}


//****************************************************************************
void gyrospi::Update(  uint32 rst,//    : in  std_ulogic;
                        SClock clk,//    : in  std_ulogic;
                        apb_slv_in_type &apbi,//   : in  apb_slv_in_type;
                        apb_slv_out_type &apbo,//   : out apb_slv_out_type;
                        uint32 inInt1,
                        uint32 inInt2,
                        uint32 inSDI,
                        uint32 &outSPC,
                        uint32 &outSDO,
                        uint32 &outCSn )
{
  ((ahb_device_reg*)(&apbo.pconfig.arr[0]))->vendor  = VENDOR_GNSSSENSOR;
  ((ahb_device_reg*)(&apbo.pconfig.arr[0]))->device  = GNSSSENSOR_GYROSCOPE;
  ((ahb_device_reg*)(&apbo.pconfig.arr[0]))->version = 1;
  ((ahb_device_reg*)(&apbo.pconfig.arr[0]))->cfgver  = 0;
  ((ahb_device_reg*)(&apbo.pconfig.arr[0]))->interrupt = 0;
  ((apb_iobar*)(&apbo.pconfig.arr[1]))->area_id  = 0x1;
  ((apb_iobar*)(&apbo.pconfig.arr[1]))->addrmask = pmask;
  ((apb_iobar*)(&apbo.pconfig.arr[1]))->zero     = 0x0;
  ((apb_iobar*)(&apbo.pconfig.arr[1]))->memaddr  = paddr;


  v = r.Q;

  switch(BITS32(apbi.paddr,8,2))
  {
    case 0x0F: readdata = r.Q.WHO_AM_I; break;
    case 0x20: readdata = r.Q.CTRL_REG1; break;
    case 0x21: readdata = r.Q.CTRL_REG2; break;
    case 0x22: readdata = r.Q.CTRL_REG3; break;
    case 0x23: readdata = r.Q.CTRL_REG4; break;
    case 0x24: readdata = r.Q.CTRL_REG5; break;
    case 0x25: readdata = r.Q.REFERENCE; break;
    case 0x26: readdata = r.Q.OUT_TEMP; break;
    case 0x27: readdata = r.Q.STATUS_REG; break;
    case 0x28: readdata = r.Q.OUT_X_L; break;
    case 0x29: readdata = r.Q.OUT_X_H; break;
    case 0x2A: readdata = r.Q.OUT_Y_L; break;
    case 0x2B: readdata = r.Q.OUT_Y_H; break;
    case 0x2C: readdata = r.Q.OUT_Z_L; break;
    case 0x2D: readdata = r.Q.OUT_Z_H; break;
    case 0x2E: readdata = r.Q.FIFO_CTRL_REG; break;
    case 0x2F: readdata = r.Q.FIFO_SRC_REG; break;
    case 0x30: readdata = r.Q.INT1_CFG; break;
    case 0x31: readdata = r.Q.INT1_SRC; break;
    case 0x32: readdata = r.Q.INT1_TSH_XH; break;
    case 0x33: readdata = r.Q.INT1_TSH_XL; break;
    case 0x34: readdata = r.Q.INT1_TSH_YH; break;
    case 0x35: readdata = r.Q.INT1_TSH_YL; break;
    case 0x36: readdata = r.Q.INT1_TSH_ZH; break;
    case 0x37: readdata = r.Q.INT1_TSH_ZL; break;
    case 0x38: readdata = r.Q.INT1_DURATION; break;
    case 0x40: readdata = r.Q.scale; break;
    case 0x41: readdata = r.Q.rd_interval; break;
    case 0x42:
      readdata = (r.Q.FifoCnt<<24)|(r.Q.rd_wordcnt<<16) | (r.Q.rd_all<<15)
        | (r.Q.cfg_wrword_ena<<3)|(r.Q.cfg_int2_ena<<2)|(r.Q.cfg_int1_ena<<1)|(r.Q.cfg_icnt_ena);
    break;
    default:   readdata = 0;
  }

  wWrFifo = BIT32(apbi.psel,pindex) & apbi.penable & apbi.pwrite &!BIT32(r.Q.FifoCnt,4);
  wRdFifo = ((r.Q.FifoCnt!=0)&&!r.Q.Writting&&!r.Q.Reading) ? 1: 0;
  
  if(wWrFifo)
  {
    for(int32 i=1; i<FIFO_SIZE; i++)
      v.FifoData[i] = r.Q.FifoData[i-1];

    v.FifoData[0] = (uint64(BITS32(apbi.paddr,8,2))<<32) | uint64(apbi.pwdata);
    if(!wRdFifo) v.FifoCnt = r.Q.FifoCnt+1;
  }
  if(wRdFifo)
  {
    if(!wWrFifo) v.FifoCnt = r.Q.FifoCnt-1;
  }
  
  wr_run = 0;
  rd_run_wrword = 0; 
  uint32 addr = uint32(BITS64(r.Q.FifoData[r.Q.FifoCnt-1],38,32));
  if (wRdFifo)
  {
    switch(addr)
    {
      case 0x20: case 0x21: case 0x22: case 0x23: case 0x24:
      case 0x25: case 0x2E: case 0x30: case 0x32: case 0x33:
      case 0x34: case 0x35: case 0x36: case 0x37: case 0x38:
        v.WriteWord = (BITS32(addr,5,0)<<8)|uint32(BITS64(r.Q.FifoData[r.Q.FifoCnt-1],7,0)); // [15]=nRW (1=read;0-write); [14]=auto-incr ena;
        wr_run = 1;
      break;
      case 0x40:  // SPI frequency setting
        v.scale  = uint32(BITS64(r.Q.FifoData[r.Q.FifoCnt-1], 31, 0));
      break;
      case 0x41:  // internal counter that set interval of reading
        v.rd_interval = uint32(BITS64(r.Q.FifoData[r.Q.FifoCnt-1], 31, 0));
        v.rd_intervalcnt = 0;
      break;
      case 0x42:  // config register
        v.cfg_icnt_ena = BIT32(r.Q.FifoData[r.Q.FifoCnt-1], 0);
        v.cfg_int1_ena = BIT32(r.Q.FifoData[r.Q.FifoCnt-1], 1);
        v.cfg_int2_ena = BIT32(r.Q.FifoData[r.Q.FifoCnt-1], 2);
        v.cfg_wrword_ena = BIT32(r.Q.FifoData[r.Q.FifoCnt-1], 3);
      break;
      case 0x43: rd_run_wrword = r.Q.cfg_wrword_ena; break;
      default:;
    }
  }
  v.wr_run = wr_run;


  v.Int1 = inInt1;
  v.Int2 = inInt2;


  // sending word:
  if(r.Q.wr_run)                          v.ShiftTx = (0<<15) | (0<<14) | r.Q.WriteWord;
  else if((r.Q.BitCnt!=0)&r.Q.ClkNegedge) v.ShiftTx = r.Q.ShiftTx<<1;

  // reading interval:
  rd_possible = ((r.Q.FifoCnt==0)&&!r.Q.Writting&&!wr_run);
  rd_run_icnt = ((r.Q.rd_interval==r.Q.rd_intervalcnt)&&!wr_run&&!wRdFifo) ? r.Q.cfg_icnt_ena :0;
  rd_run_int1 = inInt1&!r.Q.Int1 ? r.Q.cfg_int1_ena : 0;
  rd_run_int2 = inInt1&!r.Q.Int2 ? r.Q.cfg_int2_ena : 0;
  
  rd_all_run = rd_run_icnt|rd_run_int1|rd_run_int1|rd_run_wrword;
  
  if(rd_all_run|!r.Q.cfg_icnt_ena)  v.rd_intervalcnt = 0;
  else if(rd_possible&!r.Q.rd_all)  v.rd_intervalcnt = r.Q.rd_intervalcnt + 1;
  
  if(rd_all_run&!r.Q.rd_all)                                       v.rd_all = 1;
  else if((r.Q.BitCnt==17)&&r.Q.ClkNegedge&&(r.Q.rd_wordcnt==25) ) v.rd_all = 0;
  
  if(rd_all_run&!r.Q.rd_all)                             v.rd_wordcnt = 0;
  else if(r.Q.Reading&&(r.Q.BitCnt==17)&&r.Q.ClkNegedge) v.rd_wordcnt = r.Q.rd_wordcnt + 1;

  rd_word_run = rd_all_run | (r.Q.rd_all&!r.Q.Reading&!wRdFifo&!r.Q.Writting);
  
  v.rd_word_run = rd_word_run;
  
  
  // form address and flags for the reading:
  if(rd_all_run)                          v.RxAdr = (1<<7) | (0<<6) | 0x0F;
  else if(rd_word_run)                    v.RxAdr = (1<<7) | (0<<6) | BITS32((0x1F+(r.Q.rd_wordcnt)),7,0);
  else if((r.Q.BitCnt!=0)&r.Q.ClkNegedge) v.RxAdr = r.Q.RxAdr<<1;
  
  // latch input data:
  if(r.Q.Reading& r.Q.ClkPosedge) v.RxData = (r.Q.RxData<<1) | inSDI;
  
  rd_word_rdy = (r.Q.Reading&&r.Q.ClkNegedge&&(r.Q.BitCnt==16)) ? 1: 0;
  if(rd_word_rdy)
  {
    switch(r.Q.rd_wordcnt)
    {
      case 0x00: v.WHO_AM_I = r.Q.RxData; break;
      case 0x01: v.CTRL_REG1 = r.Q.RxData; break;
      case 0x02: v.CTRL_REG2 = r.Q.RxData; break;
      case 0x03: v.CTRL_REG3 = r.Q.RxData; break;
      case 0x04: v.CTRL_REG4 = r.Q.RxData; break;
      case 0x05: v.CTRL_REG5 = r.Q.RxData; break;
      case 0x06: v.REFERENCE = r.Q.RxData; break;
      case 0x07: v.OUT_TEMP = r.Q.RxData; break;
      case 0x08: v.STATUS_REG = r.Q.RxData; break;
      case 0x09: v.OUT_X_L = r.Q.RxData; break;
      case 0x0A: v.OUT_X_H = r.Q.RxData; break;
      case 0x0B: v.OUT_Y_L = r.Q.RxData; break;
      case 0x0C: v.OUT_Y_H = r.Q.RxData; break;
      case 0x0D: v.OUT_Z_L = r.Q.RxData; break;
      case 0x0E: v.OUT_Z_H = r.Q.RxData; break;
      case 0x0F: v.FIFO_CTRL_REG = r.Q.RxData; break;
      case 0x10: v.FIFO_SRC_REG = r.Q.RxData; break;
      case 0x11: v.INT1_CFG = r.Q.RxData; break;
      case 0x12: v.INT1_SRC = r.Q.RxData; break;
      case 0x13: v.INT1_TSH_XH = r.Q.RxData; break;
      case 0x14: v.INT1_TSH_XL = r.Q.RxData; break;
      case 0x15: v.INT1_TSH_YH = r.Q.RxData; break;
      case 0x16: v.INT1_TSH_YL = r.Q.RxData; break;
      case 0x17: v.INT1_TSH_ZH = r.Q.RxData; break;
      case 0x18: v.INT1_TSH_ZL = r.Q.RxData; break;
      case 0x19: v.INT1_DURATION = r.Q.RxData; break;
      default:;
    }
  }



  // loading procedure:
  if(wr_run)                                v.Writting = 1;
  else if((r.Q.BitCnt==17)&&r.Q.ClkNegedge) v.Writting = 0;

  if(rd_word_run)                           v.Reading = 1;
  else if((r.Q.BitCnt==17)&&r.Q.ClkNegedge) v.Reading = 0;
  
  if((!r.Q.Writting&!r.Q.Reading)|r.Q.ClkPosedge) v.ScaleCnt = 0;
  else                                            v.ScaleCnt = r.Q.ScaleCnt+1;
  
  if(wr_run | rd_word_run | !(r.Q.Writting|r.Q.Reading))  v.BitCnt = 0;
  else if((r.Q.Writting|r.Q.Reading) & r.Q.ClkNegedge)    v.BitCnt = r.Q.BitCnt+1;

  // scaler pulse:
  if(r.Q.scale&&(r.Q.ScaleCnt==(BITS32(r.Q.scale,31,1)-1))) v.ClkNegedge = 1;
  else                                                      v.ClkNegedge = 0;

  if(r.Q.scale&&(r.Q.ScaleCnt==r.Q.scale)) v.ClkPosedge = 1;
  else                                     v.ClkPosedge = 0;



  // SCLK former:
  if(r.Q.ClkPosedge|!(r.Q.Writting|r.Q.Reading))              v.SPC = 1;
  else if(r.Q.ClkNegedge&&(r.Q.BitCnt!=16)&&(r.Q.BitCnt!=17)) v.SPC = 0;

  // CS signal:
  if(!r.Q.Writting&!r.Q.Reading)                v.nCS = 1;
  else if(r.Q.ClkNegedge&&(r.Q.BitCnt==16))     v.nCS = 1;
  else if(r.Q.wr_run|r.Q.rd_word_run)           v.nCS = 0;
  

  if (!rst)
  {
    v.Writting = 0;
    v.Reading = 0;
    v.scale = 4;
    v.ScaleCnt = 0;
    v.ClkNegedge = 0;
    v.ClkPosedge = 0;
    v.SPC = 1;
    v.nCS   = 1;
    v.BitCnt = 0;
    v.rd_interval = 66000;
    v.rd_intervalcnt = 0;
    v.wr_run = 0;
    v.rd_word_run = 0;
    v.rd_all = 0;
    v.cfg_icnt_ena = 0;    // run reading using internal counter
    v.cfg_int1_ena = 0;    // run reading using gyroscope interrpupt 1
    v.cfg_int2_ena = 0;    // run reading using gyroscope interrpupt 1
    v.cfg_wrword_ena = 1;  // run reading by writting into certain address
    v.RxAdr = 0;
    v.ShiftTx = 0;
  }


  r.CLK = clk;
  r.D = v;

  // APB interface:
  apbo.prdata = readdata;
  apbo.pirq   = 0;
  apbo.pindex = pindex;

  // external signals:
  outSPC = r.Q.SPC;
  outCSn  = r.Q.nCS;
  outSDO = r.Q.Writting ? BIT32(r.Q.ShiftTx,15) : BIT32(r.Q.RxAdr,7);
  
}

