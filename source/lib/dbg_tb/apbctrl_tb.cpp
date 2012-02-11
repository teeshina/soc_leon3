//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "lheaders.h"

extern leon3mp  topLeon3mp;

extern void ResetPutStr();
extern void PrintIndexStr();


//****************************************************************************
void dbg::apbctrl_tb(SystemOnChipIO &io)
{
  char chTmp[256];
  ahb_slv_in_type    *pin_ahbsi = &topLeon3mp.stCtrl2Slv;//    : in  ahb_slv_in_type;
  ahb_slv_out_type   *pch_ahbso = &topLeon3mp.stSlv2Ctrl.arr[AHB_SLAVE_APBBRIDGE];//    : out ahb_slv_out_type;
  apb_slv_in_type    *pch_apbi = &topLeon3mp.apbi;//    : out apb_slv_in_type;
  apb_slv_out_vector *pin_apbo = &topLeon3mp.apbo;//    : in  apb_slv_out_vector

#ifdef DBG_apbctrl
  if(io.inClk.eClock_z==SClock::CLK_POSEDGE)
  {
    //
    in_ahbsi.hsel = 0x1<<AHB_SLAVE_APBBRIDGE;//rand()&MSK32(AHB_SLAVES_MAX-1,0);//  : (0 to AHB_SLAVES_MAX-1);     -- slave select
    in_ahbsi.haddr = (0x800<<20)|(0x1<<8)|(rand()&0xff);// : (31 downto 0);  -- address bus (byte)
    in_ahbsi.hwrite = rand()&0x1;//  ;                           -- read/write
    in_ahbsi.htrans = rand()&0x3;//  : (1 downto 0);   -- transfer type
    in_ahbsi.hsize = rand()&0x7;// : (2 downto 0);   -- transfer size
    in_ahbsi.hburst = rand()&0x7;//  : (2 downto 0);   -- burst type
    in_ahbsi.hwdata = ((rand()<<17) + rand())&MSK32(CFG_AHBDW-1,0);//  : (AHBDW-1 downto 0);   -- write data bus
    in_ahbsi.hprot = rand()&0xf;// : (3 downto 0);   -- protection control
    in_ahbsi.hready = rand()&0x1;//  ;                -- transfer done
    in_ahbsi.hmaster = rand()&0xf;// : (3 downto 0);  -- current master
    in_ahbsi.hmastlock = rand()&0x1;// ;              -- locked access
    in_ahbsi.hmbsel = rand()&MSK32(AHB_MEM_ID_WIDTH-1,0);//  : (0 to AHB_MEM_ID_WIDTH-1); -- memory bank select
    in_ahbsi.hcache = rand()&0x1;//  ;                -- cacheable
    in_ahbsi.hirq = rand()&MSK32(AHB_IRQ_MAX-1,0);//  : (AHB_IRQ_MAX-1 downto 0); -- interrupt result bus
    in_ahbsi.testen = 0;//                        -- scan test enable
    in_ahbsi.testrst = 0;//                       -- scan test reset
    in_ahbsi.scanen = 0;//                        -- scan enable
    in_ahbsi.testoen = 0;//                       -- test output enable 
    //
    for(int32 i=0; i<APB_SLAVES_MAX; i++)
      in_apbo.arr[i] = apb_none;
    //
    in_apbo.arr[APB_UART_CFG].prdata = (rand()<<17) | rand();//[31:0]  : (31 downto 0);  -- read data bus
    in_apbo.arr[APB_UART_CFG].pirq = (rand()<<17) | rand();//[31:0]  : (AHB_IRQ_MAX-1 downto 0); -- interrupt bus
    ((ahb_device_reg*)(&in_apbo.arr[APB_UART_CFG].pconfig.arr[0]))->vendor  = VENDOR_GAISLER;
    ((ahb_device_reg*)(&in_apbo.arr[APB_UART_CFG].pconfig.arr[0]))->device  = GAISLER_APBUART;
    ((ahb_device_reg*)(&in_apbo.arr[APB_UART_CFG].pconfig.arr[0]))->version = 0;
    ((ahb_device_reg*)(&in_apbo.arr[APB_UART_CFG].pconfig.arr[0]))->cfgver  = 0;
    ((ahb_device_reg*)(&in_apbo.arr[APB_UART_CFG].pconfig.arr[0]))->interrupt = IRQ_UART_CFG;
    ((apb_iobar*)(&in_apbo.arr[APB_UART_CFG].pconfig.arr[1]))->area_id  = 0x1;
    ((apb_iobar*)(&in_apbo.arr[APB_UART_CFG].pconfig.arr[1]))->addrmask = 0xfff;
    ((apb_iobar*)(&in_apbo.arr[APB_UART_CFG].pconfig.arr[1]))->zero     = 0x0;
    ((apb_iobar*)(&in_apbo.arr[APB_UART_CFG].pconfig.arr[1]))->memaddr  = 0x1;
    in_apbo.arr[APB_UART_CFG].pindex = APB_UART_CFG;//[0:15]      : integer range 0 to APB_SLAVES_MAX -1;  -- diag use only
  }
  
  ptst_apbctrl->Update(io.inNRst,
                       io.inClk,
                       in_ahbsi,
                       ch_ahbso,
                       ch_apbi,
                       in_apbo);
  
  pin_ahbsi = &in_ahbsi;//    : in  ahb_slv_in_type;
  pch_ahbso = &ch_ahbso;//    : out ahb_slv_out_type;
  pch_apbi = &ch_apbi;//    : out apb_slv_in_type;
  pin_apbo = &in_apbo;//    : in  apb_slv_out_vector
#endif

  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';

    ResetPutStr();
  
    // inputs:
    pStr = PutToStr(pStr, io.inNRst, 1, "inNRst");
    //
    pStr = PutToStr(pStr, pin_ahbsi->hsel,AHB_SLAVES_MAX,"in_ahbsi.hsel",true);//  : (0 to AHB_SLAVES_MAX-1);     -- slave select
    pStr = PutToStr(pStr, pin_ahbsi->haddr,32,"in_ahbsi.haddr");// : (31 downto 0);  -- address bus (byte)
    pStr = PutToStr(pStr, pin_ahbsi->hwrite,1,"in_ahbsi.hwrite");//  ;                           -- read/write
    pStr = PutToStr(pStr, pin_ahbsi->htrans,2,"in_ahbsi.htrans");//  : (1 downto 0);   -- transfer type
    pStr = PutToStr(pStr, pin_ahbsi->hsize,3,"in_ahbsi.hsize");// : (2 downto 0);   -- transfer size
    pStr = PutToStr(pStr, pin_ahbsi->hburst,3,"in_ahbsi.hburst");//  : (2 downto 0);   -- burst type
    pStr = PutToStr(pStr, pin_ahbsi->hwdata,CFG_AHBDW,"in_ahbsi.hwdata");//  : (AHBDW-1 downto 0);   -- write data bus
    pStr = PutToStr(pStr, pin_ahbsi->hprot,4,"in_ahbsi.hprot");// : (3 downto 0);   -- protection control
    pStr = PutToStr(pStr, pin_ahbsi->hready,1,"in_ahbsi.hready");//  ;                -- transfer done
    pStr = PutToStr(pStr, pin_ahbsi->hmaster,4,"in_ahbsi.hmaster");// : (3 downto 0);  -- current master
    pStr = PutToStr(pStr, pin_ahbsi->hmastlock,1,"in_ahbsi.hmastlock");// ;              -- locked access
    pStr = PutToStr(pStr, pin_ahbsi->hmbsel,AHB_MEM_ID_WIDTH,"in_ahbsi.hmbsel",true);//  : (0 to AHB_MEM_ID_WIDTH-1); -- memory bank select
    pStr = PutToStr(pStr, pin_ahbsi->hcache,1,"in_ahbsi.hcache");//  ;                -- cacheable
    pStr = PutToStr(pStr, pin_ahbsi->hirq,AHB_IRQ_MAX,"in_ahbsi.hirq");//  : (AHB_IRQ_MAX-1 downto 0); -- interrupt result bus
    pStr = PutToStr(pStr, pin_ahbsi->testen,1,"in_ahbsi.testen");//                        -- scan test enable
    pStr = PutToStr(pStr, pin_ahbsi->testrst,1,"in_ahbsi.testrst");//                       -- scan test reset
    pStr = PutToStr(pStr, pin_ahbsi->scanen,1,"in_ahbsi.scanen");//                        -- scan enable
    pStr = PutToStr(pStr, pin_ahbsi->testoen,1,"in_ahbsi.testoen");;//                       -- test output enable 
    //
    for(int32 i=0; i<APB_TOTAL; i++)
    {
      sprintf_s(chTmp,"in_apbo(%i).prdata",i);
      pStr = PutToStr(pStr, pin_apbo->arr[i].prdata,32,chTmp);//                           -- transfer done
      sprintf_s(chTmp,"in_apbo(%i).pirq",i);
      pStr = PutToStr(pStr, pin_apbo->arr[i].pirq,32,chTmp);
      for(int32 k=0; k<2; k++)
      {
        sprintf_s(chTmp,"in_apbo(%i).pconfig(%i)",i,k);
        pStr = PutToStr(pStr, pin_apbo->arr[i].pconfig.arr[k],32,chTmp);
      }
      sprintf_s(chTmp,"conv_integer:in_apbo(%i).pindex",i);
      pStr = PutToStr(pStr, pin_apbo->arr[i].pindex,4,chTmp);
    }


    // Output:
    pStr = PutToStr(pStr, pch_apbi->psel,APB_SLAVES_MAX,"ch_apbi.psel",true);//[0:15]  : (0 to APB_SLAVES_MAX-1); -- slave select
    pStr = PutToStr(pStr, pch_apbi->penable,1,"ch_apbi.penable");// ;                       -- strobe
    pStr = PutToStr(pStr, pch_apbi->paddr,32,"ch_apbi.paddr");//[31:0] : (31 downto 0);  -- address bus (byte)
    pStr = PutToStr(pStr, pch_apbi->pwrite,1,"ch_apbi.pwrite");//  ;                       -- write
    pStr = PutToStr(pStr, pch_apbi->pwdata,32,"ch_apbi.pwdata");//[31:0]  : (31 downto 0);  -- write data bus
    pStr = PutToStr(pStr, pch_apbi->pirq,AHB_IRQ_MAX,"ch_apbi.pirq");//[31:0]  : (AHB_IRQ_MAX-1 downto 0); -- interrupt result bus
    pStr = PutToStr(pStr, pch_apbi->testen,1,"ch_apbi.testen");//                          -- scan test enable
    pStr = PutToStr(pStr, pch_apbi->testrst,1,"ch_apbi.testrst");//                         -- scan test reset
    pStr = PutToStr(pStr, pch_apbi->scanen,1,"ch_apbi.scanen");//                          -- scan enable
    pStr = PutToStr(pStr, pch_apbi->testoen,1,"ch_apbi.testoen");//                         -- test output enable

    //
    pStr = PutToStr(pStr, pch_ahbso->hready,1,"ch_ahbso.hready");//                           -- transfer done
    pStr = PutToStr(pStr, pch_ahbso->hresp,2,"ch_ahbso.hresp");//[1:0] : (1 downto 0);   -- response type
    pStr = PutToStr(pStr, pch_ahbso->hrdata,CFG_AHBDW,"ch_ahbso.hrdata");//(AHBDW-1 downto 0);   -- read data bus
    pStr = PutToStr(pStr, pch_ahbso->hsplit,16,"ch_ahbso.hsplit");//  : (15 downto 0);  -- split completion
    pStr = PutToStr(pStr, pch_ahbso->hcache,1,"ch_ahbso.hcache");//                           -- cacheable
    pStr = PutToStr(pStr, pch_ahbso->hirq,AHB_IRQ_MAX,"ch_ahbso.hirq");//    : (AHB_IRQ_MAX-1 downto 0); -- interrupt bus
    pStr = PutToStr(pStr, pch_ahbso->hconfig.arr[0],32,"ch_ahbso.hconfig(0)");//   : ahb_config_type;      -- memory access reg.
    pStr = PutToStr(pStr, pch_ahbso->hconfig.arr[1],32,"ch_ahbso.hconfig(1)");
    pStr = PutToStr(pStr, pch_ahbso->hconfig.arr[2],32,"ch_ahbso.hconfig(2)");
    pStr = PutToStr(pStr, pch_ahbso->hconfig.arr[3],32,"ch_ahbso.hconfig(3)");
    pStr = PutToStr(pStr, pch_ahbso->hconfig.arr[4],32,"ch_ahbso.hconfig(4)");
    pStr = PutToStr(pStr, pch_ahbso->hconfig.arr[5],32,"ch_ahbso.hconfig(5)");
    pStr = PutToStr(pStr, pch_ahbso->hconfig.arr[6],32,"ch_ahbso.hconfig(6)");
    pStr = PutToStr(pStr, pch_ahbso->hconfig.arr[7],32,"ch_ahbso.hconfig(7)");
    pStr = PutToStr(pStr, pch_ahbso->hindex,4,"conv_integer:ch_ahbso.hindex");//    : integer range 0 to AHB_SLAVES_MAX-1;   -- diagnostic use only

    PrintIndexStr();

    *posBench[TB_apbctrl] << chStr << "\n";
  }

#ifdef DBG_apbctrl
  ptst_apbctrl->ClkUpdate();
#endif
}

