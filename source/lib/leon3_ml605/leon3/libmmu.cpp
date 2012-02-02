//-----------------------------------------------------------------------------
//-- Description: MMU component declaration
//------------------------------------------------------------------------------
#include "lheaders.h"

void TLB_CheckFault( uint32 ACC,//        : in  std_logic_vector(2 downto 0);
                      mmu_idcache isid,//       : in  mmu_idcache;
                      uint32 su,//         : in  std_logic;
                      uint32 read,//       : in  std_logic;
                      uint32 &fault_pro,//  : out std_logic;
                      uint32 &fault_pri)//  : out std_logic ) is
{
  uint32 c_isd;//    : std_logic;

  fault_pro = 0;
  fault_pri = 0;
  
  //-- use '0' == icache '1' == dcache
  if (isid == id_icache)   c_isd = 0;
  else                     c_isd = 1;
  
  //--# fault, todo: should we flush on a fault?
  switch (ACC)
  {
    case 0: fault_pro = (!c_isd) | (!read); break;
    case 1: fault_pro = (!c_isd); break;
    case 2: fault_pro = (!read); break;
    case 3: break;
    case 4: fault_pro = (c_isd); break;
    case 5: fault_pro = (!c_isd) | ((!read) & (!su)); break;
    case 6: fault_pri = (!su);
            fault_pro = (!read);
    break;
    case 7: fault_pri = (!su);
    default:;
  }
}
 
