#include "lheaders.h"

//****************************************************************************
void syncram64::Update( SClock clk,//     : in  std_ulogic;
                        uint32 address,// : in  std_logic_vector (abits -1 downto 0);
                        uint64 datain,//  : in  std_logic_vector (63 downto 0);
                        uint64 &dataout,// : out std_logic_vector (63 downto 0);
                        uint32 enable,//  : in  std_logic_vector (1 downto 0);
                        uint32 write,//   : in  std_logic_vector (1 downto 0);
                        uint32 testin)//  : in  std_logic_vector (3 downto 0) := "0000");
{
  uint32 dataout_msb, dataout_lsb;

  x0->Update(clk, address, (uint32)BITS64(datain,63,32), dataout_msb, 
	          BIT32(enable,1), BIT32(write,1), testin);
  x1->Update(clk, address, (uint32)BITS64(datain,31,0), dataout_lsb, 
	          BIT32(enable,0), BIT32(write,0), testin);

  dataout = ((uint64)dataout_msb)<<32;
  dataout |= ((uint64)dataout_lsb);
}

//****************************************************************************
void syncram_2p::Update(SClock rclk,//     : in std_ulogic;
                        uint32 renable,//  : in std_ulogic;
                        uint32 raddress,// : in std_logic_vector((abits -1) downto 0);
                        uint32 &dataout,//  : out std_logic_vector((dbits -1) downto 0);
                        SClock wclk,//     : in std_ulogic;
                        uint32 write,//    : in std_ulogic;
                        uint32 waddress,// : in std_logic_vector((abits -1) downto 0);
                        uint32 datain,//   : in std_logic_vector((dbits -1) downto 0);
                        uint32 testin)//   : in std_logic_vector(3 downto 0) := "0000");
{
  r_raddr.CLK = wclk;
  r_raddr.D  = raddress;
  
  r_waddr.CLK = wclk;
  r_waddr.D  = waddress;
  
  r_datain.CLK = wclk;
  r_datain.D = datain;
  
  r_write.CLK = wclk;
  r_write.D  = write;
  
  r_renable.CLK = wclk;
  r_renable.D = renable;

  uint32 dataoutx;
  pSyncRam_2p->Update(rclk, wclk, raddress, waddress, datain, write, dataoutx);
  
  if((wrfst==1)&&(r_write.Q&r_renable.Q)&&(r_raddr.Q==r_waddr.Q))
    dataout = r_datain.Q;
  else
    dataout = dataoutx;  
}

//****************************************************************************
void generic_regfile_3p::Update( SClock wclk,//   : in  std_ulogic;
                                 uint32 waddr,//  : in  std_logic_vector((abits -1) downto 0);
                                 uint32 wdata,//  : in  std_logic_vector((dbits -1) downto 0);
                                 uint32 we,//     : in  std_ulogic;
                                 SClock rclk,//   : in  std_ulogic;
                                 uint32 raddr1,// : in  std_logic_vector((abits -1) downto 0);
                                 uint32 re1,//    : in  std_ulogic;
                                 uint32 &rdata1,// : out std_logic_vector((dbits -1) downto 0);
                                 uint32 raddr2,// : in  std_logic_vector((abits -1) downto 0);
                                 uint32 re2,//    : in  std_ulogic;
                                 uint32 &rdata2// : out std_logic_vector((dbits -1) downto 0)
                               )
{
  WRCLK = wclk;
  
  wa.CLK = wclk;
  if ((we==1) && (waddr < numregs))    wa.D = waddr;

  ra1.CLK = wclk;
  if ((re1==1) && (raddr1 < numregs))  ra1.D = raddr1;

  ra2.CLK = wclk;
  if ((re2==1) && (raddr2 < numregs))  ra2.D = raddr2;

  din.CLK = wclk;
  din.D = wdata;
  
  wr.CLK = wclk;
  wr.D = we;

  rdata1 = ((wr.Q==1) && (wa.Q==ra1.Q) && (wrfst==1)) ? din.Q
                                                      : memarr[ra1.Q];
  
  rdata2 = ((wr.Q==1) && (wa.Q==ra2.Q) && (wrfst==1)) ? din.Q
                                                      : memarr[ra2.Q];
}


