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
void dbg::ahbmst_tb(SystemOnChipIO &io)
{
  ahb_dma_in_type *pin_dmai = &topLeon3mp.clAhbMasterJtag.jcomout_dmai;
  ahb_dma_out_type *pch_dmao = &topLeon3mp.clAhbMasterJtag.ahbmstout_dmao;
  ahb_mst_in_type *pin_ahbi = &topLeon3mp.stCtrl2Mst;
  ahb_mst_out_type  *pch_ahbo = &topLeon3mp.stMst2Ctrl.arr[AHB_MASTER_JTAG];

  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';
    ResetPutStr();
  
    // inputs:
    pStr = PutToStr(pStr, io.inNRst, 1, "inNRst");
    //
    pStr = PutToStr(pStr, pin_dmai->address, 32, "in_dmai.address");
    pStr = PutToStr(pStr, pin_dmai->wdata, 32, "in_dmai.wdata");
    pStr = PutToStr(pStr, pin_dmai->start, 1, "in_dmai.start");
    pStr = PutToStr(pStr, pin_dmai->burst, 1, "in_dmai.burst");
    pStr = PutToStr(pStr, pin_dmai->write, 1, "in_dmai.write");
    pStr = PutToStr(pStr, pin_dmai->busy, 1, "in_dmai.busy");
    pStr = PutToStr(pStr, pin_dmai->irq, 1, "in_dmai.irq");
    pStr = PutToStr(pStr, pin_dmai->size, 3, "in_dmai.size");
    //
    pStr = PutToStr(pStr, pin_ahbi->hgrant, 16,"in_ahbi.hgrant",true);
    pStr = PutToStr(pStr, pin_ahbi->hready, 1,"in_ahbi.hready");
    pStr = PutToStr(pStr, pin_ahbi->hresp, 2,"in_ahbi.hresp");
    pStr = PutToStr(pStr, pin_ahbi->hrdata, 32,"in_ahbi.hrdata");

    // Outputs:
    pStr = PutToStr(pStr, pch_dmao->start, 1,"ch_dmao.start");
    pStr = PutToStr(pStr, pch_dmao->active, 1,"ch_dmao.active");
    pStr = PutToStr(pStr, pch_dmao->ready, 1,"ch_dmao.ready");
    pStr = PutToStr(pStr, pch_dmao->retry, 1,"ch_dmao.retry");
    pStr = PutToStr(pStr, pch_dmao->mexc, 1,"ch_dmao.mexc");
    pStr = PutToStr(pStr, pch_dmao->haddr, 10,"ch_dmao.haddr");
    pStr = PutToStr(pStr, pch_dmao->rdata, 32,"ch_dmao.rdata");
    //
    pStr = PutToStr(pStr, pch_ahbo->hbusreq,1,"ch_ahbo.hbusreq");// ;                           -- bus request
    pStr = PutToStr(pStr, pch_ahbo->hlock,1,"ch_ahbo.hlock");// ;                           -- lock request
    pStr = PutToStr(pStr, pch_ahbo->htrans,2,"ch_ahbo.htrans");//  : (1 downto 0);   -- transfer type
    pStr = PutToStr(pStr, pch_ahbo->haddr,32,"ch_ahbo.haddr");// : (31 downto 0);  -- address bus (byte)
    pStr = PutToStr(pStr, pch_ahbo->hwrite,1,"ch_ahbo.hwrite");//  ;                           -- read/write
    pStr = PutToStr(pStr, pch_ahbo->hsize,3,"ch_ahbo.hsize");// : (2 downto 0);   -- transfer size
    pStr = PutToStr(pStr, pch_ahbo->hburst,3,"ch_ahbo.hburst");//  : (2 downto 0);   -- burst type
    pStr = PutToStr(pStr, pch_ahbo->hprot,4,"ch_ahbo.hprot");// : (3 downto 0);   -- protection control
    pStr = PutToStr(pStr, pch_ahbo->hwdata,32,"ch_ahbo.hwdata");//  : (AHBDW-1 downto 0);   -- write data bus
    pStr = PutToStr(pStr, pch_ahbo->hirq,32,"ch_ahbo.hirq");//    : (AHB_IRQ_MAX-1 downto 0); -- interrupt bus
    pStr = PutToStr(pStr, pch_ahbo->hconfig.arr[0],32,"ch_ahbo.hconfig(0)");//   : ahb_config_type;      -- memory access reg.
    pStr = PutToStr(pStr, pch_ahbo->hconfig.arr[1],32,"ch_ahbo.hconfig(1)");//   : ahb_config_type;      -- memory access reg.
    pStr = PutToStr(pStr, pch_ahbo->hconfig.arr[2],32,"ch_ahbo.hconfig(2)");//   : ahb_config_type;      -- memory access reg.
    pStr = PutToStr(pStr, pch_ahbo->hconfig.arr[3],32,"ch_ahbo.hconfig(3)");//   : ahb_config_type;      -- memory access reg.
    pStr = PutToStr(pStr, pch_ahbo->hconfig.arr[4],32,"ch_ahbo.hconfig(4)");//   : ahb_config_type;      -- memory access reg.
    pStr = PutToStr(pStr, pch_ahbo->hconfig.arr[5],32,"ch_ahbo.hconfig(5)");//   : ahb_config_type;      -- memory access reg.
    pStr = PutToStr(pStr, pch_ahbo->hconfig.arr[6],32,"ch_ahbo.hconfig(6)");//   : ahb_config_type;      -- memory access reg.
    pStr = PutToStr(pStr, pch_ahbo->hconfig.arr[7],32,"ch_ahbo.hconfig(7)");//   : ahb_config_type;      -- memory access reg.
    pStr = PutToStr(pStr, pch_ahbo->hindex, 4,"conv_integer:ch_ahbo.hindex");//    : integer range 0 to AHB_MASTERS_MAX-1;   -- diagnostic use only

    PrintIndexStr();
    
    *posBench[TB_ahbmaster] << chStr << "\n";
  }
}
