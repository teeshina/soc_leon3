#include "lheaders.h"

#define AHB_MASTER_HIGHEST 0
#define AHB_MASTERS_TOTAL_2n (log2x[AHB_MASTER_TOTAL])

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
uint32 ahbctrl::tz( uint32 inVect[],
                    uint32 inVectLength)
{
  uint32 wbVect[64];// : std_logic_vector(63 downto 0);
  l1_type l1;// : l1_type;
  l2_type l2;// : l2_type;
  l3_type l3;// : l3_type;
  uint32 l4;// : std_logic_vector(6 downto 0);
  uint32 bci_lsb, bci_msb;// : std_logic_vector(3 downto 0);
  uint32 bco_lsb, bco_msb;// : std_logic_vector(2 downto 0);    
  int sel;// : std_logic;

  for (uint32 i=0; i<64; i++)
  {
    if(i<inVectLength) wbVect[i] = inVect[i];
    else               wbVect[i] = 1;
  }
  
#if 1
#if 1
  uint32 wbTZ[8];
  uint32 wb4bits;
  for(int32 i=0; i<8; i++)
  {
    wb4bits = BusToInt32(&wbVect[4*i], 4);
    wbTZ[i] = wbVect[4*i]   ? 0:
              wbVect[4*i+1] ? 1:
              wbVect[4*i+2] ? 2:
              wbVect[4*i+3] ? 3 : 4;
  }
#endif
  // level 0
  for (int i=0; i<8; i++)
  {
    bci_lsb = BusToInt32(&wbVect[8*i], 4);
    bci_msb = BusToInt32(&wbVect[8*i+4], 4);
    //lookup the highest priority request in each nibble
    bco_lsb = tztab.arr[bci_lsb];
    bco_msb = tztab.arr[bci_msb];
    //select which of two nibbles contain the highest priority ACTIVE
    //signal, and forward the corresponding vector to the next level
    sel = (bco_lsb>>2)&0x1;
    if(sel==0) l1.arr[i] = bco_lsb;
    else       l1.arr[i] = (((bco_msb>>2)&0x1)<<3)
                         | (((~bco_msb>>2)&0x1)<<2)
                         | (bco_msb&0x3);
  }
  
  //level 1
  for (int i=0; i<4; i++)
  {
    sel = (l1.arr[2*i]>>3)&0x1;
    //select which of two 8-bit vectors contain the
    //highest priority ACTIVE signal. the msb set at the previous level
    //for each 8-bit slice determines this
    if(sel==0) l2.arr[i] = l1.arr[2*i];
    else l2.arr[i] = (((l1.arr[2*i+1]>>3)&0x1)<<4)
                   | (((~l1.arr[2*i+1]>>3)&0x1)<<3)
                   | (l1.arr[2*i+1]&0x7);
  }

  //level 2
  for (int i=0; i<2; i++)
  {
    sel = (l2.arr[2*i]>>4)&0x1;
    if(sel==0) l3.arr[i] = l2.arr[2*i];
    else l3.arr[i] = (((l2.arr[2*i+1]>>4)&0x1)<<5)
                   | (((~l2.arr[2*i+1]>>4)&0x1)<<4)
                   | (l2.arr[2*i+1]&0xF);
  }
  
  //level 3
  sel = (l3.arr[0]>>5)&0x1;
  if(sel==0) l4 = l3.arr[0];
  else l4 = (((l3.arr[1]>>5)&0x1)<<6)
          | (((~l3.arr[1]>>5)&0x1)<<5)
          | (l3.arr[1]&0x1F);


  return l4;
#else
  return 0;
#endif
  
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
    vect2 |= ( ((vect>>(vec_length-1-i))&0x1) << i );
  
  uint32 tmpVect2[64];
  Int32ToBus(vect_in, tmpVect2, vec_length);
  return(tz(tmpVect2, vec_length));
};


