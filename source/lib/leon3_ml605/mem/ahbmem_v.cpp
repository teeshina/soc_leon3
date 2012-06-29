#include "id.h"
#include "ahbmem_v.h"

//****************************************************************************
AhbSlaveMem::AhbSlaveMem(uint32 hindex_,
                         uint32 uiVendor_,
                         uint32 uiDevId_,
                         uint32 uiMemaddr_,
                         uint32 uiAddrmask_
                        )
{
  hindex     = hindex_;
  uiVendor   = uiVendor_;
  uiDevId    = uiDevId_;
  uiMemaddr  = uiMemaddr_;
  uiAddrmask = uiAddrmask_;

  // debug memory init:
  for (int32 i=0; i<RAM_SIZE; i++)
    arrMem[i] = 0x500 + 4*i;
}

//****************************************************************************
void AhbSlaveMem::Update( uint32 inNRst,
                          SClock inClk,
                          ahb_slv_in_type   inCtrl2Slv,
                          ahb_slv_out_type  &outSlv2Ctrl )
{
  ((ahb_device_reg*)(&outSlv2Ctrl.hconfig.arr[0]))->vendor  = uiVendor;
  ((ahb_device_reg*)(&outSlv2Ctrl.hconfig.arr[0]))->device  = uiDevId;
  ((ahb_device_reg*)(&outSlv2Ctrl.hconfig.arr[0]))->version = 1;
  ((ahb_membar_type*)(&outSlv2Ctrl.hconfig.arr[4]))->memaddr  = uiMemaddr;
  ((ahb_membar_type*)(&outSlv2Ctrl.hconfig.arr[4]))->addrmask = uiAddrmask;
  ((ahb_membar_type*)(&outSlv2Ctrl.hconfig.arr[4]))->area_id  = CFGAREA_TYPE_AHB_MEM;
  ((ahb_membar_type*)(&outSlv2Ctrl.hconfig.arr[4]))->prefetch = 0;
  ((ahb_membar_type*)(&outSlv2Ctrl.hconfig.arr[4]))->cache    = 0;


  uint32 wBitSel = BIT32(inCtrl2Slv.hsel,hindex);
  uint32 wTransSel = ((inCtrl2Slv.htrans==HTRANS_NONSEQ)||(inCtrl2Slv.htrans==HTRANS_SEQ))? 1: 0;
  
  // slave has been selected:
  uint32 wbAdr = (inCtrl2Slv.haddr&RAM_ADDR_MASK)>>2;

  rbData.CLK = inClk;
  if(!inNRst)                                       rbData.D = 0;
  else if(wBitSel & wTransSel & inCtrl2Slv.hready)  rbData.D = arrMem[wbAdr];


  if(inClk.eClock==SClock::CLK_POSEDGE)
  {
    if(wBitSel & wTransSel & inCtrl2Slv.hready & inCtrl2Slv.hwrite) 
      arrMem[wbAdr] = inCtrl2Slv.hwdata;
  }



  // always 1, cause memory rd/wr per 1 clock:
  // Warning!! Gaisler use 2 clock writting
  outSlv2Ctrl.hready = 1;

  outSlv2Ctrl.hindex = hindex;
  outSlv2Ctrl.hrdata = rbData.Q;
  outSlv2Ctrl.hresp  = HRESP_OKAY;
  outSlv2Ctrl.hirq   = 0;
  outSlv2Ctrl.hsplit = 0;


}

