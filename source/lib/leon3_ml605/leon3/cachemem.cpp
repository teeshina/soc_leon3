#include "lheaders.h"

//#define tech      : integer range 0 to NTECH := 0;
//#define icen      CFG_ICEN//: integer range 0 to 1 := 0;
//#define irepl     CFG_IREPL//: integer range 0 to 3 := 0;
//#define isets     CFG_ISETS//: integer range 1 to 4 := 1;
//#define ilinesize CFG_ILINE//: integer range 4 to 8 := 4;
//#define isetsize  CFG_ISETSZ//: integer range 1 to 256 := 1;
//#define isetlock  CFG_ILOCK//: integer range 0 to 1 := 0;
//#define dcen      CFG_DCEN//: integer range 0 to 1 := 0;
//#define drepl     CFG_DREPL//: integer range 0 to 3 := 0;
//#define dsets     CFG_DSETS//: integer range 1 to 4 := 1;
//#define dlinesize CFG_DLINE//: integer range 4 to 8 := 4;
//#define dsetsize  CFG_DSETSZ//: integer range 1 to 256 := 1;
//#define dsetlock  CFG_DLOCK//: integer range 0 to 1 := 0;
//#define dsnoop    CFG_DSNOOP//: integer range 0 to 6 := 0;
//#define ilram      CFG_ILRAMEN//: integer range 0 to 1 := 0;
//#define ilramsize  CFG_ILRAMSZ//: integer range 1 to 512 := 1;        
//#define dlram      CFG_DLRAMEN//: integer range 0 to 1 := 0;
//#define dlramsize  CFG_DLRAMSZ//: integer range 1 to 512 := 1;
//#define mmuen     CFG_MMUEN//: integer range 0 to 1 := 0; 
//#define testen    CFG_SCANTEST_ENA//: integer range 0 to 3 := 0

#define DSNOOPSEP (CFG_DSNOOP>3)//   : boolean := (dsnoop > 3);
#define DSNOOPFAST   ((CFG_DSNOOP==2)||(CFG_DSNOOP==6))//: boolean := (dsnoop = 2) or (dsnoop = 6);

//#define ILINE_BITS   (log2[CFG_ILINE])//: integer := log2(ilinesize);
//#define IOFFSET_BITS (8 +log2[CFG_ISETSZ] - ILINE_BITS)//: integer := 8 +log2(isetsize) - ILINE_BITS;
#define DLINE_BITS   (log2[CFG_DLINE])//: integer := log2(dlinesize);
#define DOFFSET_BITS (8 +log2[CFG_DSETSZ] - DLINE_BITS)//: integer := 8 +log2(dsetsize) - DLINE_BITS;
#define ITAG_BITS    (TAG_HIGH - IOFFSET_BITS - ILINE_BITS - 2 + CFG_ILINE + 1)//: integer := TAG_HIGH - IOFFSET_BITS - ILINE_BITS - 2 + CFG_ILINE + 1;
#define DTAG_BITS    (TAG_HIGH - DOFFSET_BITS - DLINE_BITS - 2 + CFG_DLINE + 1)//: integer := TAG_HIGH - DOFFSET_BITS - DLINE_BITS - 2 + CFG_DLINE + 1;
#define IPTAG_BITS   (TAG_HIGH - IOFFSET_BITS - ILINE_BITS - 2 + 1)//: integer := TAG_HIGH - IOFFSET_BITS - ILINE_BITS - 2 + 1;
#define DPTAG_BITS   (TAG_HIGH - DOFFSET_BITS - DLINE_BITS - 2 + 1)//: integer := TAG_HIGH - DOFFSET_BITS - DLINE_BITS - 2 + 1;
#define ILRR_BIT     (creplalg_tbl.arr[CFG_IREPL])//: integer := creplalg_tbl(irepl);
#define DLRR_BIT     (creplalg_tbl.arr[CFG_DREPL])//: integer := creplalg_tbl(drepl);
#define ITAG_LOW     (IOFFSET_BITS + ILINE_BITS + 2)//: integer := IOFFSET_BITS + ILINE_BITS + 2;
#define DTAG_LOW     (DOFFSET_BITS + DLINE_BITS + 2)//: integer := DOFFSET_BITS + DLINE_BITS + 2;
#define ICLOCK_BIT   CFG_ILOCK//: integer := isetlock;
#define DCLOCK_BIT   CFG_DLOCK//: integer := dsetlock;
#define ILRAM_BITS   (log2[CFG_ILRAMSZ] + 10)//: integer := log2(ilramsize) + 10;
#define DLRAM_BITS   (log2[CFG_DLRAMSZ] + 10)//: integer := log2(dlramsize) + 10;