//------------------------------------------------------------------------------
void TLB_MergeData( uint32 mmupgsz,//     : in  integer range 0 to 5;
                     mmctrl_type1 &mmctrl,//      : in  mmctrl_type1;
                     uint32 LVL,//         : in  std_logic_vector(1 downto 0);
                     uint32 PTE,//         : in  std_logic_vector(31 downto 0);
                     uint32 data,//        : in  std_logic_vector(31 downto 0);
                     uint32 &transdata)//   : out std_logic_vector(31 downto 0) ) is
{
  uint32 pagesize;//      : integer range 0 to 3;
  
  //--# merge data
  transdata = 0;
  pagesize  = MMU_getpagesize(mmupgsz, mmctrl);
  switch(pagesize)
  {
    case 1:
      //-- 8k
      switch(LVL)
      {
        case LVL_PAGE: 
          transdata |= BITS32(data, P8K_VA_OFFPAG_U, P8K_VA_OFFPAG_D) ;
          transdata |= (BITS32(PTE,P8K_PTE_PPN32PAG_U,P8K_PTE_PPN32PAG_D) << (P8K_VA_OFFPAG_U-P8K_VA_OFFPAG_D+1) );
        break;
        case LVL_SEGMENT:
          transdata |= BITS32(data, P8K_VA_OFFSEG_U, P8K_VA_OFFSEG_D);
          transdata |= (BITS32(PTE,P8K_PTE_PPN32SEG_U,P8K_PTE_PPN32SEG_D) << (P8K_VA_OFFSEG_U-P8K_VA_OFFSEG_D+1));
        break;
        case LVL_REGION: 
          transdata |= BITS32(data, P8K_VA_OFFREG_U, P8K_VA_OFFREG_D);
          transdata |= (BITS32(PTE,P8K_PTE_PPN32REG_U,P8K_PTE_PPN32REG_D) << (P8K_VA_OFFREG_U-P8K_VA_OFFREG_D+1));
        break;
        case LVL_CTX:
          transdata |= BITS32(data, P8K_VA_OFFCTX_U, P8K_VA_OFFCTX_D);
        default: transdata = 0;//(others => 'X');
      }
    break;
    case 2:
      //-- 16k
      switch(LVL)
      {
        case LVL_PAGE:
          transdata |= BITS32(data,P16K_VA_OFFPAG_U,P16K_VA_OFFPAG_D);
          transdata |= (BITS32(PTE,P16K_PTE_PPN32PAG_U,P16K_PTE_PPN32PAG_D) << (P16K_VA_OFFPAG_U-P16K_VA_OFFPAG_D+1));
        break;
        case LVL_SEGMENT:
          transdata |= BITS32(data,P16K_VA_OFFSEG_U, P16K_VA_OFFSEG_D);
          transdata |= (BITS32(PTE,P16K_PTE_PPN32SEG_U,P16K_PTE_PPN32SEG_D) << (P16K_VA_OFFSEG_U-P16K_VA_OFFSEG_D+1));
        break;
        case LVL_REGION:
          transdata |= BITS32(data,P16K_VA_OFFREG_U,P16K_VA_OFFREG_D);
          transdata |= (BITS32(PTE,P16K_PTE_PPN32REG_U,P16K_PTE_PPN32REG_D) << (P16K_VA_OFFREG_U-P16K_VA_OFFREG_D+1));
        break;
        case LVL_CTX:
          transdata |= BITS32(data,P16K_VA_OFFCTX_U,P16K_VA_OFFCTX_D);
        default: transdata = 0;//(others => 'X');
      }
    break;
    case 3:
      //-- 32k
      switch(LVL)
      {
        case LVL_PAGE:
          transdata |= BITS32(data,P32K_VA_OFFPAG_U,P32K_VA_OFFPAG_D);
          transdata |= (BITS32(PTE,P32K_PTE_PPN32PAG_U,P32K_PTE_PPN32PAG_D) << (P32K_VA_OFFPAG_U-P32K_VA_OFFPAG_D+1));
        break;
        case LVL_SEGMENT:
          transdata |= BITS32(data,P32K_VA_OFFSEG_U,P32K_VA_OFFSEG_D);
          transdata |= (BITS32(PTE,P32K_PTE_PPN32SEG_U,P32K_PTE_PPN32SEG_D) << (P32K_VA_OFFSEG_U-P32K_VA_OFFSEG_D+1));
        break;
        case LVL_REGION:
          transdata |= BITS32(data,P32K_VA_OFFREG_U,P32K_VA_OFFREG_D);
          transdata |= (BITS32(PTE,P32K_PTE_PPN32REG_U,P32K_PTE_PPN32REG_D) << (P32K_VA_OFFREG_U-P32K_VA_OFFREG_D+1));
        break;
        case LVL_CTX:
          transdata |= BITS32(data,P32K_VA_OFFCTX_U,P32K_VA_OFFCTX_D);
        break;
        default: transdata = 0;//(others => 'X');
      }
    break;
    default:
      //-- 4k
      switch (LVL)
      {
        case LVL_PAGE:
          transdata |= BITS32(data,VA_OFFPAG_U,VA_OFFPAG_D);
          transdata |= (BITS32(PTE,PTE_PPN32PAG_U,PTE_PPN32PAG_D) << (VA_OFFPAG_U-VA_OFFPAG_D+1));
        break;
        case LVL_SEGMENT:
          transdata |= BITS32(data,VA_OFFSEG_U,VA_OFFSEG_D);
          transdata |= (BITS32(PTE,PTE_PPN32SEG_U,PTE_PPN32SEG_D) << (VA_OFFSEG_U-VA_OFFSEG_D+1));
        break;
        case LVL_REGION:
          transdata |= BITS32(data,VA_OFFREG_U,VA_OFFREG_D);
          transdata |= (BITS32(PTE,PTE_PPN32REG_U,PTE_PPN32REG_D) << (VA_OFFREG_U-VA_OFFREG_D+1));
        break;
        case LVL_CTX:
          transdata = BITS32(data,VA_OFFCTX_U,VA_OFFCTX_D);
        break;
        default: transdata = 0;//(others => 'X');
      }
  }
}

