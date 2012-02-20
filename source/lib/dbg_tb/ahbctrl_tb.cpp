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
void dbg::ahbctrl_tb(SystemOnChipIO &io)
{
  char chTmp[256];
  ahb_mst_in_type    *pstCtrl2Mst = &topLeon3mp.stCtrl2Mst;
  ahb_mst_out_vector *pstMst2Ctrl = &topLeon3mp.stMst2Ctrl;
  ahb_slv_in_type    *pstCtrl2Slv = &topLeon3mp.stCtrl2Slv;
  ahb_slv_out_vector *pstSlv2Ctrl = &topLeon3mp.stSlv2Ctrl;

#ifdef DBG_ahbctrl
  if(io.inClk.eClock_z==SClock::CLK_POSEDGE)
  {
    for(int32 i=0; i<AHB_MASTER_TOTAL; i++)
    {
      in_msto.arr[i].hbusreq = rand()&0x1;
      in_msto.arr[i].hlock = rand()&0x1;
      in_msto.arr[i].htrans = rand()&0x3;
      in_msto.arr[i].haddr  = 0x80000000 | rand();
      in_msto.arr[i].hwrite = rand()&0x1;
      in_msto.arr[i].hsize  = 2;
      in_msto.arr[i].hburst = rand()&0x7;
      in_msto.arr[i].hprot = rand()&0xf;
      in_msto.arr[i].hwdata = (rand()<<17) | rand();
      in_msto.arr[i].hirq = (rand()<<17) | rand();
      for(int32 k=0; k<8; k++)
      {
        in_msto.arr[i].hconfig.arr[k] = (rand()<<17) | rand();
      }
      in_msto.arr[i].hindex = i;
    }
    //
    r.hmaster = rand()&MSK32(log2x[AHB_MASTER_TOTAL]-1,0);
    //
    in_split = rand()& MSK32((0x1<<log2[AHB_MASTER_TOTAL])-1, 0);

  }
  tst_ahbctrl.selmast(r, in_msto, in_split, ch_mast, ch_defmst);

  pstMst2Ctrl = &in_msto;
#endif

  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';
    ResetPutStr();

    // Input:
    pStr = PutToStr(pStr, io.inNRst, 1,"inNRst");
    //
    for(int32 i=0; i<AHB_MASTER_TOTAL; i++)
    {
      sprintf_s(chTmp,"in_msto(%i).hbusreq",i);
      pStr = PutToStr(pStr, pstMst2Ctrl->arr[i].hbusreq,1,chTmp);
      sprintf_s(chTmp,"in_msto(%i).hlock",i);
      pStr = PutToStr(pStr, pstMst2Ctrl->arr[i].hlock,1,chTmp);
      sprintf_s(chTmp,"in_msto(%i).htrans",i);
      pStr = PutToStr(pStr, pstMst2Ctrl->arr[i].htrans,2,chTmp);
      sprintf_s(chTmp,"in_msto(%i).haddr",i);
      pStr = PutToStr(pStr, pstMst2Ctrl->arr[i].haddr,32,chTmp);
      sprintf_s(chTmp,"in_msto(%i).hwrite",i);
      pStr = PutToStr(pStr, pstMst2Ctrl->arr[i].hwrite,1,chTmp);
      sprintf_s(chTmp,"in_msto(%i).hsize",i);
      pStr = PutToStr(pStr, pstMst2Ctrl->arr[i].hsize,3,chTmp);
      sprintf_s(chTmp,"in_msto(%i).hburst",i);
      pStr = PutToStr(pStr, pstMst2Ctrl->arr[i].hburst,3,chTmp);
      sprintf_s(chTmp,"in_msto(%i).hprot",i);
      pStr = PutToStr(pStr, pstMst2Ctrl->arr[i].hprot,4,chTmp);
      sprintf_s(chTmp,"in_msto(%i).hwdata",i);
      pStr = PutToStr(pStr, pstMst2Ctrl->arr[i].hwdata,32,chTmp);
      sprintf_s(chTmp,"in_msto(%i).hirq",i);
      pStr = PutToStr(pStr, pstMst2Ctrl->arr[i].hirq,32,chTmp);
      for(int32 k=0; k<8; k++)
      {
        sprintf_s(chTmp,"in_msto(%i).hconfig(%i)",i,k);
        pStr = PutToStr(pStr, pstMst2Ctrl->arr[i].hconfig.arr[k],32,chTmp);
      }
      sprintf_s(chTmp,"conv_integer:in_msto(%i).hindex",i);
      pStr = PutToStr(pStr, pstMst2Ctrl->arr[i].hindex,4,chTmp);
    }

//    pStr = PutToStr(pStr, r.hmaster, log2x[AHB_MASTER_TOTAL], "conv_integer:in_r.hmaster");
//    pStr = PutToStr(pStr, in_split, (0x1<<log2[AHB_MASTER_TOTAL]), "in_split", true);
//    pStr = PutToStr(pStr, ch_mast, log2x[AHB_MASTER_TOTAL], "conv_integer:ch_mast");
//    pStr = PutToStr(pStr, ch_defmst, 1, "ch_defmst");
    //
    for(int32 i=0; i<AHB_SLAVE_TOTAL; i++)
    {
      sprintf_s(chTmp,"in_slvo(%i).hready",i);
      pStr = PutToStr(pStr, pstSlv2Ctrl->arr[i].hready,1,chTmp);//                           -- transfer done
      sprintf_s(chTmp,"in_slvo(%i).hresp",i);
      pStr = PutToStr(pStr, pstSlv2Ctrl->arr[i].hresp,2,chTmp);// : (1 downto 0);   -- response type
      sprintf_s(chTmp,"in_slvo(%i).hrdata",i);
      pStr = PutToStr(pStr, pstSlv2Ctrl->arr[i].hrdata,32,chTmp);//[31:0]  : (AHBDW-1 downto 0);   -- read data bus
      sprintf_s(chTmp,"in_slvo(%i).hsplit",i);
      pStr = PutToStr(pStr, pstSlv2Ctrl->arr[i].hsplit,16,chTmp);//  : (15 downto 0);  -- split completion
      sprintf_s(chTmp,"in_slvo(%i).hcache",i);
      pStr = PutToStr(pStr, pstSlv2Ctrl->arr[i].hcache,1,chTmp);//                           -- cacheable
      sprintf_s(chTmp,"in_slvo(%i).hirq",i);
      pStr = PutToStr(pStr, pstSlv2Ctrl->arr[i].hirq,32,chTmp);//[31:0]    : (AHB_IRQ_MAX-1 downto 0); -- interrupt bus
      for(int32 k=0; k<8; k++)
      {
        sprintf_s(chTmp,"in_slvo(%i).hconfig(%i)",i,k);
        pStr = PutToStr(pStr, pstSlv2Ctrl->arr[i].hconfig.arr[k],32,chTmp);
      }
      sprintf_s(chTmp,"conv_integer:in_slvo(%i).hindex",i);
      pStr = PutToStr(pStr, pstSlv2Ctrl->arr[i].hindex,4,chTmp);//[15:0]    : integer range 0 to AHB_SLAVES_MAX-1;   -- diagnostic use only
    }

    // Outputs:
    pStr = PutToStr(pStr, pstCtrl2Mst->hgrant,16,"ch_msti.hgrant",true);//[0:15]  : (0 to AHB_MASTERS_MAX-1);     -- bus grant
    pStr = PutToStr(pStr, pstCtrl2Mst->hready,1,"ch_msti.hready");//  ;                           -- transfer done
    pStr = PutToStr(pStr, pstCtrl2Mst->hresp,2,"ch_msti.hresp");// : (1 downto 0);   -- response type
    pStr = PutToStr(pStr, pstCtrl2Mst->hrdata,32,"ch_msti.hrdata");//[31:0]  : (AHBDW-1 downto 0);   -- read data bus
    pStr = PutToStr(pStr, pstCtrl2Mst->hcache,1,"ch_msti.hcache");//                             -- cacheable
    pStr = PutToStr(pStr, pstCtrl2Mst->hirq,32,"ch_msti.hirq");//    : (AHB_IRQ_MAX-1 downto 0); -- interrupt result bus
    pStr = PutToStr(pStr, pstCtrl2Mst->testen,1,"ch_msti.testen");//                             -- scan test enable
    pStr = PutToStr(pStr, pstCtrl2Mst->testrst,1,"ch_msti.testrst");//                            -- scan test reset
    pStr = PutToStr(pStr, pstCtrl2Mst->scanen,1,"ch_msti.scanen");//                             -- scan enable
    pStr = PutToStr(pStr, pstCtrl2Mst->testoen,1,"ch_msti.testoen");//                              -- test output enable 

    pStr = PutToStr(pStr, pstCtrl2Slv->hsel,16,"ch_slvi.hsel",true);//[0:15]  : (0 to AHB_SLAVES_MAX-1);     -- slave select
    pStr = PutToStr(pStr, pstCtrl2Slv->haddr,32,"ch_slvi.haddr");// : (31 downto 0);  -- address bus (byte)
    pStr = PutToStr(pStr, pstCtrl2Slv->hwrite,1,"ch_slvi.hwrite");//  ;                           -- read/write
    pStr = PutToStr(pStr, pstCtrl2Slv->htrans,2,"ch_slvi.htrans");//  : (1 downto 0);   -- transfer type
    pStr = PutToStr(pStr, pstCtrl2Slv->hsize,3,"ch_slvi.hsize");// : (2 downto 0);   -- transfer size
    pStr = PutToStr(pStr, pstCtrl2Slv->hburst,3,"ch_slvi.hburst");//  : (2 downto 0);   -- burst type
    pStr = PutToStr(pStr, pstCtrl2Slv->hwdata,32,"ch_slvi.hwdata");//[31:0]  : (AHBDW-1 downto 0);   -- write data bus
    pStr = PutToStr(pStr, pstCtrl2Slv->hprot,4,"ch_slvi.hprot");//[3:0] : (3 downto 0);   -- protection control
    pStr = PutToStr(pStr, pstCtrl2Slv->hready,1,"ch_slvi.hready");//  ;                -- transfer done
    pStr = PutToStr(pStr, pstCtrl2Slv->hmaster,4,"ch_slvi.hmaster");// : (3 downto 0);  -- current master
    pStr = PutToStr(pStr, pstCtrl2Slv->hmastlock,1,"ch_slvi.hmastlock");// ;              -- locked access
    pStr = PutToStr(pStr, pstCtrl2Slv->hmbsel,4,"ch_slvi.hmbsel",true);//[0:3]  : (0 to AHB_MEM_ID_WIDTH-1); -- memory bank select
    pStr = PutToStr(pStr, pstCtrl2Slv->hcache,1,"ch_slvi.hcache");//  ;                -- cacheable
    pStr = PutToStr(pStr, pstCtrl2Slv->hirq,32,"ch_slvi.hirq");//[31:0]    : (AHB_IRQ_MAX-1 downto 0); -- interrupt result bus
    pStr = PutToStr(pStr, pstCtrl2Slv->testen,1,"ch_slvi.testen");//                        -- scan test enable
    pStr = PutToStr(pStr, pstCtrl2Slv->testrst,1,"ch_slvi.testrst");//                       -- scan test reset
    pStr = PutToStr(pStr, pstCtrl2Slv->scanen,1,"ch_slvi.scanen");//                        -- scan enable
    pStr = PutToStr(pStr, pstCtrl2Slv->testoen,1,"ch_slvi.testoen");//                       -- test output enable 
    
    PrintIndexStr();
    
    *posBench[TB_ahbctrl] << chStr << "\n";
  }
}