#define  ITDEPTH      (0x1<<IOFFSET_BITS)//: natural := 2**IOFFSET_BITS;
#define  DTDEPTH      (0x1<<DOFFSET_BITS)//: natural := 2**DOFFSET_BITS;
#define  MMUCTX_BITS  (8*CFG_MMUEN)//: natural := 8*mmuen;

//  -- i/d tag layout
//  -- +-----+----------+--------+-----+-------+
//  -- | LRR | LOCK_BIT | MMUCTX | TAG | VALID |
//  -- +-----+----------+--------+-----+-------+

#define ITWIDTH (ITAG_BITS + ILRR_BIT + ICLOCK_BIT + MMUCTX_BITS) //natural
#define DTWIDTH (DTAG_BITS + DLRR_BIT + DCLOCK_BIT + MMUCTX_BITS)
#define IDWIDTH 32
#define DDWIDTH 32

//******************************************************************
cachemem::cachemem()
{
  for (int32 i=0; i<CFG_ISETS; i++)
  {
    itags0[i] = new syncram(IOFFSET_BITS, ITWIDTH);
    idata0[i] = new syncram(IOFFSET_BITS+ILINE_BITS, IDWIDTH);
  }
  for (int32 i=0; i<CFG_DSETS; i++)
  {
    dtags0[i] = new syncram(DOFFSET_BITS, DTWIDTH);
    dtags1[i] = new syncram_2p(DOFFSET_BITS, DPTAG_BITS, 0, 1);
    ddata0[i] = new syncram(DOFFSET_BITS+DLINE_BITS, DDWIDTH);
  }
}
cachemem::~cachemem()
{
  for (int32 i=0; i<CFG_ISETS; i++)
  {
    free(itags0[i]);
    free(idata0[i]);
  }
  for (int32 i=0; i<CFG_DSETS; i++)
  {
    free(dtags0[i]);
    free(dtags1[i]);
    free(ddata0[i]);
  }
}


