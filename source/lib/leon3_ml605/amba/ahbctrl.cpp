#include "lheaders.h"

#define nahbm   AHB_MASTER_TOTAL
#define nahbs   AHB_SLAVE_TOTAL
#define nahbmx  (0x1<<log2[AHB_MASTER_TOTAL])//: integer := 2**log2(nahbm);
#define rrobin  CFG_AHBCTRL_RROBIN
#define split   CFG_AHBCTRL_SPLIT
#define defmast CFG_AHBCTRL_DEFMST
#define mprio   CFG_AHBCTRL_MPRIO
#define primst  mprio//: std_logic_vector(NAHBMST downto 0) := conv_std_logic_vector(mprio, NAHBMST+1);
#define ioaddr  CFG_AHBCTRL_IOADDR
#define iomask  CFG_AHBCTRL_IOMASK
#define cfgaddr CFG_AHBCTRL_CFGADDR
#define cfgmask CFG_AHBCTRL_CFGMASK
#define fpnpen  CFG_AHBCTRL_FPNPENA
#define fixbrst CFG_AHBCTRL_FIXBURST
#define ioen    CFG_AHBCTRL_IOEN// 
#define acdm    CFG_AHBCTRL_ACDM
#define devid   CFG_AHBCTRL_DEVID
#define disirq  CFG_AHBCTRL_DISIRQ
#define hwdebug   0//    : integer := 0;  --Hardware debug
#define fourgslv  0//  : integer := 0   --1=Single slave with single 4 GB bar


#define MIMAX   (log2x[nahbmx]-1)//: integer := log2x(nahbmx) - 1;
#define SIMAX   (log2x[nahbs]-1)//: integer := log2x(nahbs) - 1;
#define IOAREA  (ioaddr)//: std_logic_vector(11 downto 0) :=   conv_std_logic_vector(ioaddr, 12);
#define IOMSK   (iomask)//: std_logic_vector(11 downto 0) := conv_std_logic_vector(iomask, 12);
#define CFGAREA (cfgaddr)//: std_logic_vector(11 downto 0) := conv_std_logic_vector(cfgaddr, 12);
#define CFGMSK  (cfgmask)//): std_logic_vector(11 downto 0) := conv_std_logic_vector(cfgmask, 12);
#define FULLPNP (fpnpen!=0)//: boolean := (fpnpen /= 0);


//****************************************************************************
struct tztab_type { int arr[16]; }; //[2:0]arr[0:15]
const tztab_type tztab = { 0x4, // 0000 = 100
                           0x0, // 0001 = 000
                           0x1, // 0010 = 001
                           0x0, // 0011 = 000
                           
                           0x2, // 0100 = 010
                           0x0, // 0101 = 000
                           0x1, // 0110 = 001
                           0x0, // 0111 = 000
                           
                           0x3, // 1000 = 011
                           0x0, // 1001 = 000
                           0x1, // 1010 = 001
                           0x0, // 1011 = 000
                           
                           0x2, // 1100 = 010
                           0x0, // 1101 = 000
                           0x1, // 1110 = 001
                           0x0  // 1111 = 000
                         };

//****************************************************************************


