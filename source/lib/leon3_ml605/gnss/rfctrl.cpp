//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "lheaders.h"


//****************************************************************************
RfControl::RfControl(uint32 pindex_, uint32 paddr_, uint32 pmask_)
{
  pindex = pindex_;
  paddr  = paddr_;
  pmask  = pmask_;
}


//****************************************************************************
void RfControl::Update(  uint32 rst,//    : in  std_ulogic;
                        SClock clk,//    : in  std_ulogic;
                        apb_slv_in_type &apbi,//   : in  apb_slv_in_type;
                        apb_slv_out_type &apbo,//   : out apb_slv_out_type;
                        uint32 inLD[SystemOnChipIO::TOTAL_MAXIM2769],
                        uint32 &outSCLK,
                        uint32 &outSDATA,
                        uint32 *outCSn,
                        uint32 inExtAntStat,
                        uint32 inExtAntDetect,
                        uint32 &outExtAntEna )
{
  ((ahb_device_reg*)(&apbo.pconfig.arr[0]))->vendor  = VENDOR_GNSSSENSOR;
  ((ahb_device_reg*)(&apbo.pconfig.arr[0]))->device  = GNSSSENSOR_RF_CONTROL;
  ((ahb_device_reg*)(&apbo.pconfig.arr[0]))->version = 1;
  ((ahb_device_reg*)(&apbo.pconfig.arr[0]))->cfgver  = 0;
  ((ahb_device_reg*)(&apbo.pconfig.arr[0]))->interrupt = 0;
  ((apb_iobar*)(&apbo.pconfig.arr[1]))->area_id  = 0x1;
  ((apb_iobar*)(&apbo.pconfig.arr[1]))->addrmask = pmask;
  ((apb_iobar*)(&apbo.pconfig.arr[1]))->zero     = 0x0;
  ((apb_iobar*)(&apbo.pconfig.arr[1]))->memaddr  = paddr;


  v = r.Q;
  v.load_run = 0;

  switch(BITS32(apbi.paddr,7,2))
  {
    case 0x00: readdata = BITS32(r.Q.conf1,31,4); break;
    case 0x01: readdata = BITS32(r.Q.conf2,31,4); break;
    case 0x02: readdata = BITS32(r.Q.conf3,31,4); break;
    case 0x03: readdata = BITS32(r.Q.pllconf,31,4); break;
    case 0x04: readdata = BITS32(r.Q.div,31,4); break;
    case 0x05: readdata = BITS32(r.Q.fdiv,31,4); break;
    case 0x06: readdata = BITS32(r.Q.strm,31,4); break;
    case 0x07: readdata = BITS32(r.Q.clkdiv,31,4); break;
    case 0x08: readdata = BITS32(r.Q.test1,31,4); break;
    case 0x09: readdata = BITS32(r.Q.test2,31,4); break;
    case 0x0A: readdata = r.Q.scale; break;
    case 0x0B: readdata = (r.Q.BitCnt<<4)|(r.Q.loading<<2)|(inLD[1]<<1)|inLD[0]; break;  // PLL status of MAX2769
    case 0x0F: readdata = (inExtAntStat<<5)|(inExtAntDetect<<4)|r.Q.ExtAntEna; break;
    case 0x3F: readdata = r.Q.BootID; break;
    default:   readdata = 0;
  }

  if (BIT32(apbi.psel,pindex) & apbi.penable & apbi.pwrite)
  {
    switch (BITS32(apbi.paddr,7,2))
    {
      case 0x00: v.conf1  = (apbi.pwdata<<4)|0x0; break;
      case 0x01: v.conf2  = (apbi.pwdata<<4)|0x1; break;
      case 0x02: v.conf3  = (apbi.pwdata<<4)|0x2; break;
      case 0x03: v.pllconf = (apbi.pwdata<<4)|0x3; break;
      case 0x04: v.div    = (apbi.pwdata<<4)|0x4; break;
      case 0x05: v.fdiv   = (apbi.pwdata<<4)|0x5; break;
      case 0x06: v.strm   = (apbi.pwdata<<4)|0x6; break;
      case 0x07: v.clkdiv = (apbi.pwdata<<4)|0x7; break;
      case 0x08: v.test1  = (apbi.pwdata<<4)|0x8; break;
      case 0x09: v.test2  = (apbi.pwdata<<4)|0x9; break;
      case 0x0A: 
        if(apbi.pwdata<2) v.scale = 2;
        else              v.scale  = apbi.pwdata;
      break;
      case 0x0B: 
        v.load_run = 1;
        v.ScaleCnt = 0;
        v.BitCnt = 0;
        if(apbi.pwdata==0)      v.select_spi = 1;
        else if(apbi.pwdata==1) v.select_spi = (1<<1);
        else                    v.select_spi = 0;
      break;
      case 0x0F:
        v.ExtAntEna = BIT32(apbi.pwdata,0);
      break;
      case 0x3F: v.BootID = apbi.pwdata;  break;
      default:;
    }
  }

  // loading procedure:
  wNewWord = (r.Q.SClkNegedge&&(r.Q.BitCnt==33)) ? 1: 0;
  
  if(r.Q.load_run)                         v.loading = 1;
  else if(wNewWord&&(r.Q.WordSelector==0)) v.loading = 0;
  
  if(r.Q.loading&r.Q.SClkNegedge) v.ScaleCnt = 0;
  else if(r.Q.loading)            v.ScaleCnt = r.Q.ScaleCnt+1;

  // scaler pulse:
  if(r.Q.scale&&(r.Q.ScaleCnt==r.Q.scale)) v.SClkNegedge = 1;
  else                                     v.SClkNegedge = 0;

  if(r.Q.scale&&(r.Q.ScaleCnt==BITS32(r.Q.scale,31,1))) v.SClkPosedge = 1;
  else                                                  v.SClkPosedge = 0;

  // SCLK former:
  if(r.Q.SClkPosedge)      v.SCLK = 1;
  else if(r.Q.SClkNegedge) v.SCLK = 0;

  // Not inversed CS signal:
  if(r.Q.SClkNegedge&&(r.Q.BitCnt==33)) v.BitCnt = 0;
  else if(r.Q.SClkNegedge)              v.BitCnt = r.Q.BitCnt + 1;
  
  if((r.Q.BitCnt==0)||((r.Q.BitCnt==33))) v.CS = 0;
  else                                    v.CS = 1;
  
  // Word multiplexer:
  if(r.Q.load_run)  v.WordSelector = 1;
  else if(wNewWord) v.WordSelector = r.Q.WordSelector<<1;
  
  if(r.Q.load_run)                            v.SendWord = r.Q.conf1;
  else if(wNewWord&BIT32(r.Q.WordSelector,0)) v.SendWord = r.Q.conf2;
  else if(wNewWord&BIT32(r.Q.WordSelector,1)) v.SendWord = r.Q.conf3;
  else if(wNewWord&BIT32(r.Q.WordSelector,2)) v.SendWord = r.Q.pllconf;
  else if(wNewWord&BIT32(r.Q.WordSelector,3)) v.SendWord = r.Q.div;
  else if(wNewWord&BIT32(r.Q.WordSelector,4)) v.SendWord = r.Q.fdiv;
  else if(wNewWord&BIT32(r.Q.WordSelector,5)) v.SendWord = r.Q.strm;
  else if(wNewWord&BIT32(r.Q.WordSelector,6)) v.SendWord = r.Q.clkdiv;
  else if(wNewWord&BIT32(r.Q.WordSelector,7)) v.SendWord = r.Q.test1;
  else if(wNewWord&BIT32(r.Q.WordSelector,8)) v.SendWord = r.Q.test2;
  else if(r.Q.SClkNegedge&&(r.Q.BitCnt!=0)&&(r.Q.BitCnt!=33))   v.SendWord = r.Q.SendWord<<1;
  

  if (!rst)
  {
    v.load_run = 0;
    v.conf1 = 0;
    v.conf2 = 0;
    v.conf3 = 0;
    v.pllconf = 0;
    v.div = 0;
    v.fdiv = 0;
    v.strm = 0;
    v.clkdiv = 0;
    v.test1 = 0;
    v.test2 = 0;
    v.scale = 0;
    v.SCLK = 0;
    v.BitCnt = 0;
    v.CS = 0;
    v.select_spi = 0;
    v.ExtAntEna = 0;
    v.SendWord = 0;
  }


  r.CLK = clk;
  r.D = v;

  // APB interface:
  apbo.prdata = readdata;
  apbo.pirq   = 0;
  apbo.pindex = pindex;

  // external signals:
  outSCLK = r.Q.SCLK;
  outCSn[0]  = !(r.Q.CS & BIT32(r.Q.select_spi,0));
  outCSn[1]  = !(r.Q.CS & BIT32(r.Q.select_spi,1));
  outSDATA = BIT32(r.Q.SendWord,31);
  
  outExtAntEna = r.Q.ExtAntEna;
}