//------------------------------------------------------------------------------
tlbcam_reg TLB_CreateCamWrite( uint32 two_data,//  : std_logic_vector(31 downto 0);
                               uint32 read,//      : std_logic;
                               uint32 lvl,//       : std_logic_vector(1 downto 0);
                               uint32 ctx,//       : std_logic_vector(M_CTX_SZ-1 downto 0);
                               uint32 vaddr)//     : std_logic_vector(31 downto 0)//) return tlbcam_reg is
{
  tlbcam_reg tlbcam_tagwrite;//      : tlbcam_reg;

  tlbcam_tagwrite.ET    = BITS32(two_data,PT_ET_U,PT_ET_D);
  tlbcam_tagwrite.ACC   = BITS32(two_data,PTE_ACC_U,PTE_ACC_D);
  tlbcam_tagwrite.M     = BIT32(two_data,PTE_M) | (!read); //-- tw : p-update modified
  tlbcam_tagwrite.R     = 1;                         
  switch (tlbcam_tagwrite.ACC)// is      -- tw : p-su ACC >= 6
  {
    case 6: case 7:
      tlbcam_tagwrite.SU = 1;
    break;
    default:        tlbcam_tagwrite.SU = 0;
  }
  tlbcam_tagwrite.VALID = 1;
  tlbcam_tagwrite.LVL   = lvl;
  tlbcam_tagwrite.I1    = BITS32(vaddr,VA_I1_U,VA_I1_D);
  tlbcam_tagwrite.I2    = BITS32(vaddr,VA_I2_U,VA_I2_D);
  tlbcam_tagwrite.I3    = BITS32(vaddr,VA_I3_U,VA_I3_D);
  tlbcam_tagwrite.CTX   = ctx;
  tlbcam_tagwrite.PPN   = BITS32(two_data,PTE_PPN_U,PTE_PPN_D);
  tlbcam_tagwrite.C     = BIT32(two_data,PTE_C);
  return tlbcam_tagwrite;
}

//------------------------------------------------------------------------------
mmu_gpsz_typ MMU_getpagesize( uint32 mmupgsz,//     : in  integer range 0 to 4;
                              mmctrl_type1 &mmctrl)//      : in  mmctrl_type1) return mmu_gpsz_typ is
{
  mmu_gpsz_typ pagesize;//      : mmu_gpsz_typ;

  if (mmupgsz == 4) pagesize = mmctrl.pagesize;  //-- variable
  else              pagesize = mmupgsz;
  return pagesize;
}

//------------------------------------------------------------------------------
tlbcam_tfp TLB_CreateCamTrans( uint32 vaddr,//     : std_logic_vector(31 downto 0);
                               uint32 read,//      : std_logic;
                               uint32 ctx)//       : std_logic_vector(M_CTX_SZ-1 downto 0)) return tlbcam_tfp is
{
  tlbcam_tfp mtag;//            : tlbcam_tfp;

  mtag.TYP = 0;
  mtag.I1  = BITS32(vaddr,VA_I1_U,VA_I1_D);
  mtag.I2  = BITS32(vaddr,VA_I2_U,VA_I2_D);
  mtag.I3  = BITS32(vaddr,VA_I3_U,VA_I3_D);
  mtag.CTX =  ctx;
  mtag.M   =  !(read);
  return mtag;
}

//------------------------------------------------------------------------------
tlbcam_tfp TLB_CreateCamFlush( uint32 data,//      : std_logic_vector(31 downto 0);
                               uint32 ctx)//       : std_logic_vector(M_CTX_SZ-1 downto 0)) return tlbcam_tfp is
{
  tlbcam_tfp ftag;//            : tlbcam_tfp;

  ftag.TYP = BITS32(data,FPTY_U,FPTY_D);
  ftag.I1  = BITS32(data,FPA_I1_U,FPA_I1_D);
  ftag.I2  = BITS32(data,FPA_I2_U,FPA_I2_D);
  ftag.I3  = BITS32(data,FPA_I3_U,FPA_I3_D);
  ftag.CTX = ctx;
  ftag.M   = 0;
  return ftag;
}


