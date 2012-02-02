#include "lheaders.h"

//#define tech    memtech//: integer := 0;
//#define abits   IRFBITS//: integer := 6;
//#define dbits   32//: integer := 8;
//#define wrfst   CFG_IRFWT//: integer := 0;
//#define numregs IREGNUM//: integer := 64;
//#define testen  CFG_SCANTEST_ENA: integer := 0


//#define rfinfer : boolean := (regfile_3p_infer(tech) = 1) or (((is_unisim(tech) = 1)) and (abits <= 5));
#define rfinfer (CFG_IRFBITS<=5)

//****************************************************************************
regfile_3p::regfile_3p()
{
#ifdef regfile_3p_inferred
  pclMem3p = new generic_regfile_3p(CFG_IRFBITS, 32, CFG_IRFWT, CFG_IREGNUM);
#else
  x0 = new syncram_2p(CFG_IRFBITS, 32, 0, CFG_IRFWT);
  x1 = new syncram_2p(CFG_IRFBITS, 32, 0, CFG_IRFWT);
#endif
}

regfile_3p::~regfile_3p()
{ 
#ifdef regfile_3p_inferred
  free(pclMem3p);
#else
  free(x0);
  free(x1);
#endif
}


//****************************************************************************
void regfile_3p::Update( SClock wclk,//   : in  std_ulogic;
                        uint32 waddr,//  : in  std_logic_vector((abits -1) downto 0);
                        uint32 wdata,//  : in  std_logic_vector((dbits -1) downto 0);
                        uint32 we,//     : in  std_ulogic;
                        SClock rclk,//   : in  std_ulogic;
                        uint32 raddr1,// : in  std_logic_vector((abits -1) downto 0);
                        uint32 re1,//    : in  std_ulogic;
                        uint32 &rdata1,// : out std_logic_vector((dbits -1) downto 0);
                        uint32 raddr2,// : in  std_logic_vector((abits -1) downto 0);
                        uint32 re2,//    : in  std_ulogic;
                        uint32 &rdata2,// : out std_logic_vector((dbits -1) downto 0);
                        uint32 testin//   : in std_logic_vector(3 downto 0) := "0000");
                       )
{
#ifdef regfile_3p_inferred
  pclMem3p->Update(wclk, waddr, wdata, we, rclk, raddr1, re1, rdata1, raddr2, re2, rdata2);
#else
  x0->Update(rclk, re1, raddr1, rdata1, wclk, we, waddr, wdata, testin);
  x1->Update(rclk, re2, raddr2, rdata2, wclk, we, waddr, wdata, testin);
#endif
}