//****************************************************************************
void ahbctrl::selmast( uint32  inAhbMaster,
                       uint32  inAhbBusRequest[],
                       uint32  inAhbSplit[],// : in std_logic_vector(0 to nahbmx-1);
                       uint32& outSelMast,//   : out integer range 0 to nahbmx-1;
                       uint32& outDefmstEna)// : out std_ulogic)
{
  uint32 wbbMasters[4] = {0};//[1:3]
  bool   wbValid[4] = {false};//[1:3]

  uint32 rrvec[2*AHB_MASTERS_MAX];
  uint32 zcnt;//  : std_logic_vector(log2(nahbmx)+1 downto 0);
  
#if!defined CONFIG_AHB_RROBIN
  uint32 zcnt2;// : std_logic_vector(log2(nahbmx) downto 0);
  uint32 hpvec;// : std_logic_vector(nahbmx-1 downto 0);
  hpvec = 0;
  for (int i=0; i<AHB_MASTERS_TOTAL_2n; i++)
  {
    //masters which have received gen_split are not granted 
    if( !inAhbSplit[i] | (CONFIG_AHB_SPLIT==0))
    {
      hpvec &= ~(0x1<<i); // clear bit
      hpvec |= (inAhbBusRequest[i]<<i);//((in_msto.arr[i].hbusreq)<<i);
    }
    //check if any bus requests are active (nvalid(2) set to true)
    //and determine the gen_index (zcnt2) of the highest priority master
    zcnt2 = lz(hpvec, AHB_MASTERS_TOTAL_2n);//SH !!!!
    if(((zcnt2>>log2[AHB_MASTERS_TOTAL_2n])&0x1)==0) wbValid[2] = true;
    wbbMasters[2] = (~zcnt2)&0x3;
    //find the default master number
    for (int i=0; i<AHB_MASTERS_TOTAL_2n; i++)
    {
      if( !((wbbMasters[3]==CONFIG_AHB_DEFMST) && wbValid[3]) )
      {
        wbbMasters[3] = i;
        wbValid[3] = true;
      }
    }
  }
#else
  //mask requests up to and including current master. Concatenate
  //an unmasked request vector above the masked vector. Otherwise
  //the rules are the same as for fixed priority
  for (uint32 i=0; i<AHB_MASTERS_TOTAL_2n; i++)
  {
#ifdef CONFIG_AHB_SPLIT
    if(inAhbSplit[i])       rrvec[i] = 0;
    else if(i<=inAhbMaster) rrvec[i] = 0;
    else                    rrvec[i] = inAhbBusRequest[i];

    if(inAhbSplit[i]) rrvec[i+AHB_MASTERS_TOTAL_2n] = 0;
    else              rrvec[i+AHB_MASTERS_TOTAL_2n] = inAhbBusRequest[i];
#else
    if(i<=inAhbMaster) rrvec[i] = 0;
    else               rrvec[i] = inAhbBusRequest[i];
    rrvec[i+AHB_MASTERS_TOTAL_2n] = inAhbBusRequest[i];
#endif
  }
  //find the next master uzing tz which gives priority to lower
  //indexes
  zcnt = tz( rrvec,
             2*AHB_MASTERS_TOTAL_2n );
  
  if( ((zcnt>>(log2[AHB_MASTERS_TOTAL_2n]+1))&0x1)==0 ) wbValid[2] = true;
  wbbMasters[2] = zcnt;
  //if no other master is requesting the bus select the current one
  wbbMasters[3] = inAhbMaster;
  wbValid[3] = true;
  //check if any masters configured with higher priority are requesting
  //the bus
  if(AHB_MASTER_HIGHEST!=0)
  {
#ifdef CONFIG_AHB_SPLIT
    for (int i=0; i<AHB_MASTER_TOTAL; i++)
    {
      if ( !inAhbSplit[i] & ((primst>>i)&0x1))
      {
	      if (inAhbBusRequest[i])
        {
          wbbMasters[1] = i;
          wbValid[1] = true;
        }
      }
    }
#endif//config_ahb_split
  }
#endif
  
  
  //select the next master. If for round robin a high priority master
  //(AHB_MASTER_HIGHEST) requested the bus if nvalid(1) is true. Otherwise
  //if nvalid(2) is true at least one master was requesting the bus
  //and the one with highest priority was selected. If none of these
  //were true then the default master is selected (nvalid(3) true)
#if(AHB_MASTER_TOTAL==1)
  outSelMast = 0;
#else
  outSelMast = wbValid[1] ? wbbMasters[1]
             : wbValid[2] ? wbbMasters[2]
             : wbValid[3] ? wbbMasters[3] : inAhbMaster;
#endif


  //if no master was requesting the bus and gen_split is enabled
  //then select builtin dummy master which only does
  //idle transfers
#ifdef CONFIG_AHB_SPLIT
  outDefmstEna = ( wbValid[3] && BusToInt32(wbAhbSplit, AHB_MASTERS_TOTAL_2n) ) ? 1: 0;
#else
  outDefmstEna = 0;
#endif

}

