//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "ldbg.h"
#include "leon3_ml605\leon3mp.h"
extern leon3mp topLeon3mp;
extern dbg     clDbg;



char *PrintAllRegIU(char *pStr, registers *pr);

//****************************************************************************
void dbg::soc_leon3_tb(SystemOnChipIO &io)
{
  char chTmp[256];
  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';

    ResetPutStr();

    uint32 hrdata=0;
    uint32 iTck = ((pStr, io.jtag.TCK.eClock==SClock::CLK_POSEDGE)||(pStr, io.jtag.TCK.eClock==SClock::CLK_POSITIVE)) ? 1 : 0;
    iu3 *piu3 = &topLeon3mp.pclLeon3s[0]->pclProc3->clIU3;
    uint32 wClkBus = ((io.inClk.eClock==SClock::CLK_POSEDGE)||(io.inClk.eClock==SClock::CLK_POSITIVE)) ? 1: 0;
    
    pStr = PutToStr(pStr, io.inNRst, 1, "inNRst");
    // UART:
    pStr = PutToStr(pStr, io.uart1.CTS,1,"in_CTS");
    pStr = PutToStr(pStr, io.uart1.RD,1,"in_RX");
    pStr = PutToStr(pStr, io.uart1.RTS,1,"ch_RTS");
    pStr = PutToStr(pStr, io.uart1.TD,1,"ch_TX");
    // JTAG:
    pStr = PutToStr(pStr, io.jtag.nTRST,1,"in_trst"); // in: Test Reset
    pStr = PutToStr(pStr, iTck,1,"in_tck");   // in: Test Clock
    pStr = PutToStr(pStr, io.jtag.TMS,1,"in_tms");   // in: Test Mode State
    pStr = PutToStr(pStr, io.jtag.TDI,1,"in_tdi");   // in: Test Data Input
    pStr = PutToStr(pStr, io.jtag.TDO,1,"ch_tdo");   // out: Test Data Output
    // ADC samples:
    pStr = PutToStr(pStr, io.gnss.I[0],2,"in_Ia");
    pStr = PutToStr(pStr, io.gnss.Q[0],2,"in_Qa");
    pStr = PutToStr(pStr, io.gnss.I[1],2,"in_Ib");
    pStr = PutToStr(pStr, io.gnss.Q[1],2,"in_Qb");
    // MAX2769 SPI interface:
    pStr = PutToStr(pStr, io.spimax2769.LD,2,"in_LD");
    pStr = PutToStr(pStr, io.spimax2769.SCLK,1,"ch_SCLK");
    pStr = PutToStr(pStr, io.spimax2769.nSDATA,1,"ch_SDATA");
    pStr = PutToStr(pStr, io.spimax2769.nCS,2,"ch_CSn");
    // Antenna controller:
    pStr = PutToStr(pStr, io.antctrl.ExtAntStat,1,"in_ExtAntStat");
    pStr = PutToStr(pStr, io.antctrl.ExtAntDetect,1,"in_ExtAntDetect");
    pStr = PutToStr(pStr, io.antctrl.ExtAntEna,1,"ch_ExtAntEna");
    // STM Gyroscope SPI
    pStr = PutToStr(pStr, io.gyro.SDO,1,"in_GyroSDI");
    pStr = PutToStr(pStr, io.gyro.Int1,1,"in_GyroInt1");
    pStr = PutToStr(pStr, io.gyro.Int2,1,"in_GyroInt2");
    pStr = PutToStr(pStr, io.gyro.SDI,1,"ch_GyroSDO");
    pStr = PutToStr(pStr, io.gyro.nCS,1,"ch_GyroCSn");
    pStr = PutToStr(pStr, io.gyro.SPC,1,"ch_GyroSPC");
    // STM Accelerometer SPI
    pStr = PutToStr(pStr, io.acceler.SDO,1,"in_AccelerSDI");
    pStr = PutToStr(pStr, io.acceler.Int1,1,"in_AccelerInt1");
    pStr = PutToStr(pStr, io.acceler.Int2,1,"in_AccelerInt2");
    pStr = PutToStr(pStr, io.acceler.SDI,1,"ch_AccelerSDO");
    pStr = PutToStr(pStr, io.acceler.nCS,1,"ch_AccelerCSn");
    pStr = PutToStr(pStr, io.acceler.SPC,1,"ch_AccelerSPC");
    // User pins:
    pStr = PutToStr(pStr, uint32(0),8,"unsigned:in_DIP");
    pStr = PutToStr(pStr, uint32(0),8,"unsigned:ch_LED");
    pStr = PutToStr(pStr, wClkBus, 1, "unsigned:ch_ClkBus");
    // SRAM signals:
    pStr = PutToStr(pStr, topLeon3mp.pclAhbRAM->ramaddr,CFG_SRAM_ADRBITS,"ch_ramaddr");
    pStr = PutToStr(pStr, topLeon3mp.pclAhbRAM->hwdata,AHBDW,"ch_hwdata");
    pStr = PutToStr(pStr, topLeon3mp.pclAhbRAM->ramdata,AHBDW,"in_ramdata");
    uint32 ramsel = (topLeon3mp.pclAhbRAM->v.hsel | topLeon3mp.pclAhbRAM->r.Q.hwrite) ? 0xf : 0;
    pStr = PutToStr(pStr, ramsel,AHBDW/8,"ch_ramsel");
    pStr = PutToStr(pStr, topLeon3mp.pclAhbRAM->write,AHBDW/8,"ch_write");

    // ************ SoC internal signals debug ***********
    //
    // AHB master interface:
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Mst.hgrant,16,"t_msti.hgrant",true);//[0:15]  : (0 to AHB_MASTERS_MAX-1);     -- bus grant
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Mst.hready,1,"t_msti.hready");//  ;                           -- transfer done
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Mst.hresp,2,"t_msti.hresp");// : (1 downto 0);   -- response type
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Mst.hrdata,32,"t_msti.hrdata");//[31:0]  : (AHBDW-1 downto 0);   -- read data bus
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Mst.hcache,1,"t_msti.hcache");//                             -- cacheable
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Mst.hirq,32,"t_msti.hirq");//    : (AHB_IRQ_MAX-1 downto 0); -- interrupt result bus
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Mst.testen,1,"t_msti.testen");//                             -- scan test enable
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Mst.testrst,1,"t_msti.testrst");//                            -- scan test reset
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Mst.scanen,1,"t_msti.scanen");//                             -- scan enable
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Mst.testoen,1,"t_msti.testoen");//                              -- test output enable 

    //for(int32 i=0; i<AHB_MASTER_TOTAL; i++)
    for(int32 i=0; i<1; i++)
    {
      sprintf_s(chTmp,"t_msto(%i).hbusreq",i);
      pStr = PutToStr(pStr, topLeon3mp.stMst2Ctrl.arr[i].hbusreq,1,chTmp);
      sprintf_s(chTmp,"t_msto(%i).hlock",i);
      pStr = PutToStr(pStr, topLeon3mp.stMst2Ctrl.arr[i].hlock,1,chTmp);
      sprintf_s(chTmp,"t_msto(%i).htrans",i);
      pStr = PutToStr(pStr, topLeon3mp.stMst2Ctrl.arr[i].htrans,2,chTmp);
      sprintf_s(chTmp,"t_msto(%i).haddr",i);
      pStr = PutToStr(pStr, topLeon3mp.stMst2Ctrl.arr[i].haddr,32,chTmp);
      sprintf_s(chTmp,"t_msto(%i).hwrite",i);
      pStr = PutToStr(pStr, topLeon3mp.stMst2Ctrl.arr[i].hwrite,1,chTmp);
      sprintf_s(chTmp,"t_msto(%i).hsize",i);
      pStr = PutToStr(pStr, topLeon3mp.stMst2Ctrl.arr[i].hsize,3,chTmp);
      sprintf_s(chTmp,"t_msto(%i).hburst",i);
      pStr = PutToStr(pStr, topLeon3mp.stMst2Ctrl.arr[i].hburst,3,chTmp);
      sprintf_s(chTmp,"t_msto(%i).hprot",i);
      pStr = PutToStr(pStr, topLeon3mp.stMst2Ctrl.arr[i].hprot,4,chTmp);
      sprintf_s(chTmp,"t_msto(%i).hwdata",i);
      pStr = PutToStr(pStr, topLeon3mp.stMst2Ctrl.arr[i].hwdata,32,chTmp);
      sprintf_s(chTmp,"t_msto(%i).hirq",i);
      pStr = PutToStr(pStr, topLeon3mp.stMst2Ctrl.arr[i].hirq,32,chTmp);
      for(int32 k=0; k<8; k++)
      {
        sprintf_s(chTmp,"t_msto(%i).hconfig(%i)",i,k);
        pStr = PutToStr(pStr, topLeon3mp.stMst2Ctrl.arr[i].hconfig.arr[k],32,chTmp);
      }
      sprintf_s(chTmp,"conv_integer:t_msto(%i).hindex",i);
      pStr = PutToStr(pStr, topLeon3mp.stMst2Ctrl.arr[i].hindex,4,chTmp);
    }

    // AHB slave interface:
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Slv.hsel,AHB_SLAVES_MAX,"t_slvi.hsel",true);//  : (0 to AHB_SLAVES_MAX-1);     -- slave select
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Slv.haddr,32,"t_slvi.haddr");// : (31 downto 0);  -- address bus (byte)
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Slv.hwrite,1,"t_slvi.hwrite");//  ;                           -- read/write
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Slv.htrans,2,"t_slvi.htrans");//  : (1 downto 0);   -- transfer type
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Slv.hsize,3,"t_slvi.hsize");// : (2 downto 0);   -- transfer size
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Slv.hburst,3,"t_slvi.hburst");//  : (2 downto 0);   -- burst type
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Slv.hwdata,CFG_AHBDW,"t_slvi.hwdata");//  : (AHBDW-1 downto 0);   -- write data bus
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Slv.hprot,4,"t_slvi.hprot");// : (3 downto 0);   -- protection control
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Slv.hready,1,"t_slvi.hready");//  ;                -- transfer done
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Slv.hmaster,4,"t_slvi.hmaster");// : (3 downto 0);  -- current master
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Slv.hmastlock,1,"t_slvi.hmastlock");// ;              -- locked access
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Slv.hmbsel,AHB_MEM_ID_WIDTH,"t_slvi.hmbsel",true);//  : (0 to AHB_MEM_ID_WIDTH-1); -- memory bank select
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Slv.hcache,1,"t_slvi.hcache");//  ;                -- cacheable
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Slv.hirq,AHB_IRQ_MAX,"t_slvi.hirq");//  : (AHB_IRQ_MAX-1 downto 0); -- interrupt result bus
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Slv.testen,1,"t_slvi.testen");//                        -- scan test enable
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Slv.testrst,1,"t_slvi.testrst");//                       -- scan test reset
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Slv.scanen,1,"t_slvi.scanen");//                        -- scan enable
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Slv.testoen,1,"t_slvi.testoen");;//                       -- test output enable 

    // APB interface:
    pStr = PutToStr(pStr, topLeon3mp.apbi.psel,APB_SLAVES_MAX,"t_apbi.psel",true);//[0:15]  : (0 to APB_SLAVES_MAX-1); -- slave select
    pStr = PutToStr(pStr, topLeon3mp.apbi.penable,1,"t_apbi.penable");// ;                       -- strobe
    pStr = PutToStr(pStr, topLeon3mp.apbi.paddr,32,"t_apbi.paddr");//[31:0] : (31 downto 0);  -- address bus (byte)
    pStr = PutToStr(pStr, topLeon3mp.apbi.pwrite,1,"t_apbi.pwrite");//  ;                       -- write
    pStr = PutToStr(pStr, topLeon3mp.apbi.pwdata,32,"t_apbi.pwdata");//[31:0]  : (31 downto 0);  -- write data bus
    pStr = PutToStr(pStr, topLeon3mp.apbi.pirq,AHB_IRQ_MAX,"t_apbi.pirq");//[31:0]  : (AHB_IRQ_MAX-1 downto 0); -- interrupt result bus
    pStr = PutToStr(pStr, topLeon3mp.apbi.testen,1,"t_apbi.testen");//                          -- scan test enable
    pStr = PutToStr(pStr, topLeon3mp.apbi.testrst,1,"t_apbi.testrst");//                         -- scan test reset
    pStr = PutToStr(pStr, topLeon3mp.apbi.scanen,1,"t_apbi.scanen");//                          -- scan enable
    pStr = PutToStr(pStr, topLeon3mp.apbi.testoen,1,"t_apbi.testoen");//                         -- test output enable

    // IU3 regisrers:
    pStr = PutToStr(pStr, piu3->rbR.Q.f.pc, 32, "t_pc");
    pStr = PutToStr(pStr, piu3->ir.Q.addr, 32, "t_npc");

    hrdata = (15<<28) | (3<<24) | //(IMPL<<28) | (VER<<24) |
          (piu3->rbR.Q.w.s.icc<<20) | (piu3->rbR.Q.w.s.ec<<13) | (piu3->rbR.Q.w.s.ef<<12) | (piu3->rbR.Q.w.s.pil<<8) |
          (piu3->rbR.Q.w.s.s<<7) | (piu3->rbR.Q.w.s.ps<<6) | (piu3->rbR.Q.w.s.et<<5) |
          BITS32(piu3->rbR.Q.w.s.cwp,NWINLOG2-1,0);
    pStr = PutToStr(pStr, hrdata, 32, "t_psr");

    hrdata = (piu3->rbR.Q.w.s.tba<<12) | (piu3->rbR.Q.w.s.tt<<4);
    pStr = PutToStr(pStr, hrdata, 32, "t_tbr");

    // DSU registers:
    hrdata=0;    
    hrdata |= topLeon3mp.pclDsu3x->r.Q.te[0];
    hrdata |= (topLeon3mp.pclDsu3x->r.Q.be[0]<<1);
    hrdata |= (topLeon3mp.pclDsu3x->r.Q.bw[0]<<2);
    hrdata |= (topLeon3mp.pclDsu3x->r.Q.bs[0]<<3);
    hrdata |= (topLeon3mp.pclDsu3x->r.Q.bx[0]<<4);
    hrdata |= (topLeon3mp.pclDsu3x->r.Q.bz[0]<<5);
    hrdata |= (topLeon3mp.dbgo.arr[0].dsumode<<6);
    hrdata |= (BIT32(topLeon3mp.pclDsu3x->r.Q.dsuen,2)<<7);
    hrdata |= (BIT32(topLeon3mp.pclDsu3x->r.Q.dsubre,2)<<8);
    hrdata |= ((!topLeon3mp.dbgo.arr[0].error)<<9);
    hrdata |= (topLeon3mp.dbgo.arr[0].halt<<10);
    hrdata |= (topLeon3mp.dbgo.arr[0].pwd<<11);
    pStr = PutToStr(pStr, hrdata, 12, "dsu_status");

    
    // Internal IU3 registers pack:
    pStr = PrintAllRegIU(pStr, &piu3->rbR.Q);

    pStr = PutToStr(pStr, topLeon3mp.pclLeon3s[0]->crami.icramin.tag.arr[0],32,"t_icrami_tag0");
    pStr = PutToStr(pStr, topLeon3mp.pclLeon3s[0]->cramo.icramo.tag.arr[0],32,"t_icramo_tag0");
    pStr = PutToStr(pStr, topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->clICache.hit,1,"t_hit");
    pStr = PutToStr(pStr, topLeon3mp.pclLeon3s[0]->pclProc3->ici.inull,1,"t_ici_inull");
    pStr = PutToStr(pStr, topLeon3mp.pclLeon3s[0]->pclProc3->ici.fpc,32,"t_ici_fpc");
    
    pStr = PutToStr(pStr, topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->clICache.setrepl,log2x[CFG_ISETS],"t_setrepl");
    
    uint32 ITAG_BITS = (TAG_HIGH - IOFFSET_BITS - ILINE_BITS - 2 + CFG_ILINE + 1);
    uint32 ILRR_BIT  = (creplalg_tbl.arr[CFG_IREPL]);
    uint32 ICLOCK_BIT = CFG_ILOCK;
    uint32 MMUCTX_BITS = (8*CFG_MMUEN);
    uint32 ITWIDTH   = (ITAG_BITS + ILRR_BIT + ICLOCK_BIT + MMUCTX_BITS);
    pStr = PutToStr(pStr, topLeon3mp.pclLeon3s[0]->clCacheMem.itaddr>>ILINE_BITS,IOFFSET_BITS,"t_itaddr");
    pStr = PutToStr(pStr, topLeon3mp.pclLeon3s[0]->clCacheMem.itdatain.arr[0],ITWIDTH+1,"t_itdatain0");
    pStr = PutToStr(pStr, topLeon3mp.pclLeon3s[0]->clCacheMem.itdataout.arr[0],ITWIDTH,"t_itdataout0");
    pStr = PutToStr(pStr, topLeon3mp.pclLeon3s[0]->clCacheMem.itenable,1,"t_itenable");
    pStr = PutToStr(pStr, topLeon3mp.pclLeon3s[0]->clCacheMem.itwrite,MAXSETS,"t_itwrite",true);
   

    PrintIndexStr();
    
    *posBench[TB_soc_leon3] << chStr << "\n";
  }
}


