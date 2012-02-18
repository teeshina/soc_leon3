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
void dbg::mmu_acache_tb(SystemOnChipIO &io)
{
  char chTmp[256];
  mmu_acache *ptst_mmu_acache = topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->pclACache;
  
  uint32 *pin_hclken  = &topLeon3mp.pclLeon3s[0]->vcc;// : in std_ulogic
  memory_ic_in_type *pin_mcii   = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->mcii;//   : in  memory_ic_in_type;
  memory_ic_out_type *pch_mcio  = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->mcio;//   : out memory_ic_out_type;
  memory_dc_in_type *pin_mcdi   = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->mcdi;//   : in  memory_dc_in_type;
  memory_dc_out_type *pch_mcdo  = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->mcdo;//   : out memory_dc_out_type;
  memory_mm_in_type *pin_mcmmi  = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->mcmmi;//  : in  memory_mm_in_type;
  memory_mm_out_type *pch_mcmmo = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->mcmmo;//  : out memory_mm_out_type;
  ahb_mst_in_type *pin_ahbi     = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->ahbi2;//   : in  ahb_mst_in_type;
  ahb_mst_out_type *pch_ahbo    = &topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->ahbo2;//   : out ahb_mst_out_type;
  ahb_slv_out_vector *pin_ahbso = &topLeon3mp.stSlv2Ctrl;//  : in  ahb_slv_out_vector;
  
#ifdef DBG_mmu_acache
  if(io.inClk.eClock_z==SClock::CLK_POSEDGE)
  {
    in_hclken = 1;
    //
    in_mcii.address = (rand()<<17) + rand();//          : std_logic_vector(31 downto 0); // memory address
    in_mcii.burst = rand()&0x1;//            : std_ulogic;      // burst request
    in_mcii.req = rand()&0x1;//              : std_ulogic;      // memory cycle request
    in_mcii.su = rand()&0x1;//               : std_ulogic;      // supervisor address space
    in_mcii.flush = rand()&0x1;//            : std_ulogic;      // flush in progress
    //
    in_mcdi.address = ((0x555+1)<<20);//(rand()<<17) + rand();//          : std_logic_vector(31 downto 0); 
    in_mcdi.data = (rand()<<17) + rand();//             : std_logic_vector(31 downto 0);
    in_mcdi.asi = rand()&0xf;//              : std_logic_vector(3 downto 0); // ASI for load/store
    in_mcdi.size = rand()&0x3;//             : std_logic_vector(1 downto 0);
    in_mcdi.burst = rand()&0x1;//            : std_ulogic;
    in_mcdi.read = rand()&0x1;//             : std_ulogic;
    in_mcdi.req = rand()&0x1;//              : std_ulogic;
    in_mcdi.lock = rand()&0x1;//             : std_ulogic;
    in_mcdi.cache = rand()&0x1;//            : std_ulogic;
    //
    in_mcmmi.address = (rand()<<17) + rand();//          : std_logic_vector(31 downto 0); 
    in_mcmmi.data = (rand()<<17) + rand();//             : std_logic_vector(31 downto 0);
    in_mcmmi.size = rand()&0x3;//          : std_logic_vector(1 downto 0);
    in_mcmmi.burst = rand()&0x1;//          : std_logic;
    in_mcmmi.read = rand()&0x1;//          : std_logic;
    in_mcmmi.req = rand()&0x1;//          : std_logic;
    in_mcmmi.lock = rand()&0x1;//          : std_logic;
    //
    in_ahbi.hgrant = rand()&0x1;//[0:15]  : (0 to AHB_MASTERS_MAX-1);     -- bus grant
    in_ahbi.hready = rand()&0x1;//  ;                           -- transfer done
    in_ahbi.hresp = rand()&0x3;//[1:0] : (1 downto 0);   -- response type
    in_ahbi.hrdata = ((rand()<<17) + rand()) & MSK32(CFG_AHBDW-1,0);//[31:0]  : (AHBDW-1 downto 0);   -- read data bus
    in_ahbi.hcache = rand()&0x1;//  ;                           -- cacheable
    in_ahbi.hirq = ((rand()<<17) + rand()) & MSK32(AHB_IRQ_MAX-1,0);//[31:0]    : (AHB_IRQ_MAX-1 downto 0); -- interrupt result bus
    in_ahbi.testen = 0;//  ;                           -- scan test enable
    in_ahbi.testrst = 0;// ;                           -- scan test reset
    in_ahbi.scanen = 0;//  ;                           -- scan enable
    in_ahbi.testoen = 0;//   ;                           -- test output enable 
    //
    for(int32 i=0; i<3; i++)
    {
      in_ahbso.arr[i].hready = rand()&0x1;//                           -- transfer done
      in_ahbso.arr[i].hresp = rand()&0x3;//[1:0] : (1 downto 0);   -- response type
      in_ahbso.arr[i].hrdata = ((rand()<<17) + rand()) & MSK32(CFG_AHBDW-1,0);//[31:0]  : (AHBDW-1 downto 0);   -- read data bus
      in_ahbso.arr[i].hsplit = rand();//[15:0]  : (15 downto 0);  -- split completion
      in_ahbso.arr[i].hcache = rand()&0x1;//                           -- cacheable
      in_ahbso.arr[i].hirq = ((rand()<<17) + rand()) & MSK32(AHB_IRQ_MAX-1,0);//[31:0]    : (AHB_IRQ_MAX-1 downto 0); -- interrupt bus
      in_ahbso.arr[i].hconfig.arr[0] = (rand()<<17) + rand();//   : ahb_config_type;      -- memory access reg.
      in_ahbso.arr[i].hconfig.arr[1] = (rand()<<17) + rand();
      in_ahbso.arr[i].hconfig.arr[2] = (rand()<<17) + rand();
      in_ahbso.arr[i].hconfig.arr[3] = (rand()<<17) + rand();
      in_ahbso.arr[i].hconfig.arr[4] = ((0x555+i)<<20) | (1<<16) | (0xFFF0);//(rand()<<17) + rand();
      in_ahbso.arr[i].hconfig.arr[5] = (rand()<<17) + rand();
      in_ahbso.arr[i].hconfig.arr[6] = (rand()<<17) + rand();
      in_ahbso.arr[i].hconfig.arr[7] = (rand()<<17) + rand();
      in_ahbso.arr[i].hindex = rand()&0xf;//[15:0]    : integer range 0 to AHB_SLAVES_MAX-1;   -- diagnostic use only
    }
    for(int32 i=3; i<AHB_SLAVES_MAX; i++)
    {
      in_ahbso.arr[i] = ahbs_none;
    }
  }

  tst_mmu_acache.Update(io.inNRst,//    : in  std_logic;
                        io.inClk,//    : in  std_logic;
                        in_mcii,//   : in  memory_ic_in_type;
                        ch_mcio,//   : out memory_ic_out_type;
                        in_mcdi,//   : in  memory_dc_in_type;
                        ch_mcdo,//   : out memory_dc_out_type;
                        in_mcmmi,//  : in  memory_mm_in_type;
                        ch_mcmmo,//  : out memory_mm_out_type;
                        in_ahbi,//   : in  ahb_mst_in_type;
                        ch_ahbo,//   : out ahb_mst_out_type;
                        in_ahbso,//  : in  ahb_slv_out_vector;
                        in_hclken);
  ptst_mmu_acache = &tst_mmu_acache;
  
  pin_hclken  = &in_hclken;
  pin_mcii    = &in_mcii;
  pch_mcio    = &ch_mcio;
  pin_mcdi    = &in_mcdi;
  pch_mcdo    = &ch_mcdo;
  pin_mcmmi   = &in_mcmmi;
  pch_mcmmo   = &ch_mcmmo;
  pin_ahbi    = &in_ahbi;
  pch_ahbo    = &ch_ahbo;
  pin_ahbso   = &in_ahbso;

#endif

  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';

    ResetPutStr();
  
    // inputs:
    pStr = PutToStr(pStr, io.inNRst, 1, "inNRst");
    pStr = PutToStr(pStr, *pin_hclken, 1, "in_hclken");
    //
    pStr = PutToStr(pStr, pin_mcii->address,32,"in_mcii.address");//          : std_logic_vector(31 downto 0); // memory address
    pStr = PutToStr(pStr, pin_mcii->burst,1,"in_mcii.burst");//            : std_ulogic;      // burst request
    pStr = PutToStr(pStr, pin_mcii->req,1,"in_mcii.req");//              : std_ulogic;      // memory cycle request
    pStr = PutToStr(pStr, pin_mcii->su,1,"in_mcii.su");//               : std_ulogic;      // supervisor address space
    pStr = PutToStr(pStr, pin_mcii->flush,1,"in_mcii.flush");//            : std_ulogic;      // flush in progress
    //
    pStr = PutToStr(pStr, pin_mcdi->address,32,"in_mcdi.address");//          : std_logic_vector(31 downto 0); 
    pStr = PutToStr(pStr, pin_mcdi->data,32,"in_mcdi.data");//             : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pin_mcdi->asi,4,"in_mcdi.asi");//              : std_logic_vector(3 downto 0); // ASI for load/store
    pStr = PutToStr(pStr, pin_mcdi->size,2,"in_mcdi.size");//             : std_logic_vector(1 downto 0);
    pStr = PutToStr(pStr, pin_mcdi->burst,1,"in_mcdi.burst");//            : std_ulogic;
    pStr = PutToStr(pStr, pin_mcdi->read,1,"in_mcdi.read");//             : std_ulogic;
    pStr = PutToStr(pStr, pin_mcdi->req,1,"in_mcdi.req");//              : std_ulogic;
    pStr = PutToStr(pStr, pin_mcdi->lock,1,"in_mcdi.lock");//             : std_ulogic;
    pStr = PutToStr(pStr, pin_mcdi->cache,1,"in_mcdi.cache");//            : std_ulogic;
    //
    pStr = PutToStr(pStr, pin_mcmmi->address,32,"in_mcmmi.address");//          : std_logic_vector(31 downto 0); 
    pStr = PutToStr(pStr, pin_mcmmi->data,32,"in_mcmmi.data");//             : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pin_mcmmi->size,2,"in_mcmmi.size");//          : std_logic_vector(1 downto 0);
    pStr = PutToStr(pStr, pin_mcmmi->burst,1,"in_mcmmi.burst");//          : std_logic;
    pStr = PutToStr(pStr, pin_mcmmi->read,1,"in_mcmmi.read");//          : std_logic;
    pStr = PutToStr(pStr, pin_mcmmi->req,1,"in_mcmmi.req");//          : std_logic;
    pStr = PutToStr(pStr, pin_mcmmi->lock,1,"in_mcmmi.lock");//          : std_logic;
    //
    pStr = PutToStr(pStr, pin_ahbi->hgrant,AHB_MASTERS_MAX,"in_ahbi.hgrant",true);//[0:15]  : (0 to AHB_MASTERS_MAX-1);     -- bus grant
    pStr = PutToStr(pStr, pin_ahbi->hready,1,"in_ahbi.hready");//  ;                           -- transfer done
    pStr = PutToStr(pStr, pin_ahbi->hresp,2,"in_ahbi.hresp");//[1:0] : (1 downto 0);   -- response type
    pStr = PutToStr(pStr, pin_ahbi->hrdata,CFG_AHBDW,"in_ahbi.hrdata");//[31:0]  : (AHBDW-1 downto 0);   -- read data bus
    pStr = PutToStr(pStr, pin_ahbi->hcache,1,"in_ahbi.hcache");//  ;                           -- cacheable
    pStr = PutToStr(pStr, pin_ahbi->hirq,AHB_IRQ_MAX,"in_ahbi.hirq");//[31:0]    : (AHB_IRQ_MAX-1 downto 0); -- interrupt result bus
    pStr = PutToStr(pStr, pin_ahbi->testen,1,"in_ahbi.testen");//  ;                           -- scan test enable
    pStr = PutToStr(pStr, pin_ahbi->testrst,1,"in_ahbi.testrst");// ;                           -- scan test reset
    pStr = PutToStr(pStr, pin_ahbi->scanen,1,"in_ahbi.scanen");//  ;                           -- scan enable
    pStr = PutToStr(pStr, pin_ahbi->testoen,1,"in_ahbi.testoen");//   ;                           -- test output enable 
    //
    for (int32 i=0; i<AHB_SLAVE_TOTAL; i++)
    {
      sprintf_s(chTmp,"in_ahbso(%i).hready",i);
      pStr = PutToStr(pStr, pin_ahbso->arr[i].hready,1,chTmp);//                           -- transfer done
      sprintf_s(chTmp,"in_ahbso(%i).hresp",i);
      pStr = PutToStr(pStr, pin_ahbso->arr[i].hresp,2,chTmp);//[1:0] : (1 downto 0);   -- response type
      sprintf_s(chTmp,"in_ahbso(%i).hrdata",i);
      pStr = PutToStr(pStr, pin_ahbso->arr[i].hrdata,CFG_AHBDW,chTmp);//(AHBDW-1 downto 0);   -- read data bus
      sprintf_s(chTmp,"in_ahbso(%i).hsplit",i);
      pStr = PutToStr(pStr, pin_ahbso->arr[i].hsplit,16,chTmp);//  : (15 downto 0);  -- split completion
      sprintf_s(chTmp,"in_ahbso(%i).hcache",i);
      pStr = PutToStr(pStr, pin_ahbso->arr[i].hcache,1,chTmp);//                           -- cacheable
      sprintf_s(chTmp,"in_ahbso(%i).hirq",i);
      pStr = PutToStr(pStr, pin_ahbso->arr[i].hirq,AHB_IRQ_MAX,chTmp);//    : (AHB_IRQ_MAX-1 downto 0); -- interrupt bus
      for(int32 k=0; k<8; k++)
      {
        sprintf_s(chTmp,"in_ahbso(%i).hconfig(%i)",i,k);
        pStr = PutToStr(pStr, pin_ahbso->arr[i].hconfig.arr[k],32,chTmp);//   : ahb_config_type;      -- memory access reg.
      }
      sprintf_s(chTmp,"conv_integer:in_ahbso(%i).hindex",i);
      pStr = PutToStr(pStr, pin_ahbso->arr[i].hindex,4,chTmp);//    : integer range 0 to AHB_SLAVES_MAX-1;   -- diagnostic use only
    }

    // outputs:
    pStr = PutToStr(pStr, pch_mcio->data,32,"ch_mcio.data");//             : std_logic_vector(31 downto 0); // memory data
    pStr = PutToStr(pStr, pch_mcio->ready,1,"ch_mcio.ready");//            : std_ulogic;          // cycle ready
    pStr = PutToStr(pStr, pch_mcio->grant,1,"ch_mcio.grant");//            : std_ulogic;          // 
    pStr = PutToStr(pStr, pch_mcio->retry,1,"ch_mcio.retry");//            : std_ulogic;          // 
    pStr = PutToStr(pStr, pch_mcio->mexc,1,"ch_mcio.mexc");//             : std_ulogic;          // memory exception
    pStr = PutToStr(pStr, pch_mcio->cache,1,"ch_mcio.cache");//            : std_ulogic;    // cacheable data
    pStr = PutToStr(pStr, pch_mcio->par,4,"--ch_mcio.par");//              : std_logic_vector(3 downto 0);  // parity
    pStr = PutToStr(pStr, pch_mcio->scanen,1,"ch_mcio.scanen");//           : std_ulogic;
    //
    pStr = PutToStr(pStr, pch_mcdo->data,32,"ch_mcdo.data");//             : std_logic_vector(31 downto 0); // memory data
    pStr = PutToStr(pStr, pch_mcdo->ready,1,"ch_mcdo.ready");//            : std_ulogic;    // cycle ready
    pStr = PutToStr(pStr, pch_mcdo->grant,1,"ch_mcdo.grant");//            : std_ulogic;
    pStr = PutToStr(pStr, pch_mcdo->retry,1,"ch_mcdo.retry");//            : std_ulogic;
    pStr = PutToStr(pStr, pch_mcdo->mexc,1,"ch_mcdo.mexc");//             : std_ulogic;    // memory exception
    pStr = PutToStr(pStr, pch_mcdo->werr,1,"ch_mcdo.werr");//             : std_ulogic;    // memory write error
    pStr = PutToStr(pStr, pch_mcdo->cache,1,"ch_mcdo.cache");//            : std_ulogic;    // cacheable data
    pStr = PutToStr(pStr, pch_mcdo->ba,1,"ch_mcdo.ba");//               : std_ulogic;    // bus active (used for snooping)
    pStr = PutToStr(pStr, pch_mcdo->bg,1,"ch_mcdo.bg");//               : std_ulogic;    // bus grant  (used for snooping)
    pStr = PutToStr(pStr, pch_mcdo->par,4,"--ch_mcdo.par");//              : std_logic_vector(3 downto 0);  // parity
    pStr = PutToStr(pStr, pch_mcdo->scanen,1,"ch_mcdo.scanen");//           : std_ulogic;
    pStr = PutToStr(pStr, pch_mcdo->testen,1,"ch_mcdo.testen");//           : std_ulogic;
    //
    pStr = PutToStr(pStr, pch_mcmmo->data,32,"ch_mcmmo.data");//             : std_logic_vector(31 downto 0); -- memory data
    pStr = PutToStr(pStr, pch_mcmmo->ready,1,"ch_mcmmo.ready");//           : std_logic;         -- cycle ready
    pStr = PutToStr(pStr, pch_mcmmo->grant,1,"ch_mcmmo.grant");//           : std_logic;         -- 
    pStr = PutToStr(pStr, pch_mcmmo->retry,1,"ch_mcmmo.retry");//           : std_logic;         -- 
    pStr = PutToStr(pStr, pch_mcmmo->mexc,1,"ch_mcmmo.mexc");//           : std_logic;         -- memory exception
    pStr = PutToStr(pStr, pch_mcmmo->werr,1,"ch_mcmmo.werr");//           : std_logic;         -- memory write error
    pStr = PutToStr(pStr, pch_mcmmo->cache,1,"ch_mcmmo.cache");//           : std_logic;               -- cacheable data
    //
    pStr = PutToStr(pStr, pch_ahbo->hbusreq,1,"ch_ahbo.hbusreq");// ;                           -- bus request
    pStr = PutToStr(pStr, pch_ahbo->hlock,1,"ch_ahbo.hlock");// ;                           -- lock request
    pStr = PutToStr(pStr, pch_ahbo->htrans,2,"ch_ahbo.htrans");//[1:0]  : (1 downto 0);   -- transfer type
    pStr = PutToStr(pStr, pch_ahbo->haddr,32,"ch_ahbo.haddr");//[31:0] : (31 downto 0);  -- address bus (byte)
    pStr = PutToStr(pStr, pch_ahbo->hwrite,1,"ch_ahbo.hwrite");//  ;                           -- read/write
    pStr = PutToStr(pStr, pch_ahbo->hsize,3,"ch_ahbo.hsize");//[2:0] : (2 downto 0);   -- transfer size
    pStr = PutToStr(pStr, pch_ahbo->hburst,3,"ch_ahbo.hburst");//[2:0]  : (2 downto 0);   -- burst type
    pStr = PutToStr(pStr, pch_ahbo->hprot,4,"ch_ahbo.hprot");//[3:0] : (3 downto 0);   -- protection control
    pStr = PutToStr(pStr, pch_ahbo->hwdata,CFG_AHBDW,"ch_ahbo.hwdata");//[31:0]  : (AHBDW-1 downto 0);   -- write data bus
    pStr = PutToStr(pStr, pch_ahbo->hirq,AHB_IRQ_MAX,"ch_ahbo.hirq");//[31:0]    : (AHB_IRQ_MAX-1 downto 0); -- interrupt bus
    for(int32 k=0; k<8; k++)
    {
      sprintf_s(chTmp,"ch_ahbo.hconfig(%i)",k);
      pStr = PutToStr(pStr, pch_ahbo->hconfig.arr[k],32,chTmp);//   : ahb_config_type;      -- memory access reg.
    }
    pStr = PutToStr(pStr, pch_ahbo->hindex,4,"conv_integer:ch_ahbo.hindex");// integer range 0 to AHB_MASTERS_MAX-1;   -- diagnostic use only
    //


    PrintIndexStr();

    *posBench[TB_mmu_acache] << chStr << "\n";
  }
#ifdef DBG_mmu_acache
  tst_mmu_acache.ClkUpdate();
#endif
}

