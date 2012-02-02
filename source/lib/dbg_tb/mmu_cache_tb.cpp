#include "lheaders.h"

extern leon3mp  topLeon3mp;

extern void ResetPutStr();
extern void PrintIndexStr();

#ifdef DBG_mmu_cache

//****************************************************************************
void dbg::mmu_cache_tb(LeonDataIO &io)
{
  if(ioLib.inClk.eClock_z==SClock::CLK_POSEDGE)
  {
    in_fpuholdn = 1;
    in_hclken = rand()&0x1;
    //
    in_ici.rpc = (rand()<<17) + rand();//              : std_logic_vector(31 downto 0); -- raw address (npc)
    in_ici.fpc = (rand()<<17) + rand();//              : std_logic_vector(31 downto 0); -- latched address (fpc)
    in_ici.dpc = (rand()<<17) + rand();//              : std_logic_vector(31 downto 0); -- latched address (dpc)
    in_ici.rbranch = rand()&0x1;//          : std_ulogic;      -- Instruction branch
    in_ici.fbranch = rand()&0x1;//          : std_ulogic;      -- Instruction branch
    in_ici.inull   = rand()&0x1;//            : std_ulogic;      -- instruction nullify
    in_ici.su      = rand()&0x1;//               : std_ulogic;      -- super-user
    in_ici.flush   = rand()&0x1;//            : std_ulogic;      -- flush icache
    in_ici.flushl  = rand()&0x1;//           : std_ulogic;                        -- flush line
    in_ici.fline   = ((rand()<<17) + rand())&0xFFFFFFFC;//            : std_logic_vector(31 downto 3);     -- flush line offset
    in_ici.pnull   = rand()&0x1;//            : std_ulogic;
    //
    in_dci.asi = rand()&0xFF;//              : std_logic_vector(7 downto 0); 
    in_dci.maddress = (rand()<<17) + rand();//         : std_logic_vector(31 downto 0); 
    in_dci.eaddress = (rand()<<17) + rand();//         : std_logic_vector(31 downto 0); 
    in_dci.edata = (rand()<<17) + rand();//            : std_logic_vector(31 downto 0); 
    in_dci.size = rand()&0x3;//             : std_logic_vector(1 downto 0);
    in_dci.enaddr = rand()&0x1;//           : std_ulogic;
    in_dci.eenaddr = rand()&0x1;//          : std_ulogic;
    in_dci.nullify = rand()&0x1;//          : std_ulogic;
    in_dci.lock = rand()&0x1;//             : std_ulogic;
    in_dci.read = rand()&0x1;//             : std_ulogic;
    in_dci.write = rand()&0x1;//            : std_ulogic;
    in_dci.flush = rand()&0x1;//            : std_ulogic;
    in_dci.flushl = rand()&0x1;//           : std_ulogic;                        -- flush line  
    in_dci.dsuen = rand()&0x1;//            : std_ulogic;
    in_dci.msu = rand()&0x1;//              : std_ulogic;                   -- memory stage supervisor
    in_dci.esu = rand()&0x1;//              : std_ulogic;                   -- execution stage supervisor
    in_dci.intack = rand()&0x1;//           : std_ulogic;
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
    //
    in_cramo.icramo.tag.arr[0] = (rand()<<17) + rand();//     : cdatatype;
    in_cramo.icramo.tag.arr[1] = (rand()<<17) + rand();
    in_cramo.icramo.tag.arr[2] = (rand()<<17) + rand();
    in_cramo.icramo.tag.arr[3] = (rand()<<17) + rand();
    in_cramo.icramo.data.arr[0] = (rand()<<17) + rand();//      : cdatatype;
    in_cramo.icramo.data.arr[1] = (rand()<<17) + rand();
    in_cramo.icramo.data.arr[2] = (rand()<<17) + rand();
    in_cramo.icramo.data.arr[3] = (rand()<<17) + rand();
    in_cramo.icramo.ctx.arr[0] = (rand()<<17) + rand();//           : ctxdatatype;
    in_cramo.icramo.ctx.arr[1] = (rand()<<17) + rand();
    in_cramo.icramo.ctx.arr[2] = (rand()<<17) + rand();
    in_cramo.icramo.ctx.arr[3] = (rand()<<17) + rand();
    in_cramo.icramo.tpar.arr[0] = (rand()<<17) + rand();//          : cpartype;
    in_cramo.icramo.tpar.arr[1] = (rand()<<17) + rand();
    in_cramo.icramo.tpar.arr[2] = (rand()<<17) + rand();
    in_cramo.icramo.tpar.arr[3] = (rand()<<17) + rand();
    in_cramo.icramo.dpar.arr[0] = (rand()<<17) + rand();//          : cpartype;
    in_cramo.icramo.dpar.arr[1] = (rand()<<17) + rand();
    in_cramo.icramo.dpar.arr[2] = (rand()<<17) + rand();
    in_cramo.icramo.dpar.arr[3] = (rand()<<17) + rand();

    in_cramo.dcramo.tag.arr[0] = ((rand()<<17) + rand())&MSK32(IDBITS-1,0);//     : cdatatype;
    in_cramo.dcramo.tag.arr[1] = (rand()<<17) + rand();
    in_cramo.dcramo.tag.arr[2] = (rand()<<17) + rand();
    in_cramo.dcramo.tag.arr[3] = (rand()<<17) + rand();
    in_cramo.dcramo.data.arr[0] = (rand()<<17) + rand();;//    : cdatatype;
    in_cramo.dcramo.data.arr[1] = (rand()<<17) + rand();
    in_cramo.dcramo.data.arr[2] = (rand()<<17) + rand();
    in_cramo.dcramo.data.arr[3] = (rand()<<17) + rand();
    in_cramo.dcramo.stag.arr[0] = (rand()<<17) + rand();//      : cdatatype;
    in_cramo.dcramo.stag.arr[1] = (rand()<<17) + rand();
    in_cramo.dcramo.stag.arr[2] = (rand()<<17) + rand();
    in_cramo.dcramo.stag.arr[3] = (rand()<<17) + rand();
    in_cramo.dcramo.ctx.arr[0] = rand()&MSK32(M_CTX_SZ-1,0);//           : ctxdatatype;
    in_cramo.dcramo.ctx.arr[1] = rand()&MSK32(M_CTX_SZ-1,0);
    in_cramo.dcramo.ctx.arr[2] = rand()&MSK32(M_CTX_SZ-1,0);
    in_cramo.dcramo.ctx.arr[3] = rand()&MSK32(M_CTX_SZ-1,0);
    in_cramo.dcramo.tpar.arr[0] = rand()&0xf;//          : cpartype; // tag parity
    in_cramo.dcramo.tpar.arr[1] = rand()&0xf;
    in_cramo.dcramo.tpar.arr[2] = rand()&0xf;
    in_cramo.dcramo.tpar.arr[3] = rand()&0xf;
    in_cramo.dcramo.dpar.arr[0] = rand()&0xf;//          : cpartype;   // data parity
    in_cramo.dcramo.dpar.arr[1] = rand()&0xf;
    in_cramo.dcramo.dpar.arr[2] = rand()&0xf;
    in_cramo.dcramo.dpar.arr[3] = rand()&0xf;
    in_cramo.dcramo.spar = rand()&0xf;//          : std_logic_vector(3 downto 0);   // snoop tag parity
  }

  // 
  tst_mmu_cache.Update( io.inNRst,
                        io.inClk,
                        in_ici,//   : in  icache_in_type;
                        ch_ico,//   : out icache_out_type;
                        in_dci,//   : in  dcache_in_type;
                        ch_dco,//   : out dcache_out_type;
                        in_ahbi,//  : in  ahb_mst_in_type;
                        ch_ahbo,//  : out ahb_mst_out_type;
                        in_ahbsi,// : in  ahb_slv_in_type;
                        in_ahbso,//  : in  ahb_slv_out_vector;            
                        ch_crami,// : out cram_in_type;
                        in_cramo,// : in  cram_out_type;
                        in_fpuholdn,// : in  std_ulogic;
                        io.inClk,//
                        io.inClk,// : in std_ulogic;
                        in_hclken);// : in std_ulogic

  // output file writting:
  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';

    ResetPutStr();
  
    // inputs:
    pStr = PutToStr(pStr, io.inNRst, 1, "inNRst");
    //
    pStr = PutToStr(pStr, in_fpuholdn, 1, "in_fpuholdn");
    pStr = PutToStr(pStr, in_ici.rpc,32,"in_ici.rpc");//              : std_logic_vector(31 downto 0); -- raw address (npc)
    pStr = PutToStr(pStr, in_ici.fpc,32,"in_ici.fpc");//              : std_logic_vector(31 downto 0); -- latched address (fpc)
    pStr = PutToStr(pStr, in_ici.dpc,32,"in_ici.dpc");//              : std_logic_vector(31 downto 0); -- latched address (dpc)
    pStr = PutToStr(pStr, in_ici.rbranch,1,"in_ici.rbranch");//          : std_ulogic;      -- Instruction branch
    pStr = PutToStr(pStr, in_ici.fbranch,1,"in_ici.fbranch");//          : std_ulogic;      -- Instruction branch
    pStr = PutToStr(pStr, in_ici.inull,1,"in_ici.inull");//            : std_ulogic;      -- instruction nullify
    pStr = PutToStr(pStr, in_ici.su,1,"in_ici.su");//               : std_ulogic;      -- super-user
    pStr = PutToStr(pStr, in_ici.flush,1,"in_ici.flush");//            : std_ulogic;      -- flush icache
    pStr = PutToStr(pStr, in_ici.flushl,1,"in_ici.flushl");//           : std_ulogic;                        -- flush line
    pStr = PutToStr(pStr, (in_ici.fline>>3),29,"in_ici.fline");//            : std_logic_vector(31 downto 3);     -- flush line offset
    pStr = PutToStr(pStr, in_ici.pnull,1,"in_ici.pnull");//            : std_ulogic;
    //
    pStr = PutToStr(pStr, in_dci.asi,8,"in_dci.asi");//              : std_logic_vector(7 downto 0); 
    pStr = PutToStr(pStr, in_dci.maddress,32,"in_dci.maddress");//         : std_logic_vector(31 downto 0); 
    pStr = PutToStr(pStr, in_dci.eaddress,32,"in_dci.eaddress");//         : std_logic_vector(31 downto 0); 
    pStr = PutToStr(pStr, in_dci.edata,32,"in_dci.edata");//            : std_logic_vector(31 downto 0); 
    pStr = PutToStr(pStr, in_dci.size,2,"in_dci.size");//             : std_logic_vector(1 downto 0);
    pStr = PutToStr(pStr, in_dci.enaddr,1,"in_dci.enaddr");//           : std_ulogic;
    pStr = PutToStr(pStr, in_dci.eenaddr,1,"in_dci.eenaddr");//          : std_ulogic;
    pStr = PutToStr(pStr, in_dci.nullify,1,"in_dci.nullify");//          : std_ulogic;
    pStr = PutToStr(pStr, in_dci.lock,1,"in_dci.lock");//             : std_ulogic;
    pStr = PutToStr(pStr, in_dci.read,1,"in_dci.read");//             : std_ulogic;
    pStr = PutToStr(pStr, in_dci.write,1,"in_dci.write");//            : std_ulogic;
    pStr = PutToStr(pStr, in_dci.flush,1,"in_dci.flush");//            : std_ulogic;
    pStr = PutToStr(pStr, in_dci.flushl,1,"in_dci.flushl");//           : std_ulogic;                        -- flush line  
    pStr = PutToStr(pStr, in_dci.dsuen,1,"in_dci.dsuen");//            : std_ulogic;
    pStr = PutToStr(pStr, in_dci.msu,1,"in_dci.msu");//              : std_ulogic;                   -- memory stage supervisor
    pStr = PutToStr(pStr, in_dci.esu,1,"in_dci.esu");//              : std_ulogic;                   -- execution stage supervisor
    pStr = PutToStr(pStr, in_dci.intack,1,"in_dci.intack");//           : std_ulogic;
    //
    pStr = PutToStr(pStr, in_ahbi.hgrant,AHB_MASTERS_MAX,"in_ahbi.hgrant",true);//[0:15]  : (0 to AHB_MASTERS_MAX-1);     -- bus grant
    pStr = PutToStr(pStr, in_ahbi.hready,1,"in_ahbi.hready");//  ;                           -- transfer done
    pStr = PutToStr(pStr, in_ahbi.hresp,2,"in_ahbi.hresp");//[1:0] : (1 downto 0);   -- response type
    pStr = PutToStr(pStr, in_ahbi.hrdata,CFG_AHBDW,"in_ahbi.hrdata");//[31:0]  : (AHBDW-1 downto 0);   -- read data bus
    pStr = PutToStr(pStr, in_ahbi.hcache,1,"in_ahbi.hcache");//  ;                           -- cacheable
    pStr = PutToStr(pStr, in_ahbi.hirq,AHB_IRQ_MAX,"in_ahbi.hirq");//[31:0]    : (AHB_IRQ_MAX-1 downto 0); -- interrupt result bus
    pStr = PutToStr(pStr, in_ahbi.testen,1,"in_ahbi.testen");//  ;                           -- scan test enable
    pStr = PutToStr(pStr, in_ahbi.testrst,1,"in_ahbi.testrst");// ;                           -- scan test reset
    pStr = PutToStr(pStr, in_ahbi.scanen,1,"in_ahbi.scanen");//  ;                           -- scan enable
    pStr = PutToStr(pStr, in_ahbi.testoen,1,"in_ahbi.testoen");//   ;                           -- test output enable 
    //
    int32 i=0;
    pStr = PutToStr(pStr, in_ahbso.arr[i].hready,1,"in_ahbso(0).hready");//                           -- transfer done
    pStr = PutToStr(pStr, in_ahbso.arr[i].hresp,2,"in_ahbso(0).hresp");//[1:0] : (1 downto 0);   -- response type
    pStr = PutToStr(pStr, in_ahbso.arr[i].hrdata,CFG_AHBDW,"in_ahbso(0).hrdata");//(AHBDW-1 downto 0);   -- read data bus
    pStr = PutToStr(pStr, in_ahbso.arr[i].hsplit,16,"in_ahbso(0).hsplit");//  : (15 downto 0);  -- split completion
    pStr = PutToStr(pStr, in_ahbso.arr[i].hcache,1,"in_ahbso(0).hcache");//                           -- cacheable
    pStr = PutToStr(pStr, in_ahbso.arr[i].hirq,AHB_IRQ_MAX,"in_ahbso(0).hirq");//    : (AHB_IRQ_MAX-1 downto 0); -- interrupt bus
    pStr = PutToStr(pStr, in_ahbso.arr[i].hconfig.arr[0],32,"in_ahbso(0).hconfig(0)");//   : ahb_config_type;      -- memory access reg.
    pStr = PutToStr(pStr, in_ahbso.arr[i].hconfig.arr[1],32,"in_ahbso(0).hconfig(1)");
    pStr = PutToStr(pStr, in_ahbso.arr[i].hconfig.arr[2],32,"in_ahbso(0).hconfig(2)");
    pStr = PutToStr(pStr, in_ahbso.arr[i].hconfig.arr[3],32,"in_ahbso(0).hconfig(3)");
    pStr = PutToStr(pStr, in_ahbso.arr[i].hconfig.arr[4],32,"in_ahbso(0).hconfig(4)");
    pStr = PutToStr(pStr, in_ahbso.arr[i].hconfig.arr[5],32,"in_ahbso(0).hconfig(5)");
    pStr = PutToStr(pStr, in_ahbso.arr[i].hconfig.arr[6],32,"in_ahbso(0).hconfig(6)");
    pStr = PutToStr(pStr, in_ahbso.arr[i].hconfig.arr[7],32,"in_ahbso(0).hconfig(7)");
    pStr = PutToStr(pStr, in_ahbso.arr[i].hindex,4,"in_ahbso(0).hindex",true);//    : integer range 0 to AHB_SLAVES_MAX-1;   -- diagnostic use only
    i=1;
    pStr = PutToStr(pStr, in_ahbso.arr[i].hready,1,"in_ahbso(1).hready");//                           -- transfer done
    pStr = PutToStr(pStr, in_ahbso.arr[i].hresp,2,"in_ahbso(1).hresp");//[1:0] : (1 downto 0);   -- response type
    pStr = PutToStr(pStr, in_ahbso.arr[i].hrdata,CFG_AHBDW,"in_ahbso(1).hrdata");//(AHBDW-1 downto 0);   -- read data bus
    pStr = PutToStr(pStr, in_ahbso.arr[i].hsplit,16,"in_ahbso(1).hsplit");//  : (15 downto 0);  -- split completion
    pStr = PutToStr(pStr, in_ahbso.arr[i].hcache,1,"in_ahbso(1).hcache");//                           -- cacheable
    pStr = PutToStr(pStr, in_ahbso.arr[i].hirq,AHB_IRQ_MAX,"in_ahbso(1).hirq");//    : (AHB_IRQ_MAX-1 downto 0); -- interrupt bus
    pStr = PutToStr(pStr, in_ahbso.arr[i].hconfig.arr[0],32,"in_ahbso(1).hconfig(0)");//   : ahb_config_type;      -- memory access reg.
    pStr = PutToStr(pStr, in_ahbso.arr[i].hconfig.arr[1],32,"in_ahbso(1).hconfig(1)");
    pStr = PutToStr(pStr, in_ahbso.arr[i].hconfig.arr[2],32,"in_ahbso(1).hconfig(2)");
    pStr = PutToStr(pStr, in_ahbso.arr[i].hconfig.arr[3],32,"in_ahbso(1).hconfig(3)");
    pStr = PutToStr(pStr, in_ahbso.arr[i].hconfig.arr[4],32,"in_ahbso(1).hconfig(4)");
    pStr = PutToStr(pStr, in_ahbso.arr[i].hconfig.arr[5],32,"in_ahbso(1).hconfig(5)");
    pStr = PutToStr(pStr, in_ahbso.arr[i].hconfig.arr[6],32,"in_ahbso(1).hconfig(6)");
    pStr = PutToStr(pStr, in_ahbso.arr[i].hconfig.arr[7],32,"in_ahbso(1).hconfig(7)");
    pStr = PutToStr(pStr, in_ahbso.arr[i].hindex,4,"in_ahbso(1).hindex",true);//    : integer range 0 to AHB_SLAVES_MAX-1;   -- diagnostic use only
    i=2;
    pStr = PutToStr(pStr, in_ahbso.arr[i].hready,1,"in_ahbso(2).hready");//                           -- transfer done
    pStr = PutToStr(pStr, in_ahbso.arr[i].hresp,2,"in_ahbso(2).hresp");//[1:0] : (1 downto 0);   -- response type
    pStr = PutToStr(pStr, in_ahbso.arr[i].hrdata,CFG_AHBDW,"in_ahbso(2).hrdata");//(AHBDW-1 downto 0);   -- read data bus
    pStr = PutToStr(pStr, in_ahbso.arr[i].hsplit,16,"in_ahbso(2).hsplit");//  : (15 downto 0);  -- split completion
    pStr = PutToStr(pStr, in_ahbso.arr[i].hcache,1,"in_ahbso(2).hcache");//                           -- cacheable
    pStr = PutToStr(pStr, in_ahbso.arr[i].hirq,AHB_IRQ_MAX,"in_ahbso(2).hirq");//    : (AHB_IRQ_MAX-1 downto 0); -- interrupt bus
    pStr = PutToStr(pStr, in_ahbso.arr[i].hconfig.arr[0],32,"in_ahbso(2).hconfig(0)");//   : ahb_config_type;      -- memory access reg.
    pStr = PutToStr(pStr, in_ahbso.arr[i].hconfig.arr[1],32,"in_ahbso(2).hconfig(1)");
    pStr = PutToStr(pStr, in_ahbso.arr[i].hconfig.arr[2],32,"in_ahbso(2).hconfig(2)");
    pStr = PutToStr(pStr, in_ahbso.arr[i].hconfig.arr[3],32,"in_ahbso(2).hconfig(3)");
    pStr = PutToStr(pStr, in_ahbso.arr[i].hconfig.arr[4],32,"in_ahbso(2).hconfig(4)");
    pStr = PutToStr(pStr, in_ahbso.arr[i].hconfig.arr[5],32,"in_ahbso(2).hconfig(5)");
    pStr = PutToStr(pStr, in_ahbso.arr[i].hconfig.arr[6],32,"in_ahbso(2).hconfig(6)");
    pStr = PutToStr(pStr, in_ahbso.arr[i].hconfig.arr[7],32,"in_ahbso(2).hconfig(7)");
    pStr = PutToStr(pStr, in_ahbso.arr[i].hindex,4,"in_ahbso(2).hindex",true);//    : integer range 0 to AHB_SLAVES_MAX-1;   -- diagnostic use only
    //
    pStr = PutToStr(pStr, in_cramo.icramo.tag.arr[0],32,"in_cramo.icramo.tag(0)");//     : cdatatype;
    pStr = PutToStr(pStr, in_cramo.icramo.tag.arr[1],32,"in_cramo.icramo.tag(1)");
    pStr = PutToStr(pStr, in_cramo.icramo.tag.arr[2],32,"in_cramo.icramo.tag(2)");
    pStr = PutToStr(pStr, in_cramo.icramo.tag.arr[3],32,"in_cramo.icramo.tag(3)");
    pStr = PutToStr(pStr, in_cramo.icramo.data.arr[0],32,"in_cramo.icramo.data(0)");//      : cdatatype;
    pStr = PutToStr(pStr, in_cramo.icramo.data.arr[1],32,"in_cramo.icramo.data(1)");
    pStr = PutToStr(pStr, in_cramo.icramo.data.arr[2],32,"in_cramo.icramo.data(2)");
    pStr = PutToStr(pStr, in_cramo.icramo.data.arr[3],32,"in_cramo.icramo.data(3)");
    pStr = PutToStr(pStr, in_cramo.icramo.ctx.arr[0],M_CTX_SZ,"in_cramo.icramo.ctx(0)");//           : ctxdatatype;
    pStr = PutToStr(pStr, in_cramo.icramo.ctx.arr[1],M_CTX_SZ,"in_cramo.icramo.ctx(1)");
    pStr = PutToStr(pStr, in_cramo.icramo.ctx.arr[2],M_CTX_SZ,"in_cramo.icramo.ctx(2)");
    pStr = PutToStr(pStr, in_cramo.icramo.ctx.arr[3],M_CTX_SZ,"in_cramo.icramo.ctx(3)");
    pStr = PutToStr(pStr, in_cramo.icramo.tpar.arr[0],4,"in_cramo.icramo.tpar(0)");//          : cpartype;
    pStr = PutToStr(pStr, in_cramo.icramo.tpar.arr[1],4,"in_cramo.icramo.tpar(1)");
    pStr = PutToStr(pStr, in_cramo.icramo.tpar.arr[2],4,"in_cramo.icramo.tpar(2)");
    pStr = PutToStr(pStr, in_cramo.icramo.tpar.arr[3],4,"in_cramo.icramo.tpar(3)");
    pStr = PutToStr(pStr, in_cramo.icramo.dpar.arr[0],4,"in_cramo.icramo.dpar(0)");//          : cpartype;
    pStr = PutToStr(pStr, in_cramo.icramo.dpar.arr[1],4,"in_cramo.icramo.dpar(0)");
    pStr = PutToStr(pStr, in_cramo.icramo.dpar.arr[2],4,"in_cramo.icramo.dpar(0)");
    pStr = PutToStr(pStr, in_cramo.icramo.dpar.arr[3],4,"in_cramo.icramo.dpar(0)");
    //
    pStr = PutToStr(pStr, in_cramo.dcramo.tag.arr[0],IDBITS,"in_cramo.dcramo.tag(0)");
    pStr = PutToStr(pStr, in_cramo.dcramo.tag.arr[1],IDBITS,"in_cramo.dcramo.tag(1)");
    pStr = PutToStr(pStr, in_cramo.dcramo.tag.arr[2],IDBITS,"in_cramo.dcramo.tag(2)");
    pStr = PutToStr(pStr, in_cramo.dcramo.tag.arr[3],IDBITS,"in_cramo.dcramo.tag(3)");
    pStr = PutToStr(pStr, in_cramo.dcramo.data.arr[0],IDBITS,"in_cramo.dcramo.data(0)");
    pStr = PutToStr(pStr, in_cramo.dcramo.data.arr[1],IDBITS,"in_cramo.dcramo.data(1)");
    pStr = PutToStr(pStr, in_cramo.dcramo.data.arr[2],IDBITS,"in_cramo.dcramo.data(2)");
    pStr = PutToStr(pStr, in_cramo.dcramo.data.arr[3],IDBITS,"in_cramo.dcramo.data(3)");
    pStr = PutToStr(pStr, in_cramo.dcramo.stag.arr[0],IDBITS,"in_cramo.dcramo.stag(0)");
    pStr = PutToStr(pStr, in_cramo.dcramo.stag.arr[1],IDBITS,"in_cramo.dcramo.stag(1)");
    pStr = PutToStr(pStr, in_cramo.dcramo.stag.arr[2],IDBITS,"in_cramo.dcramo.stag(2)");
    pStr = PutToStr(pStr, in_cramo.dcramo.stag.arr[3],IDBITS,"in_cramo.dcramo.stag(3)");
    pStr = PutToStr(pStr, in_cramo.dcramo.ctx.arr[0],M_CTX_SZ,"in_cramo.dcramo.ctx(0)");
    pStr = PutToStr(pStr, in_cramo.dcramo.ctx.arr[1],M_CTX_SZ,"in_cramo.dcramo.ctx(1)");
    pStr = PutToStr(pStr, in_cramo.dcramo.ctx.arr[2],M_CTX_SZ,"in_cramo.dcramo.ctx(2)");
    pStr = PutToStr(pStr, in_cramo.dcramo.ctx.arr[3],M_CTX_SZ,"in_cramo.dcramo.ctx(3)");
    pStr = PutToStr(pStr, in_cramo.dcramo.tpar.arr[0],4,"in_cramo.dcramo.tpar(0)");
    pStr = PutToStr(pStr, in_cramo.dcramo.tpar.arr[1],4,"in_cramo.dcramo.tpar(1)");
    pStr = PutToStr(pStr, in_cramo.dcramo.tpar.arr[2],4,"in_cramo.dcramo.tpar(2)");
    pStr = PutToStr(pStr, in_cramo.dcramo.tpar.arr[3],4,"in_cramo.dcramo.tpar(3)");
    pStr = PutToStr(pStr, in_cramo.dcramo.dpar.arr[0],4,"in_cramo.dcramo.dpar(0)");
    pStr = PutToStr(pStr, in_cramo.dcramo.dpar.arr[1],4,"in_cramo.dcramo.dpar(0)");
    pStr = PutToStr(pStr, in_cramo.dcramo.dpar.arr[2],4,"in_cramo.dcramo.dpar(0)");
    pStr = PutToStr(pStr, in_cramo.dcramo.dpar.arr[3],4,"in_cramo.dcramo.dpar(0)");
    pStr = PutToStr(pStr, in_cramo.dcramo.spar,4,"in_cramo.dcramo.spar");


  
    // Outputs:
    pStr = PutToStr(pStr, ch_ico.data.arr[0],IDBITS,"ch_ico.data(0)");
    pStr = PutToStr(pStr, ch_ico.data.arr[1],IDBITS,"ch_ico.data(1)");
    pStr = PutToStr(pStr, ch_ico.data.arr[2],IDBITS,"ch_ico.data(2)");
    pStr = PutToStr(pStr, ch_ico.data.arr[3],IDBITS,"ch_ico.data(3)");
    pStr = PutToStr(pStr, ch_ico.set,2,"ch_ico.set");//              : std_logic_vector(1 downto 0);
    pStr = PutToStr(pStr, ch_ico.mexc,1,"ch_ico.mexc");//             : std_ulogic;
    pStr = PutToStr(pStr, ch_ico.hold,1,"ch_ico.hold");//             : std_ulogic;
    pStr = PutToStr(pStr, ch_ico.flush,1,"ch_ico.flush");//            : std_ulogic;			-- flush in progress
    pStr = PutToStr(pStr, ch_ico.diagrdy,1,"ch_ico.diagrdy");//          : std_ulogic;			-- diagnostic access ready
    pStr = PutToStr(pStr, ch_ico.diagdata,IDBITS,"ch_ico.diagdata");//         : std_logic_vector(IDBITS-1 downto 0);-- diagnostic data
    pStr = PutToStr(pStr, ch_ico.mds,1,"ch_ico.mds");//              : std_ulogic;			-- memory data strobe
    pStr = PutToStr(pStr, ch_ico.cfg,32,"ch_ico.cfg");//              : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, ch_ico.idle,1,"ch_ico.idle");//             : std_ulogic;			-- idle mode
    pStr = PutToStr(pStr, ch_ico.cstat.cmiss,1,"--ch_ico.cstat.cmiss");//            : l3_cstat_type;
    pStr = PutToStr(pStr, ch_ico.cstat.tmiss,1,"--ch_ico.cstat.tmiss");//   : std_ulogic;			-- TLB miss
    pStr = PutToStr(pStr, ch_ico.cstat.chold,1,"--ch_ico.cstat.chold");//   : std_ulogic;			-- cache hold
    pStr = PutToStr(pStr, ch_ico.cstat.mhold,1,"--ch_ico.cstat.mhold");//   : std_ulogic;			-- cache mmu hold
    //
    pStr = PutToStr(pStr, ch_dco.data.arr[0],IDBITS,"ch_dco.data(0)");//             : cdatatype;
    pStr = PutToStr(pStr, ch_dco.data.arr[1],IDBITS,"ch_dco.data(1)");
    pStr = PutToStr(pStr, ch_dco.data.arr[2],IDBITS,"ch_dco.data(2)");
    pStr = PutToStr(pStr, ch_dco.data.arr[3],IDBITS,"ch_dco.data(3)");
    pStr = PutToStr(pStr, ch_dco.set,2,"ch_dco.set");//              : std_logic_vector(1 downto 0);
    pStr = PutToStr(pStr, ch_dco.mexc,1,"ch_dco.mexc");//             : std_ulogic;
    pStr = PutToStr(pStr, ch_dco.hold,1,"ch_dco.hold");//             : std_ulogic;
    pStr = PutToStr(pStr, ch_dco.mds,1,"ch_dco.mds");//              : std_ulogic;
    pStr = PutToStr(pStr, ch_dco.werr,1,"ch_dco.werr");//             : std_ulogic;	
    pStr = PutToStr(pStr, ch_dco.icdiag.addr,32,"ch_dco.icdiag.addr");//             : std_logic_vector(31 downto 0); -- memory stage address
    pStr = PutToStr(pStr, ch_dco.icdiag.enable,1,"ch_dco.icdiag.enable");//           : std_ulogic;
    pStr = PutToStr(pStr, ch_dco.icdiag.read,1,"ch_dco.icdiag.read");//             : std_ulogic;
    pStr = PutToStr(pStr, ch_dco.icdiag.tag,1,"ch_dco.icdiag.tag");//              : std_ulogic;
    pStr = PutToStr(pStr, ch_dco.icdiag.ctx,1,"ch_dco.icdiag.ctx");//              : std_ulogic;
    pStr = PutToStr(pStr, ch_dco.icdiag.flush,1,"ch_dco.icdiag.flush");//            : std_ulogic;
    pStr = PutToStr(pStr, ch_dco.icdiag.ilramen,1,"ch_dco.icdiag.ilramen");//          : std_ulogic;
    pStr = PutToStr(pStr, ch_dco.icdiag.cctrl.burst,1,"ch_dco.icdiag.cctrl.burst");//  : std_ulogic;				-- icache burst enable
    pStr = PutToStr(pStr, ch_dco.icdiag.cctrl.dfrz,1,"ch_dco.icdiag.cctrl.dfrz");//   : std_ulogic;				-- dcache freeze enable
    pStr = PutToStr(pStr, ch_dco.icdiag.cctrl.ifrz,1,"ch_dco.icdiag.cctrl.ifrz");//   : std_ulogic;				-- icache freeze enable
    pStr = PutToStr(pStr, ch_dco.icdiag.cctrl.dsnoop,1,"ch_dco.icdiag.cctrl.dsnoop");// : std_ulogic;				-- data cache snooping
    pStr = PutToStr(pStr, ch_dco.icdiag.cctrl.dcs,2,"ch_dco.icdiag.cctrl.dcs");//    : std_logic_vector(1 downto 0);	-- dcache state
    pStr = PutToStr(pStr, ch_dco.icdiag.cctrl.ics,2,"ch_dco.icdiag.cctrl.ics");//    : std_logic_vector(1 downto 0);	-- icache state
    pStr = PutToStr(pStr, ch_dco.icdiag.pflush,1,"ch_dco.icdiag.pflush");//           : std_ulogic;
    pStr = PutToStr(pStr, ch_dco.icdiag.pflushaddr,VA_I_U-VA_I_D+1,"ch_dco.icdiag.pflushaddr");//       : std_logic_vector(VA_I_U downto VA_I_D); 
    pStr = PutToStr(pStr, ch_dco.icdiag.pflushtyp,1,"ch_dco.icdiag.pflushtyp");//        : std_ulogic;
    pStr = PutToStr(pStr, ch_dco.icdiag.ilock,4,"--ch_dco.icdiag.ilock");//            : std_logic_vector(0 to 3); 
    pStr = PutToStr(pStr, ch_dco.icdiag.scanen,1,"ch_dco.icdiag.scanen");//           : std_ulogic;
    pStr = PutToStr(pStr, ch_dco.cache,1,"--ch_dco.cache");//            : std_ulogic;
    pStr = PutToStr(pStr, ch_dco.idle,1,"ch_dco.idle");//             : std_ulogic;			-- idle mode
    pStr = PutToStr(pStr, ch_dco.scanen,1,"ch_dco.scanen");//           : std_ulogic;
    pStr = PutToStr(pStr, ch_dco.testen,1,"ch_dco.testen");//           : std_ulogic;
    //
    pStr = PutToStr(pStr, ch_ahbo.hbusreq,1,"ch_ahbo.hbusreq");// ;                           -- bus request
    pStr = PutToStr(pStr, ch_ahbo.hlock,1,"ch_ahbo.hlock");// ;                           -- lock request
    pStr = PutToStr(pStr, ch_ahbo.htrans,2,"ch_ahbo.htrans");//[1:0]  : (1 downto 0);   -- transfer type
    pStr = PutToStr(pStr, ch_ahbo.haddr,32,"ch_ahbo.haddr");//[31:0] : (31 downto 0);  -- address bus (byte)
    pStr = PutToStr(pStr, ch_ahbo.hwrite,1,"ch_ahbo.hwrite");//  ;                           -- read/write
    pStr = PutToStr(pStr, ch_ahbo.hsize,3,"ch_ahbo.hsize");//[2:0] : (2 downto 0);   -- transfer size
    pStr = PutToStr(pStr, ch_ahbo.hburst,3,"ch_ahbo.hburst");//[2:0]  : (2 downto 0);   -- burst type
    pStr = PutToStr(pStr, ch_ahbo.hprot,4,"ch_ahbo.hprot");//[3:0] : (3 downto 0);   -- protection control
    pStr = PutToStr(pStr, ch_ahbo.hwdata,CFG_AHBDW,"ch_ahbo.hwdata");//[31:0]  : (AHBDW-1 downto 0);   -- write data bus
    pStr = PutToStr(pStr, ch_ahbo.hirq,AHB_IRQ_MAX,"ch_ahbo.hirq");//[31:0]    : (AHB_IRQ_MAX-1 downto 0); -- interrupt bus
    pStr = PutToStr(pStr, ch_ahbo.hconfig.arr[0],32,"ch_ahbo.hconfig(0)");//   : ahb_config_type;      -- memory access reg.
    pStr = PutToStr(pStr, ch_ahbo.hconfig.arr[1],32,"ch_ahbo.hconfig(1)");
    pStr = PutToStr(pStr, ch_ahbo.hconfig.arr[2],32,"ch_ahbo.hconfig(2)");
    pStr = PutToStr(pStr, ch_ahbo.hconfig.arr[3],32,"ch_ahbo.hconfig(3)");
    pStr = PutToStr(pStr, ch_ahbo.hconfig.arr[4],32,"ch_ahbo.hconfig(4)");
    pStr = PutToStr(pStr, ch_ahbo.hconfig.arr[5],32,"ch_ahbo.hconfig(5)");
    pStr = PutToStr(pStr, ch_ahbo.hconfig.arr[6],32,"ch_ahbo.hconfig(6)");
    pStr = PutToStr(pStr, ch_ahbo.hconfig.arr[7],32,"ch_ahbo.hconfig(7)");
    pStr = PutToStr(pStr, ch_ahbo.hindex,4,"ch_ahbo.hindex",true);// integer range 0 to AHB_MASTERS_MAX-1;   -- diagnostic use only
    //
    pStr = PutToStr(pStr, ch_crami.icramin.address,20,"ch_crami.icramin.address");// : std_logic_vector(19 downto 0);
    pStr = PutToStr(pStr, ch_crami.icramin.tag.arr[0],IDBITS,"ch_crami.icramin.tag(0)");//     : cdatatype;
    pStr = PutToStr(pStr, ch_crami.icramin.tag.arr[1],IDBITS,"ch_crami.icramin.tag(1)");//     : cdatatype;
    pStr = PutToStr(pStr, ch_crami.icramin.tag.arr[2],IDBITS,"--ch_crami.icramin.tag(2)");//     : cdatatype;
    pStr = PutToStr(pStr, ch_crami.icramin.tag.arr[3],IDBITS,"--ch_crami.icramin.tag(3)");//     : cdatatype;
    pStr = PutToStr(pStr, ch_crami.icramin.twrite,4,"ch_crami.icramin.twrite",true);//  : std_logic_vector(0 to 3);
    pStr = PutToStr(pStr, ch_crami.icramin.tenable,1,"ch_crami.icramin.tenable");// : std_ulogic;
    pStr = PutToStr(pStr, ch_crami.icramin.flush,1,"ch_crami.icramin.flush");//   : std_ulogic;
    pStr = PutToStr(pStr, ch_crami.icramin.data,32,"ch_crami.icramin.data");//    : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, ch_crami.icramin.denable,1,"ch_crami.icramin.denable");// : std_ulogic;
    pStr = PutToStr(pStr, ch_crami.icramin.dwrite,4,"ch_crami.icramin.dwrite",true);//  : std_logic_vector(0 to 3);
    pStr = PutToStr(pStr, ch_crami.icramin.ldramin.enable,1,"ch_crami.icramin.ldramin.enable");//        : std_ulogic;                       
    pStr = PutToStr(pStr, ch_crami.icramin.ldramin.read,1,"ch_crami.icramin.ldramin.read");//          : std_ulogic;                         
    pStr = PutToStr(pStr, ch_crami.icramin.ldramin.write,1,"ch_crami.icramin.ldramin.write");//         : std_ulogic;                       
    pStr = PutToStr(pStr, ch_crami.icramin.ctx,M_CTX_SZ,"ch_crami.icramin.ctx");//           : std_logic_vector(M_CTX_SZ-1 downto 0);
    pStr = PutToStr(pStr, ch_crami.icramin.tpar.arr[0],4,"--ch_crami.icramin.tpar(0)");//          : cpartype;
    pStr = PutToStr(pStr, ch_crami.icramin.tpar.arr[1],4,"--ch_crami.icramin.tpar(1)");
    pStr = PutToStr(pStr, ch_crami.icramin.tpar.arr[2],4,"--ch_crami.icramin.tpar(2)");
    pStr = PutToStr(pStr, ch_crami.icramin.tpar.arr[3],4,"--ch_crami.icramin.tpar(3)");
    pStr = PutToStr(pStr, ch_crami.icramin.dpar,4,"--ch_crami.icramin.dpar");//          : std_logic_vector(3 downto 0);
    //
    pStr = PutToStr(pStr, ch_crami.dcramin.address,20,"ch_crami.dcramin.address");// : std_logic_vector(19 downto 0);
    pStr = PutToStr(pStr, ch_crami.dcramin.tag.arr[0],IDBITS,"ch_crami.dcramin.tag(0)");//     : cdatatype; //std_logic_vector(31  downto 0);
    pStr = PutToStr(pStr, ch_crami.dcramin.tag.arr[1],IDBITS,"ch_crami.dcramin.tag(1)");
    pStr = PutToStr(pStr, ch_crami.dcramin.tag.arr[2],IDBITS,"ch_crami.dcramin.tag(2)");
    pStr = PutToStr(pStr, ch_crami.dcramin.tag.arr[3],IDBITS,"ch_crami.dcramin.tag(3)");
    pStr = PutToStr(pStr, ch_crami.dcramin.ptag.arr[0],IDBITS,"ch_crami.dcramin.ptag(0)");//      : cdatatype; //std_logic_vector(31  downto 0);
    pStr = PutToStr(pStr, ch_crami.dcramin.ptag.arr[1],IDBITS,"ch_crami.dcramin.ptag(1)");
    pStr = PutToStr(pStr, ch_crami.dcramin.ptag.arr[2],IDBITS,"ch_crami.dcramin.ptag(2)");
    pStr = PutToStr(pStr, ch_crami.dcramin.ptag.arr[3],IDBITS,"ch_crami.dcramin.ptag(3)");
    pStr = PutToStr(pStr, ch_crami.dcramin.twrite,4,"ch_crami.dcramin.twrite",true);//  : std_logic_vector(0 to 3);
    pStr = PutToStr(pStr, ch_crami.dcramin.tpwrite,4,"ch_crami.dcramin.tpwrite",true);// : std_logic_vector(0 to 3);
    pStr = PutToStr(pStr, ch_crami.dcramin.tenable,4,"ch_crami.dcramin.tenable",true);// : std_logic_vector(0 to 3);
    pStr = PutToStr(pStr, ch_crami.dcramin.flush,1,"ch_crami.dcramin.flush");//   : std_ulogic;
    pStr = PutToStr(pStr, ch_crami.dcramin.data.arr[0],IDBITS,"ch_crami.dcramin.data(0)");//    : cdatatype;
    pStr = PutToStr(pStr, ch_crami.dcramin.data.arr[1],IDBITS,"ch_crami.dcramin.data(1)");
    pStr = PutToStr(pStr, ch_crami.dcramin.data.arr[2],IDBITS,"ch_crami.dcramin.data(2)");
    pStr = PutToStr(pStr, ch_crami.dcramin.data.arr[3],IDBITS,"ch_crami.dcramin.data(3)");
    pStr = PutToStr(pStr, ch_crami.dcramin.denable,4,"ch_crami.dcramin.denable",true);// : std_logic_vector(0 to 3);
    pStr = PutToStr(pStr, ch_crami.dcramin.dwrite,4,"ch_crami.dcramin.dwrite",true);//  : std_logic_vector(0 to 3);
    pStr = PutToStr(pStr, ch_crami.dcramin.senable,4,"ch_crami.dcramin.senable",true);// : std_logic_vector(0 to 3);
    pStr = PutToStr(pStr, ch_crami.dcramin.swrite,4,"ch_crami.dcramin.swrite",true);//  : std_logic_vector(0 to 3);
    pStr = PutToStr(pStr, ch_crami.dcramin.saddress,20,"ch_crami.dcramin.saddress");//  : std_logic_vector(19 downto 0);
    pStr = PutToStr(pStr, ch_crami.dcramin.faddress,20,"--ch_dcrami.faddress");//  : std_logic_vector(19 downto 0);
    pStr = PutToStr(pStr, ch_crami.dcramin.spar,1,"--ch_dcrami.spar");//          : std_logic;
    pStr = PutToStr(pStr, ch_crami.dcramin.ldramin.address>>2,24-2,"ch_crami.dcramin.ldramin.address");// : std_logic_vector(23 downto 2);
    pStr = PutToStr(pStr, ch_crami.dcramin.ldramin.enable,1,"ch_crami.dcramin.ldramin.enable");//  : std_ulogic;
    pStr = PutToStr(pStr, ch_crami.dcramin.ldramin.read,1,"ch_crami.dcramin.ldramin.read");//    : std_ulogic;
    pStr = PutToStr(pStr, ch_crami.dcramin.ldramin.write,1,"ch_crami.dcramin.ldramin.write");//   : std_ulogic;
    pStr = PutToStr(pStr, ch_crami.dcramin.ctx.arr[0],M_CTX_SZ,"ch_crami.dcramin.ctx(0)");//           : ctxdatatype;
    pStr = PutToStr(pStr, ch_crami.dcramin.ctx.arr[1],M_CTX_SZ,"ch_crami.dcramin.ctx(1)");
    pStr = PutToStr(pStr, ch_crami.dcramin.ctx.arr[2],M_CTX_SZ,"ch_crami.dcramin.ctx(2)");
    pStr = PutToStr(pStr, ch_crami.dcramin.ctx.arr[3],M_CTX_SZ,"ch_crami.dcramin.ctx(3)");
    pStr = PutToStr(pStr, ch_crami.dcramin.tpar.arr[0],4,"--ch_dcrami.tpar(0)");//          : cpartype;
    pStr = PutToStr(pStr, ch_crami.dcramin.tpar.arr[1],4,"--ch_dcrami.tpar(1)");
    pStr = PutToStr(pStr, ch_crami.dcramin.tpar.arr[2],4,"--ch_dcrami.tpar(2)");
    pStr = PutToStr(pStr, ch_crami.dcramin.tpar.arr[3],4,"--ch_dcrami.tpar(3)");
    pStr = PutToStr(pStr, ch_crami.dcramin.dpar.arr[0],4,"--ch_dcrami.dpar(0)");//          : cpartype;
    pStr = PutToStr(pStr, ch_crami.dcramin.dpar.arr[1],4,"--ch_dcrami.dpar(1)");
    pStr = PutToStr(pStr, ch_crami.dcramin.dpar.arr[2],4,"--ch_dcrami.dpar(2)");
    pStr = PutToStr(pStr, ch_crami.dcramin.dpar.arr[3],4,"--ch_dcrami.dpar(3)");
    pStr = PutToStr(pStr, ch_crami.dcramin.tdiag,4,"ch_crami.dcramin.tdiag");//         : std_logic_vector(3 downto 0);
    pStr = PutToStr(pStr, ch_crami.dcramin.ddiag,4,"ch_crami.dcramin.ddiag");//         : std_logic_vector(3 downto 0);
    pStr = PutToStr(pStr, ch_crami.dcramin.sdiag,4,"--ch_dcrami.sdiag");//         : std_logic_vector(3 downto 0);


  
    PrintIndexStr();

    *posBench[TB_mmu_cache] << chStr << "\n";
  }

  // Clock update:
  tst_mmu_cache.ClkUpdate();
}
#endif

