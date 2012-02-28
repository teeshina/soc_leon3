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


void dbg::soc_leon3_tb(SystemOnChipIO &io)
{
  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';

    ResetPutStr();

    pStr = PutToStr(pStr, io.inNRst, 1, "inNRst");
    pStr = PutToStr(pStr, topLeon3mp.dbgo.arr[0].error, 1, "ch_dbg(0).error");
    pStr = PutToStr(pStr, topLeon3mp.pclLeon3s[0]->pclProc3->clIU3.v.x.nerror, 1, "t_v_x_nerror");
    pStr = PutToStr(pStr, topLeon3mp.pclLeon3s[0]->pclProc3->clIU3.rp.Q.error, 1, "t_rp_error");
    pStr = PutToStr(pStr, topLeon3mp.pclLeon3s[0]->pclProc3->clIU3.dummy, 1, "t_dummy");
    pStr = PutToStr(pStr, topLeon3mp.pclLeon3s[0]->pclProc3->clIU3.rbR.Q.f.pc, 32, "t_r_f_pc");
    pStr = PutToStr(pStr, topLeon3mp.pclLeon3s[0]->pclProc3->clIU3.in_ir.addr, 32, "in_ir_addr");
    pStr = PutToStr(pStr, topLeon3mp.pclLeon3s[0]->pclProc3->clIU3.rbR.Q.a.ctrl.inst, 32, "t_a_ctrl_inst");

    uint32 hrdata=0;    
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


    PrintIndexStr();
    
    *posBench[TB_soc_leon3] << chStr << "\n";
  }
}
