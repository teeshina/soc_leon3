#include "lheaders.h"

extern leon3mp  topLeon3mp;

extern void ResetPutStr();
extern void PrintIndexStr();


//****************************************************************************
void dbg::leon3s_tb(SystemOnChipIO &io)
{
  // default output using SoC top level:
  ahb_mst_in_type     *pin_ahbi = &topLeon3mp.stCtrl2Mst;
  ahb_mst_out_type    *pch_ahbo = &topLeon3mp.stMst2Ctrl[AHB_MASTER_LEON3];
  ahb_slv_in_type     *pin_ahbsi = &topLeon3mp.stCtrl2Slv;
  ahb_slv_out_vector  *pin_ahbso = &topLeon3mp.in_slvo;
  l3_irq_in_type      *pin_irqi = &topLeon3mp.irqi;
  l3_irq_out_type     *pch_irqo = &topLeon3mp.irqo;
  l3_debug_in_type    *pin_dbgi = &topLeon3mp.dbgi;
  l3_debug_out_type   *pch_dbgo = &topLeon3mp.dbgo;

#ifdef DBG_leon3s

  if(io.inClk.eClock_z==SClock::CLK_POSEDGE)
  {
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
    in_ahbsi.hsel = rand()&MSK32(AHB_SLAVES_MAX-1,0);//  : (0 to AHB_SLAVES_MAX-1);     -- slave select
    in_ahbsi.haddr = (rand()<<17) + rand();// : (31 downto 0);  -- address bus (byte)
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
    //
    in_irqi.irl = rand()&0xf;//     : std_logic_vector(3 downto 0);
    in_irqi.rst = rand()&0x1;//     : std_ulogic;
    in_irqi.run = rand()&0x1;//     : std_ulogic;
    in_irqi.rstvec = rand()<<12;//  : std_logic_vector(31 downto 12);
    in_irqi.iact = rand()&0x1;//        : std_ulogic;
    in_irqi.index = 1;//   :
    //
    in_dbgi.dsuen = 1;//(tmp>>0)&0x1;//   : std_ulogic;  -- DSU enable
    in_dbgi.denable = rand()&0x1;// : std_ulogic;  -- diagnostic register access enable
    in_dbgi.dbreak = rand()&0x1;//  : std_ulogic;  -- debug break-in
    in_dbgi.step = rand()&0x1;//    : std_ulogic;  -- single step    
    in_dbgi.halt = rand()&0x1;//    : std_ulogic;  -- halt processor
    in_dbgi.reset = rand()&0x1;//   : std_ulogic;  -- reset processor
    in_dbgi.dwrite = rand()&0x1;//  : std_ulogic;  -- read/write
    in_dbgi.daddr = ((rand()<<15) + (rand()<<2))&0xFFFFFC;//   : std_logic_vector(23 downto 2); -- diagnostic address
    in_dbgi.ddata = (rand()<<17) + rand();//   : std_logic_vector(31 downto 0); -- diagnostic data
    in_dbgi.btrapa = rand()&0x1;//  : std_ulogic;	   -- break on IU trap
    in_dbgi.btrape = rand()&0x1;//  : std_ulogic;	-- break on IU trap
    in_dbgi.berror = rand()&0x1;//  : std_ulogic;	-- break on IU error mode
    in_dbgi.bwatch = rand()&0x1;//  : std_ulogic;	-- break on IU watchpoint
    in_dbgi.bsoft = rand()&0x1;//   : std_ulogic;	-- break on software breakpoint (TA 1)
    in_dbgi.tenable = rand()&0x1;// : std_ulogic;
    in_dbgi.timer = ((rand()<<17) + (rand()))&0x7FFFFFFF;//   :  std_logic_vector(30 downto 0);
  }

  // 
  tst_leon3s.Update( io.inClk,
                     io.inNRst,
                     in_ahbi,//   : in  ahb_mst_in_type;
                     ch_ahbo,//   : out ahb_mst_out_type;
                     in_ahbsi,//  : in  ahb_slv_in_type;
                     in_ahbso,//  : in  ahb_slv_out_vector;
                     in_irqi,//   : in  l3_irq_in_type;
                     ch_irqo,//   : out l3_irq_out_type;
                     in_dbgi,//   : in  l3_debug_in_type;
                     ch_dbgo//   : out l3_debug_out_type
                   );

  pin_ahbi = &in_ahbi;
  pch_ahbo = &ch_ahbo;
  pin_ahbsi = &in_ahbsi;
  pin_ahbso = &in_ahbso;
  pin_irqi = &in_irqi;
  pch_irqo = &ch_irqo;
  pin_dbgi = &in_dbgi;
  pch_dbgo = &ch_dbgo;
#endif

  // output file writting:
  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';

    ResetPutStr();
  
    // inputs:
    pStr = PutToStr(pStr, io.inNRst, 1, "inNRst");
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
    pStr = PutToStr(pStr, pin_ahbsi->hmbsel,AHB_MEM_ID_WIDTH,"in_ahbsi.hmbsel");//  : (0 to AHB_MEM_ID_WIDTH-1); -- memory bank select
    pStr = PutToStr(pStr, pin_ahbsi->hcache,1,"in_ahbsi.hcache");//  ;                -- cacheable
    pStr = PutToStr(pStr, pin_ahbsi->hirq,AHB_IRQ_MAX,"in_ahbsi.hirq");//  : (AHB_IRQ_MAX-1 downto 0); -- interrupt result bus
    pStr = PutToStr(pStr, pin_ahbsi->testen,1,"in_ahbsi.testen");//                        -- scan test enable
    pStr = PutToStr(pStr, pin_ahbsi->testrst,1,"in_ahbsi.testrst");//                       -- scan test reset
    pStr = PutToStr(pStr, pin_ahbsi->scanen,1,"in_ahbsi.scanen");//                        -- scan enable
    pStr = PutToStr(pStr, pin_ahbsi->testoen,1,"in_ahbsi.testoen");;//                       -- test output enable 
    //
    int32 i=0;
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hready,1,"in_ahbso(0).hready");//                           -- transfer done
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hresp,2,"in_ahbso(0).hresp");//[1:0] : (1 downto 0);   -- response type
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hrdata,CFG_AHBDW,"in_ahbso(0).hrdata");//(AHBDW-1 downto 0);   -- read data bus
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hsplit,16,"in_ahbso(0).hsplit");//  : (15 downto 0);  -- split completion
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hcache,1,"in_ahbso(0).hcache");//                           -- cacheable
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hirq,AHB_IRQ_MAX,"in_ahbso(0).hirq");//    : (AHB_IRQ_MAX-1 downto 0); -- interrupt bus
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hconfig.arr[0],32,"in_ahbso(0).hconfig(0)");//   : ahb_config_type;      -- memory access reg.
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hconfig.arr[1],32,"in_ahbso(0).hconfig(1)");
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hconfig.arr[2],32,"in_ahbso(0).hconfig(2)");
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hconfig.arr[3],32,"in_ahbso(0).hconfig(3)");
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hconfig.arr[4],32,"in_ahbso(0).hconfig(4)");
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hconfig.arr[5],32,"in_ahbso(0).hconfig(5)");
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hconfig.arr[6],32,"in_ahbso(0).hconfig(6)");
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hconfig.arr[7],32,"in_ahbso(0).hconfig(7)");
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hindex,4,"conv_integer(in_ahbso(0).hindex)",true);//    : integer range 0 to AHB_SLAVES_MAX-1;   -- diagnostic use only
    i=1;
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hready,1,"in_ahbso(1).hready");//                           -- transfer done
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hresp,2,"in_ahbso(1).hresp");//[1:0] : (1 downto 0);   -- response type
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hrdata,CFG_AHBDW,"in_ahbso(1).hrdata");//(AHBDW-1 downto 0);   -- read data bus
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hsplit,16,"in_ahbso(1).hsplit");//  : (15 downto 0);  -- split completion
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hcache,1,"in_ahbso(1).hcache");//                           -- cacheable
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hirq,AHB_IRQ_MAX,"in_ahbso(1).hirq");//    : (AHB_IRQ_MAX-1 downto 0); -- interrupt bus
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hconfig.arr[0],32,"in_ahbso(1).hconfig(0)");//   : ahb_config_type;      -- memory access reg.
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hconfig.arr[1],32,"in_ahbso(1).hconfig(1)");
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hconfig.arr[2],32,"in_ahbso(1).hconfig(2)");
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hconfig.arr[3],32,"in_ahbso(1).hconfig(3)");
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hconfig.arr[4],32,"in_ahbso(1).hconfig(4)");
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hconfig.arr[5],32,"in_ahbso(1).hconfig(5)");
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hconfig.arr[6],32,"in_ahbso(1).hconfig(6)");
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hconfig.arr[7],32,"in_ahbso(1).hconfig(7)");
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hindex,4,"conv_integer(in_ahbso(1).hindex)",true);//    : integer range 0 to AHB_SLAVES_MAX-1;   -- diagnostic use only
    i=2;
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hready,1,"in_ahbso(2).hready");//                           -- transfer done
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hresp,2,"in_ahbso(2).hresp");//[1:0] : (1 downto 0);   -- response type
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hrdata,CFG_AHBDW,"in_ahbso(2).hrdata");//(AHBDW-1 downto 0);   -- read data bus
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hsplit,16,"in_ahbso(2).hsplit");//  : (15 downto 0);  -- split completion
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hcache,1,"in_ahbso(2).hcache");//                           -- cacheable
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hirq,AHB_IRQ_MAX,"in_ahbso(2).hirq");//    : (AHB_IRQ_MAX-1 downto 0); -- interrupt bus
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hconfig.arr[0],32,"in_ahbso(2).hconfig(0)");//   : ahb_config_type;      -- memory access reg.
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hconfig.arr[1],32,"in_ahbso(2).hconfig(1)");
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hconfig.arr[2],32,"in_ahbso(2).hconfig(2)");
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hconfig.arr[3],32,"in_ahbso(2).hconfig(3)");
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hconfig.arr[4],32,"in_ahbso(2).hconfig(4)");
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hconfig.arr[5],32,"in_ahbso(2).hconfig(5)");
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hconfig.arr[6],32,"in_ahbso(2).hconfig(6)");
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hconfig.arr[7],32,"in_ahbso(2).hconfig(7)");
    pStr = PutToStr(pStr, pin_ahbso->arr[i].hindex,4,"conv_integer(in_ahbso(2).hindex)",true);//    : integer range 0 to AHB_SLAVES_MAX-1;   -- diagnostic use only
    //
    pStr = PutToStr(pStr, pin_irqi->irl,4,"in_irqi.irl");//     : std_logic_vector(3 downto 0);
    pStr = PutToStr(pStr, pin_irqi->rst,1,"in_irqi.rst");//     : std_ulogic;
    pStr = PutToStr(pStr, pin_irqi->run,1,"in_irqi.run");//     : std_ulogic;
    pStr = PutToStr(pStr, pin_irqi->rstvec>>12,20,"in_irqi.rstvec");//  : std_logic_vector(31 downto 12);
    pStr = PutToStr(pStr, pin_irqi->iact,1,"in_irqi.iact");//        : std_ulogic;
    pStr = PutToStr(pStr, pin_irqi->index,4,"in_irqi.index");//   :
    //
    pStr = PutToStr(pStr, pin_dbgi->dsuen,1,"in_dbgi.dsuen");//   : std_ulogic;  -- DSU enable
    pStr = PutToStr(pStr, pin_dbgi->denable,1,"in_dbgi.denable");// : std_ulogic;  -- diagnostic register access enable
    pStr = PutToStr(pStr, pin_dbgi->dbreak,1,"in_dbgi.dbreak");//  : std_ulogic;  -- debug break-in
    pStr = PutToStr(pStr, pin_dbgi->step,1,"in_dbgi.step");//    : std_ulogic;  -- single step    
    pStr = PutToStr(pStr, pin_dbgi->halt,1,"in_dbgi.halt");//    : std_ulogic;  -- halt processor
    pStr = PutToStr(pStr, pin_dbgi->reset,1,"in_dbgi.reset");//   : std_ulogic;  -- reset processor
    pStr = PutToStr(pStr, pin_dbgi->dwrite,1,"in_dbgi.dwrite");//  : std_ulogic;  -- read/write
    pStr = PutToStr(pStr, pin_dbgi->daddr>>2,22,"in_dbgi.daddr");//   : std_logic_vector(23 downto 2); -- diagnostic address
    pStr = PutToStr(pStr, pin_dbgi->ddata,32,"in_dbgi.ddata");//   : std_logic_vector(31 downto 0); -- diagnostic data
    pStr = PutToStr(pStr, pin_dbgi->btrapa,1,"in_dbgi.btrapa");//  : std_ulogic;	   -- break on IU trap
    pStr = PutToStr(pStr, pin_dbgi->btrape,1,"in_dbgi.btrape");//  : std_ulogic;	-- break on IU trap
    pStr = PutToStr(pStr, pin_dbgi->berror,1,"in_dbgi.berror");//  : std_ulogic;	-- break on IU error mode
    pStr = PutToStr(pStr, pin_dbgi->bwatch,1,"in_dbgi.bwatch");//  : std_ulogic;	-- break on IU watchpoint
    pStr = PutToStr(pStr, pin_dbgi->bsoft,1,"in_dbgi.bsoft");//   : std_ulogic;	-- break on software breakpoint (TA 1)
    pStr = PutToStr(pStr, pin_dbgi->tenable,1,"in_dbgi.tenable");// : std_ulogic;
    pStr = PutToStr(pStr, pin_dbgi->timer,31,"in_dbgi.timer");//   :  std_logic_vector(30 downto 0);


    // outputs:
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
    pStr = PutToStr(pStr, pch_ahbo->hconfig.arr[0],32,"ch_ahbo.hconfig(0)");//   : ahb_config_type;      -- memory access reg.
    pStr = PutToStr(pStr, pch_ahbo->hconfig.arr[1],32,"ch_ahbo.hconfig(1)");
    pStr = PutToStr(pStr, pch_ahbo->hconfig.arr[2],32,"ch_ahbo.hconfig(2)");
    pStr = PutToStr(pStr, pch_ahbo->hconfig.arr[3],32,"ch_ahbo.hconfig(3)");
    pStr = PutToStr(pStr, pch_ahbo->hconfig.arr[4],32,"ch_ahbo.hconfig(4)");
    pStr = PutToStr(pStr, pch_ahbo->hconfig.arr[5],32,"ch_ahbo.hconfig(5)");
    pStr = PutToStr(pStr, pch_ahbo->hconfig.arr[6],32,"ch_ahbo.hconfig(6)");
    pStr = PutToStr(pStr, pch_ahbo->hconfig.arr[7],32,"ch_ahbo.hconfig(7)");
    pStr = PutToStr(pStr, pch_ahbo->hindex,4,"conv_integer(ch_ahbo.hindex)",true);// integer range 0 to AHB_MASTERS_MAX-1;   -- diagnostic use only
    //
    pStr = PutToStr(pStr, pch_irqo->intack,1,"ch_irqo.intack");//  : std_ulogic;
    pStr = PutToStr(pStr, pch_irqo->irl,4,"ch_irqo.irl");//   : std_logic_vector(3 downto 0);
    pStr = PutToStr(pStr, pch_irqo->pwd,1,"ch_irqo.pwd");//         : std_ulogic;
    pStr = PutToStr(pStr, pch_irqo->fpen,1,"--ch_irqo.fpen");//        : std_ulogic;
    //
    pStr = PutToStr(pStr, pch_dbgo->data,32,"ch_dbgo.data");//    : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pch_dbgo->crdy,1,"ch_dbgo.crdy");//    : std_ulogic;
    pStr = PutToStr(pStr, pch_dbgo->dsu,1,"ch_dbgo.dsu");//     : std_ulogic;
    pStr = PutToStr(pStr, pch_dbgo->dsumode,1,"ch_dbgo.dsumode");// : std_ulogic;
    pStr = PutToStr(pStr, pch_dbgo->error,1,"ch_dbgo.error");//   : std_ulogic;
    pStr = PutToStr(pStr, pch_dbgo->halt,1,"ch_dbgo.halt");//    : std_ulogic;
    pStr = PutToStr(pStr, pch_dbgo->pwd,1,"ch_dbgo.pwd");//     : std_ulogic;
    pStr = PutToStr(pStr, pch_dbgo->idle,1,"ch_dbgo.idle");//    : std_ulogic;
    pStr = PutToStr(pStr, pch_dbgo->ipend,1,"ch_dbgo.ipend");//   : std_ulogic;
    pStr = PutToStr(pStr, pch_dbgo->icnt,1,"ch_dbgo.icnt");//    : std_ulogic;
    pStr = PutToStr(pStr, pch_dbgo->fcnt,1,"--ch_dbgo.fcnt");//    : std_ulogic;
    pStr = PutToStr(pStr, pch_dbgo->optype,6,"--ch_dbgo.optype");//  : std_logic_vector(5 downto 0); -- instruction type
    pStr = PutToStr(pStr, pch_dbgo->bpmiss,1,"--ch_dbgo.bpmiss");//  : std_ulogic;     -- branch predict miss
    pStr = PutToStr(pStr, pch_dbgo->istat.cmiss,1,"--ch_dbgo.istat.cmiss");//   : std_ulogic;			-- cache miss
    pStr = PutToStr(pStr, pch_dbgo->istat.tmiss,1,"--ch_dbgo.istat.tmiss");//   : std_ulogic;			-- TLB miss
    pStr = PutToStr(pStr, pch_dbgo->istat.chold,1,"--ch_dbgo.istat.chold");//   : std_ulogic;			-- cache hold
    pStr = PutToStr(pStr, pch_dbgo->istat.mhold,1,"--ch_dbgo.istat.mhold");//   : std_ulogic;			-- cache mmu hold
    pStr = PutToStr(pStr, pch_dbgo->dstat.cmiss,1,"--ch_dbgo.dstat.cmiss");//   : std_ulogic;			-- cache miss
    pStr = PutToStr(pStr, pch_dbgo->dstat.tmiss,1,"--ch_dbgo.dstat.tmiss");//   : std_ulogic;			-- TLB miss
    pStr = PutToStr(pStr, pch_dbgo->dstat.chold,1,"--ch_dbgo.dstat.chold");//   : std_ulogic;			-- cache hold
    pStr = PutToStr(pStr, pch_dbgo->dstat.mhold,1,"--ch_dbgo.dstat.mhold");//   : std_ulogic;			-- cache mmu hold
    pStr = PutToStr(pStr, pch_dbgo->wbhold,1,"--ch_dbgo.wbhold");//  : std_ulogic;     -- write buffer hold
    pStr = PutToStr(pStr, pch_dbgo->su,1,"--ch_dbgo.su");//      : std_ulogic;     -- supervisor state

    // internal:
                        
    PrintIndexStr();

    *posBench[TB_leon3s] << chStr << "\n";
  }

#ifdef DBG_leon3s
  // Clock update:
  tst_leon3s.ClkUpdate();
#endif
}