//******************************************************************
void cachemem::Update(  SClock clk,//   : in  std_ulogic;
                        cram_in_type &crami,// : in  cram_in_type;
                        cram_out_type &cramo,// : out cram_out_type;
                        SClock sclk )//  : in  std_ulogic
{
  vcc = 1;
  gnd = 0; 
  itaddr = crami.icramin.address &MSK32(IOFFSET_BITS+ILINE_BITS-1, ILINE_BITS);
  idaddr = BITS32(crami.icramin.address, IOFFSET_BITS+ILINE_BITS-1, 0);
  ildaddr = BITS32(crami.icramin.address, ILRAM_BITS-3, 0);
  
  uint32        viddatain;//  : std_logic_vector(IDWIDTH -1 downto 0);
  cdatatype     vdddatain;//  : cdatatype;
  itdatain_type vitdatain;// : itdatain_type;
  dtdatain_type vdtdatain;// : dtdatain_type;
  dtdatain_type vdtdatain2;// : dtdatain_type;
  dtdatain_type vdtdatain3;// : dtdatain_type;
  dtdatain_type vdtdatainu;// : dtdatain_type;

  viddatain = 0;// := (others => '0');
  memset(&vdddatain,0,sizeof(cdatatype));//(others => (others => '0'));

  viddatain = crami.icramin.data;
  
  uint32 tmpa, tmpb;
  for (uint32 i=0; i<CFG_DSETS; i++)
  {
    vdtdatain.arr[i] = 0;
    if (CFG_MMUEN == 1)
    {
      //vdtdatain.arr[i] &= ~MSK32( (DTWIDTH - (DLRR_BIT+DCLOCK_BIT+1)), (DTWIDTH - (DLRR_BIT+DCLOCK_BIT+M_CTX_SZ)) ); 
      vdtdatain.arr[i] |= ( uint64(crami.dcramin.ctx.arr[i])<<(DTWIDTH - (DLRR_BIT+DCLOCK_BIT+M_CTX_SZ)) );
    }

    vdtdatain.arr[i] &= ~(uint64(0x1)<<(DTWIDTH-(DCLOCK_BIT + DLRR_BIT)));
    vdtdatain.arr[i] |= ( uint64(BIT32(crami.dcramin.tag.arr[i],CTAG_LOCKPOS)) << (DTWIDTH-(DCLOCK_BIT + DLRR_BIT)) );
    
    if (CFG_DREPL == lrr)
    {
      vdtdatain.arr[i] |= ( uint64(BIT32(crami.dcramin.tag.arr[i],CTAG_LRRPOS))<<(DTWIDTH-DLRR_BIT) );
    }
    
    vdtdatain.arr[i] &= ~MSK64(DTAG_BITS-1, 0);
    vdtdatain.arr[i] |= uint64(BITS32(crami.dcramin.tag.arr[i], CFG_DLINE-1, 0));
    vdtdatain.arr[i] |= (uint64(BITS32(crami.dcramin.tag.arr[i], TAG_HIGH, DTAG_LOW))<<CFG_DLINE);
    
    if (crami.dcramin.flush == 1)
    {
      vdtdatain.arr[i] &= ~MSK64(DTAG_BITS-1, DTAG_BITS-8);
      vdtdatain.arr[i] |= (uint64(0xF))<<(DTAG_BITS-4); 
      vdtdatain.arr[i] |= (uint64((i&0x3))<<(DTAG_BITS-6));
      vdtdatain.arr[i] |= (uint64((i&0x3))<<(DTAG_BITS-8));
    };
  }
  
  for (int32 i=0; i<MAXSETS; i++)  vdtdatain2.arr[i] = 0;
  for (int32 i=0; i<CFG_DSETS; i++)
  {
    vdtdatain2.arr[i] |=  (uint64(0xF)<<(DTAG_BITS-4));
    vdtdatain2.arr[i] |=  (uint64(i)<<(DTAG_BITS-6));
    vdtdatain2.arr[i] |=  (uint64(i)<<(DTAG_BITS-8));
  }
  vdddatain = crami.dcramin.data;

  for (int32 i=0; i<MAXSETS; i++) { vdtdatainu.arr[i]=0; vdtdatain3.arr[i]=0; }
  for (int32 i=0; i<CFG_DSETS; i++)
  {
    vdtdatain3.arr[i] = 0;
    vdtdatain3.arr[i] |= (uint64(BITS32(crami.dcramin.ptag.arr[i], TAG_HIGH, DTAG_LOW))<<(DTAG_BITS-DPTAG_BITS));
  }

#if 1
  if(iClkCnt>=3)
  bool stop =true;
  uint32 x1 = (ITWIDTH - (ILRR_BIT+ICLOCK_BIT+1));
  uint32 x2 = (ITWIDTH - (ILRR_BIT+ICLOCK_BIT+M_CTX_SZ));
  uint32 x3 = (ITWIDTH-(ICLOCK_BIT + ILRR_BIT));
  uint32 x4 = (ITWIDTH-ILRR_BIT);
  uint32 x5 = ITAG_BITS-1 ;
  uint32 x6 = TAG_HIGH + CFG_ILINE;
  uint32 x7 = DTWIDTH;
#endif
  for (int32 i=0; i<CFG_ISETS; i++)
  {
    vitdatain.arr[i] = 0;
    if (CFG_MMUEN)
    {
      vitdatain.arr[i] |= ( ((uint64)BITS32(crami.icramin.ctx,M_CTX_SZ-1,0)) << (ITWIDTH - (ILRR_BIT+ICLOCK_BIT+M_CTX_SZ)) );
    }
    vitdatain.arr[i] |= ( ((uint64)BIT32(crami.icramin.tag.arr[i],CTAG_LOCKPOS))<< (ITWIDTH-(ICLOCK_BIT + ILRR_BIT)));
     
    if (CFG_IREPL==lrr)
    {
      vitdatain.arr[i] |= ( ((uint64)BIT32(crami.icramin.tag.arr[i],CTAG_LRRPOS)) << (ITWIDTH-ILRR_BIT));
    }
    vitdatain.arr[i] |= ( ((uint64(BITS32(crami.icramin.tag.arr[i], TAG_HIGH, ITAG_LOW)))<<CFG_ILINE) &MSK64(ITAG_BITS-1,0) );
    vitdatain.arr[i] |= ((uint64)BITS32(crami.icramin.tag.arr[i], CFG_ILINE-1, 0));
    if (crami.icramin.flush == 1)
    {
      vitdatain.arr[i] |=  (((uint64)0xF)<<(ITAG_BITS-4));
      vitdatain.arr[i] &= ~(MSK64(ITAG_BITS-5,ITAG_BITS-6));
      vitdatain.arr[i] |=  (uint64(i)<<(ITAG_BITS-6));
      vitdatain.arr[i] &= ~(MSK64(ITAG_BITS-7,ITAG_BITS-8));
      vitdatain.arr[i] |=  (uint64(i)<<(ITAG_BITS-8));
    }
  }

  itdatain = vitdatain;
  iddatain = viddatain;
  dtdatain = vdtdatain;
  dtdatain2 = vdtdatain2;
  dtdatain3 = vdtdatain3;
  dtdatainu = vdtdatainu;
  dddatain  = vdddatain;


  itwrite   = crami.icramin.twrite;
  idwrite   = crami.icramin.dwrite;
  itenable  = crami.icramin.tenable;
  idenable  = crami.icramin.denable;

  dtaddr  = crami.dcramin.address & MSK32(DOFFSET_BITS+DLINE_BITS-1, DLINE_BITS);
  dtaddr2 = BITS32(crami.dcramin.saddress,DOFFSET_BITS-1,0) << DLINE_BITS;  
  ddaddr  = BITS32(crami.dcramin.address, DOFFSET_BITS + DLINE_BITS -1, 0);
  ldaddr  = crami.dcramin.ldramin.address &MSK32(DLRAM_BITS-1, 2);
  dtwrite   = crami.dcramin.twrite;
  dtwrite2  = crami.dcramin.swrite;
  dtwrite3  = crami.dcramin.tpwrite;
  ddwrite   = crami.dcramin.dwrite;
  dtenable  = crami.dcramin.tenable;
  dtenable2 = crami.dcramin.senable;
  ddenable  = crami.dcramin.denable;


#if (CFG_ICEN==1)
  for (int32 i=0; i<CFG_ISETS; i++)
  {
    itags0[i]->Update(clk, BITS32(itaddr,IOFFSET_BITS+ILINE_BITS-1,ILINE_BITS), itdatain.arr[i], itdataout.arr[i], itenable, BIT32(itwrite,i) );
    idata0[i]->Update(clk, idaddr, iddatain, iddataout.arr[i], idenable, BIT32(idwrite,i) );
  }
  for (int32 i=CFG_ISETS; i<MAXSETS; i++)
  {
    itdataout.arr[i] = 0;
    iddataout.arr[i] = 0;
  }
#else
  for (int32 i=0; i<MAXSETS; i++)
  {
    itdataout.arr[i] = 0;
    iddataout.arr[i] = 0;
  }
#endif

#if (CFG_ILRAMEN==1)
  ildata0 : syncram
   generic map (tech, ILRAM_BITS-2, 32)
    port map (clk, ildaddr, iddatain, ildataout, 
  crami.icramin.ldramin.enable, crami.icramin.ldramin.write);
#endif

#if (CFG_DCEN==1)
  #if (CFG_DSNOOP==0)
      dt0 : for i in 0 to DSETS-1 generate
        dtags0 : syncram
        generic map (tech, DOFFSET_BITS, DTWIDTH)
        port map (clk, dtaddr, dtdatain(i)(DTWIDTH-1 downto 0), 
      dtdataout(i)(DTWIDTH-1 downto 0), dtenable(i), dtwrite(i));
      end generate;
      
    nodtags1 : if DSNOOP = 0 generate
      for i in 0 to DSETS-1 generate
      {
        dtdataout2(i)(DTWIDTH-1 downto 0) <= zero64(DTWIDTH-1 downto 0);
        dtdataout3(i)(DTWIDTH-1 downto 0) <= zero64(DTWIDTH-1 downto 0);
      }
  #else//(CFG_DSNOOP != 0)
    #if !(DSNOOPSEP)
        for (int32 i=0; i<CFG_DSETS; i++)
        {
          dtags0 : syncram_dp generic map (tech, DOFFSET_BITS, DTWIDTH) port map 
          (
              clk, dtaddr, dtdatain(i)(DTWIDTH-1 downto 0), 
              dtdataout(i)(DTWIDTH-1 downto 0), dtenable(i), dtwrite(i),
              sclk, dtaddr2, dtdatain2(i)(DTWIDTH-1 downto 0), 
              dtdataout2(i)(DTWIDTH-1 downto 0), dtenable2(i), dtwrite2(i)
          );
        }
    #else//-- virtual address snoop case
      #if !(DSNOOPFAST)
      for (int32 i=0; i<CFG_DSETS; i++)
      {
        dtags0 : syncram_dp generic map (tech, DOFFSET_BITS, DTWIDTH) port map 
        (
          clk, dtaddr, dtdatain(i)(DTWIDTH-1 downto 0), 
          dtdataout(i)(DTWIDTH-1 downto 0), dtenable(i), dtwrite(i),
          sclk, dtaddr2, dtdatain2(i)(DTWIDTH-1 downto 0), 
          dtdataout2(i)(DTWIDTH-1 downto 0), dtenable2(i), dtwrite2(i)
        );
        dtags1 : syncram_dp generic map (tech, DOFFSET_BITS, DPTAG_BITS) port map 
        (
          clk, dtaddr, dtdatain3(i)(DTAG_BITS-1 downto DTAG_BITS-DPTAG_BITS), 
          open, dtwrite3(i), dtwrite3(i),
          sclk, dtaddr2, dtdatainu(i)(DTAG_BITS-1 downto DTAG_BITS-DPTAG_BITS), 
          dtdataout3(i)(DTAG_BITS-1 downto DTAG_BITS-DPTAG_BITS), dtenable2(i), dtwrite2(i)
        );
      }
      #else
      for (int32 i=0; i<CFG_DSETS; i++)
      {
        dtags0[i]->Update( clk,
                           BITS32(dtaddr,DOFFSET_BITS + DLINE_BITS-1,DLINE_BITS),
                           (uint32)BITS64(dtdatain.arr[i],DTWIDTH-1,0), 
                           dtdataout.arr[i],
                           BIT32(dtenable,i),
                           BIT32(dtwrite,i) );
        
        
        dtags1[i]->Update( sclk,
                           BIT32(dtenable2,i),
                           BITS32(dtaddr2,DOFFSET_BITS + DLINE_BITS-1,DLINE_BITS),
                           tmpa,//(uint32)BITS32(dtdataout3.arr[i],DTAG_BITS-1,DTAG_BITS-DPTAG_BITS),
                           clk,
                           BIT32(dtwrite3,i),
                           BITS32(dtaddr,DOFFSET_BITS + DLINE_BITS-1,DLINE_BITS),
                           (uint32)BITS64(dtdatain3.arr[i], DTAG_BITS-1, DTAG_BITS-DPTAG_BITS) );
        dtdataout3.arr[i] &= ~MSK32(DTAG_BITS-1,DTAG_BITS-DPTAG_BITS);
        dtdataout3.arr[i] |= (tmpa<<(DTAG_BITS-DPTAG_BITS));
      }
      #endif//slow/fast
    #endif//!snoopsep/snoopsep
  #endif//!cfg_dsnoop/cfg_dsnoop

    for (int32 i=0; i<CFG_DSETS; i++)
    {
      ddata0[i]->Update(clk, ddaddr, dddatain.arr[i], dddataout.arr[i], (ddenable>>i)&0x1, (ddwrite>>i)&0x1 );
    }
    for (int32 i=CFG_DSETS; i<MAXSETS; i++)
    {
      dtdataout.arr[i]  = 0;
      dtdataout2.arr[i] = 0;
      dtdataout3.arr[i] = 0;
      dddataout.arr[i]  = 0;
    }

#else//CFG_DCEN==0
    dnd0 : for i in 0 to DSETS-1 generate
      dtdataout(i) <= (others => '0');
      dtdataout2(i) <= (others => '0');
      dtdataout3(i) <= (others => '0');
      dddataout(i) <= (others => '0');
    end generate;
#endif

#if!((CFG_DLRAMEN==1) && (CFG_DSETS > 1))
  lddatain = dddatain.arr[0];    
#else
  lddatain = dddatain.arr[1];    
#endif


  
#if (CFG_DLRAMEN==1)
  ldata0 : syncram
   generic map (tech, DLRAM_BITS-2, 32)
    port map (clk, ldaddr, lddatain, ldataout, crami.dcramin.ldramin.enable,
              crami.dcramin.ldramin.write);
#endif


  for (int32 i=0; i<CFG_ISETS; i++)
  {
    tmpa = (uint32)BITS64(itdataout.arr[i], ITAG_BITS-1, (ITAG_BITS-1) - (TAG_HIGH - ITAG_LOW));
    cramo.icramo.tag.arr[i] &= ~MSK32(TAG_HIGH,ITAG_LOW);
    cramo.icramo.tag.arr[i] |= (tmpa<<ITAG_LOW);
    
    cramo.icramo.tag.arr[i] &= ~MSK32(CFG_ILINE-1,0);
    cramo.icramo.tag.arr[i] |= BITS64(itdataout.arr[i],CFG_ILINE-1,0);
    
    cramo.icramo.tag.arr[i] &= ~(0x1<<CTAG_LRRPOS);
    cramo.icramo.tag.arr[i] |= (BIT64(itdataout.arr[i],(ITWIDTH - (1+ICLOCK_BIT))) << CTAG_LRRPOS);
    
    cramo.icramo.tag.arr[i] &= ~(0x1<<CTAG_LOCKPOS);
    cramo.icramo.tag.arr[i] |= (BIT64(itdataout.arr[i],(ITWIDTH-1))<<CTAG_LOCKPOS);
#if(CFG_MMUEN==1)
    cramo.icramo.ctx.arr[i] = (uint32)BITS64( itdataout.arr[i],(ITWIDTH - (ILRR_BIT+ICLOCK_BIT+1)),(ITWIDTH - (ILRR_BIT+ICLOCK_BIT+M_CTX_SZ)) );
#else
    cramo.icramo.ctx.arr[i] = 0;
#endif
    if((CFG_ILRAMEN==1) && ((CFG_ISETS==1) || (i==1)) && (crami.icramin.ldramin.read==1))
      cramo.icramo.data.arr[i] = ildataout;
    else 
      cramo.icramo.data.arr[i] = iddataout.arr[i];//(31 downto 0);
#if(CFG_ILINE==4)
      cramo.icramo.tag.arr[i] &=  ~0xF0;
#endif
    for (int32 j=10; j<ITAG_LOW; j++)
      cramo.icramo.tag.arr[i] &= ~(0x1<<j);//'0';
  }

  for (int32 i=CFG_ISETS; i<MAXSETS; i++)
  {
    cramo.icramo.tag.arr[i]  = 0;
    cramo.icramo.data.arr[i] = 0;
    cramo.icramo.ctx.arr[i]  = 0;
  }

#if 1

  for (int32 i=0; i<CFG_DSETS; i++)
  {
    tmpa = BITS32(dtdataout.arr[i],DTAG_BITS-1, (DTAG_BITS-1) - (TAG_HIGH - DTAG_LOW));
    cramo.dcramo.tag.arr[i] &= ~MSK32(TAG_HIGH, DTAG_LOW);
    cramo.dcramo.tag.arr[i] |= (tmpa<<DTAG_LOW);
    cramo.dcramo.tag.arr[i] &= ~MSK32(CFG_DLINE-1, 0);
    cramo.dcramo.tag.arr[i] |= BITS32(dtdataout.arr[i], CFG_DLINE-1, 0);
    
    cramo.dcramo.tag.arr[i] &= ~(0x1<<CTAG_LRRPOS);
    cramo.dcramo.tag.arr[i] |= (BIT32(dtdataout.arr[i],(DTWIDTH-(1+DCLOCK_BIT)))<<CTAG_LRRPOS);
    
    cramo.dcramo.tag.arr[i] &= ~(0x1<<CTAG_LOCKPOS);
    cramo.dcramo.tag.arr[i] |= (BIT32(dtdataout.arr[i],(DTWIDTH-1))<<CTAG_LOCKPOS);
#if (CFG_MMUEN!=0)
    tmpa = BITS32(dtdataout.arr[i], (DTWIDTH-(DLRR_BIT+DCLOCK_BIT+1)), (DTWIDTH - (DLRR_BIT+DCLOCK_BIT+M_CTX_SZ)) );
    cramo.dcramo.ctx.arr[i] = tmpa;
#else
    cramo.dcramo.ctx.arr[i] = 0;
#endif
    
    tmpa = BITS32( dtdataout3.arr[i], DTAG_BITS-1, (DTAG_BITS-1) - (TAG_HIGH - DTAG_LOW) );
    tmpb = BITS32( dtdataout2.arr[i], DTAG_BITS-1, (DTAG_BITS-1) - (TAG_HIGH - DTAG_LOW) );
    cramo.dcramo.stag.arr[i] &= ~MSK32(TAG_HIGH, DTAG_LOW);
    if(DSNOOPSEP) cramo.dcramo.stag.arr[i] |= (tmpa<<DTAG_LOW);
    else          cramo.dcramo.stag.arr[i] |= (tmpb<<DTAG_LOW);
    
    cramo.dcramo.stag.arr[i] &= ~MSK32(DTAG_LOW-1, 0);
    if((CFG_DLRAMEN==1) && ((CFG_DSETS==1) || (i==1)) && (crami.dcramin.ldramin.read==1))
      cramo.dcramo.data.arr[i] = ldataout;
    else 
      cramo.dcramo.data.arr[i] = dddataout.arr[i];//(31 downto 0);
#if (CFG_DLINE==4)
    cramo.dcramo.tag.arr[i] &= ~0xF0;
#endif
    cramo.dcramo.tag.arr[i]&= ~MSK32(DTAG_LOW-1,10);
  }

  for (int32 i=CFG_DSETS; i<MAXSETS; i++)
  {
    cramo.dcramo.tag.arr[i]  = 0;
    cramo.dcramo.stag.arr[i] = 0;
    cramo.dcramo.data.arr[i] = 0;
    cramo.dcramo.ctx.arr[i]  = 0;
  }

  for (int32 i=0; i<MAXSETS; i++)
  {
    cramo.dcramo.tpar.arr[i] = 0;
    cramo.dcramo.dpar.arr[i] = 0;
    cramo.dcramo.spar = 0;
    cramo.icramo.tpar.arr[i] = 0;
    cramo.icramo.dpar.arr[i] = 0;
  }
#endif
}


