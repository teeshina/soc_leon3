#pragma once

#include "stdtypes.h"
#include "leon3_ml605\leon3\mmuconfig.h"


void TLB_CheckFault( uint32 ACC,//        : in  std_logic_vector(2 downto 0);
                      mmu_idcache isid,//       : in  mmu_idcache;
                      uint32 su,//         : in  std_logic;
                      uint32 read,//       : in  std_logic;
                      uint32 &fault_pro,//  : out std_logic;
                      uint32 &fault_pri);//  : out std_logic ) is
                      
void TLB_MergeData( uint32 mmupgsz,//     : in  integer range 0 to 5;
                     mmctrl_type1 &mmctrl,//      : in  mmctrl_type1;
                     uint32 LVL,//         : in  std_logic_vector(1 downto 0);
                     uint32 PTE,//         : in  std_logic_vector(31 downto 0);
                     uint32 data,//        : in  std_logic_vector(31 downto 0);
                     uint32 &transdata);//   : out std_logic_vector(31 downto 0) ) is
                     
tlbcam_reg TLB_CreateCamWrite( uint32 two_data,//  : std_logic_vector(31 downto 0);
                               uint32 read,//      : std_logic;
                               uint32 lvl,//       : std_logic_vector(1 downto 0);
                               uint32 ctx,//       : std_logic_vector(M_CTX_SZ-1 downto 0);
                               uint32 vaddr);//     : std_logic_vector(31 downto 0)//) return tlbcam_reg is

typedef uint32 mmu_gpsz_typ;// is integer range 0 to 3;                               
mmu_gpsz_typ MMU_getpagesize( uint32 mmupgsz,//     : in  integer range 0 to 4;
                              mmctrl_type1 &mmctrl);//      : in  mmctrl_type1) return mmu_gpsz_typ is
                              
tlbcam_tfp TLB_CreateCamTrans( uint32 vaddr,//     : std_logic_vector(31 downto 0);
                               uint32 read,//      : std_logic;
                               uint32 ctx);//       : std_logic_vector(M_CTX_SZ-1 downto 0)) return tlbcam_tfp is
                               
tlbcam_tfp TLB_CreateCamFlush( uint32 data,//      : std_logic_vector(31 downto 0);
                               uint32 ctx);//       : std_logic_vector(M_CTX_SZ-1 downto 0)) return tlbcam_tfp is





