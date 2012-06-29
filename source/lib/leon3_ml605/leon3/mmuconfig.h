#pragma once

#include "stdtypes.h"
#include "const.h"

const uint32 M_CTX_SZ       = 8;
const uint32 M_ENT_MAX      = 64;
const uint32 XM_ENT_MAX_LOG = log2[M_ENT_MAX];
const uint32 M_ENT_MAX_LOG  = XM_ENT_MAX_LOG;

enum mmu_idcache {id_icache, id_dcache};

//-- ##############################################################
//--     1.0 virtual address [sparc V8: p.243,Appx.H,Figure H-4]               
//--     +--------+--------+--------+---------------+
//--  a) | INDEX1 | INDEX2 | INDEX3 |    OFFSET     |  
//--     +--------+--------+--------+---------------+
//--      31    24 23    18 17    12 11            0

const uint32 VA_I1_SZ = 8;
const uint32 VA_I2_SZ = 6;
const uint32 VA_I3_SZ = 6;
const uint32 VA_I_SZ  = VA_I1_SZ+VA_I2_SZ+VA_I3_SZ;
const uint32 VA_I_MAX = 8;

const uint32 VA_I1_U  = 31;
const uint32 VA_I1_D  = 32-VA_I1_SZ;
const uint32 VA_I2_U  = 31-VA_I1_SZ;
const uint32 VA_I2_D  = 32-VA_I1_SZ-VA_I2_SZ;
const uint32 VA_I3_U  = 31-VA_I1_SZ-VA_I2_SZ;
const uint32 VA_I3_D  = 32-VA_I_SZ;
const uint32 VA_I_U   = 31;
const uint32 VA_I_D   = 32-VA_I_SZ;
const uint32 VA_OFF_U = 31-VA_I_SZ;
const uint32 VA_OFF_D = 0;

const uint32 VA_OFFCTX_U = 31;
const uint32 VA_OFFCTX_D = 0;
const uint32 VA_OFFREG_U = 31-VA_I1_SZ;
const uint32 VA_OFFREG_D = 0;
const uint32 VA_OFFSEG_U = 31-VA_I1_SZ-VA_I2_SZ;
const uint32 VA_OFFSEG_D = 0;
const uint32 VA_OFFPAG_U = 31-VA_I_SZ;
const uint32 VA_OFFPAG_D = 0;


//-- 8k pages
//--         7        6        6         13
//--     +--------+--------+--------+---------------+
//--  a) | INDEX1 | INDEX2 | INDEX3 |    OFFSET     |  
//--     +--------+--------+--------+---------------+
//--      31    25 24    19 18    13 12            0

const uint32 P8K_VA_I1_SZ = 7;
const uint32 P8K_VA_I2_SZ = 6;
const uint32 P8K_VA_I3_SZ = 6;
const uint32 P8K_VA_I_SZ  = P8K_VA_I1_SZ+P8K_VA_I2_SZ+P8K_VA_I3_SZ;
const uint32 P8K_VA_I_MAX = 7;

const uint32 P8K_VA_I1_U  = 31;
const uint32 P8K_VA_I1_D  = 32-P8K_VA_I1_SZ;
const uint32 P8K_VA_I2_U  = 31-P8K_VA_I1_SZ;
const uint32 P8K_VA_I2_D  = 32-P8K_VA_I1_SZ-P8K_VA_I2_SZ;
const uint32 P8K_VA_I3_U  = 31-P8K_VA_I1_SZ-P8K_VA_I2_SZ;
const uint32 P8K_VA_I3_D  = 32-P8K_VA_I_SZ;
const uint32 P8K_VA_I_U   = 31;
const uint32 P8K_VA_I_D   = 32-P8K_VA_I_SZ;
const uint32 P8K_VA_OFF_U = 31-P8K_VA_I_SZ;
const uint32 P8K_VA_OFF_D = 0;

const uint32 P8K_VA_OFFCTX_U = 31;
const uint32 P8K_VA_OFFCTX_D = 0;
const uint32 P8K_VA_OFFREG_U = 31-P8K_VA_I1_SZ;
const uint32 P8K_VA_OFFREG_D = 0;
const uint32 P8K_VA_OFFSEG_U = 31-P8K_VA_I1_SZ-P8K_VA_I2_SZ;
const uint32 P8K_VA_OFFSEG_D = 0;
const uint32 P8K_VA_OFFPAG_U = 31-P8K_VA_I_SZ;
const uint32 P8K_VA_OFFPAG_D = 0;