//****************************************************************************
uint32 ahbctrl::tz( uint32 in_vect,
                    uint32 in_length)
{
  uint64 vect;// : std_logic_vector(63 downto 0);
  l1_type l1;// : l1_type;
  l2_type l2;// : l2_type;
  l3_type l3;// : l3_type;
  uint32 l4;// : std_logic_vector(6 downto 0);
  uint32 bci_lsb, bci_msb;// : std_logic_vector(3 downto 0);
  uint32 bco_lsb, bco_msb;// : std_logic_vector(2 downto 0);    
  uint32 sel;// : std_logic;

  vect = MSK64(63,in_length);
  vect |= uint64(BITS32(in_vect,in_length-1,0));

  // level 0
  for (int i=0; i<8; i++)
  {
    bci_lsb = uint32(BITS64(vect, 8*i+3, 8*i));
    bci_msb = uint32(BITS64(vect, 8*i+7, 8*i+4));
    //lookup the highest priority request in each nibble
    bco_lsb = tztab.arr[bci_lsb];
    bco_msb = tztab.arr[bci_msb];
    //select which of two nibbles contain the highest priority ACTIVE
    //signal, and forward the corresponding vector to the next level
    sel = BIT32(bco_lsb,2);
    if(sel==0) l1.arr[i] = bco_lsb;
    else       l1.arr[i] = (BIT32(bco_msb,2)<<3) | (BIT32(~bco_msb,2)<<2) | BITS32(bco_msb,1,0);
  }
  
  //level 1
  for (int i=0; i<4; i++)
  {
    sel = BIT32(l1.arr[2*i],3);
    //select which of two 8-bit vectors contain the
    //highest priority ACTIVE signal. the msb set at the previous level
    //for each 8-bit slice determines this
    if(sel==0) l2.arr[i] = l1.arr[2*i];
    else l2.arr[i] = (BIT32(l1.arr[2*i+1],3)<<4)
                   | (BIT32(~l1.arr[2*i+1],3)<<3)
                   | BITS32(l1.arr[2*i+1],2,0);
  }

  //level 2
  for (int i=0; i<2; i++)
  {
    sel = BIT32(l2.arr[2*i],4);
    if(sel==0) l3.arr[i] = l2.arr[2*i];
    else l3.arr[i] = (BIT32(l2.arr[2*i+1],4)<<5)
                   | (BIT32(~l2.arr[2*i+1],4)<<4)
                   | BITS32(l2.arr[2*i+1],3,0);
  }
  
  //level 3
  sel = BIT32(l3.arr[0],5);
  if(sel==0) l4 = l3.arr[0];
  else l4 = (BIT32(l3.arr[1],5)<<6)
          | (BIT32(~l3.arr[1],5)<<5)
          | BITS32(l3.arr[1],4,0);


  return l4;
}

//****************************************************************************
//invert the bit order of the hbusreq signals located in vect_in
//since the highest hbusreq has the highest priority but the
//algorithm in tz has the highest priority on lsb
uint32 ahbctrl::lz(uint32 vect_in, int vec_length)
{
  uint32 vect;// : std_logic_vector(vect_in'length-1 downto 0);
  uint32 vect2;// : std_logic_vector(vect_in'length-1 downto 0);    

  vect = vect_in;
  vect2 = 0;
  for (int i=0; i<vec_length; i++)
    vect2 |= ( BIT32(vect,vec_length-1-i) << i );
  
  return(tz(vect2, vec_length));
};


