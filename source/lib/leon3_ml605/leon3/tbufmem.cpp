#include "const.h"
#include "tbufmem.h"

//#define tech   memtech//: integer := 0;
//#define tbuf   CFG_ITBSZ//: integer := 0; -- trace buf size in kB (0 - no trace buffer)
//#define testen : integer := 0

//#define ADDRBITS (10+log2[CFG_ITBSZ]-4)

//****************************************************************************
tbufmem::tbufmem(uint32 kbytes_)
{
  kbytes = kbytes_;
  ADDRBITS = (10+log2[kbytes]-4);
  
  pclRam64[0] = new syncram64(ADDRBITS);
  pclRam64[1] = new syncram64(ADDRBITS);
}
tbufmem::~tbufmem()
{
  free(pclRam64[0]);
  free(pclRam64[1]);
}



//****************************************************************************
void tbufmem::Update( SClock clk,// : in std_ulogic;
                      tracebuf_in_type &tbi,//  : in tracebuf_in_type;
                      tracebuf_out_type &tbo)//  : out tracebuf_out_type)
{
  enable = (tbi.enable<<1) | tbi.enable;
  
  for (int i=0; i<2; i++)
  {
    pclRam64[i]->Update(clk, BITS32(tbi.addr,ADDRBITS-1,0), tbi.data[i],
                        tbo.data[i], enable ,BITS32(tbi.write,2*i+1,2*i),
                        tbi.diag);
  }
}
  