//-- 16k pages
//--         6        6        6         14
//--     +--------+--------+--------+---------------+
//--  a) | INDEX1 | INDEX2 | INDEX3 |    OFFSET     |  
//--     +--------+--------+--------+---------------+
//--      31    26 25    20 19    14 13            0

const uint32 P16K_VA_I1_SZ = 6;
const uint32 P16K_VA_I2_SZ = 6;
const uint32 P16K_VA_I3_SZ = 6;
const uint32 P16K_VA_I_SZ  = P16K_VA_I1_SZ+P16K_VA_I2_SZ+P16K_VA_I3_SZ;
const uint32 P16K_VA_I_MAX = 6;

const uint32 P16K_VA_I1_U  = 31;
const uint32 P16K_VA_I1_D  = 32-P16K_VA_I1_SZ;
const uint32 P16K_VA_I2_U  = 31-P16K_VA_I1_SZ;
const uint32 P16K_VA_I2_D  = 32-P16K_VA_I1_SZ-P16K_VA_I2_SZ;
const uint32 P16K_VA_I3_U  = 31-P16K_VA_I1_SZ-P16K_VA_I2_SZ;
const uint32 P16K_VA_I3_D  = 32-P16K_VA_I_SZ;
const uint32 P16K_VA_I_U   = 31;
const uint32 P16K_VA_I_D   = 32-P16K_VA_I_SZ;
const uint32 P16K_VA_OFF_U = 31-P16K_VA_I_SZ;
const uint32 P16K_VA_OFF_D = 0;

const uint32 P16K_VA_OFFCTX_U = 31;
const uint32 P16K_VA_OFFCTX_D = 0;
const uint32 P16K_VA_OFFREG_U = 31-P16K_VA_I1_SZ;
const uint32 P16K_VA_OFFREG_D = 0;
const uint32 P16K_VA_OFFSEG_U = 31-P16K_VA_I1_SZ-P16K_VA_I2_SZ;
const uint32 P16K_VA_OFFSEG_D = 0;
const uint32 P16K_VA_OFFPAG_U = 31-P16K_VA_I_SZ;
const uint32 P16K_VA_OFFPAG_D = 0;


//-- 32k pages
//--         4        7        6         15
//--     +--------+--------+--------+---------------+
//--  a) | INDEX1 | INDEX2 | INDEX3 |    OFFSET     |  
//--     +--------+--------+--------+---------------+
//--      31    28 27    21 20    15 14            0

const uint32 P32K_VA_I1_SZ = 4;
const uint32 P32K_VA_I2_SZ = 7;
const uint32 P32K_VA_I3_SZ = 6;
const uint32 P32K_VA_I_SZ  = P32K_VA_I1_SZ+P32K_VA_I2_SZ+P32K_VA_I3_SZ;
const uint32 P32K_VA_I_MAX = 7;

const uint32 P32K_VA_I1_U  = 31;
const uint32 P32K_VA_I1_D  = 32-P32K_VA_I1_SZ;
const uint32 P32K_VA_I2_U  = 31-P32K_VA_I1_SZ;
const uint32 P32K_VA_I2_D  = 32-P32K_VA_I1_SZ-P32K_VA_I2_SZ;
const uint32 P32K_VA_I3_U  = 31-P32K_VA_I1_SZ-P32K_VA_I2_SZ;
const uint32 P32K_VA_I3_D  = 32-P32K_VA_I_SZ;
const uint32 P32K_VA_I_U   = 31;
const uint32 P32K_VA_I_D   = 32-P32K_VA_I_SZ;
const uint32 P32K_VA_OFF_U = 31-P32K_VA_I_SZ;
const uint32 P32K_VA_OFF_D = 0;

const uint32 P32K_VA_OFFCTX_U = 31;
const uint32 P32K_VA_OFFCTX_D = 0;
const uint32 P32K_VA_OFFREG_U = 31-P32K_VA_I1_SZ;
const uint32 P32K_VA_OFFREG_D = 0;
const uint32 P32K_VA_OFFSEG_U = 31-P32K_VA_I1_SZ-P32K_VA_I2_SZ;
const uint32 P32K_VA_OFFSEG_D = 0;
const uint32 P32K_VA_OFFPAG_U = 31-P32K_VA_I_SZ;
const uint32 P32K_VA_OFFPAG_D = 0;


