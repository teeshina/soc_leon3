#include "lheaders.h"

extern leon3mp  topLeon3mp;

extern void ResetPutStr();
extern void PrintIndexStr();

/*struct DsuAdrFields
{
  uint32 nul_a    : 2;
  uint32 DsuRegs  : 5;//(6:2)hsel2
  uint32 nul_b    : 13;
  uint32 TypeRegs : 4;//(23:20)hsel1
  uint32 CpuInd   : 1;//(24+log2x[ncpu]-1;24)
  uint32 nul_c    : 7;
};*/



void DsuCpu::Update(SClock inClk, ahb_slv_out_type *pahbso)
{
  if(inClk.eClock_z!=SClock::CLK_POSEDGE)
    return;
        
  switch(eState)
  {
    case IDLE:
      ahbsi.htrans = HTRANS_IDLE;
      ahbsi.hsel   = 0;
      if(bNewWord)
      {
        eState = NONSEQ;
        ahbsi.haddr  = 0x90000000 | rDsuRegBits.u.r.ADR;
        ahbsi.hsize  = HSIZE_WORD;
        ahbsi.htrans = HTRANS_NONSEQ;
        ahbsi.hwdata = 0x22223333;
        ahbsi.hburst = HBURST_WRAP8;//HBURST_SINGLE;
        ahbsi.hsel   = 1<<AHB_SLAVE_DSU;
        ahbsi.hwrite = 1;
        ahbsi.hready = 1;
        iSeqCnt      = 0;
      }
    break;
    case NONSEQ:
      if(pahbso->hready)
      {
        iSeqCnt++;
        if(ahbsi.hburst == HBURST_SINGLE) 
        {
          eState = IDLE;
          bNewWord = false;
        }else
        {
          eState = SEQ;
          ahbsi.htrans = HTRANS_SEQ;;
          ahbsi.haddr += 4;
        }
      }
    break;
    case SEQ:
      if(pahbso->hready)
      {
        iSeqCnt++;
        if(iSeqCnt>rDsuRegBits.u.r.SEQ)
        {
          eState = IDLE;
          bNewWord = false;
        }
      }
    break;
    default:;
  }
}