//****************************************************************************
void ahbctrl::selmast(  reg_type &in_r,//      : in reg_type;
                        ahb_mst_out_vector &in_msto,//   : in ahb_mst_out_vector;
                        uint32 in_rsplit,// : in std_logic_vector(0 to nahbmx-1);
                        uint32 &out_mast,//   : out integer range 0 to nahbmx-1;
                        uint32 &out_defmst// : out std_ulogic;
                      )
{
  nmstarr nmst;//    : nmstarr;
  nvalarr nvalid;//  : nvalarr;                  

  uint32 rrvec;// : std_logic_vector(nahbmx*2-1 downto 0);
  uint32 zcnt;//  : std_logic_vector(log2(nahbmx)+1 downto 0);
  uint32 hpvec;// : std_logic_vector(nahbmx-1 downto 0);
  uint32 zcnt2;// : std_logic_vector(log2(nahbmx) downto 0);

  for(int32 i=1; i<=3; i++)
  {
    nmst.arr[i] = 0;
    nvalid.arr[i] = false;
  }


  
  out_mast = in_r.hmaster;
  out_defmst = 0;
    
  if (nahbm == 1)
    out_mast = 0;
  else if (rrobin == 0)
  {
    hpvec = 0;
    for (int32 i=0; i<=nahbmx-1; i++)
    {
      //--masters which have received split are not granted
      if ((BIT32(in_rsplit,i)==0) || (split == 0))
      {
        hpvec |= (in_msto.arr[i].hbusreq<<i);
      }
    }
    //--check if any bus requests are active (nvalid(2) set to true)
    //--and determine the index (zcnt2) of the highest priority master
    uint32 tmp = lz(hpvec, nahbmx);
    zcnt2 = BITS32(tmp,log2[nahbmx],0);
    if (BIT32(zcnt2,log2[nahbmx]) == 0)
      nvalid.arr[2] = true;
    nmst.arr[2] = BITS32(~zcnt2, log2[nahbmx]-1, 0);
    //--find the default master number
    for (int32 i=0; i<=nahbmx-1; i++)
    {
      if (!((nmst.arr[3] == defmast) && nvalid.arr[3]))
      {
        nmst.arr[3]   = i;
        nvalid.arr[3] = true; 
      }
    }
  }else
  {
    rrvec = 0;
    //--mask requests up to and including current master. Concatenate
    //--an unmasked request vector above the masked vector. Otherwise
    //--the rules are the same as for fixed priority
    for (int32 i=0; i<=nahbmx-1; i++)
    {
      if ((BIT32(in_rsplit,i) == 0) || (split == 0))
      {
        if (uint32(i) > in_r.hmaster) rrvec |= (in_msto.arr[i].hbusreq<<i);
        rrvec |= (in_msto.arr[i].hbusreq<<(nahbmx+i));
      }
    }
    //--find the next master uzing tz which gives priority to lower
    //--indexes
    uint32 tmp = tz(rrvec, 2*nahbmx);
    zcnt = BITS32(tmp,log2[nahbmx]+1,0);
    //--was there a master requesting the bus?
    if (BIT32(zcnt,log2[nahbmx]+1) == 0) 
      nvalid.arr[2] = true;
    nmst.arr[2] = BITS32(zcnt,log2[nahbmx]-1,0);
    //--if no other master is requesting the bus select the current one
    nmst.arr[3]   = in_r.hmaster;
    nvalid.arr[3] = true;
    //--check if any masters configured with higher priority are requesting
    //--the bus
    if (mprio != 0)
    {
      for(int32 i=0; i<=nahbm-1; i++)
      {
        if (((BIT32(in_rsplit,i) == 0) || (split == 0)) && (BIT32(primst,i) == 1)) 
        {
	        if (in_msto.arr[i].hbusreq == 1)
          {
            nmst.arr[1]   = i;
            nvalid.arr[1] = true;
          }
        }
      }
    }
  }
  
  //--select the next master. If for round robin a high priority master
  //--(mprio) requested the bus if nvalid(1) is true. Otherwise
  //--if nvalid(2) is true at least one master was requesting the bus
  //--and the one with highest priority was selected. If none of these
  //--were true then the default master is selected (nvalid(3) true)
  for (int32 i=1; i<=3; i++)
  {
    if (nvalid.arr[i]) 
    {
      out_mast = nmst.arr[i];
      break;
    }
  }

  //--if no master was requesting the bus and split is enabled
  //--then select builtin dummy master which only does
  //--idle transfers
  if ((!(nvalid.arr[1] || nvalid.arr[2])) && (split != 0))
    out_defmst = orv(in_rsplit);
}

