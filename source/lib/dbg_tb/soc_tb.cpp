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
void dbg::soc_leon3_tb(SystemOnChipIO &io)
{
  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';

    ResetPutStr();

    uint32 hrdata=0;
    uint32 iTck = ((pStr, io.jtag.TCK.eClock==SClock::CLK_POSEDGE)||(pStr, io.jtag.TCK.eClock==SClock::CLK_POSITIVE)) ? 1 : 0;
    iu3 *piu3 = &topLeon3mp.pclLeon3s[0]->pclProc3->clIU3;
    
    pStr = PutToStr(pStr, io.inNRst, 1, "inNRst");
    pStr = PutToStr(pStr, io.jtag.nTRST,1,"in_trst"); // in: Test Reset
    pStr = PutToStr(pStr, iTck,1,"in_tck");   // in: Test Clock
    pStr = PutToStr(pStr, io.jtag.TMS,1,"in_tms");   // in: Test Mode State
    pStr = PutToStr(pStr, io.jtag.TDI,1,"in_tdi");   // in: Test Data Input
    pStr = PutToStr(pStr, io.jtag.TDO,1,"ch_tdo");   // out: Test Data Output


    pStr = PutToStr(pStr, topLeon3mp.dbgo.arr[0].error, 1, "ch_dbgo(0).error");
    pStr = PutToStr(pStr, piu3->v.x.nerror, 1, "t_v_x_nerror");
    pStr = PutToStr(pStr, piu3->rp.Q.error, 1, "t_rp_error");
    pStr = PutToStr(pStr, piu3->rbR.Q.a.ctrl.inst, 32, "t_a_ctrl_inst");
    pStr = PutToStr(pStr, piu3->dummy, 1, "t_dummy");
    pStr = PutToStr(pStr, piu3->rbR.Q.f.pc>>2, 30, "t_pc");
    pStr = PutToStr(pStr, piu3->ir.Q.addr>>2, 30, "t_npc");

    hrdata = (15<<28) | (3<<24) | //(IMPL<<28) | (VER<<24) |
          (piu3->rbR.Q.w.s.icc<<20) | (piu3->rbR.Q.w.s.ec<<13) | (piu3->rbR.Q.w.s.ef<<12) | (piu3->rbR.Q.w.s.pil<<8) |
          (piu3->rbR.Q.w.s.s<<7) | (piu3->rbR.Q.w.s.ps<<6) | (piu3->rbR.Q.w.s.et<<5) |
          BITS32(piu3->rbR.Q.w.s.cwp,NWINLOG2-1,0);
    pStr = PutToStr(pStr, hrdata, 32, "t_psr");

    hrdata = (piu3->rbR.Q.w.s.tba<<12) | (piu3->rbR.Q.w.s.tt<<4);
    pStr = PutToStr(pStr, hrdata, 32, "t_tbr");

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

    pStr = PutToStr(pStr, topLeon3mp.pclLeon3s[0]->pclProc3->ico.data.arr[0], 32, "t_ico0_data");
    pStr = PutToStr(pStr, topLeon3mp.pclLeon3s[0]->pclProc3->pclCacheMMU->mcio.data,32,"t_icramo_data");
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Mst.hrdata, 32, "t_hrdata");
    pStr = PutToStr(pStr, topLeon3mp.stMst2Ctrl.arr[AHB_MASTER_JTAG].haddr, 32, "jtag_haddr");

    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.clJTagPad.rResetLogic.Q, 1, "tResetLogic");
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.clJTagPad.rRunTestIdle.Q, 1, "tRunTestIdle");

    pStr = PrintAllRegIU(pStr, &piu3->rbR.Q);

    PrintIndexStr();
    
    *posBench[TB_soc_leon3] << chStr << "\n";
  }
}