//-- ##############################################################
//--     2.0 PAGE TABE DESCRIPTOR (PTD) [sparc V8: p.247,Appx.H,Figure H-7]                             
//--                                                                  
//--     +-------------------------------------------------+---+---+      
//--     |    Page Table Pointer (PTP)                     | 0 | 0 |      
//--     +-------------------------------------------------+---+---+      
//--      31                                              2  1   0        
//--
//--     2.1 PAGE TABE ENTRY (PTE) [sparc V8: p.247,Appx.H,Figure H-8]
//--                                                                                       
//--     +-----------------------------+---+---+---+-----------+---+
//--     |Physical Page Number (PPN)   | C | M | R |     ACC   | ETæ   
//--     +-----------------------------+---+---+---+-----------+---+
//--      31                          8  7   6   5  4         2 1 0
//--                                                                     
const uint32 PTD_PTP_U = 31;   //-- PTD: page table pointer
const uint32 PTD_PTP_D = 2;    
const uint32 PTD_PTP32_U = 27;   //-- PTD: page table pointer 32 bit
const uint32 PTD_PTP32_D = 2;    
const uint32 PTE_PPN_U = 31;   //-- PTE: physical page number
const uint32 PTE_PPN_D = 8;     
const uint32 PTE_PPN_S = (PTE_PPN_U+1)-PTE_PPN_D;  //-- PTE: pysical page number size
const uint32 PTE_PPN32_U = 27; //-- PTE: physical page number 32 bit addr
const uint32 PTE_PPN32_D = 8;    
const uint32 PTE_PPN32_S = (PTE_PPN32_U+1)-PTE_PPN32_D;  //-- PTE: pysical page number 32 bit size

const uint32 PTE_PPN32REG_U = PTE_PPN32_U;  //-- PTE: pte part of merged result address
const uint32 PTE_PPN32REG_D = PTE_PPN32_U+1-VA_I1_SZ;
const uint32 PTE_PPN32SEG_U = PTE_PPN32_U;
const uint32 PTE_PPN32SEG_D = PTE_PPN32_U+1-VA_I1_SZ-VA_I2_SZ;
const uint32 PTE_PPN32PAG_U = PTE_PPN32_U;
const uint32 PTE_PPN32PAG_D = PTE_PPN32_U+1-VA_I_SZ;

//-- 8k pages 
const uint32 P8K_PTE_PPN32REG_U = PTE_PPN32_U;  //-- PTE: pte part of merged result address
const uint32 P8K_PTE_PPN32REG_D = PTE_PPN32_U+1-P8K_VA_I1_SZ;
const uint32 P8K_PTE_PPN32SEG_U = PTE_PPN32_U;
const uint32 P8K_PTE_PPN32SEG_D = PTE_PPN32_U+1-P8K_VA_I1_SZ-P8K_VA_I2_SZ;
const uint32 P8K_PTE_PPN32PAG_U = PTE_PPN32_U;
const uint32 P8K_PTE_PPN32PAG_D = PTE_PPN32_U+1-P8K_VA_I_SZ;

//-- 16k pages 
const uint32 P16K_PTE_PPN32REG_U = PTE_PPN32_U;  //-- PTE: pte part of merged result address
const uint32 P16K_PTE_PPN32REG_D = PTE_PPN32_U+1-P16K_VA_I1_SZ;
const uint32 P16K_PTE_PPN32SEG_U = PTE_PPN32_U;
const uint32 P16K_PTE_PPN32SEG_D = PTE_PPN32_U+1-P16K_VA_I1_SZ-P16K_VA_I2_SZ;
const uint32 P16K_PTE_PPN32PAG_U = PTE_PPN32_U;
const uint32 P16K_PTE_PPN32PAG_D = PTE_PPN32_U+1-P16K_VA_I_SZ;

//-- 32k pages 
const uint32 P32K_PTE_PPN32REG_U = PTE_PPN32_U;  //-- PTE: pte part of merged result address
const uint32 P32K_PTE_PPN32REG_D = PTE_PPN32_U+1-P32K_VA_I1_SZ;
const uint32 P32K_PTE_PPN32SEG_U = PTE_PPN32_U;
const uint32 P32K_PTE_PPN32SEG_D = PTE_PPN32_U+1-P32K_VA_I1_SZ-P32K_VA_I2_SZ;
const uint32 P32K_PTE_PPN32PAG_U = PTE_PPN32_U;
const uint32 P32K_PTE_PPN32PAG_D = PTE_PPN32_U+1-P32K_VA_I_SZ;



