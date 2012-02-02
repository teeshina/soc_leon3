#include "lheaders.h"

void dbg::ahbmst_tb(SystemOnChipIO &io)
{
  if(io.inClk.eClock==SClock::CLK_POSEDGE)
  {
    pStr = chStr;
    chStr[0] = '\0';

    pStr = PutToStr(pStr, io.inNRst, 1);
    //DMA input:
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jcomout_dmai.address, 32);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jcomout_dmai.wdata, 32);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jcomout_dmai.start, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jcomout_dmai.burst, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jcomout_dmai.write, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jcomout_dmai.busy, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jcomout_dmai.irq, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.jcomout_dmai.size, 3);
    // out_ahbo
    pStr = PutToStr(pStr, topLeon3mp.stMst2Ctrl[AHB_MASTER_JTAG].hbusreq,1);// ;                           -- bus request
    pStr = PutToStr(pStr, topLeon3mp.stMst2Ctrl[AHB_MASTER_JTAG].hlock,1);// ;                           -- lock request
    pStr = PutToStr(pStr, topLeon3mp.stMst2Ctrl[AHB_MASTER_JTAG].htrans,2);//[1:0]  : (1 downto 0);   -- transfer type
    pStr = PutToStr(pStr, topLeon3mp.stMst2Ctrl[AHB_MASTER_JTAG].haddr,32);//[31:0] : (31 downto 0);  -- address bus (byte)
    pStr = PutToStr(pStr, topLeon3mp.stMst2Ctrl[AHB_MASTER_JTAG].hwrite,1);//  ;                           -- read/write
    pStr = PutToStr(pStr, topLeon3mp.stMst2Ctrl[AHB_MASTER_JTAG].hsize,3);//[2:0] : (2 downto 0);   -- transfer size
    pStr = PutToStr(pStr, topLeon3mp.stMst2Ctrl[AHB_MASTER_JTAG].hburst,3);//[2:0]  : (2 downto 0);   -- burst type
    pStr = PutToStr(pStr, topLeon3mp.stMst2Ctrl[AHB_MASTER_JTAG].hprot,4);//[3:0] : (3 downto 0);   -- protection control
    pStr = PutToStr(pStr, topLeon3mp.stMst2Ctrl[AHB_MASTER_JTAG].hwdata,32);//[31:0]  : (AHBDW-1 downto 0);   -- write data bus
    pStr = PutToStr(pStr, topLeon3mp.stMst2Ctrl[AHB_MASTER_JTAG].hirq,32);//[31:0]    : (AHB_IRQ_MAX-1 downto 0); -- interrupt bus
    pStr = PutToStr(pStr, topLeon3mp.stMst2Ctrl[AHB_MASTER_JTAG].hconfig.arr[0],32);//   : ahb_config_type;      -- memory access reg.
    pStr = PutToStr(pStr, topLeon3mp.stMst2Ctrl[AHB_MASTER_JTAG].hindex, 16);//[0:15]    : integer range 0 to AHB_MASTERS_MAX-1;   -- diagnostic use only

    // AHB response: in_ahbi
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Mst.hgrant, 16);
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Mst.hready, 1);
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Mst.hresp, 2);
    pStr = PutToStr(pStr, topLeon3mp.stCtrl2Mst.hrdata, 32);

    // DMA output:  chk_dmao
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.ahbmstout_dmao.start, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.ahbmstout_dmao.active, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.ahbmstout_dmao.ready, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.ahbmstout_dmao.retry, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.ahbmstout_dmao.mexc, 1);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.ahbmstout_dmao.haddr, 10);
    pStr = PutToStr(pStr, topLeon3mp.clAhbMasterJtag.ahbmstout_dmao.rdata, 32);

    
    *posBench[TB_AHBMASTER] << chStr << "\n";
  }
}
