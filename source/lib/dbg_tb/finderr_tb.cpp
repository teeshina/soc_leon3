#include "lheaders.h"

extern leon3mp  topLeon3mp;

extern void ResetPutStr();
extern void PrintIndexStr();


//****************************************************************************
void dbg::finderr_tb(SystemOnChipIO &io)
{
  char chTmp[256];
  ahb_mst_in_type     *pin_ahbi = &topLeon3mp.stCtrl2Mst;
  ahb_mst_out_type    *pch_ahbo = &topLeon3mp.stMst2Ctrl.arr[AHB_MASTER_LEON3];
  ahb_slv_in_type     *pin_ahbsi = &topLeon3mp.stCtrl2Slv;
  ahb_slv_out_vector  *pin_ahbso = &topLeon3mp.stSlv2Ctrl;

    // output file writting:
  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';

    ResetPutStr();
  
    // inputs:
    pStr = PutToStr(pStr, io.inNRst, 1, "inNRst");

    pStr = PutToStr(pStr, pin_ahbi->hgrant,AHB_MASTERS_MAX,"ahbmi"DIG".hgrant",true);//[0:15]  : (0 to AHB_MASTERS_MAX-1);     -- bus grant
    pStr = PutToStr(pStr, pin_ahbi->hready,1,"ahbmi"DIG".hready");//  ;                           -- transfer done
    pStr = PutToStr(pStr, pin_ahbi->hresp,2,"ahbmi"DIG".hresp");//[1:0] : (1 downto 0);   -- response type
    pStr = PutToStr(pStr, pin_ahbi->hrdata,CFG_AHBDW,"ahbmi"DIG".hrdata");//[31:0]  : (AHBDW-1 downto 0);   -- read data bus
    pStr = PutToStr(pStr, pin_ahbi->hcache,1,"ahbmi"DIG".hcache");//  ;                           -- cacheable
    pStr = PutToStr(pStr, pin_ahbi->hirq,AHB_IRQ_MAX,"ahbmi"DIG".hirq");//[31:0]    : (AHB_IRQ_MAX-1 downto 0); -- interrupt result bus
    pStr = PutToStr(pStr, pin_ahbi->testen,1,"ahbmi"DIG".testen");//  ;                           -- scan test enable
    pStr = PutToStr(pStr, pin_ahbi->testrst,1,"ahbmi"DIG".testrst");// ;                           -- scan test reset
    pStr = PutToStr(pStr, pin_ahbi->scanen,1,"ahbmi"DIG".scanen");//  ;                           -- scan enable
    pStr = PutToStr(pStr, pin_ahbi->testoen,1,"ahbmi"DIG".testoen");//   ;                           -- test output enable 
    //
    for (int32 i=0; i<AHB_MASTER_TOTAL;i++)
    {
      pch_ahbo = &topLeon3mp.stMst2Ctrl.arr[i];
      sprintf_s(chTmp,"ahbmo"DIG"(%i).hbusreq",i);
      pStr = PutToStr(pStr, pch_ahbo->hbusreq,1,chTmp);// ;                           -- bus request
      sprintf_s(chTmp,"ahbmo"DIG"(%i).hlock",i);
      pStr = PutToStr(pStr, pch_ahbo->hlock,1,chTmp);// ;                           -- lock request
      sprintf_s(chTmp,"ahbmo"DIG"(%i).htrans",i);
      pStr = PutToStr(pStr, pch_ahbo->htrans,2,chTmp);//[1:0]  : (1 downto 0);   -- transfer type
      sprintf_s(chTmp,"ahbmo"DIG"(%i).haddr",i);
      pStr = PutToStr(pStr, pch_ahbo->haddr,32,chTmp);//[31:0] : (31 downto 0);  -- address bus (byte)
      sprintf_s(chTmp,"ahbmo"DIG"(%i).hwrite",i);
      pStr = PutToStr(pStr, pch_ahbo->hwrite,1,chTmp);//  ;                           -- read/write
      sprintf_s(chTmp,"ahbmo"DIG"(%i).hsize",i);
      pStr = PutToStr(pStr, pch_ahbo->hsize,3,chTmp);//[2:0] : (2 downto 0);   -- transfer size
      sprintf_s(chTmp,"ahbmo"DIG"(%i).hburst",i);
      pStr = PutToStr(pStr, pch_ahbo->hburst,3,chTmp);//[2:0]  : (2 downto 0);   -- burst type
      sprintf_s(chTmp,"ahbmo"DIG"(%i).hprot",i);
      pStr = PutToStr(pStr, pch_ahbo->hprot,4,chTmp);//[3:0] : (3 downto 0);   -- protection control
      sprintf_s(chTmp,"ahbmo"DIG"(%i).hwdata",i);
      pStr = PutToStr(pStr, pch_ahbo->hwdata,CFG_AHBDW,chTmp);//[31:0]  : (AHBDW-1 downto 0);   -- write data bus
      sprintf_s(chTmp,"ahbmo"DIG"(%i).hirq",i);
      pStr = PutToStr(pStr, pch_ahbo->hirq,AHB_IRQ_MAX,chTmp);//[31:0]    : (AHB_IRQ_MAX-1 downto 0); -- interrupt bus
      for(int32 k=0; k<8; k++)
      {
        sprintf_s(chTmp,"ahbmo"DIG"(%i).hconfig(%i)",i,k);
        pStr = PutToStr(pStr, pch_ahbo->hconfig.arr[k],32,chTmp);//   : ahb_config_type;      -- memory access reg.
      }
      sprintf_s(chTmp,"conv_integer:ahbmo"DIG"(%i).hindex",i);
      pStr = PutToStr(pStr, pch_ahbo->hindex,4,chTmp);// integer range 0 to AHB_MASTERS_MAX-1;   -- diagnostic use only
    }
    //
    pStr = PutToStr(pStr, pin_ahbsi->hsel,AHB_SLAVES_MAX,"ahbsi"DIG".hsel",true);//  : (0 to AHB_SLAVES_MAX-1);     -- slave select
    pStr = PutToStr(pStr, pin_ahbsi->haddr,32,"ahbsi"DIG".haddr");// : (31 downto 0);  -- address bus (byte)
    pStr = PutToStr(pStr, pin_ahbsi->hwrite,1,"ahbsi"DIG".hwrite");//  ;                           -- read/write
    pStr = PutToStr(pStr, pin_ahbsi->htrans,2,"ahbsi"DIG".htrans");//  : (1 downto 0);   -- transfer type
    pStr = PutToStr(pStr, pin_ahbsi->hsize,3,"ahbsi"DIG".hsize");// : (2 downto 0);   -- transfer size
    pStr = PutToStr(pStr, pin_ahbsi->hburst,3,"ahbsi"DIG".hburst");//  : (2 downto 0);   -- burst type
    pStr = PutToStr(pStr, pin_ahbsi->hwdata,CFG_AHBDW,"ahbsi"DIG".hwdata");//  : (AHBDW-1 downto 0);   -- write data bus
    pStr = PutToStr(pStr, pin_ahbsi->hprot,4,"ahbsi"DIG".hprot");// : (3 downto 0);   -- protection control
    pStr = PutToStr(pStr, pin_ahbsi->hready,1,"ahbsi"DIG".hready");//  ;                -- transfer done
    pStr = PutToStr(pStr, pin_ahbsi->hmaster,4,"ahbsi"DIG".hmaster");// : (3 downto 0);  -- current master
    pStr = PutToStr(pStr, pin_ahbsi->hmastlock,1,"ahbsi"DIG".hmastlock");// ;              -- locked access
    pStr = PutToStr(pStr, pin_ahbsi->hmbsel,AHB_MEM_ID_WIDTH,"ahbsi"DIG".hmbsel");//  : (0 to AHB_MEM_ID_WIDTH-1); -- memory bank select
    pStr = PutToStr(pStr, pin_ahbsi->hcache,1,"ahbsi"DIG".hcache");//  ;                -- cacheable
    pStr = PutToStr(pStr, pin_ahbsi->hirq,AHB_IRQ_MAX,"ahbsi"DIG".hirq");//  : (AHB_IRQ_MAX-1 downto 0); -- interrupt result bus
    pStr = PutToStr(pStr, pin_ahbsi->testen,1,"ahbsi"DIG".testen");//                        -- scan test enable
    pStr = PutToStr(pStr, pin_ahbsi->testrst,1,"ahbsi"DIG".testrst");//                       -- scan test reset
    pStr = PutToStr(pStr, pin_ahbsi->scanen,1,"ahbsi"DIG".scanen");//                        -- scan enable
    pStr = PutToStr(pStr, pin_ahbsi->testoen,1,"ahbsi"DIG".testoen");;//                       -- test output enable 
    //
    for (int32 i=0; i<AHB_SLAVE_TOTAL; i++)
    {
      sprintf_s(chTmp,"ahbso"DIG"(%i).hready",i);
      pStr = PutToStr(pStr, pin_ahbso->arr[i].hready,1,chTmp);//                           -- transfer done
      sprintf_s(chTmp,"ahbso"DIG"(%i).hresp",i);
      pStr = PutToStr(pStr, pin_ahbso->arr[i].hresp,2,chTmp);//[1:0] : (1 downto 0);   -- response type
      sprintf_s(chTmp,"ahbso"DIG"(%i).hrdata",i);
      pStr = PutToStr(pStr, pin_ahbso->arr[i].hrdata,CFG_AHBDW,chTmp);//(AHBDW-1 downto 0);   -- read data bus
      sprintf_s(chTmp,"ahbso"DIG"(%i).hsplit",i);
      pStr = PutToStr(pStr, pin_ahbso->arr[i].hsplit,16,chTmp);//  : (15 downto 0);  -- split completion
      sprintf_s(chTmp,"ahbso"DIG"(%i).hcache",i);
      pStr = PutToStr(pStr, pin_ahbso->arr[i].hcache,1,chTmp);//                           -- cacheable
      sprintf_s(chTmp,"ahbso"DIG"(%i).hirq",i);
      pStr = PutToStr(pStr, pin_ahbso->arr[i].hirq,AHB_IRQ_MAX,chTmp);//    : (AHB_IRQ_MAX-1 downto 0); -- interrupt bus
      for(int32 k=0; k<8; k++)
      {
        sprintf_s(chTmp,"ahbso"DIG"(%i).hconfig(%i)",i,k);
        pStr = PutToStr(pStr, pin_ahbso->arr[i].hconfig.arr[k],32,chTmp);//   : ahb_config_type;      -- memory access reg.
      }
      sprintf_s(chTmp,"conv_integer:ahbso"DIG"(%i).hindex",i);
      pStr = PutToStr(pStr, pin_ahbso->arr[i].hindex,4,chTmp);//    : integer range 0 to AHB_SLAVES_MAX-1;   -- diagnostic use only
    }

    PrintIndexStr();

    *posBench[TB_finderr] << chStr << "\n";
  }
}