const uint32 PTE_C = 7;        //-- PTE: Cacheable bit
const uint32 PTE_M = 6;        //-- PTE: Modified bit 
const uint32 PTE_R = 5;        //-- PTE: Reference Bit - a "1" indicates an PTE 
                                       
const uint32 PTE_ACC_U = 4;    //-- PTE: Access field 
const uint32 PTE_ACC_D = 2;     
const uint32 ACC_W = 2;        //-- PTE::ACC : write permission
const uint32 ACC_E = 3;        //-- PTE::ACC : exec permission
const uint32 ACC_SU = 4;       //-- PTE::ACC : privileged
                                       
const uint32 PT_ET_U = 1;      //-- PTD/PTE: PTE Type
const uint32 PT_ET_D = 0;            
const uint32 ET_INV = 0;  
const uint32 ET_PTD = 1;
const uint32 ET_PTE = 2;
const uint32 ET_RVD = 3;

const uint32 PADDR_PTD_U = 31;   
const uint32 PADDR_PTD_D = 6;

//-- ##############################################################
//--     3.0 TLBCAM TAG hardware representation (TTG)
//--
struct tlbcam_reg
{
   uint32 ET ;//    : std_logic_vector(1 downto 0);              -- et field 
   uint32 ACC;//    : std_logic_vector(2 downto 0);              -- on flush/probe this will become FPTY
   uint32 M  ;//    : std_logic;                                 -- modified
   uint32 R  ;//    : std_logic;                                 -- referenced
   uint32 SU ;//    : std_logic;                                 -- equal ACC >= 6
   uint32 VALID ;// : std_logic;                                  
   uint32 LVL ;//   : std_logic_vector(1 downto 0);              -- level in pth
   uint32 I1 ;//    : std_logic_vector(7 downto 0);              -- vaddr
   uint32 I2 ;//    : std_logic_vector(5 downto 0);               
   uint32 I3 ;//    : std_logic_vector(5 downto 0);               
   uint32 CTX;//    : std_logic_vector(M_CTX_SZ-1 downto 0);     -- ctx number
   uint32 PPN;//     : std_logic_vector(PTE_PPN_S-1 downto 0);    -- physical page number
   uint32 C ;//     : std_logic;                                 -- cachable
};

const tlbcam_reg tlbcam_reg_none = {0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, {0},0};
//-- tlbcam_reg::LVL 
const uint32 LVL_PAGE    = 0;//"00"; -- equal tlbcam_tfp::TYP FPTY_PAGE
const uint32 LVL_SEGMENT = 1;//"01"; -- equal tlbcam_tfp::TYP FPTY_SEGMENT
const uint32 LVL_REGION  = 2;//"10"; -- equal tlbcam_tfp::TYP FPTY_REGION
const uint32 LVL_CTX     = 3;//"11"; -- equal tlbcam_tfp::TYP FPTY_CTX

//-- ##############################################################
//--     4.0 TLBCAM tag i/o for translation/flush/(probe)
//--
struct tlbcam_tfp
{
   uint32 TYP :3;//   : std_logic_vector(2 downto 0);        -- f/(p) type
   uint32 I1  :8;//   : std_logic_vector(7 downto 0);        -- vaddr
   uint32 I2  :6;//   : std_logic_vector(5 downto 0);
   uint32 I3  :6;//   : std_logic_vector(5 downto 0);
   uint32 CTX;//    : std_logic_vector(M_CTX_SZ-1 downto 0);  -- ctx number
   uint32 M :1;//     : std_logic;
};

const tlbcam_tfp tlbcam_tfp_none = {0, 0, 0, 0, 0, 0};

//--tlbcam_tfp::TYP
const uint32 FPTY_PAGE    = 0;//"000";  -- level 3 PTE  match I1+I2+I3
const uint32 FPTY_SEGMENT = 1;//"001";  -- level 2/3 PTE/PTD match I1+I2
const uint32 FPTY_REGION  = 2;//"010";  -- level 1/2/3 PTE/PTD match I1
const uint32 FPTY_CTX     = 3;//"011";  -- level 0/1/2/3 PTE/PTD ctx
const uint32 FPTY_N       = 4;//"100";  -- entire tlb

