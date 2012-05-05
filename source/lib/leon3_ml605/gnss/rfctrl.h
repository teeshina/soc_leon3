//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

class RfControl
{
  friend class dbg;
  private:
    uint32 pindex;
    uint32 paddr;
    uint32 pmask;
  private:
    struct regs
    {
      uint32 conf1;
      uint32 conf2;
      uint32 conf3;
      uint32 pllconf;
      uint32 div;
      uint32 fdiv;
      uint32 strm;
      uint32 clkdiv;
      uint32 test1;
      uint32 test2;
      uint32 scale;
      uint32 load_run   : 1;
      uint32 select_spi : 2;
      uint32 loading    : 1;
      uint32 ScaleCnt;
      uint32 SClkPosedge  : 1;
      uint32 SClkNegedge  : 1;
      uint32 SCLK         : 1;
      uint32 BitCnt       : 6;
      uint32 CS           : 1;  //!! not inversed!!
      uint32 WordSelector : 9;
      uint32 SendWord;
      uint32 ExtAntEna    : 1;
      uint32 BootID;
    };

    regs v;
    TDFF<regs> r;//

    uint32 readdata;
    uint32 wNewWord;


  public:
    RfControl(uint32 pindex_=APB_RF_CONTROL, uint32 paddr_=0x4, uint32 pmask_=0xfff);
    
    void Update(  uint32 rst,//    : in  std_ulogic;
                  SClock clk,//    : in  std_ulogic;
                  apb_slv_in_type &in_apbi,//   : in  apb_slv_in_type;
                  apb_slv_out_type &out_apbo,//   : out apb_slv_out_type;
                  uint32 inLD[SystemOnChipIO::TOTAL_MAXIM2769],
                  uint32 &outSCLK,
                  uint32 &outSDATA,
                  uint32 *outCSn,
                  // Antenna control:
                  uint32 inExtAntStat,
                  uint32 inExtAntDetect,
                  uint32 &outExtAntEna );

    void ClkUpdate()
    {
      r.ClkUpdate();
    }
};
