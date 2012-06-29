#include "ldbg.h"
#include "leon3_ml605\leon3mp.h"
extern leon3mp topLeon3mp;

//#define DBG_cachemem

#ifdef DBG_cachemem
  cram_in_type in_crami;// : in  cram_in_type;
  cram_out_type ch_cramo;// : out cram_out_type;

  cachemem tst_cachemem;
#endif


//****************************************************************************
void dbg::cachemem_tb(SystemOnChipIO &io)
{
  cachemem *ptst_cachemem = &topLeon3mp.pclLeon3s[0]->clCacheMem;
  cram_in_type *pin_crami = &topLeon3mp.pclLeon3s[0]->crami;// : in  cram_in_type;
  cram_out_type *pch_cramo = &topLeon3mp.pclLeon3s[0]->cramo;// : out cram_out_type;
#ifdef DBG_cachemem
  if(io.inClk.eClock_z==SClock::CLK_POSEDGE)
  {
    //
    in_crami.icramin.address = ((rand()<<15)|rand())&MSK32(19,0);// : std_logic_vector(19 downto 0);
    in_crami.icramin.tag.arr[0] = (rand()<<17)|rand();//     : cdatatype;
    in_crami.icramin.tag.arr[1] = (rand()<<17)|rand();//     : cdatatype;
    in_crami.icramin.tag.arr[2] = (rand()<<17)|rand();//     : cdatatype;
    in_crami.icramin.tag.arr[3] = (rand()<<17)|rand();//     : cdatatype;
    in_crami.icramin.twrite = rand()&0xf;//  : std_logic_vector(0 to 3);
    in_crami.icramin.tenable = rand()&0x1;// : std_ulogic;
    in_crami.icramin.flush = rand()&0x1;//   : std_ulogic;
    in_crami.icramin.data = (rand()<<17)|rand();//    : std_logic_vector(31 downto 0);
    in_crami.icramin.denable = rand()&0x1;// : std_ulogic;
    in_crami.icramin.dwrite = rand()&0xf;//  : std_logic_vector(0 to 3);
    in_crami.icramin.ldramin.enable = rand()&0x1;//        : std_ulogic;                       
    in_crami.icramin.ldramin.read = rand()&0x1;//          : std_ulogic;                         
    in_crami.icramin.ldramin.write = rand()&0x1;//         : std_ulogic;                       
    in_crami.icramin.ctx = rand()&MSK32(M_CTX_SZ-1,0);//           : std_logic_vector(M_CTX_SZ-1 downto 0);
    in_crami.icramin.tpar.arr[0] = rand()&0xf;//          : cpartype;
    in_crami.icramin.tpar.arr[1] = rand()&0xf;
    in_crami.icramin.tpar.arr[2] = rand()&0xf;
    in_crami.icramin.tpar.arr[3] = rand()&0xf;
    in_crami.icramin.dpar = rand()&0xf;//          : std_logic_vector(3 downto 0);
    //
    in_crami.dcramin.address = ((rand()<<15)|rand())&MSK32(19,0);// : std_logic_vector(19 downto 0);
    in_crami.dcramin.tag.arr[0] = (rand()<<17)|rand();//     : cdatatype; //std_logic_vector(31  downto 0);
    in_crami.dcramin.tag.arr[1] = (rand()<<17)|rand();
    in_crami.dcramin.tag.arr[2] = (rand()<<17)|rand();
    in_crami.dcramin.tag.arr[3] = (rand()<<17)|rand();
    in_crami.dcramin.ptag.arr[0] = (rand()<<17)|rand();//      : cdatatype; //std_logic_vector(31  downto 0);
    in_crami.dcramin.ptag.arr[1] = (rand()<<17)|rand();
    in_crami.dcramin.ptag.arr[2] = (rand()<<17)|rand();
    in_crami.dcramin.ptag.arr[3] = (rand()<<17)|rand();
    in_crami.dcramin.twrite = rand()&0xf;//  : std_logic_vector(0 to 3);
    in_crami.dcramin.tpwrite = rand()&0xf;// : std_logic_vector(0 to 3);
    in_crami.dcramin.tenable = rand()&0xf;// : std_logic_vector(0 to 3);
    in_crami.dcramin.flush = rand()&0x1;//   : std_ulogic;
    in_crami.dcramin.data.arr[0] = (rand()<<17)|rand();//    : cdatatype;
    in_crami.dcramin.data.arr[1] = (rand()<<17)|rand();
    in_crami.dcramin.data.arr[2] = (rand()<<17)|rand();
    in_crami.dcramin.data.arr[3] = (rand()<<17)|rand();
    in_crami.dcramin.denable = rand()&0xf;// : std_logic_vector(0 to 3);
    in_crami.dcramin.dwrite = rand()&0xf;//  : std_logic_vector(0 to 3);
    in_crami.dcramin.senable = rand()&0xf;// : std_logic_vector(0 to 3);
    in_crami.dcramin.swrite = rand()&0xf;//  : std_logic_vector(0 to 3);
    in_crami.dcramin.saddress = ((rand()<<15)|rand())&MSK32(19,0);//  : std_logic_vector(19 downto 0);
    in_crami.dcramin.faddress = ((rand()<<15)|rand())&MSK32(19,0);//  : std_logic_vector(19 downto 0);
    in_crami.dcramin.spar = rand()&0x1;//          : std_logic;
    in_crami.dcramin.ldramin.address = ((rand()<<15)|rand())&MSK32(23,2);// : std_logic_vector(23 downto 2);
    in_crami.dcramin.ldramin.enable = rand()&0x1;//  : std_ulogic;
    in_crami.dcramin.ldramin.read = rand()&0x1;//    : std_ulogic;
    in_crami.dcramin.ldramin.write = rand()&0x1;//   : std_ulogic;
    in_crami.dcramin.ctx.arr[0] = rand()&MSK32(M_CTX_SZ-1,0);//           : ctxdatatype;
    in_crami.dcramin.ctx.arr[1] = rand()&MSK32(M_CTX_SZ-1,0);
    in_crami.dcramin.ctx.arr[2] = rand()&MSK32(M_CTX_SZ-1,0);
    in_crami.dcramin.ctx.arr[3] = rand()&MSK32(M_CTX_SZ-1,0);
    in_crami.dcramin.tpar.arr[0] = rand()&0xf;//          : cpartype;
    in_crami.dcramin.tpar.arr[1] = rand()&0xf;
    in_crami.dcramin.tpar.arr[2] = rand()&0xf;
    in_crami.dcramin.tpar.arr[3] = rand()&0xf;
    in_crami.dcramin.dpar.arr[0] = rand()&0xf;//          : cpartype;
    in_crami.dcramin.dpar.arr[1] = rand()&0xf;
    in_crami.dcramin.dpar.arr[2] = rand()&0xf;
    in_crami.dcramin.dpar.arr[3] = rand()&0xf;
    in_crami.dcramin.tdiag = rand()&0xf;//         : std_logic_vector(3 downto 0);
    in_crami.dcramin.ddiag = rand()&0xf;//         : std_logic_vector(3 downto 0);
    in_crami.dcramin.sdiag = rand()&0xf;//         : std_logic_vector(3 downto 0);
  }

  // 
  tst_cachemem.Update(  io.inClk,
                        in_crami,// : in  cram_in_type;
                        ch_cramo,// : out cram_out_type;
                        io.inClk);// : in std_ulogic
  ptst_cachemem = &tst_cachemem;
  pin_crami = &in_crami;
  pch_cramo = &ch_cramo;
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
    pStr = PutToStr(pStr, pin_crami->icramin.address,20,"in_crami.icramin.address");// : std_logic_vector(19 downto 0);
    pStr = PutToStr(pStr, pin_crami->icramin.tag.arr[0],IDBITS,"in_crami.icramin.tag(0)");//     : cdatatype;
    pStr = PutToStr(pStr, pin_crami->icramin.tag.arr[1],IDBITS,"in_crami.icramin.tag(1)");//     : cdatatype;
    pStr = PutToStr(pStr, pin_crami->icramin.tag.arr[2],IDBITS,"--in_crami.icramin.tag(2)");//     : cdatatype;
    pStr = PutToStr(pStr, pin_crami->icramin.tag.arr[3],IDBITS,"--in_crami.icramin.tag(3)");//     : cdatatype;
    pStr = PutToStr(pStr, pin_crami->icramin.twrite,4,"in_crami.icramin.twrite",true);//  : std_logic_vector(0 to 3);
    pStr = PutToStr(pStr, pin_crami->icramin.tenable,1,"in_crami.icramin.tenable");// : std_ulogic;
    pStr = PutToStr(pStr, pin_crami->icramin.flush,1,"in_crami.icramin.flush");//   : std_ulogic;
    pStr = PutToStr(pStr, pin_crami->icramin.data,32,"in_crami.icramin.data");//    : std_logic_vector(31 downto 0);
    pStr = PutToStr(pStr, pin_crami->icramin.denable,1,"in_crami.icramin.denable");// : std_ulogic;
    pStr = PutToStr(pStr, pin_crami->icramin.dwrite,4,"in_crami.icramin.dwrite",true);//  : std_logic_vector(0 to 3);
    pStr = PutToStr(pStr, pin_crami->icramin.ldramin.enable,1,"in_crami.icramin.ldramin.enable");//        : std_ulogic;                       
    pStr = PutToStr(pStr, pin_crami->icramin.ldramin.read,1,"in_crami.icramin.ldramin.read");//          : std_ulogic;                         
    pStr = PutToStr(pStr, pin_crami->icramin.ldramin.write,1,"in_crami.icramin.ldramin.write");//         : std_ulogic;                       
    pStr = PutToStr(pStr, pin_crami->icramin.ctx,M_CTX_SZ,"in_crami.icramin.ctx");//           : std_logic_vector(M_CTX_SZ-1 downto 0);
    pStr = PutToStr(pStr, pin_crami->icramin.tpar.arr[0],4,"--in_crami.icramin.tpar(0)");//          : cpartype;
    pStr = PutToStr(pStr, pin_crami->icramin.tpar.arr[1],4,"--in_crami.icramin.tpar(1)");
    pStr = PutToStr(pStr, pin_crami->icramin.tpar.arr[2],4,"--in_crami.icramin.tpar(2)");
    pStr = PutToStr(pStr, pin_crami->icramin.tpar.arr[3],4,"--in_crami.icramin.tpar(3)");
    pStr = PutToStr(pStr, pin_crami->icramin.dpar,4,"--in_crami.icramin.dpar");//          : std_logic_vector(3 downto 0);
    //
    pStr = PutToStr(pStr, pin_crami->dcramin.address,20,"in_crami.dcramin.address");// : std_logic_vector(19 downto 0);
    pStr = PutToStr(pStr, pin_crami->dcramin.tag.arr[0],IDBITS,"in_crami.dcramin.tag(0)");//     : cdatatype; //std_logic_vector(31  downto 0);
    pStr = PutToStr(pStr, pin_crami->dcramin.tag.arr[1],IDBITS,"in_crami.dcramin.tag(1)");
    pStr = PutToStr(pStr, pin_crami->dcramin.tag.arr[2],IDBITS,"in_crami.dcramin.tag(2)");
    pStr = PutToStr(pStr, pin_crami->dcramin.tag.arr[3],IDBITS,"in_crami.dcramin.tag(3)");
    pStr = PutToStr(pStr, pin_crami->dcramin.ptag.arr[0],IDBITS,"in_crami.dcramin.ptag(0)");//      : cdatatype; //std_logic_vector(31  downto 0);
    pStr = PutToStr(pStr, pin_crami->dcramin.ptag.arr[1],IDBITS,"in_crami.dcramin.ptag(1)");
    pStr = PutToStr(pStr, pin_crami->dcramin.ptag.arr[2],IDBITS,"in_crami.dcramin.ptag(2)");
    pStr = PutToStr(pStr, pin_crami->dcramin.ptag.arr[3],IDBITS,"in_crami.dcramin.ptag(3)");
    pStr = PutToStr(pStr, pin_crami->dcramin.twrite,4,"in_crami.dcramin.twrite",true);//  : std_logic_vector(0 to 3);
    pStr = PutToStr(pStr, pin_crami->dcramin.tpwrite,4,"in_crami.dcramin.tpwrite",true);// : std_logic_vector(0 to 3);
    pStr = PutToStr(pStr, pin_crami->dcramin.tenable,4,"in_crami.dcramin.tenable",true);// : std_logic_vector(0 to 3);
    pStr = PutToStr(pStr, pin_crami->dcramin.flush,1,"in_crami.dcramin.flush");//   : std_ulogic;
    pStr = PutToStr(pStr, pin_crami->dcramin.data.arr[0],IDBITS,"in_crami.dcramin.data(0)");//    : cdatatype;
    pStr = PutToStr(pStr, pin_crami->dcramin.data.arr[1],IDBITS,"in_crami.dcramin.data(1)");
    pStr = PutToStr(pStr, pin_crami->dcramin.data.arr[2],IDBITS,"in_crami.dcramin.data(2)");
    pStr = PutToStr(pStr, pin_crami->dcramin.data.arr[3],IDBITS,"in_crami.dcramin.data(3)");
    pStr = PutToStr(pStr, pin_crami->dcramin.denable,4,"in_crami.dcramin.denable",true);// : std_logic_vector(0 to 3);
    pStr = PutToStr(pStr, pin_crami->dcramin.dwrite,4,"in_crami.dcramin.dwrite",true);//  : std_logic_vector(0 to 3);
    pStr = PutToStr(pStr, pin_crami->dcramin.senable,4,"in_crami.dcramin.senable",true);// : std_logic_vector(0 to 3);
    pStr = PutToStr(pStr, pin_crami->dcramin.swrite,4,"in_crami.dcramin.swrite",true);//  : std_logic_vector(0 to 3);
    pStr = PutToStr(pStr, pin_crami->dcramin.saddress,20,"in_crami.dcramin.saddress");//  : std_logic_vector(19 downto 0);
    pStr = PutToStr(pStr, pin_crami->dcramin.faddress,20,"--ch_dcrami.faddress");//  : std_logic_vector(19 downto 0);
    pStr = PutToStr(pStr, pin_crami->dcramin.spar,1,"--ch_dcrami.spar");//          : std_logic;
    pStr = PutToStr(pStr, pin_crami->dcramin.ldramin.address>>2,24-2,"in_crami.dcramin.ldramin.address");// : std_logic_vector(23 downto 2);
    pStr = PutToStr(pStr, pin_crami->dcramin.ldramin.enable,1,"in_crami.dcramin.ldramin.enable");//  : std_ulogic;
    pStr = PutToStr(pStr, pin_crami->dcramin.ldramin.read,1,"in_crami.dcramin.ldramin.read");//    : std_ulogic;
    pStr = PutToStr(pStr, pin_crami->dcramin.ldramin.write,1,"in_crami.dcramin.ldramin.write");//   : std_ulogic;
    pStr = PutToStr(pStr, pin_crami->dcramin.ctx.arr[0],M_CTX_SZ,"in_crami.dcramin.ctx(0)");//           : ctxdatatype;
    pStr = PutToStr(pStr, pin_crami->dcramin.ctx.arr[1],M_CTX_SZ,"in_crami.dcramin.ctx(1)");
    pStr = PutToStr(pStr, pin_crami->dcramin.ctx.arr[2],M_CTX_SZ,"in_crami.dcramin.ctx(2)");
    pStr = PutToStr(pStr, pin_crami->dcramin.ctx.arr[3],M_CTX_SZ,"in_crami.dcramin.ctx(3)");
    pStr = PutToStr(pStr, pin_crami->dcramin.tpar.arr[0],4,"--ch_dcrami.tpar(0)");//          : cpartype;
    pStr = PutToStr(pStr, pin_crami->dcramin.tpar.arr[1],4,"--ch_dcrami.tpar(1)");
    pStr = PutToStr(pStr, pin_crami->dcramin.tpar.arr[2],4,"--ch_dcrami.tpar(2)");
    pStr = PutToStr(pStr, pin_crami->dcramin.tpar.arr[3],4,"--ch_dcrami.tpar(3)");
    pStr = PutToStr(pStr, pin_crami->dcramin.dpar.arr[0],4,"--ch_dcrami.dpar(0)");//          : cpartype;
    pStr = PutToStr(pStr, pin_crami->dcramin.dpar.arr[1],4,"--ch_dcrami.dpar(1)");
    pStr = PutToStr(pStr, pin_crami->dcramin.dpar.arr[2],4,"--ch_dcrami.dpar(2)");
    pStr = PutToStr(pStr, pin_crami->dcramin.dpar.arr[3],4,"--ch_dcrami.dpar(3)");
    pStr = PutToStr(pStr, pin_crami->dcramin.tdiag,4,"in_crami.dcramin.tdiag");//         : std_logic_vector(3 downto 0);
    pStr = PutToStr(pStr, pin_crami->dcramin.ddiag,4,"in_crami.dcramin.ddiag");//         : std_logic_vector(3 downto 0);
    pStr = PutToStr(pStr, pin_crami->dcramin.sdiag,4,"--ch_dcrami.sdiag");//         : std_logic_vector(3 downto 0);


  
    // Outputs:
    pStr = PutToStr(pStr, pch_cramo->icramo.tag.arr[0],32,"ch_cramo.icramo.tag(0)");//     : cdatatype;
    pStr = PutToStr(pStr, pch_cramo->icramo.tag.arr[1],32,"ch_cramo.icramo.tag(1)");
    pStr = PutToStr(pStr, pch_cramo->icramo.tag.arr[2],32,"ch_cramo.icramo.tag(2)");
    pStr = PutToStr(pStr, pch_cramo->icramo.tag.arr[3],32,"ch_cramo.icramo.tag(3)");
    pStr = PutToStr(pStr, pch_cramo->icramo.data.arr[0],32,"ch_cramo.icramo.data(0)");//      : cdatatype;
    pStr = PutToStr(pStr, pch_cramo->icramo.data.arr[1],32,"ch_cramo.icramo.data(1)");
    pStr = PutToStr(pStr, pch_cramo->icramo.data.arr[2],32,"ch_cramo.icramo.data(2)");
    pStr = PutToStr(pStr, pch_cramo->icramo.data.arr[3],32,"ch_cramo.icramo.data(3)");
    pStr = PutToStr(pStr, pch_cramo->icramo.ctx.arr[0],M_CTX_SZ,"ch_cramo.icramo.ctx(0)");//           : ctxdatatype;
    pStr = PutToStr(pStr, pch_cramo->icramo.ctx.arr[1],M_CTX_SZ,"ch_cramo.icramo.ctx(1)");
    pStr = PutToStr(pStr, pch_cramo->icramo.ctx.arr[2],M_CTX_SZ,"ch_cramo.icramo.ctx(2)");
    pStr = PutToStr(pStr, pch_cramo->icramo.ctx.arr[3],M_CTX_SZ,"ch_cramo.icramo.ctx(3)");
    pStr = PutToStr(pStr, pch_cramo->icramo.tpar.arr[0],4,"ch_cramo.icramo.tpar(0)");//          : cpartype;
    pStr = PutToStr(pStr, pch_cramo->icramo.tpar.arr[1],4,"ch_cramo.icramo.tpar(1)");
    pStr = PutToStr(pStr, pch_cramo->icramo.tpar.arr[2],4,"ch_cramo.icramo.tpar(2)");
    pStr = PutToStr(pStr, pch_cramo->icramo.tpar.arr[3],4,"ch_cramo.icramo.tpar(3)");
    pStr = PutToStr(pStr, pch_cramo->icramo.dpar.arr[0],4,"ch_cramo.icramo.dpar(0)");//          : cpartype;
    pStr = PutToStr(pStr, pch_cramo->icramo.dpar.arr[1],4,"ch_cramo.icramo.dpar(0)");
    pStr = PutToStr(pStr, pch_cramo->icramo.dpar.arr[2],4,"ch_cramo.icramo.dpar(0)");
    pStr = PutToStr(pStr, pch_cramo->icramo.dpar.arr[3],4,"ch_cramo.icramo.dpar(0)");
    //
    pStr = PutToStr(pStr, pch_cramo->dcramo.tag.arr[0],IDBITS,"ch_cramo.dcramo.tag(0)");
    pStr = PutToStr(pStr, pch_cramo->dcramo.tag.arr[1],IDBITS,"ch_cramo.dcramo.tag(1)");
    pStr = PutToStr(pStr, pch_cramo->dcramo.tag.arr[2],IDBITS,"ch_cramo.dcramo.tag(2)");
    pStr = PutToStr(pStr, pch_cramo->dcramo.tag.arr[3],IDBITS,"ch_cramo.dcramo.tag(3)");
    pStr = PutToStr(pStr, pch_cramo->dcramo.data.arr[0],IDBITS,"ch_cramo.dcramo.data(0)");
    pStr = PutToStr(pStr, pch_cramo->dcramo.data.arr[1],IDBITS,"ch_cramo.dcramo.data(1)");
    pStr = PutToStr(pStr, pch_cramo->dcramo.data.arr[2],IDBITS,"ch_cramo.dcramo.data(2)");
    pStr = PutToStr(pStr, pch_cramo->dcramo.data.arr[3],IDBITS,"ch_cramo.dcramo.data(3)");
    pStr = PutToStr(pStr, pch_cramo->dcramo.stag.arr[0],IDBITS,"ch_cramo.dcramo.stag(0)");
    pStr = PutToStr(pStr, pch_cramo->dcramo.stag.arr[1],IDBITS,"ch_cramo.dcramo.stag(1)");
    pStr = PutToStr(pStr, pch_cramo->dcramo.stag.arr[2],IDBITS,"ch_cramo.dcramo.stag(2)");
    pStr = PutToStr(pStr, pch_cramo->dcramo.stag.arr[3],IDBITS,"ch_cramo.dcramo.stag(3)");
    pStr = PutToStr(pStr, pch_cramo->dcramo.ctx.arr[0],M_CTX_SZ,"ch_cramo.dcramo.ctx(0)");
    pStr = PutToStr(pStr, pch_cramo->dcramo.ctx.arr[1],M_CTX_SZ,"ch_cramo.dcramo.ctx(1)");
    pStr = PutToStr(pStr, pch_cramo->dcramo.ctx.arr[2],M_CTX_SZ,"ch_cramo.dcramo.ctx(2)");
    pStr = PutToStr(pStr, pch_cramo->dcramo.ctx.arr[3],M_CTX_SZ,"ch_cramo.dcramo.ctx(3)");
    pStr = PutToStr(pStr, pch_cramo->dcramo.tpar.arr[0],4,"ch_cramo.dcramo.tpar(0)");
    pStr = PutToStr(pStr, pch_cramo->dcramo.tpar.arr[1],4,"ch_cramo.dcramo.tpar(1)");
    pStr = PutToStr(pStr, pch_cramo->dcramo.tpar.arr[2],4,"ch_cramo.dcramo.tpar(2)");
    pStr = PutToStr(pStr, pch_cramo->dcramo.tpar.arr[3],4,"ch_cramo.dcramo.tpar(3)");
    pStr = PutToStr(pStr, pch_cramo->dcramo.dpar.arr[0],4,"ch_cramo.dcramo.dpar(0)");
    pStr = PutToStr(pStr, pch_cramo->dcramo.dpar.arr[1],4,"ch_cramo.dcramo.dpar(0)");
    pStr = PutToStr(pStr, pch_cramo->dcramo.dpar.arr[2],4,"ch_cramo.dcramo.dpar(0)");
    pStr = PutToStr(pStr, pch_cramo->dcramo.dpar.arr[3],4,"ch_cramo.dcramo.dpar(0)");
    pStr = PutToStr(pStr, pch_cramo->dcramo.spar,4,"ch_cramo.dcramo.spar");

    // Internal:
    pStr = PutToStr(pStr, ptst_cachemem->idaddr,11,"t_idaddr");
    pStr = PutToStr(pStr, ptst_cachemem->iddatain,32,"t_iddatain");
    pStr = PutToStr(pStr, ptst_cachemem->idenable,1,"t_idenable");
    pStr = PutToStr(pStr, ptst_cachemem->idwrite,MAXSETS,"t_idwrite",true);
    pStr = PutToStr(pStr, ptst_cachemem->iddataout.arr[0],32,"t_iddataout0");
    pStr = PutToStr(pStr, ptst_cachemem->itaddr>>ILINE_BITS,8,"t_itaddr");
    pStr = PutToStr(pStr, ptst_cachemem->itdatain.arr[0],36,"t_itdatain0");
    pStr = PutToStr(pStr, ptst_cachemem->itdataout.arr[0],35,"t_itdataout0");
    pStr = PutToStr(pStr, ptst_cachemem->itenable,1,"t_itenable");
    pStr = PutToStr(pStr, ptst_cachemem->itwrite,MAXSETS,"t_itwrite",true);
    pStr = PutToStr(pStr, ptst_cachemem->dtdataout2.arr[0],32,"t_dtdataout20");
    pStr = PutToStr(pStr, ptst_cachemem->dtdataout3.arr[0],32,"t_dtdataout30");
    pStr = PutToStr(pStr, ptst_cachemem->dtenable,MAXSETS,"t_dtenable",true);
    pStr = PutToStr(pStr, ptst_cachemem->dtaddr2>>DLINE_BITS,8,"t_dtaddr2");
    pStr = PutToStr(pStr, ptst_cachemem->dtwrite3,MAXSETS,"t_dtwrite3",true);
    pStr = PutToStr(pStr, ptst_cachemem->dtaddr>>DLINE_BITS,8,"t_dtaddr");
    pStr = PutToStr(pStr, ptst_cachemem->dtdatain3.arr[0],36,"t_dtdatain30");


    PrintIndexStr();

    *posBench[TB_cachemem] << chStr << "\n";
  }

#ifdef DBG_cachemem
  // Clock update:
  tst_cachemem.ClkUpdate();
#endif
}