//****************************************************************************
void dbg::dsu3x_tb(SystemOnChipIO &io)
{
  // default output using SoC top level:
  ahb_mst_in_type     *pin_ahbmi = &topLeon3mp.stCtrl2Mst;//  : in  ahb_mst_in_type;
  ahb_slv_in_type     *pin_ahbsi = &topLeon3mp.stCtrl2Slv;//  : in  ahb_slv_in_type;
  ahb_slv_out_type    *pch_ahbso = &topLeon3mp.stSlv2Ctrl[AHB_SLAVE_MEM];//  : out ahb_slv_out_type;
  l3_debug_out_vector *pin_dbgo = &topLeon3mp.dbgo;//   : in l3_debug_out_vector(0 to NCPU-1);
  l3_debug_in_vector  *pch_dbgi = &topLeon3mp.dbgi;//   : out l3_debug_in_vector(0 to NCPU-1);
  dsu_in_type         *pin_dsui = &topLeon3mp.dsui;//   : in dsu_in_type;
  dsu_out_type        *pch_dsuo = &topLeon3mp.dsuo;//   : out dsu_out_type;
  uint32              hclken = 1;// : in std_ulogic

//#define DSU_CPU_ENA
#ifdef DSU_CPU_ENA
  if(iClkCnt==100)
    clDsuCpu.Write(0,0,0);

  clDsuCpu.Update(io.inClk, &ch_ahbso);
#endif

#ifdef DBG_dsu3x
  if(io.inClk.eClock_z==SClock::CLK_POSEDGE)
  {
    in_hclken = 1;//rand()&0x1;
    //
    in_ahbmi.hgrant = rand()&0x1;//[0:15]  : (0 to AHB_MASTERS_MAX-1);     -- bus grant
    in_ahbmi.hready = rand()&0x1;//  ;                           -- transfer done
    in_ahbmi.hresp = rand()&0x3;//[1:0] : (1 downto 0);   -- response type
    in_ahbmi.hrdata = ((rand()<<17) + rand()) & MSK32(CFG_AHBDW-1,0);//[31:0]  : (AHBDW-1 downto 0);   -- read data bus
    in_ahbmi.hcache = rand()&0x1;//  ;                           -- cacheable
    in_ahbmi.hirq = ((rand()<<17) + rand()) & MSK32(AHB_IRQ_MAX-1,0);//[31:0]    : (AHB_IRQ_MAX-1 downto 0); -- interrupt result bus
    in_ahbmi.testen = 0;//  ;                           -- scan test enable
    in_ahbmi.testrst = 0;// ;                           -- scan test reset
    in_ahbmi.scanen = 0;//  ;                           -- scan enable
    in_ahbmi.testoen = 0;//   ;                           -- test output enable 
    //
    in_ahbsi.hsel = rand()&MSK32(AHB_SLAVES_MAX-1,0);//  : (0 to AHB_SLAVES_MAX-1);     -- slave select
    in_ahbsi.haddr = 0x90000000|((rand()&0xF)<<20)|rand();//(rand()<<17) + rand();// : (31 downto 0);  -- address bus (byte)
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
    in_dbgi.arr[0].data = (rand()<<17) + rand();//    : std_logic_vector(31 downto 0);
    in_dbgi.arr[0].crdy = rand()&0x1;//    : std_ulogic;
    in_dbgi.arr[0].dsu = rand()&0x1;//     : std_ulogic;
    in_dbgi.arr[0].dsumode = rand()&0x1;// : std_ulogic;
    in_dbgi.arr[0].error = rand()&0x1;//   : std_ulogic;
    in_dbgi.arr[0].halt = rand()&0x1;//    : std_ulogic;
    in_dbgi.arr[0].pwd = rand()&0x1;//     : std_ulogic;
    in_dbgi.arr[0].idle = rand()&0x1;//    : std_ulogic;
    in_dbgi.arr[0].ipend = rand()&0x1;//   : std_ulogic;
    in_dbgi.arr[0].icnt = rand()&0x1;//    : std_ulogic;
    in_dbgi.arr[0].fcnt = rand()&0x1;//    : std_ulogic;
    in_dbgi.arr[0].optype = rand()&0x3F;//  : std_logic_vector(5 downto 0); -- instruction type
    in_dbgi.arr[0].bpmiss = rand()&0x1;//  : std_ulogic;     -- branch predict miss
    in_dbgi.arr[0].istat.cmiss = rand()&0x1;//   : std_ulogic;			-- cache miss
    in_dbgi.arr[0].istat.tmiss = rand()&0x1;//   : std_ulogic;			-- TLB miss
    in_dbgi.arr[0].istat.chold = rand()&0x1;//   : std_ulogic;			-- cache hold
    in_dbgi.arr[0].istat.mhold = rand()&0x1;//   : std_ulogic;			-- cache mmu hold
    in_dbgi.arr[0].dstat.cmiss = rand()&0x1;//   : std_ulogic;			-- cache miss
    in_dbgi.arr[0].dstat.tmiss = rand()&0x1;//   : std_ulogic;			-- TLB miss
    in_dbgi.arr[0].dstat.chold = rand()&0x1;//   : std_ulogic;			-- cache hold
    in_dbgi.arr[0].dstat.mhold = rand()&0x1;//   : std_ulogic;			-- cache mmu hold
    in_dbgi.arr[0].wbhold = rand()&0x1;//  : std_ulogic;     -- write buffer hold
    in_dbgi.arr[0].su = rand()&0x1;//      : std_ulogic;     -- supervisor state
    //
    in_dsui.enable = rand()&0x1;//  : std_ulogic;
    in_dsui.Break  = rand()&0x1;//std_ulogic;
  }

#ifdef DSU_CPU_ENA
  pin_ahbsi = clDsuCpu.GetpAhsi();
#else
  pin_ahbsi = &in_ahbsi;//  : in  ahb_slv_in_type;
#endif

  
  tst_dsu3x.Update(io.inNRst,//    : in  std_ulogic;
                io.inClk,//   : in  std_ulogic;
                io.inClk,// : in std_ulogic;
                in_ahbmi,//  : in  ahb_mst_in_type;
                *pin_ahbsi,//  : in  ahb_slv_in_type;
                ch_ahbso,//  : out ahb_slv_out_type;
                in_dbgi,//   : in l3_debug_out_vector(0 to NCPU-1);
                ch_dbgo,//   : out l3_debug_in_vector(0 to NCPU-1);
                in_dsui,//   : in dsu_in_type;
                ch_dsuo,//   : out dsu_out_type;
                in_hclken// : in std_ulogic
                );

  pin_ahbmi = &in_ahbmi;//  : in  ahb_mst_in_type;
  pch_ahbso = &ch_ahbso;//  : out ahb_slv_out_type;
  pin_dbgi = &in_dbgi;//   : in l3_debug_out_vector(0 to NCPU-1);
  pch_dbgo = &ch_dbgo;//   : out l3_debug_in_vector(0 to NCPU-1);
  pin_dsui = &in_dsui;//   : in dsu_in_type;
  pch_dsuo = &ch_dsuo;//   : out dsu_out_type;
  hclken = in_hclken;// : in std_ulogic
#endif

  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';

    ResetPutStr();
  
    // inputs:
    pStr = PutToStr(pStr, io.inNRst, 1, "inNRst");
    pStr = PutToStr(pStr, hclken, 1, "in_hclken");
    //
    pStr = PutToStr(pStr, pin_ahbmi->hgrant,AHB_MASTERS_MAX,"in_ahbmi.hgrant",true);//[0:15]  : (0 to AHB_MASTERS_MAX-1);     -- bus grant
    pStr = PutToStr(pStr, pin_ahbmi->hready,1,"in_ahbmi.hready");//  ;                           -- transfer done
    pStr = PutToStr(pStr, pin_ahbmi->hresp,2,"in_ahbmi.hresp");//[1:0] : (1 downto 0);   -- response type
    pStr = PutToStr(pStr, pin_ahbmi->hrdata,CFG_AHBDW,"in_ahbmi.hrdata");//[31:0]  : (AHBDW-1 downto 0);   -- read data bus
    pStr = PutToStr(pStr, pin_ahbmi->hcache,1,"in_ahbmi.hcache");//  ;                           -- cacheable
    pStr = PutToStr(pStr, pin_ahbmi->hirq,AHB_IRQ_MAX,"in_ahbmi.hirq");//[31:0]    : (AHB_IRQ_MAX-1 downto 0); -- interrupt result bus
    pStr = PutToStr(pStr, pin_ahbmi->testen,1,"in_ahbmi.testen");//  ;                           -- scan test enable
    pStr = PutToStr(pStr, pin_ahbmi->testrst,1,"in_ahbmi.testrst");// ;                           -- scan test reset
    pStr = PutToStr(pStr, pin_ahbmi->scanen,1,"in_ahbmi.scanen");//  ;                           -- scan enable
    pStr = PutToStr(pStr, pin_ahbmi->testoen,1,"in_ahbmi.testoen");//   ;                           -- test output enable 
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
    pStr = PutToStr(pStr, pin_dbgo->arr[0].data,32,"in_dbgi(0).data");//    : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pin_dbgo->arr[0].crdy,1,"in_dbgi(0).crdy");//    : std_ulogic;
    pStr = PutToStr(pStr, pin_dbgo->arr[0].dsu,1,"in_dbgi(0).dsu");//     : std_ulogic;
    pStr = PutToStr(pStr, pin_dbgo->arr[0].dsumode,1,"in_dbgi(0).dsumode");// : std_ulogic;
    pStr = PutToStr(pStr, pin_dbgo->arr[0].error,1,"in_dbgi(0).error");//   : std_ulogic;
    pStr = PutToStr(pStr, pin_dbgo->arr[0].halt,1,"in_dbgi(0).halt");//    : std_ulogic;
    pStr = PutToStr(pStr, pin_dbgo->arr[0].pwd,1,"in_dbgi(0).pwd");//     : std_ulogic;
    pStr = PutToStr(pStr, pin_dbgo->arr[0].idle,1,"in_dbgi(0).idle");//    : std_ulogic;
    pStr = PutToStr(pStr, pin_dbgo->arr[0].ipend,1,"in_dbgi(0).ipend");//   : std_ulogic;
    pStr = PutToStr(pStr, pin_dbgo->arr[0].icnt,1,"in_dbgi(0).icnt");//    : std_ulogic;
    pStr = PutToStr(pStr, pin_dbgo->arr[0].fcnt,1,"in_dbgi(0).fcnt");//    : std_ulogic;
    pStr = PutToStr(pStr, pin_dbgo->arr[0].optype,6,"in_dbgi(0).optype");//  : std_logic_vector(5 downto 0); -- instruction type
    pStr = PutToStr(pStr, pin_dbgo->arr[0].bpmiss,1,"in_dbgi(0).bpmiss");//  : std_ulogic;     -- branch predict miss
    pStr = PutToStr(pStr, pin_dbgo->arr[0].istat.cmiss,1,"in_dbgi(0).istat.cmiss");//   : std_ulogic;			-- cache miss
    pStr = PutToStr(pStr, pin_dbgo->arr[0].istat.tmiss,1,"in_dbgi(0).istat.tmiss");//   : std_ulogic;			-- TLB miss
    pStr = PutToStr(pStr, pin_dbgo->arr[0].istat.chold,1,"in_dbgi(0).istat.chold");//   : std_ulogic;			-- cache hold
    pStr = PutToStr(pStr, pin_dbgo->arr[0].istat.mhold,1,"in_dbgi(0).istat.mhold");//   : std_ulogic;			-- cache mmu hold
    pStr = PutToStr(pStr, pin_dbgo->arr[0].dstat.cmiss,1,"in_dbgi(0).dstat.cmiss");//   : std_ulogic;			-- cache miss
    pStr = PutToStr(pStr, pin_dbgo->arr[0].dstat.tmiss,1,"in_dbgi(0).dstat.tmiss");//   : std_ulogic;			-- TLB miss
    pStr = PutToStr(pStr, pin_dbgo->arr[0].dstat.chold,1,"in_dbgi(0).dstat.chold");//   : std_ulogic;			-- cache hold
    pStr = PutToStr(pStr, pin_dbgo->arr[0].dstat.mhold,1,"in_dbgi(0).dstat.mhold");//   : std_ulogic;			-- cache mmu hold
    pStr = PutToStr(pStr, pin_dbgo->arr[0].wbhold,1,"in_dbgi(0).wbhold");//  : std_ulogic;     -- write buffer hold
    pStr = PutToStr(pStr, pin_dbgo->arr[0].su,1,"in_dbgi(0).su");//      : std_ulogic;     -- supervisor state
    //
    pStr = PutToStr(pStr, pin_dsui->enable,1,"in_dsui.enable");//  : std_ulogic;
    pStr = PutToStr(pStr, pin_dsui->Break,1,"in_dsui.break");//std_ulogic;


    // Output:
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
    pStr = PutToStr(pStr, pch_ahbso->hindex,4,"conv_integer(ch_ahbso.hindex)");//    : integer range 0 to AHB_SLAVES_MAX-1;   -- diagnostic use only
    //
    pStr = PutToStr(pStr, pch_dbgi->arr[0].dsuen,1,"ch_dbgo(0).dsuen");//   : std_ulogic;  -- DSU enable
    pStr = PutToStr(pStr, pch_dbgi->arr[0].denable,1,"ch_dbgo(0).denable");// : std_ulogic;  -- diagnostic register access enable
    pStr = PutToStr(pStr, pch_dbgi->arr[0].dbreak,1,"ch_dbgo(0).dbreak");//  : std_ulogic;  -- debug break-in
    pStr = PutToStr(pStr, pch_dbgi->arr[0].step,1,"ch_dbgo(0).step");//    : std_ulogic;  -- single step    
    pStr = PutToStr(pStr, pch_dbgi->arr[0].halt,1,"ch_dbgo(0).halt");//    : std_ulogic;  -- halt processor
    pStr = PutToStr(pStr, pch_dbgi->arr[0].reset,1,"ch_dbgo(0).reset");//   : std_ulogic;  -- reset processor
    pStr = PutToStr(pStr, pch_dbgi->arr[0].dwrite,1,"ch_dbgo(0).dwrite");//  : std_ulogic;  -- read/write
    pStr = PutToStr(pStr, pch_dbgi->arr[0].daddr>>2,22,"ch_dbgo(0).daddr");//   : std_logic_vector(23 downto 2); -- diagnostic address
    pStr = PutToStr(pStr, pch_dbgi->arr[0].ddata,32,"ch_dbgo(0).ddata");//   : std_logic_vector(31 downto 0); -- diagnostic data
    pStr = PutToStr(pStr, pch_dbgi->arr[0].btrapa,1,"ch_dbgo(0).btrapa");//  : std_ulogic;	   -- break on IU trap
    pStr = PutToStr(pStr, pch_dbgi->arr[0].btrape,1,"ch_dbgo(0).btrape");//  : std_ulogic;	-- break on IU trap
    pStr = PutToStr(pStr, pch_dbgi->arr[0].berror,1,"ch_dbgo(0).berror");//  : std_ulogic;	-- break on IU error mode
    pStr = PutToStr(pStr, pch_dbgi->arr[0].bwatch,1,"ch_dbgo(0).bwatch");//  : std_ulogic;	-- break on IU watchpoint
    pStr = PutToStr(pStr, pch_dbgi->arr[0].bsoft,1,"ch_dbgo(0).bsoft");//   : std_ulogic;	-- break on software breakpoint (TA 1)
    pStr = PutToStr(pStr, pch_dbgi->arr[0].tenable,1,"ch_dbgo(0).tenable");// : std_ulogic;
    pStr = PutToStr(pStr, pch_dbgi->arr[0].timer,31,"ch_dbgo(0).timer");//   :  std_logic_vector(30 downto 0);                                                -- 
    //
    pStr = PutToStr(pStr, pch_dsuo->active,1,"ch_dsuo.active");//std_ulogic;
    pStr = PutToStr(pStr, pch_dsuo->tstop,1,"ch_dsuo.tstop");//std_ulogic;
    pStr = PutToStr(pStr, pch_dsuo->pwd,16,"ch_dsuo.pwd");//std_logic_vector(15 downto 0);

    PrintIndexStr();

    *posBench[TB_dsu3x] << chStr << "\n";
  }

#ifdef DBG_dsu3x
  // Clock update:
  tst_dsu3x.ClkUpdate();
#endif
  
}