//-- ##############################################################
//--     5.0 MMU Control Register [sparc V8: p.253,Appx.H,Figure H-10]
//--
//--     +-------+-----+------------------+-----+-------+--+--+      
//--     |  IMPL | VER |        SC        | PSO | resvd |NF|E |      
//--     +-------+-----+------------------+-----+-------+--+--+
//--      31  28  27 24 23               8   7   6     2  1  0
//--      
//--     MMU Context Pointer [sparc V8: p.254,Appx.H,Figure H-11]                      
//--     +-------------------------------------------+--------+
//--     |         Context Table Pointer             |  resvd |      
//--     +-------------------------------------------+--------+
//--      31                                        2 1      0                
//--
//--     MMU Context Number [sparc V8: p.255,Appx.H,Figure H-12]                                                          
//--     +----------------------------------------------------+
//--     |              Context Table Pointer                 |      
//--     +----------------------------------------------------+
//--      31                                                 0
//--      
//--     fault status/address register [sparc V8: p.256,Appx.H,Table H-13/14] 
//--     +------------+-----+---+----+----+-----+----+
//--     |   reserved | EBE | L | AT | FT | FAV | OW |     
//--     +------------+-----+---+----+----+-----+----+
//--     31         18 17 10 9 8 7  5 4  2   1    0
//--
//--     +----------------------------------------------------+
//--     |              fault address register                |      
//--     +----------------------------------------------------+
//--      31                                                 0                

const uint32 MMCTRL_CTXP_SZ = 30;
const uint32 MMCTRL_PTP32_U = 25;   
const uint32 MMCTRL_PTP32_D = 0;   

const uint32 MMCTRL_E  = 0;
const uint32 MMCTRL_NF = 1;
const uint32 MMCTRL_PSO = 7;
const uint32 MMCTRL_SC_U = 23;
const uint32 MMCTRL_SC_D = 8;

const uint32 MMCTRL_PGSZ_U = 17;
const uint32 MMCTRL_PGSZ_D = 16;

const uint32 MMCTRL_VER_U = 27;
const uint32 MMCTRL_VER_D = 24;
const uint32 MMCTRL_IMPL_U = 31;
const uint32 MMCTRL_IMPL_D = 28;
const uint32 MMCTRL_TLBDIS = 15;
const uint32 MMCTRL_TLBSEP = 14;

const uint32 MMCTXP_U = 31;
const uint32 MMCTXP_D = 2;

const uint32 MMCTXNR_U = M_CTX_SZ-1;
const uint32 MMCTXNR_D = 0;

const uint32 FS_SZ = 18;  //-- fault status size

const uint32 FS_EBE_U = 17;
const uint32 FS_EBE_D = 10;

const uint32 FS_L_U = 9;
const uint32 FS_L_D = 8;
const uint32 FS_L_CTX = 0;
const uint32 FS_L_L1 = 1;
const uint32 FS_L_L2 = 2;
const uint32 FS_L_L3 = 3;

const uint32 FS_AT_U = 7;
const uint32 FS_AT_D = 5;
const uint32 FS_AT_LS = 7;       //--L=0 S=1
const uint32 FS_AT_ID = 6;       //--D=0 I=1
const uint32 FS_AT_SU = 5;       //--U=0 SU=1
const uint32 FS_AT_LUDS = 0;
const uint32 FS_AT_LSDS = 1;
const uint32 FS_AT_LUIS = 2;
const uint32 FS_AT_LSIS = 3;
const uint32 FS_AT_SUDS = 4;
const uint32 FS_AT_SSDS = 5;
const uint32 FS_AT_SUIS = 6;
const uint32 FS_AT_SSIS = 7;

const uint32 FS_FT_U      = 4;
const uint32 FS_FT_D      = 2;
const uint32 FS_FT_NONE   = 0;//"000";
const uint32 FS_FT_INV    = 1;//"001";
const uint32 FS_FT_PRO    = 2;//"010";
const uint32 FS_FT_PRI    = 3;//"011";
const uint32 FS_FT_TRANS  = 6;//"110";
const uint32 FS_FT_BUS    = 5;//"101";
const uint32 FS_FT_INT    = 6;//"110";
const uint32 FS_FT_RVD    = 7;//"111";

const uint32 FS_FAV = 1;
const uint32 FS_OW = 0;