//****************************************************************************
char *dbg::PrintAllRegIU(char *pStr, registers *pr)
{
    pStr = PutToStr(pStr, pr->d.pc>>CFG_PCLOW,32-CFG_PCLOW,"t_r.d.pc");
    pStr = PutToStr(pStr, pr->d.inst.arr[0],32,"t_r.d.inst(0)");
    pStr = PutToStr(pStr, pr->d.inst.arr[1],32,"t_r.d.inst(1)");
    pStr = PutToStr(pStr, pr->d.cwp,NWINLOG2,"t_r.d.cwp");
    pStr = PutToStr(pStr, pr->d.set,ISETMSB+1,"t_r.d.set");
    pStr = PutToStr(pStr, pr->d.mexc,1,"t_r.d.mexc");
    pStr = PutToStr(pStr, pr->d.cnt,2,"t_r.d.cnt");
    pStr = PutToStr(pStr, pr->d.pv,1,"t_r.d.pv");
    pStr = PutToStr(pStr, pr->d.annul,1,"t_r.d.annul");
    pStr = PutToStr(pStr, pr->d.inull,1,"t_r.d.inull");
    pStr = PutToStr(pStr, pr->d.step,1,"t_r.d.step");
    pStr = PutToStr(pStr, pr->d.divrdy,1,"t_r.d.divrdy");

    pStr = PutToStr(pStr, pr->a.ctrl.pc>>CFG_PCLOW,32-CFG_PCLOW,"t_r.a.ctrl.pc");//    : pctype;
    pStr = PutToStr(pStr, pr->a.ctrl.inst,32,"t_r.a.ctrl.inst");//  : word;
    pStr = PutToStr(pStr, pr->a.ctrl.cnt,2,"t_r.a.ctrl.cnt");//   : std_logic_vector(1 downto 0);
    pStr = PutToStr(pStr, pr->a.ctrl.rd,RFBITS,"t_r.a.ctrl.rd");//    : rfatype;
    pStr = PutToStr(pStr, pr->a.ctrl.tt,6,"t_r.a.ctrl.tt");//    : std_logic_vector(5 downto 0);
    pStr = PutToStr(pStr, pr->a.ctrl.trap,1,"t_r.a.ctrl.trap");//  : std_ulogic;
    pStr = PutToStr(pStr, pr->a.ctrl.annul,1,"t_r.a.ctrl.annul");// : std_ulogic;
    pStr = PutToStr(pStr, pr->a.ctrl.wreg,1,"t_r.a.ctrl.wreg");//  : std_ulogic;
    pStr = PutToStr(pStr, pr->a.ctrl.wicc,1,"t_r.a.ctrl.wicc");//  : std_ulogic;
    pStr = PutToStr(pStr, pr->a.ctrl.wy,1,"t_r.a.ctrl.wy");//    : std_ulogic;
    pStr = PutToStr(pStr, pr->a.ctrl.ld,1,"t_r.a.ctrl.ld");//    : std_ulogic;
    pStr = PutToStr(pStr, pr->a.ctrl.pv,1,"t_r.a.ctrl.pv");//    : std_ulogic;
    pStr = PutToStr(pStr, pr->a.ctrl.rett,1,"t_r.a.ctrl.rett");//  : std_ulogic;
    pStr = PutToStr(pStr, pr->a.rs1,5,"t_r.a.rs1");
    pStr = PutToStr(pStr, pr->a.rfa1,RFBITS,"t_r.a.rfa1");
    pStr = PutToStr(pStr, pr->a.rfa2,RFBITS,"t_r.a.rfa2");
    pStr = PutToStr(pStr, pr->a.rsel1,3,"t_r.a.rsel1");
    pStr = PutToStr(pStr, pr->a.rsel2,3,"t_r.a.rsel2");
    pStr = PutToStr(pStr, pr->a.rfe1,1,"t_r.a.rfe1");
    pStr = PutToStr(pStr, pr->a.rfe2,1,"t_r.a.rfe2");
    pStr = PutToStr(pStr, pr->a.cwp,NWINLOG2,"t_r.a.cwp");
    pStr = PutToStr(pStr, pr->a.imm,32,"t_r.a.imm");
    pStr = PutToStr(pStr, pr->a.ldcheck1,1,"t_r.a.ldcheck1");
    pStr = PutToStr(pStr, pr->a.ldcheck2,1,"t_r.a.ldcheck2");
    pStr = PutToStr(pStr, pr->a.ldchkra,1,"t_r.a.ldchkra");
    pStr = PutToStr(pStr, pr->a.ldchkex,1,"t_r.a.ldchkex");
    pStr = PutToStr(pStr, pr->a.su,1,"t_r.a.su");
    pStr = PutToStr(pStr, pr->a.et,1,"t_r.a.et");
    pStr = PutToStr(pStr, pr->a.wovf,1,"t_r.a.wovf");
    pStr = PutToStr(pStr, pr->a.wunf,1,"t_r.a.wunf");
    pStr = PutToStr(pStr, pr->a.ticc,1,"t_r.a.ticc");
    pStr = PutToStr(pStr, pr->a.jmpl,1,"t_r.a.jmpl");
    pStr = PutToStr(pStr, pr->a.step,1,"t_r.a.step");
    pStr = PutToStr(pStr, pr->a.mulstart,1,"t_r.a.mulstart");
    pStr = PutToStr(pStr, pr->a.divstart,1,"t_r.a.divstart");
    pStr = PutToStr(pStr, pr->a.bp,1,"t_r.a.bp");
    pStr = PutToStr(pStr, pr->a.nobp,1,"t_r.a.nobp");

    pStr = PutToStr(pStr, pr->e.ctrl.pc>>CFG_PCLOW,32-CFG_PCLOW,"t_r.e.ctrl.pc");//    : pctype;
    pStr = PutToStr(pStr, pr->e.ctrl.inst,32,"t_r.e.ctrl.inst");//  : word;
    pStr = PutToStr(pStr, pr->e.ctrl.cnt,2,"t_r.e.ctrl.cnt");//   : std_logic_vector(1 downto 0);
    pStr = PutToStr(pStr, pr->e.ctrl.rd,RFBITS,"t_r.e.ctrl.rd");//    : rfatype;
    pStr = PutToStr(pStr, pr->e.ctrl.tt,6,"t_r.e.ctrl.tt");//    : std_logic_vector(5 downto 0);
    pStr = PutToStr(pStr, pr->e.ctrl.trap,1,"t_r.e.ctrl.trap");//  : std_ulogic;
    pStr = PutToStr(pStr, pr->e.ctrl.annul,1,"t_r.e.ctrl.annul");// : std_ulogic;
    pStr = PutToStr(pStr, pr->e.ctrl.wreg,1,"t_r.e.ctrl.wreg");//  : std_ulogic;
    pStr = PutToStr(pStr, pr->e.ctrl.wicc,1,"t_r.e.ctrl.wicc");//  : std_ulogic;
    pStr = PutToStr(pStr, pr->e.ctrl.wy,1,"t_r.e.ctrl.wy");//    : std_ulogic;
    pStr = PutToStr(pStr, pr->e.ctrl.ld,1,"t_r.e.ctrl.ld");//    : std_ulogic;
    pStr = PutToStr(pStr, pr->e.ctrl.pv,1,"t_r.e.ctrl.pv");//    : std_ulogic;
    pStr = PutToStr(pStr, pr->e.ctrl.rett,1,"t_r.e.ctrl.rett");//  : std_ulogic;
    pStr = PutToStr(pStr, pr->e.op1,32,"t_r.e.op1");//    : word;
    pStr = PutToStr(pStr, pr->e.op2,32,"t_r.e.op2");//    : word;
    pStr = PutToStr(pStr, pr->e.aluop,3,"t_r.e.aluop");//  : std_logic_vector(2 downto 0);  	-- Alu operation
    pStr = PutToStr(pStr, pr->e.alusel,2,"t_r.e.alusel");// : std_logic_vector(1 downto 0);  	-- Alu result select
    pStr = PutToStr(pStr, pr->e.aluadd,1,"t_r.e.aluadd");// : std_ulogic;
    pStr = PutToStr(pStr, pr->e.alucin,1,"t_r.e.alucin");// : std_ulogic;
    pStr = PutToStr(pStr, pr->e.ldbp1,1,"t_r.e.ldbp1");
    pStr = PutToStr(pStr, pr->e.ldbp2,1,"t_r.e.ldbp2");// : std_ulogic;
    pStr = PutToStr(pStr, pr->e.invop2,1,"t_r.e.invop2");// : std_ulogic;
    pStr = PutToStr(pStr, pr->e.shcnt,5,"t_r.e.shcnt");//  : std_logic_vector(4 downto 0);  	-- shift count
    pStr = PutToStr(pStr, pr->e.sari,1,"t_r.e.sari");//   : std_ulogic;				-- shift msb
    pStr = PutToStr(pStr, pr->e.shleft,1,"t_r.e.shleft");// : std_ulogic;				-- shift left/right
    pStr = PutToStr(pStr, pr->e.ymsb,1,"t_r.e.ymsb");//   : std_ulogic;				-- shift left/right
    pStr = PutToStr(pStr, pr->e.rd,5,"t_r.e.rd");// 	   : std_logic_vector(4 downto 0);
    pStr = PutToStr(pStr, pr->e.jmpl,1,"t_r.e.jmpl");//   : std_ulogic;
    pStr = PutToStr(pStr, pr->e.su,1,"t_r.e.su");//     : std_ulogic;
    pStr = PutToStr(pStr, pr->e.et,1,"t_r.e.et");//     : std_ulogic;
    pStr = PutToStr(pStr, pr->e.cwp,NWINLOG2,"t_r.e.cwp");//    : cwptype;
    pStr = PutToStr(pStr, pr->e.icc,4,"t_r.e.icc");//    : std_logic_vector(3 downto 0);
    pStr = PutToStr(pStr, pr->e.mulstep,1,"t_r.e.mulstep");//: std_ulogic;            
    pStr = PutToStr(pStr, pr->e.mul,1,"t_r.e.mul");//    : std_ulogic;            
    pStr = PutToStr(pStr, pr->e.mac,1,"t_r.e.mac");//    : std_ulogic;
    pStr = PutToStr(pStr, pr->e.bp,1,"t_r.e.bp");//     : std_ulogic;

    pStr = PutToStr(pStr, pr->m.ctrl.pc>>CFG_PCLOW,32-CFG_PCLOW,"t_r.m.ctrl.pc");//    : pctype;
    pStr = PutToStr(pStr, pr->m.ctrl.inst,32,"t_r.m.ctrl.inst");//  : word;
    pStr = PutToStr(pStr, pr->m.ctrl.cnt,2,"t_r.m.ctrl.cnt");//   : std_logic_vector(1 downto 0);
    pStr = PutToStr(pStr, pr->m.ctrl.rd,RFBITS,"t_r.m.ctrl.rd");//    : rfatype;
    pStr = PutToStr(pStr, pr->m.ctrl.tt,6,"t_r.m.ctrl.tt");//    : std_logic_vector(5 downto 0);
    pStr = PutToStr(pStr, pr->m.ctrl.trap,1,"t_r.m.ctrl.trap");//  : std_ulogic;
    pStr = PutToStr(pStr, pr->m.ctrl.annul,1,"t_r.m.ctrl.annul");// : std_ulogic;
    pStr = PutToStr(pStr, pr->m.ctrl.wreg,1,"t_r.m.ctrl.wreg");//  : std_ulogic;
    pStr = PutToStr(pStr, pr->m.ctrl.wicc,1,"t_r.m.ctrl.wicc");//  : std_ulogic;
    pStr = PutToStr(pStr, pr->m.ctrl.wy,1,"t_r.m.ctrl.wy");//    : std_ulogic;
    pStr = PutToStr(pStr, pr->m.ctrl.ld,1,"t_r.m.ctrl.ld");//    : std_ulogic;
    pStr = PutToStr(pStr, pr->m.ctrl.pv,1,"t_r.m.ctrl.pv");//    : std_ulogic;
    pStr = PutToStr(pStr, pr->m.ctrl.rett,1,"t_r.m.ctrl.rett");//  : std_ulogic;
    pStr = PutToStr(pStr, pr->m.result,32,"t_r.m.result");
    pStr = PutToStr(pStr, pr->m.y,32,"t_r.m.y");
    pStr = PutToStr(pStr, pr->m.icc,4,"t_r.m.icc");
    pStr = PutToStr(pStr, pr->m.nalign,1,"t_r.m.nalign");
    pStr = PutToStr(pStr, pr->m.dci.Signed,1,"t_r.m.dci.signed");
    pStr = PutToStr(pStr, pr->m.dci.enaddr,1,"t_r.m.dci.enaddr");
    pStr = PutToStr(pStr, pr->m.dci.read,1,"t_r.m.dci.read");
    pStr = PutToStr(pStr, pr->m.dci.write,1,"t_r.m.dci.write");
    pStr = PutToStr(pStr, pr->m.dci.lock,1,"t_r.m.dci.lock");
    pStr = PutToStr(pStr, pr->m.dci.dsuen,1,"t_r.m.dci.dsuen");
    pStr = PutToStr(pStr, pr->m.dci.size,2,"t_r.m.dci.size");
    pStr = PutToStr(pStr, pr->m.dci.asi,8,"t_r.m.dci.asi");
    pStr = PutToStr(pStr, pr->m.werr,1,"t_r.m.werr");
    pStr = PutToStr(pStr, pr->m.wcwp,1,"t_r.m.wcwp");
    pStr = PutToStr(pStr, pr->m.irqen,1,"t_r.m.irqen");
    pStr = PutToStr(pStr, pr->m.irqen2,1,"t_r.m.irqen2");
    pStr = PutToStr(pStr, pr->m.mac,1,"t_r.m.mac");
    pStr = PutToStr(pStr, pr->m.divz,1,"t_r.m.divz");
    pStr = PutToStr(pStr, pr->m.su,1,"t_r.m.su");
    pStr = PutToStr(pStr, pr->m.mul,1,"t_r.m.mul");
    pStr = PutToStr(pStr, pr->m.casa,1,"t_r.m.casa");
    pStr = PutToStr(pStr, pr->m.casaz,1,"t_r.m.casaz");

    pStr = PutToStr(pStr, pr->x.ctrl.pc>>CFG_PCLOW,32-CFG_PCLOW,"t_r.x.ctrl.pc");//    : pctype;
    pStr = PutToStr(pStr, pr->x.ctrl.inst,32,"t_r.x.ctrl.inst");//  : word;
    pStr = PutToStr(pStr, pr->x.ctrl.cnt,2,"t_r.x.ctrl.cnt");//   : std_logic_vector(1 downto 0);
    pStr = PutToStr(pStr, pr->x.ctrl.rd,RFBITS,"t_r.x.ctrl.rd");//    : rfatype;
    pStr = PutToStr(pStr, pr->x.ctrl.tt,6,"t_r.x.ctrl.tt");//    : std_logic_vector(5 downto 0);
    pStr = PutToStr(pStr, pr->x.ctrl.trap,1,"t_r.x.ctrl.trap");//  : std_ulogic;
    pStr = PutToStr(pStr, pr->x.ctrl.annul,1,"t_r.x.ctrl.annul");// : std_ulogic;
    pStr = PutToStr(pStr, pr->x.ctrl.wreg,1,"t_r.x.ctrl.wreg");//  : std_ulogic;
    pStr = PutToStr(pStr, pr->x.ctrl.wicc,1,"t_r.x.ctrl.wicc");//  : std_ulogic;
    pStr = PutToStr(pStr, pr->x.ctrl.wy,1,"t_r.x.ctrl.wy");//    : std_ulogic;
    pStr = PutToStr(pStr, pr->x.ctrl.ld,1,"t_r.x.ctrl.ld");//    : std_ulogic;
    pStr = PutToStr(pStr, pr->x.ctrl.pv,1,"t_r.x.ctrl.pv");//    : std_ulogic;
    pStr = PutToStr(pStr, pr->x.ctrl.rett,1,"t_r.x.ctrl.rett");//  : std_ulogic;
    pStr = PutToStr(pStr, pr->x.result,32,"t_r.x.result");
    pStr = PutToStr(pStr, pr->x.y,32,"t_r.x.y");
    pStr = PutToStr(pStr, pr->x.icc,4,"t_r.x.icc");
    pStr = PutToStr(pStr, pr->x.annul_all,1,"t_r.x.annul_all");
    pStr = PutToStr(pStr, pr->x.data.arr[0],32,"t_r.x.data(0)");
    pStr = PutToStr(pStr, pr->x.data.arr[1],32,"t_r.x.data(1)");
    pStr = PutToStr(pStr, pr->x.set,DSETMSB+1,"t_r.x.set");
    pStr = PutToStr(pStr, pr->x.mexc,1,"t_r.x.mexc");
    pStr = PutToStr(pStr, pr->x.dci.Signed,1,"t_r.x.dci.signed");
    pStr = PutToStr(pStr, pr->x.dci.enaddr,1,"t_r.x.dci.enaddr");
    pStr = PutToStr(pStr, pr->x.dci.read,1,"t_r.x.dci.read");
    pStr = PutToStr(pStr, pr->x.dci.write,1,"t_r.x.dci.write");
    pStr = PutToStr(pStr, pr->x.dci.lock,1,"t_r.x.dci.lock");
    pStr = PutToStr(pStr, pr->x.dci.dsuen,1,"t_r.x.dci.dsuen");
    pStr = PutToStr(pStr, pr->x.dci.size,2,"t_r.x.dci.size");
    pStr = PutToStr(pStr, pr->x.dci.asi,8,"t_r.x.dci.asi");
    pStr = PutToStr(pStr, pr->x.laddr,2,"t_r.x.laddr");
    pStr = PutToStr(pStr, (uint32)pr->x.rstate,2,"--  in_r.x.rstate");
    pStr = PutToStr(pStr, pr->x.npc,3,"t_r.x.npc");
    pStr = PutToStr(pStr, pr->x.intack,1,"t_r.x.intack");
    pStr = PutToStr(pStr, pr->x.ipend,1,"t_r.x.ipend");
    pStr = PutToStr(pStr, pr->x.mac,1,"t_r.x.mac");
    pStr = PutToStr(pStr, pr->x.debug,1,"t_r.x.debug");
    pStr = PutToStr(pStr, pr->x.nerror,1,"t_r.x.nerror");

    pStr = PutToStr(pStr, pr->f.pc>>CFG_PCLOW,32-CFG_PCLOW,"t_r.f.pc");
    pStr = PutToStr(pStr, pr->w.s.cwp,NWINLOG2,"t_r.w.s.cwp");//    : cwptype;                             -- current window pointer
    pStr = PutToStr(pStr, pr->w.s.icc,4,"t_r.w.s.icc");//    : std_logic_vector(3 downto 0);	  -- integer condition codes
    pStr = PutToStr(pStr, pr->w.s.tt,8,"t_r.w.s.tt");//     : std_logic_vector(7 downto 0);	  -- trap type
    pStr = PutToStr(pStr, pr->w.s.tba,20,"t_r.w.s.tba");//    : std_logic_vector(19 downto 0);	  -- trap base address
    pStr = PutToStr(pStr, pr->w.s.wim,CFG_NWIN,"t_r.w.s.wim");//    : std_logic_vector(NWIN-1 downto 0);   -- window invalid mask
    pStr = PutToStr(pStr, pr->w.s.pil,4,"t_r.w.s.pil");//    : std_logic_vector(3 downto 0);	  -- processor interrupt level
    pStr = PutToStr(pStr, pr->w.s.ec,1,"t_r.w.s.ec");//   : std_ulogic;
    pStr = PutToStr(pStr, pr->w.s.ef,1,"t_r.w.s.ef");//   : std_ulogic;
    pStr = PutToStr(pStr, pr->w.s.ps,1,"t_r.w.s.ps");//   : std_ulogic;
    pStr = PutToStr(pStr, pr->w.s.s,1,"t_r.w.s.s");//   : std_ulogic;
    pStr = PutToStr(pStr, pr->w.s.et,1,"t_r.w.s.et");//   : std_ulogic;
    pStr = PutToStr(pStr, pr->w.s.y,32,"t_r.w.s.y");//      : word;
    pStr = PutToStr(pStr, pr->w.s.asr18,32,"t_r.w.s.asr18");//  : word;
    pStr = PutToStr(pStr, pr->w.s.svt,1,"t_r.w.s.svt");//   : std_ulogic;
    pStr = PutToStr(pStr, pr->w.s.dwt,1,"t_r.w.s.dwt");//   : std_ulogic;
    pStr = PutToStr(pStr, pr->w.s.dbp,1,"t_r.w.s.dbp");//   : std_ulogic;
    pStr = PutToStr(pStr, pr->w.result,32,"t_r.w.result");

  return pStr;
}