//****************************************************************************
void ahbctrl::Update(
  int32 in_rst,//     : in  std_ulogic;
  SClock in_clk,//     : in  std_ulogic;
  ahb_mst_in_type    &out_msti,//    : out ahb_mst_in_type;
  ahb_mst_out_vector &in_msto,//    : in  ahb_mst_out_vector;
  ahb_slv_in_type    &out_slvi,//    : out ahb_slv_in_type;
  ahb_slv_out_vector &in_slvo//    : in  ahb_slv_out_vector;
)
{
  int32 in_testen=0;//  : in  std_ulogic := '0';
  int32 in_testrst=1;// : in  std_ulogic := '1';
  int32 in_scanen=0;//  : in  std_ulogic := '0';
  int32 in_testoen=1;// : in  std_ulogic := '1'


  const int BITS_PER_MASTERS = log2x[AHB_MASTERS_TOTAL_2n] - 1;
  const int BITS_PER_SLAVES = log2x[AHB_SLAVES_MAX] - 1;
//  const uint32 IOAREA = CONFIG_AHB_IOADDR&0xFFF;
//  const uint32 IOMSK  = gen_iomask&0xFFF;
//  const uint32 CFGAREA = gen_cfgaddr&0xFFF;
//  const uint32 CFGMSK  = gen_cfgmask&0xFFF;
  //const bool FULLPNP = (gen_fpnpen != 0) ? true: false;
  

// pragma translate_off
  ahb_mst_in_type lmsti;// : ahb_mst_in_type;
  ahb_slv_in_type lslvi;// : ahb_slv_in_type;
// pragma translate_on*/

#if 1
  for (uint32 i=0; i<AHB_MASTERS_TOTAL_2n; i++)
  {
    wbAhbBusRequest[i] = in_msto.arr[i].hbusreq;
    wbAhbSplit[i]      = rbSplit[i].Q;
  }
#endif


  reg_type      v;// : reg_type;
//  int nhmaster;// : integer range 0 to nahbmx -1; //SH: wbSelMaster
  uint32 hgrant;//  : std_logic_vector(0 to AHB_MASTERS_MAX-1);   -- bus grant
  uint32 hsel;//    : std_logic_vector(0 to 31);   -- slave select
  uint32 hmbsel;//  : std_logic_vector(0 to AHB_MEM_ID_WIDTH-1);
  int32 nslave;//  : natural range 0 to 31;
  uint32 bnslave;// : std_logic_vector(3 downto 0);
  uint32 area;//    : std_logic_vector(1 downto 0);
  uint32 hready;;//  : std_ulogic;
  uint32 defslv;//  : std_ulogic;
  uint32 cfgsel;//  : std_ulogic;
  uint32 hcache;//  : std_ulogic;
  uint32 hresp=0;//   : std_logic_vector(1 downto 0);
  uint32 hrdata;//  : std_logic_vector(AHBDW-1 downto 0);
  uint32 haddr;//   : std_logic_vector(31 downto 0);
  uint32 hirq;//    : std_logic_vector(AHB_IRQ_MAX-1 downto 0);
  uint32 arb;//     : std_ulogic;
  int32  hconfndx;// : integer range 0 to 7;
  ahb_slv_in_type vslvi;//   : ahb_slv_in_type;
  uint32        wbSelMaster=0;
  uint32        wbSelMstOutMaster;
  uint32        wSelMstOutDefmstEna;//SH: defmst   : std_ulogic;
  uint32        wDefmstEna;
  uint32 tmpv;//     : std_logic_vector(0 to nahbmx-1);

  v = r.Q;
  hgrant = 0;
  haddr = in_msto.arr[rbSelMaster[0].Q].haddr;

 
  //determine if bus should be rearbitrated. This is done if the current
  //master is not performing a locked transfer and if not in the middle
  //of burst
  arb=0;
  if((rMasterLock[0].Q | rSplitDefmstEna.Q)==0)
  {
    switch(in_msto.arr[rbSelMaster[0].Q].htrans)
    {
      case HTRANS_IDLE:
        arb = 1;
      break;
      case HTRANS_NONSEQ:
        switch(in_msto.arr[rbSelMaster[0].Q].hburst)
        {
          case HBURST_SINGLE: arb = 1; break;
          case HBURST_INCR: arb = in_msto.arr[rbSelMaster[0].Q].hbusreq; break;
          default:;
        }
      break;
      case HTRANS_SEQ:
        switch(in_msto.arr[rbSelMaster[0].Q].hburst)
        {
#ifdef AHBCTRL_FIXLENGTH_BURST
          case HBURST_WRAP4:
          case HBURST_INCR4:
            if((r.Q.beat&0x3)==0x3) arb = 1;
          break;
          
          case HBURST_WRAP8:
          case HBURST_INCR8:
            if((r.Q.beat&0x7)==0x7) arb = 1;
          break;
          
          case HBURST_WRAP16:
          case HBURST_INCR16:
            if((r.Q.beat&0xF)==0xF) arb = 1;
          break;
#endif
          
          case HBURST_INCR:
            arb = !in_msto.arr[rbSelMaster[0].Q].hbusreq;
          break;
          
          default:;
        }
      break;
      default: arb = 0;
    }
  }
  
#ifdef CONFIG_AHB_SPLIT
  for (uint32 i=0; i<AHB_MASTERS_TOTAL_2n; i++)
  {
    tmpv &= ~(0x1<<i);
    tmpv |= ( ((((in_msto.arr[i].htrans>>1)&0x1)|in_msto.arr[i].hbusreq) & !rbSplit[i].Q & !rSplitDefmstEna.Q )<<i );
  }
  if((rDefmstEna.Q&orv(tmpv))==1) arb=1;
#endif

  //rearbitrate bus with selmast. If not arbitrated one must
  //ensure that the dummy master is selected for locked splits. 
  if(arb==1)
  {
    selmast(rbSelMaster[0].Q,
            wbAhbBusRequest,
            wbAhbSplit,
            wbSelMstOutMaster,
            wSelMstOutDefmstEna);
  }
#ifdef CONFIG_AHB_SPLIT
  else
  {
    wbSelMstOutMaster = 0;
    wSelMstOutDefmstEna = rDefmstEna.Q;
  }
#endif

    
  //slave decoding
  hsel =  0; hmbsel = 0;
  for (int i=0; i<AHB_SLAVES_MAX; i++)
  {
    for (int j=AHB_REG_ID_WIDTH; j<AHB_CFG_WORDS; j++)
    {
      area = (in_slvo.arr[i].hconfig.arr[j])&0x3;
      switch(area)
      {
        case 0x2:
#ifdef AHBCTRL_IO_AREA_ENABLE
          if((CONFIG_AHB_IOADDR&AHBCTRL_ADR_IO_MASK) != (((haddr>>20)&0xFFF) & AHBCTRL_ADR_IO_MASK))
          {
#endif
            if (
               ( (((in_slvo.arr[i].hconfig.arr[j]>>20)&0xFFF) & ((in_slvo.arr[i].hconfig.arr[j]>>4)&0xFFF)) == 
                 (((haddr>>20)&0xFFF) & ((in_slvo.arr[i].hconfig.arr[j]>>4)&0xFFF)) ) & 
	             (((in_slvo.arr[i].hconfig.arr[j]>>4)&0xFFF) != 0) 
	             )
	          {
              hsel |= (0x1<<i); 
              hmbsel |= (0x1<<(j-AHB_REG_ID_WIDTH));
            }
          }
        break;
#ifdef AHBCTRL_IO_AREA_ENABLE
        case 0x3:
          if(
            ((CONFIG_AHB_IOADDR&AHBCTRL_ADR_IO_MASK) == (((haddr>>20)&0xFFF) & AHBCTRL_ADR_IO_MASK)) &
            ( (((in_slvo.arr[i].hconfig.arr[j]>>20)&0xFFF) & ((in_slvo.arr[i].hconfig.arr[j]>>4)&0xFFF)) ==
              (((haddr>>8)&0xFFF) & ((in_slvo.arr[i].hconfig.arr[j]>>4)&0xFFF)) ) & 
	          (((in_slvo.arr[i].hconfig.arr[j]>>4)&0xFFF) != 0)
	          )
	        {
            hsel |= (0x1<<i);
            hmbsel |= (0x1<<(j-AHB_REG_ID_WIDTH));
          }
        break;
#endif
        default:;
      }
    }
  }
  
  if(rDefmstEna.Q==1) hsel = 0;
  
  bnslave = ((hsel>>1)&0x1)|((hsel>>3)&0x1)|((hsel>>5)&0x1)|((hsel>>7)&0x1)
           |((hsel>>9)&0x1)|((hsel>>11)&0x1)|((hsel>>13)&0x1)|((hsel>>15)&0x1);

  bnslave |= (( ((hsel>>2)&0x1)|((hsel>>3)&0x1)|((hsel>>6)&0x1)|((hsel>>7)&0x1)
              |((hsel>>10)&0x1)|((hsel>>11)&0x1)|((hsel>>14)&0x1)|((hsel>>15)&0x1) )<<1);
  
  bnslave |= (( ((hsel>>4)&0x1)|((hsel>>5)&0x1)|((hsel>>6)&0x1)|((hsel>>7)&0x1)
              |((hsel>>12)&0x1)|((hsel>>13)&0x1)|((hsel>>14)&0x1)|((hsel>>15)&0x1) )<<2);

  bnslave |= (( ((hsel>>8)&0x1)|((hsel>>9)&0x1)|((hsel>>10)&0x1)|((hsel>>11)&0x1)
              |((hsel>>12)&0x1)|((hsel>>13)&0x1)|((hsel>>14)&0x1)|((hsel>>15)&0x1) )<<3);
              
  nslave = bnslave;

  // cfg addr: 0xFFFF Fxxx
  cfgsel = 0;
  if( ((AHBCTRL_ADR_CFG_AREA&AHBCTRL_ADR_CFG_MASK) == (((haddr>>8)&0xFFF) & AHBCTRL_ADR_CFG_MASK))
     &(AHBCTRL_ADR_CFG_MASK != 0)  )
  {
#ifdef AHBCTRL_IO_AREA_ENABLE
    if((CONFIG_AHB_IOADDR&AHBCTRL_ADR_IO_MASK)==(((haddr>>20)&0xFFF)&AHBCTRL_ADR_IO_MASK))
#else
    if(CONFIG_AHB_IOADDR==((haddr>>20)&0xFFF))
#endif
    {
      cfgsel = 1;
      hsel = 0;
    }
  }


  if ( (nslave == 0) & ((hsel&0x1)==0) & (cfgsel==0) ) defslv = 1;
  else defslv = 0;

  if(rDefmstEna.Q == 1)
  {
    cfgsel = 0;
    defslv = 1;
  }
  
// error response on undecoded area

  v.hready = 0; 
  hready = in_slvo.arr[r.Q.hslave].hready; 
  hresp = in_slvo.arr[r.Q.hslave].hresp;
  if (r.Q.defslv == 1)
  {
    // default slave
    if ((r.Q.htrans==HTRANS_IDLE) | (r.Q.htrans==HTRANS_BUSY))
    {
      hresp = HRESP_OKAY;
      hready = 1;
    }else
    {
      // return two-cycle error in case of unimplemented slave access
      hresp = HRESP_ERROR;
      hready = r.Q.hready;
      v.hready = !r.Q.hready;
    }
  }

#ifdef AHBCTRL_COMPL_DATAMUX_ENABLE
    hrdata = ahbselectdata(in_slvo.arr[r.Q.hslave].hrdata, ((r.Q.haddr>>2)&0x7), rbMstSize.Q);
#else
    hrdata = in_slvo.arr[r.Q.hslave].hrdata;
#endif

  if (AHBCTRL_ADR_CFG_MASK!=0)
  {
#ifdef AHBCTRL_FULLPNP_ENABLE
    hconfndx = (r.Q.haddr>>2)&0x7;
    if (((r.Q.haddr&0x7FF)>>(BITS_PER_MASTERS+6))==0)
    {
      uint32 tmpMask = (r.Q.haddr>>(BITS_PER_MASTERS+5))<<(BITS_PER_MASTERS+5);
      v.hrdatam = in_msto.arr[(r.Q.haddr^tmpMask)>>5].hconfig.arr[hconfndx];
    }else  v.hrdatam = 0;

    if (((r.Q.haddr&0x7FF)>>(BITS_PER_SLAVES+6))==0)
    {
      uint32 tmpMask = (r.Q.haddr>>(BITS_PER_MASTERS+5))<<(BITS_PER_MASTERS+5);
      v.hrdatas = in_slvo.arr[(r.Q.haddr^tmpMask)>>5].hconfig.arr[(r.Q.haddr>>2)&0x7];
    }else v.hrdatas = 0;
#else
    hconfndx = 0;
    if ( (((r.Q.haddr&0x7FF)>>(BITS_PER_MASTERS+6))==0) && ((r.Q.haddr&0x1C) == 0) )
    {
      uint32 tmpMask = (r.Q.haddr>>(BITS_PER_MASTERS+5))<<(BITS_PER_MASTERS+5);
      v.hrdatam = in_msto.arr[(r.Q.haddr^tmpMask)>>5].hconfig.arr[hconfndx];
    }else  v.hrdatam = 0;

    if ( (((r.Q.haddr&0x7FF)>>(BITS_PER_SLAVES+6))==0) && ((((r.Q.haddr>>2)&0x7)==0) || ((r.Q.haddr>>4)&0x1)) )
    {
      uint32 tmpMask = (r.Q.haddr>>(BITS_PER_MASTERS+5))<<(BITS_PER_MASTERS+5);
      v.hrdatas = in_slvo.arr[(r.Q.haddr^tmpMask)>>5].hconfig.arr[(r.Q.haddr>>2)&0x7];
    }else v.hrdatas = 0;
#endif

    if (((r.Q.haddr&0x7FF)>>4) == 0x7F)
    {
      v.hrdatas = LIBVHDL_BUILD&0xFFFF;
      v.hrdatas |= (AHBCTRL_DEVICE_ID<<16);
    }
    if (r.Q.cfgsel)
    {
      hrdata = 0; 
      // default slave
      if ((r.Q.htrans == HTRANS_IDLE) || (r.Q.htrans == HTRANS_BUSY) )
      {
        hresp = HRESP_OKAY; 
        hready = 1;
      }else
      {
        // return two-cycle read/write respons
        hresp = HRESP_OKAY; 
        hready = r.Q.hready; 
        v.hready = !r.Q.hready;
      }
      if (r.Q.cfga11 == 0) hrdata = r.Q.hrdatam;//SH: ahbdrivedata(r.Q.hrdatam);
      else               hrdata = r.Q.hrdatas;//SH: ahbdrivedata(r.Q.hrdatas);
    }
  }

  //degrant all masters when split occurs for locked access
#ifdef CONFIG_AHB_SPLIT
  wbSelMaster = (rMasterLock[1].Q&&((hresp==HRESP_RETRY) || (hresp==HRESP_SPLIT))) ? rbSelMaster[0].Q
                                                                                   : wbSelMstOutMaster;

  rbSelMaster[0].CLK = in_clk;
  if(!in_rst)     rbSelMaster[0].D = 0;
  else if(hready) rbSelMaster[0].D = wbSelMaster;

  rbSelMaster[1].CLK = in_clk;
  if(!in_rst)     rbSelMaster[1].D = 0;
  else if(hready) rbSelMaster[1].D = rbSelMaster[0].Q;

  
  rbSplitMaster.CLK = in_clk;
  if(!in_rst)                                       rbSplitMaster.D = 0;
  else if(rMasterLock[1].Q&&(hresp == HRESP_SPLIT)) rbSplitMaster.D = wbSelMaster;


  wDefmstEna = (rMasterLock[1].Q&&(hresp == HRESP_SPLIT)) ? rbSplit[rbSplitMaster.Q].Q
             : (rSplitDefmstEna.Q & !rbSplit[rbSplitMaster.Q].Q) ? 0
             : wSelMstOutDefmstEna;

  rDefmstEna.CLK = in_clk;
  if(!in_rst)     rDefmstEna.D = 0;
  else if(hready) rDefmstEna.D = wDefmstEna;


  if(!in_rst)                                        rSplitDefmstEna.D = 0;
  else if(rSplitDefmstEna.Q)                         rSplitDefmstEna.D = rbSplit[rbSplitMaster.Q].Q;
  else if (rMasterLock[1].Q&&(hresp == HRESP_SPLIT)) rSplitDefmstEna.D = 1;
  

  if (wDefmstEna == 0)
    hgrant |= (0x1<<wbSelMaster);
#else
  wbSelMaster = ((rMasterLock[1].Q)&&(hresp==HRESP_RETRY)) ? rbSelMaster[0].Q : wbSelMstOutMaster;

  hgrant |= (0x1<<wbSelMaster);
  rSplitDefmstEna.D = 0;
  rbSplitMaster.D = 0;
  rDefmstEna.D  = 0;
#endif

  rbMstSize.CLK = in_clk;
  if(!in_rst)     rbMstSize.D = 0;
  else if(hready) rbMstSize.D = in_msto.arr[rbSelMaster[0].Q].hsize;

  // latch active master and slave
  if (hready)
  {
    v.hslave = nslave;
    v.defslv = defslv;
#ifdef CONFIG_AHB_SPLIT
    if (!rDefmstEna.Q)  v.htrans = in_msto.arr[rbSelMaster[0].Q].htrans;
    else                v.htrans = HTRANS_IDLE;
#else
    v.htrans = in_msto.arr[rbSelMaster[0].Q].htrans;
#endif
    v.cfgsel = cfgsel;
    v.cfga11 = (in_msto.arr[rbSelMaster[0].Q].haddr>>11)&0x1;
    v.haddr = (in_msto.arr[rbSelMaster[0].Q].haddr&0xFFFF)>>2;
    if ((in_msto.arr[rbSelMaster[0].Q].htrans==HTRANS_NONSEQ) || (in_msto.arr[rbSelMaster[0].Q].htrans == HTRANS_IDLE))
    {
      v.beat = 0x1;
    }else if (in_msto.arr[rbSelMaster[0].Q].htrans == HTRANS_SEQ)
    {
#ifdef AHBCTRL_FIXLENGTH_BURST
      v.beat = r.Q.beat + 1;
#endif
    }
  }

  //assign new hmasterlock, v.hmaster is used because if hready
  //then master can have changed, and when not hready then the
  //previous master will still be selected
  rMasterLock[0].CLK = in_clk;
  if(!in_rst) rMasterLock[0].D = 0;
  else        rMasterLock[0].D = in_msto.arr[wbSelMaster].hlock | (rMasterLock[0].Q & !hready);

  rMasterLock[1].CLK = in_clk;
  if(!in_rst)     rMasterLock[1].D = 0;
  else if(hready) rMasterLock[1].D = rMasterLock[1].Q;
  
  // split support
#ifdef CONFIG_AHB_SPLIT
  uint32 wbMstSplit[AHB_MASTERS_MAX];
  for (uint32 i=0; i<AHB_SLAVES_MAX; i++)
  {
    for (uint32 j=0; j<AHB_MASTERS_TOTAL_2n; j++)
      wbMstSplit[j] = ((in_slvo.arr[i].hsplit>>j)&0x1);
  }
  
  for (uint32 i=0; i<AHB_MASTERS_TOTAL_2n; i++)
  {
    rbSplit[i].CLK = in_clk;
    if(!in_rst)                                                                  rbSplit[i].D = 0;
    else if((rbSelMaster[1].Q==i)&&(in_slvo.arr[r.Q.hslave].hresp==HRESP_SPLIT)) rbSplit[i].D = !wbMstSplit[i];
    else                                                                         rbSplit[i].D = rbSplit[i].Q & !wbMstSplit[i];
  }
#else
  for (uint32 i=0; i<AHB_MASTERS_TOTAL_2n; i++)
    rbSplit[i].D = 0;
#endif

  if (r.Q.cfgsel == 1) hcache = 1;
  else               hcache = in_slvo.arr[v.hslave].hcache;
  
  // interrupt merging
#ifdef AHBCTRL_IRQROUTING_DISABLE
  hirq = 0;
#else
  for (uint32 i=0; i<AHB_SLAVES_MAX; i++)     hirq = hirq | in_slvo.arr[i].hirq;
  for (uint32 i=0; i<AHB_MASTER_TOTAL; i++)  hirq = hirq | in_msto.arr[i].hirq;
#endif

#ifdef CONFIG_AHB_SPLIT
  if (rDefmstEna.Q)
  {
    vslvi        = ahbs_in_none;
    vslvi.hready = hready;
    vslvi.hirq   = hirq;
  }else
#endif
  {
    vslvi.haddr      = haddr;
    vslvi.htrans     = in_msto.arr[rbSelMaster[0].Q].htrans;
    vslvi.hwrite     = in_msto.arr[rbSelMaster[0].Q].hwrite;
    vslvi.hsize      = in_msto.arr[rbSelMaster[0].Q].hsize;
    vslvi.hburst     = in_msto.arr[rbSelMaster[0].Q].hburst;
    vslvi.hready     = hready;
    vslvi.hprot      = in_msto.arr[rbSelMaster[0].Q].hprot;
    vslvi.hmastlock  = rMasterLock[0].Q;
    vslvi.hmaster    = rbSelMaster[0].Q&0xF;
    vslvi.hsel       = hsel;        //SH !!!!!!!!! hsel(0 to NAHBSLV-1); 
    vslvi.hmbsel     = hmbsel; 
    vslvi.hcache     = hcache; 
    vslvi.hirq       = hirq;       
  }
#ifdef AHBCTRL_COMPL_DATAMUX_ENABLE
  vslvi.hwdata = ahbselectdata(in_msto.arr[rbSelMaster[1].Q].hwdata, ((r.Q.haddr>>2)&0x7), rbMstSize.Q);
#else
  vslvi.hwdata = in_msto.arr[rbSelMaster[1].Q].hwdata;
#endif
  
  vslvi.testen  = in_testen; 
  vslvi.testrst = in_testrst; 
  vslvi.scanen  = in_scanen & in_testen; 
  vslvi.testoen = in_testoen; 

  // reset operation
  if (!in_rst) 
  {
    v.htrans      = HTRANS_IDLE;
    v.defslv      = 0;
    // v.beat = "0001";
    v.hslave      = 0;
    v.cfgsel      = 0;
  }
    
  // drive master inputs
  out_msti.hgrant  = hgrant;
  out_msti.hready  = hready;
  out_msti.hresp   = hresp;
  out_msti.hrdata  = hrdata;
  out_msti.hcache  = hcache;
  out_msti.hirq    = hirq; 
  out_msti.testen  = in_testen; 
  out_msti.testrst = in_testrst; 
  out_msti.scanen  = in_scanen & in_testen; 
  out_msti.testoen = in_testoen; 

  // drive slave inputs
  out_slvi = vslvi;

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


  r.CLK = in_clk;
  r.D = v;

#if 1
  if(in_clk.eClock==SClock::CLK_POSEDGE)
  {
    bool stop = true;
    if(hgrant!=0x1)
      bool stop = true;
  }
  if(hready)
    bool stop = true;
#endif
}