//****************************************************************************
char *PrintAllRegIU(char *pStr, registers *pr)
{
    pStr = clDbg.PutToStr(pStr, pr->d.pc>>CFG_PCLOW,32-CFG_PCLOW,"t_r.d.pc");
    pStr = clDbg.PutToStr(pStr, pr->d.inst.arr[0],32,"t_r.d.inst(0)");
    pStr = clDbg.PutToStr(pStr, pr->d.inst.arr[1],32,"t_r.d.inst(1)");
    pStr = clDbg.PutToStr(pStr, pr->d.cwp,NWINLOG2,"t_r.d.cwp");
    pStr = clDbg.PutToStr(pStr, pr->d.set,ISETMSB+1,"vector:t_r.d.set");
    pStr = clDbg.PutToStr(pStr, pr->d.mexc,1,"t_r.d.mexc");
    pStr = clDbg.PutToStr(pStr, pr->d.cnt,2,"t_r.d.cnt");
    pStr = clDbg.PutToStr(pStr, pr->d.pv,1,"t_r.d.pv");
    pStr = clDbg.PutToStr(pStr, pr->d.annul,1,"t_r.d.annul");
    pStr = clDbg.PutToStr(pStr, pr->d.inull,1,"t_r.d.inull");
    pStr = clDbg.PutToStr(pStr, pr->d.step,1,"t_r.d.step");
    pStr = clDbg.PutToStr(pStr, pr->d.divrdy,1,"t_r.d.divrdy");

    pStr = clDbg.PutToStr(pStr, pr->a.ctrl.pc>>CFG_PCLOW,32-CFG_PCLOW,"t_r.a.ctrl.pc");//    : pctype;
    pStr = clDbg.PutToStr(pStr, pr->a.ctrl.inst,32,"t_r.a.ctrl.inst");//  : word;
    pStr = clDbg.PutToStr(pStr, pr->a.ctrl.cnt,2,"t_r.a.ctrl.cnt");//   : std_logic_vector(1 downto 0);
    pStr = clDbg.PutToStr(pStr, pr->a.ctrl.rd,RFBITS,"t_r.a.ctrl.rd");//    : rfatype;
    pStr = clDbg.PutToStr(pStr, pr->a.ctrl.tt,6,"t_r.a.ctrl.tt");//    : std_logic_vector(5 downto 0);
    pStr = clDbg.PutToStr(pStr, pr->a.ctrl.trap,1,"t_r.a.ctrl.trap");//  : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->a.ctrl.annul,1,"t_r.a.ctrl.annul");// : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->a.ctrl.wreg,1,"t_r.a.ctrl.wreg");//  : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->a.ctrl.wicc,1,"t_r.a.ctrl.wicc");//  : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->a.ctrl.wy,1,"t_r.a.ctrl.wy");//    : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->a.ctrl.ld,1,"t_r.a.ctrl.ld");//    : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->a.ctrl.pv,1,"t_r.a.ctrl.pv");//    : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->a.ctrl.rett,1,"t_r.a.ctrl.rett");//  : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->a.rs1,5,"t_r.a.rs1");
    pStr = clDbg.PutToStr(pStr, pr->a.rfa1,RFBITS,"t_r.a.rfa1");
    pStr = clDbg.PutToStr(pStr, pr->a.rfa2,RFBITS,"t_r.a.rfa2");
    pStr = clDbg.PutToStr(pStr, pr->a.rsel1,3,"t_r.a.rsel1");
    pStr = clDbg.PutToStr(pStr, pr->a.rsel2,3,"t_r.a.rsel2");
    pStr = clDbg.PutToStr(pStr, pr->a.rfe1,1,"t_r.a.rfe1");
    pStr = clDbg.PutToStr(pStr, pr->a.rfe2,1,"t_r.a.rfe2");
    pStr = clDbg.PutToStr(pStr, pr->a.cwp,NWINLOG2,"t_r.a.cwp");
    pStr = clDbg.PutToStr(pStr, pr->a.imm,32,"t_r.a.imm");
    pStr = clDbg.PutToStr(pStr, pr->a.ldcheck1,1,"t_r.a.ldcheck1");
    pStr = clDbg.PutToStr(pStr, pr->a.ldcheck2,1,"t_r.a.ldcheck2");
    pStr = clDbg.PutToStr(pStr, pr->a.ldchkra,1,"t_r.a.ldchkra");
    pStr = clDbg.PutToStr(pStr, pr->a.ldchkex,1,"t_r.a.ldchkex");
    pStr = clDbg.PutToStr(pStr, pr->a.su,1,"t_r.a.su");
    pStr = clDbg.PutToStr(pStr, pr->a.et,1,"t_r.a.et");
    pStr = clDbg.PutToStr(pStr, pr->a.wovf,1,"t_r.a.wovf");
    pStr = clDbg.PutToStr(pStr, pr->a.wunf,1,"t_r.a.wunf");
    pStr = clDbg.PutToStr(pStr, pr->a.ticc,1,"t_r.a.ticc");
    pStr = clDbg.PutToStr(pStr, pr->a.jmpl,1,"t_r.a.jmpl");
    pStr = clDbg.PutToStr(pStr, pr->a.step,1,"t_r.a.step");
    pStr = clDbg.PutToStr(pStr, pr->a.mulstart,1,"t_r.a.mulstart");
    pStr = clDbg.PutToStr(pStr, pr->a.divstart,1,"t_r.a.divstart");
    pStr = clDbg.PutToStr(pStr, pr->a.bp,1,"t_r.a.bp");
    pStr = clDbg.PutToStr(pStr, pr->a.nobp,1,"t_r.a.nobp");

    pStr = clDbg.PutToStr(pStr, pr->e.ctrl.pc>>CFG_PCLOW,32-CFG_PCLOW,"t_r.e.ctrl.pc");//    : pctype;
    pStr = clDbg.PutToStr(pStr, pr->e.ctrl.inst,32,"t_r.e.ctrl.inst");//  : word;
    pStr = clDbg.PutToStr(pStr, pr->e.ctrl.cnt,2,"t_r.e.ctrl.cnt");//   : std_logic_vector(1 downto 0);
    pStr = clDbg.PutToStr(pStr, pr->e.ctrl.rd,RFBITS,"t_r.e.ctrl.rd");//    : rfatype;
    pStr = clDbg.PutToStr(pStr, pr->e.ctrl.tt,6,"t_r.e.ctrl.tt");//    : std_logic_vector(5 downto 0);
    pStr = clDbg.PutToStr(pStr, pr->e.ctrl.trap,1,"t_r.e.ctrl.trap");//  : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->e.ctrl.annul,1,"t_r.e.ctrl.annul");// : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->e.ctrl.wreg,1,"t_r.e.ctrl.wreg");//  : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->e.ctrl.wicc,1,"t_r.e.ctrl.wicc");//  : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->e.ctrl.wy,1,"t_r.e.ctrl.wy");//    : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->e.ctrl.ld,1,"t_r.e.ctrl.ld");//    : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->e.ctrl.pv,1,"t_r.e.ctrl.pv");//    : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->e.ctrl.rett,1,"t_r.e.ctrl.rett");//  : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->e.op1,32,"t_r.e.op1");//    : word;
    pStr = clDbg.PutToStr(pStr, pr->e.op2,32,"t_r.e.op2");//    : word;
    pStr = clDbg.PutToStr(pStr, pr->e.aluop,3,"t_r.e.aluop");//  : std_logic_vector(2 downto 0);  	-- Alu operation
    pStr = clDbg.PutToStr(pStr, pr->e.alusel,2,"t_r.e.alusel");// : std_logic_vector(1 downto 0);  	-- Alu result select
    pStr = clDbg.PutToStr(pStr, pr->e.aluadd,1,"t_r.e.aluadd");// : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->e.alucin,1,"t_r.e.alucin");// : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->e.ldbp1,1,"t_r.e.ldbp1");
    pStr = clDbg.PutToStr(pStr, pr->e.ldbp2,1,"t_r.e.ldbp2");// : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->e.invop2,1,"t_r.e.invop2");// : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->e.shcnt,5,"t_r.e.shcnt");//  : std_logic_vector(4 downto 0);  	-- shift count
    pStr = clDbg.PutToStr(pStr, pr->e.sari,1,"t_r.e.sari");//   : std_ulogic;				-- shift msb
    pStr = clDbg.PutToStr(pStr, pr->e.shleft,1,"t_r.e.shleft");// : std_ulogic;				-- shift left/right
    pStr = clDbg.PutToStr(pStr, pr->e.ymsb,1,"t_r.e.ymsb");//   : std_ulogic;				-- shift left/right
    pStr = clDbg.PutToStr(pStr, pr->e.rd,5,"t_r.e.rd");// 	   : std_logic_vector(4 downto 0);
    pStr = clDbg.PutToStr(pStr, pr->e.jmpl,1,"t_r.e.jmpl");//   : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->e.su,1,"t_r.e.su");//     : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->e.et,1,"t_r.e.et");//     : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->e.cwp,NWINLOG2,"t_r.e.cwp");//    : cwptype;
    pStr = clDbg.PutToStr(pStr, pr->e.icc,4,"t_r.e.icc");//    : std_logic_vector(3 downto 0);
    pStr = clDbg.PutToStr(pStr, pr->e.mulstep,1,"t_r.e.mulstep");//: std_ulogic;            
    pStr = clDbg.PutToStr(pStr, pr->e.mul,1,"t_r.e.mul");//    : std_ulogic;            
    pStr = clDbg.PutToStr(pStr, pr->e.mac,1,"t_r.e.mac");//    : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->e.bp,1,"t_r.e.bp");//     : std_ulogic;

    pStr = clDbg.PutToStr(pStr, pr->m.ctrl.pc>>CFG_PCLOW,32-CFG_PCLOW,"t_r.m.ctrl.pc");//    : pctype;
    pStr = clDbg.PutToStr(pStr, pr->m.ctrl.inst,32,"t_r.m.ctrl.inst");//  : word;
    pStr = clDbg.PutToStr(pStr, pr->m.ctrl.cnt,2,"t_r.m.ctrl.cnt");//   : std_logic_vector(1 downto 0);
    pStr = clDbg.PutToStr(pStr, pr->m.ctrl.rd,RFBITS,"t_r.m.ctrl.rd");//    : rfatype;
    pStr = clDbg.PutToStr(pStr, pr->m.ctrl.tt,6,"t_r.m.ctrl.tt");//    : std_logic_vector(5 downto 0);
    pStr = clDbg.PutToStr(pStr, pr->m.ctrl.trap,1,"t_r.m.ctrl.trap");//  : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->m.ctrl.annul,1,"t_r.m.ctrl.annul");// : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->m.ctrl.wreg,1,"t_r.m.ctrl.wreg");//  : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->m.ctrl.wicc,1,"t_r.m.ctrl.wicc");//  : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->m.ctrl.wy,1,"t_r.m.ctrl.wy");//    : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->m.ctrl.ld,1,"t_r.m.ctrl.ld");//    : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->m.ctrl.pv,1,"t_r.m.ctrl.pv");//    : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->m.ctrl.rett,1,"t_r.m.ctrl.rett");//  : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->m.result,32,"t_r.m.result");
    pStr = clDbg.PutToStr(pStr, pr->m.y,32,"t_r.m.y");
    pStr = clDbg.PutToStr(pStr, pr->m.icc,4,"t_r.m.icc");
    pStr = clDbg.PutToStr(pStr, pr->m.nalign,1,"t_r.m.nalign");
    pStr = clDbg.PutToStr(pStr, pr->m.dci.Signed,1,"t_r.m.dci.signed");
    pStr = clDbg.PutToStr(pStr, pr->m.dci.enaddr,1,"t_r.m.dci.enaddr");
    pStr = clDbg.PutToStr(pStr, pr->m.dci.read,1,"t_r.m.dci.read");
    pStr = clDbg.PutToStr(pStr, pr->m.dci.write,1,"t_r.m.dci.write");
    pStr = clDbg.PutToStr(pStr, pr->m.dci.lock,1,"t_r.m.dci.lock");
    pStr = clDbg.PutToStr(pStr, pr->m.dci.dsuen,1,"t_r.m.dci.dsuen");
    pStr = clDbg.PutToStr(pStr, pr->m.dci.size,2,"t_r.m.dci.size");
    pStr = clDbg.PutToStr(pStr, pr->m.dci.asi,8,"t_r.m.dci.asi");
    pStr = clDbg.PutToStr(pStr, pr->m.werr,1,"t_r.m.werr");
    pStr = clDbg.PutToStr(pStr, pr->m.wcwp,1,"t_r.m.wcwp");
    pStr = clDbg.PutToStr(pStr, pr->m.irqen,1,"t_r.m.irqen");
    pStr = clDbg.PutToStr(pStr, pr->m.irqen2,1,"t_r.m.irqen2");
    pStr = clDbg.PutToStr(pStr, pr->m.mac,1,"t_r.m.mac");
    pStr = clDbg.PutToStr(pStr, pr->m.divz,1,"t_r.m.divz");
    pStr = clDbg.PutToStr(pStr, pr->m.su,1,"t_r.m.su");
    pStr = clDbg.PutToStr(pStr, pr->m.mul,1,"t_r.m.mul");
    pStr = clDbg.PutToStr(pStr, pr->m.casa,1,"t_r.m.casa");
    pStr = clDbg.PutToStr(pStr, pr->m.casaz,1,"t_r.m.casaz");

    pStr = clDbg.PutToStr(pStr, pr->x.ctrl.pc>>CFG_PCLOW,32-CFG_PCLOW,"t_r.x.ctrl.pc");//    : pctype;
    pStr = clDbg.PutToStr(pStr, pr->x.ctrl.inst,32,"t_r.x.ctrl.inst");//  : word;
    pStr = clDbg.PutToStr(pStr, pr->x.ctrl.cnt,2,"t_r.x.ctrl.cnt");//   : std_logic_vector(1 downto 0);
    pStr = clDbg.PutToStr(pStr, pr->x.ctrl.rd,RFBITS,"t_r.x.ctrl.rd");//    : rfatype;
    pStr = clDbg.PutToStr(pStr, pr->x.ctrl.tt,6,"t_r.x.ctrl.tt");//    : std_logic_vector(5 downto 0);
    pStr = clDbg.PutToStr(pStr, pr->x.ctrl.trap,1,"t_r.x.ctrl.trap");//  : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->x.ctrl.annul,1,"t_r.x.ctrl.annul");// : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->x.ctrl.wreg,1,"t_r.x.ctrl.wreg");//  : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->x.ctrl.wicc,1,"t_r.x.ctrl.wicc");//  : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->x.ctrl.wy,1,"t_r.x.ctrl.wy");//    : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->x.ctrl.ld,1,"t_r.x.ctrl.ld");//    : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->x.ctrl.pv,1,"t_r.x.ctrl.pv");//    : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->x.ctrl.rett,1,"t_r.x.ctrl.rett");//  : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->x.result,32,"t_r.x.result");
    pStr = clDbg.PutToStr(pStr, pr->x.y,32,"t_r.x.y");
    pStr = clDbg.PutToStr(pStr, pr->x.icc,4,"t_r.x.icc");
    pStr = clDbg.PutToStr(pStr, pr->x.annul_all,1,"t_r.x.annul_all");
    pStr = clDbg.PutToStr(pStr, pr->x.data.arr[0],32,"t_r.x.data(0)");
    pStr = clDbg.PutToStr(pStr, pr->x.data.arr[1],32,"t_r.x.data(1)");
    pStr = clDbg.PutToStr(pStr, pr->x.set,DSETMSB+1,"vector:t_r.x.set");
    pStr = clDbg.PutToStr(pStr, pr->x.mexc,1,"t_r.x.mexc");
    pStr = clDbg.PutToStr(pStr, pr->x.dci.Signed,1,"t_r.x.dci.signed");
    pStr = clDbg.PutToStr(pStr, pr->x.dci.enaddr,1,"t_r.x.dci.enaddr");
    pStr = clDbg.PutToStr(pStr, pr->x.dci.read,1,"t_r.x.dci.read");
    pStr = clDbg.PutToStr(pStr, pr->x.dci.write,1,"t_r.x.dci.write");
    pStr = clDbg.PutToStr(pStr, pr->x.dci.lock,1,"t_r.x.dci.lock");
    pStr = clDbg.PutToStr(pStr, pr->x.dci.dsuen,1,"t_r.x.dci.dsuen");
    pStr = clDbg.PutToStr(pStr, pr->x.dci.size,2,"t_r.x.dci.size");
    pStr = clDbg.PutToStr(pStr, pr->x.dci.asi,8,"t_r.x.dci.asi");
    pStr = clDbg.PutToStr(pStr, pr->x.laddr,2,"t_r.x.laddr");
    pStr = clDbg.PutToStr(pStr, (uint32)pr->x.rstate,2,"--  in_r.x.rstate");
    pStr = clDbg.PutToStr(pStr, pr->x.npc,3,"t_r.x.npc");
    pStr = clDbg.PutToStr(pStr, pr->x.intack,1,"t_r.x.intack");
    pStr = clDbg.PutToStr(pStr, pr->x.ipend,1,"t_r.x.ipend");
    pStr = clDbg.PutToStr(pStr, pr->x.mac,1,"t_r.x.mac");
    pStr = clDbg.PutToStr(pStr, pr->x.debug,1,"t_r.x.debug");
    pStr = clDbg.PutToStr(pStr, pr->x.nerror,1,"t_r.x.nerror");

    pStr = clDbg.PutToStr(pStr, pr->f.pc>>CFG_PCLOW,32-CFG_PCLOW,"t_r.f.pc");
    pStr = clDbg.PutToStr(pStr, pr->f.branch,1,"t_r.f.branch");
    
    pStr = clDbg.PutToStr(pStr, pr->w.s.cwp,NWINLOG2,"t_r.w.s.cwp");//    : cwptype;                             -- current window pointer
    pStr = clDbg.PutToStr(pStr, pr->w.s.icc,4,"t_r.w.s.icc");//    : std_logic_vector(3 downto 0);	  -- integer condition codes
    pStr = clDbg.PutToStr(pStr, pr->w.s.tt,8,"t_r.w.s.tt");//     : std_logic_vector(7 downto 0);	  -- trap type
    pStr = clDbg.PutToStr(pStr, pr->w.s.tba,20,"t_r.w.s.tba");//    : std_logic_vector(19 downto 0);	  -- trap base address
    pStr = clDbg.PutToStr(pStr, pr->w.s.wim,CFG_NWIN,"t_r.w.s.wim");//    : std_logic_vector(NWIN-1 downto 0);   -- window invalid mask
    pStr = clDbg.PutToStr(pStr, pr->w.s.pil,4,"t_r.w.s.pil");//    : std_logic_vector(3 downto 0);	  -- processor interrupt level
    pStr = clDbg.PutToStr(pStr, pr->w.s.ec,1,"t_r.w.s.ec");//   : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->w.s.ef,1,"t_r.w.s.ef");//   : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->w.s.ps,1,"t_r.w.s.ps");//   : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->w.s.s,1,"t_r.w.s.s");//   : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->w.s.et,1,"t_r.w.s.et");//   : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->w.s.y,32,"t_r.w.s.y");//      : word;
    pStr = clDbg.PutToStr(pStr, pr->w.s.asr18,32,"t_r.w.s.asr18");//  : word;
    pStr = clDbg.PutToStr(pStr, pr->w.s.svt,1,"t_r.w.s.svt");//   : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->w.s.dwt,1,"t_r.w.s.dwt");//   : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->w.s.dbp,1,"t_r.w.s.dbp");//   : std_ulogic;
    pStr = clDbg.PutToStr(pStr, pr->w.result,32,"t_r.w.result");

  return pStr;
}