//--# mmu ctrl reg
struct mmctrl_type1
{
  uint32 e   :1;//    : std_logic;                -- enable
  uint32 nf  :1;//    : std_logic;                -- no fault
  uint32 pso :1;//    : std_logic;                -- partial store order
//--  pre     : std_logic;                            -- pretranslation source
//--  pri     : std_logic;                            -- i/d priority 
  uint32 pagesize : 2;// std_logic_vector(1 downto 0);-- page size
  
  uint32 ctx;//     : std_logic_vector(M_CTX_SZ-1 downto 0);-- context nr
  uint32 ctxp;//    : std_logic_vector(MMCTRL_CTXP_SZ-1 downto 0);  -- context table pointer
  uint32 tlbdis :1;// : std_logic;                            -- tlb disabled
  uint32 bar    :2;// : std_logic_vector(1 downto 0);         -- preplace barrier
};

const mmctrl_type1 mmctrl_type1_none = {0, 0, 0, {0}, {0}, {0}, 0, {0}};

//--# fault status reg
struct mmctrl_fs_type
{
  uint32 ow    : 1;//std_logic;          
  uint32 fav   : 1;//std_logic;          
  uint32 ft    : 3;//std_logic_vector(2 downto 0);           -- fault type
  uint32 at_ls : 1;//std_logic;                              -- access type, load/store
  uint32 at_id : 1;//std_logic;                              -- access type, i/dcache
  uint32 at_su : 1;//std_logic;                              -- access type, su/user
  uint32 l     : 2;//std_logic_vector(1 downto 0);           -- level 
  uint32 ebe   : 8;//std_logic_vector(7 downto 0);            
};

const mmctrl_fs_type mmctrl_fs_zero =   {0, 0, 0, 0, 0, 0, 0, 0};

struct mmctrl_type2
{
  mmctrl_fs_type fs;//    : mmctrl_fs_type;
  uint32 valid :1;// std_logic;
  uint32 fa;//    : std_logic_vector(VA_I_SZ-1 downto 0);   -- fault address register
};

const mmctrl_type2 mmctrl2_zero =  {mmctrl_fs_zero, 0,{0}};

//-- ##############################################################
//--     6. Virtual Flush/Probe address [sparc V8: p.249,Appx.H,Figure H-9]
//--     +---------------------------------------+--------+-------+
//--     |   VIRTUAL FLUSH&Probe Address (VFPA)  |  type  |  rvd  |      
//--     +---------------------------------------+--------+-------+  
//--      31                                   12 11     8 7      0         
//--     
//--
typedef uint32 FPA; //subtype FPA is natural range  31 downto 12;
const uint32 FPA_I1_U = 31;
const uint32 FPA_I1_D = 24;
const uint32 FPA_I2_U = 23;
const uint32 FPA_I2_D = 18;
const uint32 FPA_I3_U = 17;
const uint32 FPA_I3_D = 12;
const uint32 FPTY_U = 10;        //-- only 3 bits
const uint32 FPTY_D = 8;

//-- ##############################################################
//--     7. control register virtual address [sparc V8: p.253,Appx.H,Table H-5] 
//--     +---------------------------------+-----+--------+
//--     |                                 | CNR |  rsvd  |      
//--     +---------------------------------+-----+--------+  
//--      31                                10  8 7      0
      
const uint32 CNR_U        = 10;
const uint32 CNR_D        = 8;    
const uint32 CNR_CTRL     = 0;//"000";
const uint32 CNR_CTXP     = 1;//"001";
const uint32 CNR_CTX      = 2;//"010";
const uint32 CNR_F        = 3;//"011";
const uint32 CNR_FADDR    = 4;//"100";

//-- ##############################################################
//--     8. Precise flush (ASI 0x10-14) [sparc V8: p.266,Appx.I]
//--        supported: ASI_FLUSH_PAGE
//--                   ASI_FLUSH_CTX
                     
const uint32 PFLUSH_PAGE = 0;
const uint32 PFLUSH_CTX  = 1;

//-- ##############################################################
//--     9. Diagnostic access
//--        
const uint32 DIAGF_LVL_U = 1;
const uint32 DIAGF_LVL_D = 0;
const uint32 DIAGF_WR    = 3;
const uint32 DIAGF_HIT   = 4;
const uint32 DIAGF_CTX_U = 12;
const uint32 DIAGF_CTX_D = 5;
const uint32 DIAGF_VALID = 13;