//****************************************************************************
void ahbctrl::Update(
  int32 rst,//     : in  std_ulogic;
  SClock clk,//     : in  std_ulogic;
  ahb_mst_in_type    &msti,//    : out ahb_mst_in_type;
  ahb_mst_out_vector &msto,//    : in  ahb_mst_out_vector;
  ahb_slv_in_type    &slvi,//    : out ahb_slv_in_type;
  ahb_slv_out_vector &slvo//    : in  ahb_slv_out_vector;
)
{
  int32 testen=0;//  : in  std_ulogic := '0';
  int32 testrst=1;// : in  std_ulogic := '1';
  int32 scanen=0;//  : in  std_ulogic := '0';
  int32 testoen=1;// : in  std_ulogic := '1'


  v = r.Q;
  hgrant = 0;
  defmst = 0;
  haddr = msto.arr[r.Q.hmaster].haddr;
    
  nhmaster = r.Q.hmaster; 


  //determine if bus should be rearbitrated. This is done if the current
  //master is not performing a locked transfer and if not in the middle
  //of burst
  arb = 0;
  if ((r.Q.hmasterlock || r.Q.ldefmst) == 0)
  {
    switch (msto.arr[r.Q.hmaster].htrans)
    {
      case HTRANS_IDLE: arb = 1;  break;
      case HTRANS_NONSEQ:
        switch(msto.arr[r.Q.hmaster].hburst)
        {
          case HBURST_SINGLE: arb = 1; break;
          case HBURST_INCR:   arb = !msto.arr[r.Q.hmaster].hbusreq; break;
          default:;
        }
      break;
      case HTRANS_SEQ:
        switch(msto.arr[r.Q.hmaster].hburst)
        {
          case HBURST_WRAP4: case HBURST_INCR4: 
            if((fixbrst == 1) && (BITS32(r.Q.beat,1,0)==0x3)) arb = 1;
          break;
          case HBURST_WRAP8: case HBURST_INCR8:
            if((fixbrst == 1) && (BITS32(r.Q.beat,2,0)==0x7)) arb = 1;
          break;
          case HBURST_WRAP16: case HBURST_INCR16:
            if((fixbrst == 1) && (BITS32(r.Q.beat,3,0)==0xf)) arb = 1;
          break;
          case HBURST_INCR: arb = !msto.arr[r.Q.hmaster].hbusreq; break;
          default:;
        }
      break;
      default: arb = 0;
    }
  }

  if (split != 0)
  {
    tmpv = 0;
    for (int32 i=0; i<=nahbmx-1; i++)
    {
      tmpv |= ( ((BIT32(msto.arr[i].htrans,1) | msto.arr[i].hbusreq) & !BIT32(rsplit.Q,i) & !r.Q.ldefmst)<<i );
    }
    if((r.Q.defmst & orv(tmpv))==1) arb = 1;
  }

  //--rearbitrate bus with selmast. If not arbitrated one must
  //--ensure that the dummy master is selected for locked splits. 
  if (arb == 1)
    selmast(r.Q, msto, rsplit.Q, nhmaster, defmst );
  else if (split != 0)
    defmst = r.Q.defmst;

  //-- slave decoding

  hsel = 0;
  hmbsel = 0;

#if (fourgslv==0)
  for (int32 i=0; i<=nahbs-1; i++)
  {
    for (int32 j=AHB_REG_ID_WIDTH; j<=AHB_CFG_WORDS-1; j++)
    {
      area = BITS32(slvo.arr[i].hconfig.arr[j],1,0);
      switch(area)
      {
        case 0x2:
          if( ((ioen == 0) || ((IOAREA&IOMSK) != (BITS32(haddr,31,20)&IOMSK)))
           && ((BITS32(slvo.arr[i].hconfig.arr[j],31,20)&BITS32(slvo.arr[i].hconfig.arr[j],15,4)) 
            == (BITS32(haddr,31,20)&BITS32(slvo.arr[i].hconfig.arr[j],15,4))) 
           && (BITS32(slvo.arr[i].hconfig.arr[j],15,4) != 0) )
          {
            hsel |= (1<<i);
            hmbsel |= (1<<(j-AHB_REG_ID_WIDTH));
          }
        break;
        case 0x3:
        if( ((ioen != 0) && ((IOAREA&IOMSK) == (BITS32(haddr,31,20)&IOMSK))) 
         && ((BITS32(slvo.arr[i].hconfig.arr[j],31,20)&BITS32(slvo.arr[i].hconfig.arr[j],15,4)) 
          == (BITS32(haddr,19,8)&BITS32(slvo.arr[i].hconfig.arr[j],15,4)))
         && (BITS32(slvo.arr[i].hconfig.arr[j],15,4) != 0) )
        {
          hsel |= (1<<i);
          hmbsel |= (1<<(j-AHB_REG_ID_WIDTH));
        }
        break;
        default:;
      }
    }
  }
#else
  //-- There is only one slave on the bus. The slave has only one bar, which
  //-- maps 4 GB address space.
  hsel   |= 1;
  hmbsel |= 1;
#endif

  if (r.Q.defmst == 1) hsel = 0;

  bnslave = BIT32(hsel,1) | BIT32(hsel,3) | BIT32(hsel,5) | BIT32(hsel,7) |
            BIT32(hsel,9) | BIT32(hsel,11) | BIT32(hsel,13) | BIT32(hsel,15);
  bnslave |= ((BIT32(hsel,2) | BIT32(hsel,3) | BIT32(hsel,6) | BIT32(hsel,7) |
               BIT32(hsel,10) | BIT32(hsel,11) | BIT32(hsel,14) | BIT32(hsel,15)) << 1);
  bnslave |= ((BIT32(hsel,4) | BIT32(hsel,5) | BIT32(hsel,6) | BIT32(hsel,7) |
               BIT32(hsel,12) | BIT32(hsel,13) | BIT32(hsel,14) | BIT32(hsel,15)) << 2);
  bnslave |= ((BIT32(hsel,8) | BIT32(hsel,9) | BIT32(hsel,10) | BIT32(hsel,11) |
               BIT32(hsel,12) | BIT32(hsel,13) | BIT32(hsel,14) | BIT32(hsel,15)) << 3);
  nslave = BITS32(bnslave,SIMAX,0);

  if( ((((IOAREA&IOMSK)==(BITS32(haddr,31,20)&IOMSK)) && (ioen != 0))
    || ((IOAREA==BITS32(haddr,31,20)) && (ioen == 0))) 
    && ((CFGAREA&CFGMSK)==(BITS32(haddr,19,8)&CFGMSK))
    && (cfgmask != 0) )
  {
    cfgsel = 1;
    hsel = 0;
  }else cfgsel = 0;

  if ((nslave == 0) && (BIT32(hsel,0) == 0) && (cfgsel == 0)) defslv = 1;
  else                                                        defslv = 0;

  if (r.Q.defmst == 1)
  {
    cfgsel = 0;
    defslv = 1;
  }
  
  //-- error response on undecoded area
  v.hready = 0; 
  hready = slvo.arr[r.Q.hslave].hready;
  hresp  = slvo.arr[r.Q.hslave].hresp;
  if (r.Q.defslv == 1)
  {
    //-- default slave
    if ((r.Q.htrans == HTRANS_IDLE) || (r.Q.htrans == HTRANS_BUSY))
    {
      hresp = HRESP_OKAY;
      hready = 1;
    }else
    {
    //-- return two-cycle error in case of unimplemented slave access
      hresp = HRESP_ERROR;
      hready = r.Q.hready;
      v.hready = !r.Q.hready;
    }
  }

#if (CFG_AHBCTRL_ACDM == 0)
    hrdata = slvo.arr[r.Q.hslave].hrdata;
#else
    hrdata = ahbselectdata(slvo.arr[r.Q.hslave].hrdata, BITS32(r.Q.haddr,4,2), r.Q.hsize);
#endif

  if (cfgmask != 0)
  {
    //-- plug&play information for masters
    if (FULLPNP) hconfndx = BITS32(r.Q.haddr,4,2);
    else         hconfndx = 0;
    if((BITS32(r.Q.haddr,10,MIMAX+6)==0) && (FULLPNP || (BITS32(r.Q.haddr,4,2)==0)))
      v.hrdatam = msto.arr[BITS32(r.Q.haddr,MIMAX+5,5)].hconfig.arr[hconfndx];
    else
      v.hrdatam = 0;

    //-- plug&play information for slaves
    if( (BITS32(r.Q.haddr,10,SIMAX+6)==0) &&
      (FULLPNP || (BITS32(r.Q.haddr,4,2)==0) || (BIT32(r.Q.haddr,4)==1)) )
      v.hrdatas = slvo.arr[BITS32(r.Q.haddr,SIMAX+5,5)].hconfig.arr[BITS32(r.Q.haddr,4,2)];
    else 
      v.hrdatas = 0;

    //-- device ID, library build and potentially debug information
    if (BITS32(r.Q.haddr,10,4) == 0x7f)
    {
      if ((hwdebug == 0) || BITS32(r.Q.haddr,3,2)==0x0)
      {
        v.hrdatas = (LIBVHDL_BUILD);
        v.hrdatas |= (devid << 16);
      }else if(BITS32(r.Q.haddr,3,2)==0x1)
      {
        for (int32 i=0; i<=nahbmx-1; i++)
        {
          v.hrdatas &= ~(1<<i);
          v.hrdatas |= (msto.arr[i].hbusreq<<i);
        }
      }else
      {
        for (int32 i=0; i<=nahbmx-1; i++)
        {
          v.hrdatas &= ~(1<<i);
          v.hrdatas |= (BIT32(rsplit.Q,i)<<i);
        }
      }
    }

    if (r.Q.cfgsel == 1)
    {
      hrdata = 0; 
      //-- default slave
      if ((r.Q.htrans == HTRANS_IDLE) || (r.Q.htrans == HTRANS_BUSY))
      {
        hresp = HRESP_OKAY;
        hready = 1;
      }else
      {
	      //-- return two-cycle read/write respons
        hresp = HRESP_OKAY;
        hready = r.Q.hready;
        v.hready = !r.Q.hready;
      }
      if (r.Q.cfga11 == 0) hrdata = r.Q.hrdatam;//ahbdrivedata(r.hrdatam);
      else                 hrdata = r.Q.hrdatas;//ahbdrivedata(r.hrdatas);
    }
  }

  //--degrant all masters when split occurs for locked access
  if (r.Q.hmasterlockd == 1)
  {
    if ((hresp == HRESP_RETRY) || ((split != 0) && (hresp == HRESP_SPLIT)))
      nhmaster = r.Q.hmaster;

    if (split != 0)
    {
      if (hresp == HRESP_SPLIT)
      {
        v.ldefmst = 1;
        defmst    = 1;
        v.lsplmst = nhmaster;
      }
    }
  }

  if ((split != 0) && (r.Q.ldefmst == 1))
  {
    if (BIT32(rsplit.Q, r.Q.lsplmst)== 0)
    {
      v.ldefmst = 0;
      defmst = 0;
    }
  }

  if ((split == 0) || (defmst == 0)) hgrant |= (1<<nhmaster);

  //-- latch active master and slave
  if (hready)
  {
    v.hmaster  = nhmaster;
    v.hmasterd = r.Q.hmaster;
    v.hsize    = msto.arr[r.Q.hmaster].hsize; 
    v.hslave   = nslave;
    v.defslv   = defslv;
    v.hmasterlockd = r.Q.hmasterlock;
    if ((split == 0) || (r.Q.defmst == 0)) v.htrans = msto.arr[r.Q.hmaster].htrans;
    else                                   v.htrans = HTRANS_IDLE;
    v.cfgsel = cfgsel;
    v.cfga11 = BIT32(msto.arr[r.Q.hmaster].haddr,11);
    v.haddr = msto.arr[r.Q.hmaster].haddr & MSK32(15,2);
    if((msto.arr[r.Q.hmaster].htrans == HTRANS_NONSEQ) || (msto.arr[r.Q.hmaster].htrans == HTRANS_IDLE))
      v.beat = 0x1;
    else if (msto.arr[r.Q.hmaster].htrans == HTRANS_SEQ)
    {
      if (fixbrst == 1) v.beat = r.Q.beat + 1;
    }
    if (split != 0) v.defmst = defmst;
  }

  //--assign new hmasterlock, v.hmaster is used because if hready
  //--then master can have changed, and when not hready then the
  //--previous master will still be selected
  v.hmasterlock = msto.arr[v.hmaster].hlock | (r.Q.hmasterlock & !hready);

  //-- split support
  vsplit = 0;
  if (split != 0)
  {
    uint32 bit;
    vsplit = rsplit.Q;
    if (slvo.arr[r.Q.hslave].hresp == HRESP_SPLIT) vsplit |= (1<<r.Q.hmasterd);
    for (int32 i=0; i<=nahbs-1; i++)
    {
      for (int32 j=0; j<=nahbmx-1; j++)
      {
        bit = BIT32(vsplit,j);
        vsplit &= ~(1<<j);
        vsplit |= ( (bit & !BIT32(slvo.arr[i].hsplit,j)) << j );
      }
    }
  }

  if (r.Q.cfgsel == 1) hcache = 1;
  else                 hcache = slvo.arr[v.hslave].hcache;
  
  //-- interrupt merging
  hirq = 0;
  if (disirq == 0)
  {
    for (int32 i=0; i<=nahbs-1; i++) hirq = hirq | slvo.arr[i].hirq;
    for (int32 i=0; i<=nahbm-1; i++) hirq = hirq | msto.arr[i].hirq;
  }

  if ((split == 0) || (r.Q.defmst == 0))
  {
    vslvi.haddr      = haddr;
    vslvi.htrans     = msto.arr[r.Q.hmaster].htrans;
    vslvi.hwrite     = msto.arr[r.Q.hmaster].hwrite;
    vslvi.hsize      = msto.arr[r.Q.hmaster].hsize;
    vslvi.hburst     = msto.arr[r.Q.hmaster].hburst;
    vslvi.hready     = hready;
    vslvi.hprot      = msto.arr[r.Q.hmaster].hprot;
//--    vslvi.hmastlock  = msto(r.hmaster).hlock;
    vslvi.hmastlock  = r.Q.hmasterlock;
    vslvi.hmaster    = r.Q.hmaster;
    vslvi.hsel       = BITS32(hsel,AHB_SLAVES_MAX-1,0); 
    vslvi.hmbsel     = hmbsel; 
    vslvi.hcache     = hcache; 
    vslvi.hirq       = hirq;       
  }else
  {
    vslvi        = ahbs_in_none;
    vslvi.hready = hready;
    vslvi.hirq   = hirq;
  }
#if (acdm == 0)
    vslvi.hwdata = msto.arr[r.Q.hmasterd].hwdata;
#else
    vslvi.hwdata = ahbselectdata(msto.arr[r.Q.hmasterd].hwdata, BITS32(r.Q.haddr,4,2), r.Q.hsize);
#endif
  vslvi.testen  = testen; 
  vslvi.testrst = testrst; 
  vslvi.scanen  = scanen & testen; 
  vslvi.testoen = testoen; 

  // reset operation
  if (!rst) 
  {
    v.hmaster = 0;
    v.hmasterlock = 0;
    vsplit = 0;
    v.htrans = HTRANS_IDLE;
    v.defslv = 0;
    //-- v.beat = "0001";
    v.hslave = 0;
    v.cfgsel = 0;
    v.defmst = 0;
    v.ldefmst = 0;
  }
    
  // drive master inputs
  msti.hgrant  = hgrant;
  msti.hready  = hready;
  msti.hresp   = hresp;
  msti.hrdata  = hrdata;
  msti.hcache  = hcache;
  msti.hirq    = hirq; 
  msti.testen  = testen; 
  msti.testrst = testrst; 
  msti.scanen  = scanen & testen; 
  msti.testoen = testoen; 

  // drive slave inputs
  slvi = vslvi;

  // pragma translate_off
  //drive internal signals to bus monitor
  lslvi         = vslvi;

  lmsti.hgrant  = hgrant;
  lmsti.hready  = hready;
  lmsti.hresp   = hresp;
  lmsti.hrdata  = hrdata;
  lmsti.hcache  = hcache;
  lmsti.hirq    = hirq; 
  // pragma translate_on

  if (split == 0)
  {
    v.ldefmst = 0;
    v.lsplmst = 0;
  }
    
  rin = v;
  rsplitin = vsplit; 

  r.CLK = clk;
  r.D = rin;
  if (split == 0) r.D.defmst = 0;

  if(split!=0)
  {
    rsplit.CLK = clk;
    rsplit.D = rsplitin;
  }else
  {
    rsplit.D = rsplit.Q = 0;
  }
}


