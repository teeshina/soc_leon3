#include "lheaders.h"

// Modificatoins:
//    1. Add reset:
//        r.w.s.cwp <= (others => '0');
//        r.w.s.tba <= (others => '0');
//        r.w.s.y   <= (others => '0');
//        r.w.s.icc <= (others => '0');
//        r.w.s.pil <= (others => '0');
//        r.w.s.wim <= (others => '0');
//        r.x.y     <= (others => '0');
//        r.x.icc   <= (others => '0');
//
//    2. Change:
//        wpr(i).exec <= '0'; wpr(i).load <= '0'; wpr(i).store <= '0';
//    on
//        wpr(i) <= wpr_none;

//#define nwin    CFG_NWIN//     : integer range 2 to 32 := 8;
//#define isets   CFG_ISETS//   : integer range 1 to 4 := 1;
//#define dsets   CFG_DSETS//   : integer range 1 to 4 := 1;
//#define fpu     CFG_FPU//     : integer range 0 to 15 := 0;
//#define v8      CFG_V8//      : integer range 0 to 63 := 0;
//#define cp      0// co-processor enable/disable
//#define gen_mac CFG_MAC//  : integer range 0 to 1 := 0;
//#define dsu     CFG_DSU//     : integer range 0 to 1 := 0;
//#define nwp     CFG_NWP//      : integer range 0 to 4 := 0;
//#define gen_pclow   CFG_PCLOW//   : integer range 0 to 2 := 2;
//#define notag   CFG_NOTAG//0//   : integer range 0 to 1 := 0;
//#define INDEX   0//   : integer range 0 to 15:= 0;
//#define lddel   CFG_LDDEL//   : integer range 1 to 2 := 2;
//#define irfwt   CFG_IRFWT//    : integer range 0 to 1 := 0;
//#define disas   CFG_DISAS//   : integer range 0 to 2 := 0;
//#define tbuf    CFG_ITBSZ//    : integer range 0 to 64 := 0;  -- trace buf size in kB (0 - no trace buffer)
//#define pwd     CFG_PWD//     : integer range 0 to 2 := 0;   -- power-down
//#define svt     CFG_SVT//     : integer range 0 to 1 := 0;   -- single-vector trapping
//#define rstaddr CFG_RSTADDR// : integer := 16#00000#;        -- reset vector MSB address
//#define smp     (CFG_NCPU-1)//     : integer range 0 to 15 := 0;  -- support SMP systems
#define fabtech 0// : integer range 0 to NTECH := 0;
#define clk2x   0//   : integer := 0;
//#define bp      CFG_BP//      : integer := 1

//#define ISETMSB   (log2x[CFG_ISETS]-1)//: integer := log2x(isets)-1;
//#define DSETMSB   (log2x[CFG_DSETS]-1)//: integer := log2x(dsets)-1;
//#define RFBITS    (log2[CFG_NWIN+1]+4)//: integer range 6 to 10 := log2(NWIN+1) + 4;
//#define NWINLOG2  (log2[CFG_NWIN])// : integer range 1 to 5 := log2(NWIN);
#define CWPOPT    (CFG_NWIN==(0x1<<NWINLOG2))//: boolean := (NWIN = (2**NWINLOG2));
#define CWPMIN    0//: std_logic_vector(NWINLOG2-1 downto 0) := (others => '0');
#define CWPMAX    (CFG_NWIN-1)//: std_logic_vector(NWINLOG2-1 downto 0) := conv_std_logic_vector(NWIN-1, NWINLOG2);
#define FPEN      (CFG_FPU!=0)//: boolean := (fpu /= 0);
#define CPEN      (CFG_COPROC_ENA==1)//: boolean := (cp = 1);
#define MULEN     (CFG_V8!=0)//: boolean := (v8 /= 0);
#define MULTYPE   (CFG_V8/16)//: integer := (v8 / 16);
#define DIVEN     (CFG_V8!=0)//: boolean := (v8 /= 0);
#define MACEN     (CFG_MAC==1)//: boolean := (mac = 1);
#define MACPIPE   ((CFG_MAC==1)&&((CFG_V8/2)==1))//: boolean := (mac = 1) and (v8/2 = 1);
#define IMPL      15//: integer := 15;
#define VER       3//: integer := 3;
#define DBGUNIT   (CFG_DSU==1)//: boolean := (dsu = 1);
#define TRACEBUF  (CFG_ITBSZ!=0)// : boolean := (tbuf /= 0);
#define TBUFBITS  (10+log2[CFG_ITBSZ]-4)//: integer := 10 + log2(tbuf) - 4;
#define PWRD1     (false)//: boolean := false; --(pwd = 1) and not (index /= 0);
#define PWRD2     (CFG_PWD!=0)//: boolean := pwd /= 0; --(pwd = 2) or (index /= 0);
#define RS1OPT    (fabtech!=0)//: boolean := (is_fpga(FABTECH) /= 0);
#define DYNRST    (CFG_RSTADDR==0xFFFF)//: boolean := (rstaddr = 16#FFFFF#);
#define CASAEN    ((CFG_NOTAG==0)&&(CFG_LDDEL==1))//: boolean := (notag = 0) and (lddel = 1);


// execute stage operations
const uint32 EXE_AND   = 0;//: std_logic_vector(2 downto 0) := "000";
const uint32 EXE_XOR   = 1;//: std_logic_vector(2 downto 0) := "001"; -- must be equal to EXE_PASS2
const uint32 EXE_OR    = 2;//: std_logic_vector(2 downto 0) := "010";
const uint32 EXE_XNOR  = 3;//: std_logic_vector(2 downto 0) := "011";
const uint32 EXE_ANDN  = 4;//: std_logic_vector(2 downto 0) := "100";
const uint32 EXE_ORN   = 5;//: std_logic_vector(2 downto 0) := "101";
const uint32 EXE_DIV   = 6;//: std_logic_vector(2 downto 0) := "110";

const uint32 EXE_PASS1 = 0;//: std_logic_vector(2 downto 0) := "000";
const uint32 EXE_PASS2 = 1;//: std_logic_vector(2 downto 0) := "001";
const uint32 EXE_STB   = 2;//: std_logic_vector(2 downto 0) := "010";
const uint32 EXE_STH   = 3;//: std_logic_vector(2 downto 0) := "011";
const uint32 EXE_ONES  = 4;//: std_logic_vector(2 downto 0) := "100";
const uint32 EXE_RDY   = 5;//: std_logic_vector(2 downto 0) := "101";
const uint32 EXE_SPR   = 6;//: std_logic_vector(2 downto 0) := "110";
const uint32 EXE_LINK  = 7;//: std_logic_vector(2 downto 0) := "111";

const uint32 EXE_SLL   = 1;//: std_logic_vector(2 downto 0) := "001";
const uint32 EXE_SRL   = 2;//: std_logic_vector(2 downto 0) := "010";
const uint32 EXE_SRA   = 4;//: std_logic_vector(2 downto 0) := "100";

const uint32 EXE_NOP   = 0;//: std_logic_vector(2 downto 0) := "000";

// EXE result select
const uint32 EXE_RES_ADD   = 0;//: std_logic_vector(1 downto 0) := "00";
const uint32 EXE_RES_SHIFT = 1;//: std_logic_vector(1 downto 0) := "01";
const uint32 EXE_RES_LOGIC = 2;//: std_logic_vector(1 downto 0) := "10";
const uint32 EXE_RES_MISC  = 3;//: std_logic_vector(1 downto 0) := "11";

// Load types
const uint32 SZBYTE    = 0;//: std_logic_vector(1 downto 0) := "00";
const uint32 SZHALF    = 1;//: std_logic_vector(1 downto 0) := "01";
const uint32 SZWORD    = 2;//: std_logic_vector(1 downto 0) := "10";
const uint32 SZDBL     = 3;//: std_logic_vector(1 downto 0) := "11";


//****************************************************************************
iu3::iu3(uint32 hindex_)
{
  hindex = hindex_;
}

//****************************************************************************
uint32 iu3::dbgexc(registers& r,
                        l3_debug_in_type &dbgi,
                        uint32 &trap,// : std_ulogic;
                        uint32 &tt )//std_logic_vector(7 downto 0)
{
  uint32 dmode = 0;
  if (!r.x.ctrl.annul & trap)
  {
    if (((tt ==TT_WATCH) && (dbgi.bwatch == 1)) ||
        ((dbgi.bsoft == 1) && (tt == 0x81)) ||
        (dbgi.btrapa == 1) ||
        ((dbgi.btrape == 1) && !((BITS32(tt,5,0) == TT_PRIV) || 
	      (BITS32(tt,5,0) == TT_FPDIS) || (BITS32(tt,5,0) == TT_WINOF) ||
        (BITS32(tt,5,0) == TT_WINUF) || (BITS32(tt,5,4) == 0x1) || (BIT32(tt,7) == 1))) || 
        (((!r.w.s.et) & dbgi.berror) == 1))
      dmode = 1;

  }
  return(dmode);
}

//****************************************************************************
uint32 iu3::dbgerr( registers &r,// : registers;
                        l3_debug_in_type &dbgi,// : l3_debug_in_type;
                        uint32 tt)// : std_logic_vector(7 downto 0))  return std_ulogic is
{
  uint32 err = !r.w.s.et;
  if (((dbgi.dbreak == 1) && (tt == TT_WATCH)) ||
      ((dbgi.bsoft == 1) && (tt == 0x81)))
    err = 0;
  return(err);
}

//****************************************************************************
void iu3::diagwr( registers &r,//    : in registers;
                      dsu_registers &dsur,// : in dsu_registers;
                      irestart_register &ir,//   : in irestart_register;
                      l3_debug_in_type &dbg,//  : in l3_debug_in_type;
                      watchpoint_registers &wpr,//  : in watchpoint_registers;
                      special_register_type &s,//    : out special_register_type;
                      watchpoint_registers &vwpr,// : out watchpoint_registers;
                      uint32 &asi,// : out std_logic_vector(7 downto 0);
                      pctype &pc,
                      pctype &npc,//  : out pctype;
                      uint32 &tbufcnt,// : out std_logic_vector(TBUFBITS-1 downto 0);
                      uint32 &wr,// : out std_ulogic;
                      uint32 &addr,// : out std_logic_vector(9 downto 0);
                      word &data,// : out word;
                      uint32 &fpcwr)// : out std_ulogic);
{
  s       = r.w.s;
  pc      = r.f.pc;
  npc     = ir.addr;
  wr      = 0;
  vwpr    = wpr;
  asi     = dsur.asi;
  addr    = 0;
  data    = dbg.ddata;
  tbufcnt = dsur.tbufcnt;
  fpcwr   = 0;
  if (dbg.dsuen & dbg.denable & dbg.dwrite)
  {
    switch (BITS32(dbg.daddr,23,20))
    {
      case 1:
        if ((BIT32(dbg.daddr,16) == 1) && TRACEBUF) //-- trace buffer control reg
          tbufcnt = BITS32(dbg.ddata,TBUFBITS-1,0);
      break;
      case 3: //-- IU reg file
        if (BIT32(dbg.daddr,12) == 0)
        {
          wr   = 1;
          addr = 0;
          addr = (addr&ILSBMSK32(RFBITS)) | (BITS32(dbg.daddr,RFBITS+1,2));
        }else  //-- FPC
          fpcwr = 1;
      break;
      case 0x4:// -- IU special registers
        switch (BITS32(dbg.daddr,7,6))
        {
          case 0x0: //-- IU regs Y - TBUF ctrl reg
            switch (BITS32(dbg.daddr,5,2))
            {
              case 0: // -- Y
                s.y = dbg.ddata;
              break;
              case 1: //-- PSR
                s.cwp = BITS32(dbg.ddata,NWINLOG2-1,0);
                s.icc = BITS32(dbg.ddata,23,20);
                s.ec  = BIT32(dbg.ddata,13);
                if (FPEN) s.ef = BIT32(dbg.ddata,12);
                s.pil = BITS32(dbg.ddata,11,8);
                s.s   = BIT32(dbg.ddata,7);
                s.ps  = BIT32(dbg.ddata,6);
                s.et  = BIT32(dbg.ddata,5);
              break;
              case 2: //-- WIM
                s.wim = dbg.ddata;
              break;
              case 3: //-- TBR
                s.tba = BITS32(dbg.ddata,31,12);
                s.tt  = BITS32(dbg.ddata,11,4);
              break;
              case 4: //-- PC
                pc = dbg.ddata & MSK32(31,CFG_PCLOW);
              break;
              case 5: //-- NPC
                npc = dbg.ddata & MSK32(31,CFG_PCLOW);
              break;
              case 6: //--FSR
                fpcwr = 1;
              break;
              case 7: //--CFSR
              break;
              case 9: //-- ASI reg
                asi = BITS32(dbg.ddata,7,0);
              //--when "1001" => -- TBUF ctrl reg
              //--  tbufcnt := dbg.ddata(TBUFBITS-1 downto 0);
              break;
              default:;
            }
          break;
          case 0x1: // -- ASR16 - ASR31
            switch(BITS32(dbg.daddr,5,2))
            {
              case 1: //-- %ASR17
	              if (CFG_BP == 2) s.dbp = (dbg.ddata>>27)&0x1;
	              s.dwt = BIT32(dbg.ddata,14);
	              s.svt = BIT32(dbg.ddata,13);
  	          break;
              case 2: //-- %ASR18
                if (MACEN) s.asr18 = dbg.ddata;
              break;
              case 8: 		//-- %ASR24 - %ASR31
                vwpr.arr[0].addr = dbg.ddata & MSK32(31,2);
                vwpr.arr[0].exec = BIT32(dbg.ddata,0);
              break;
              case 9:
                vwpr.arr[0].mask  = dbg.ddata & MSK32(31,2);
                vwpr.arr[0].load  = BIT32(dbg.ddata,1);
                vwpr.arr[0].store = BIT32(dbg.ddata,0);
              break;
              case 0xA:
                vwpr.arr[1].addr = dbg.ddata & MSK32(31,2);
                vwpr.arr[1].exec = BIT32(dbg.ddata,0); 
              break;
              case 0xB:
                vwpr.arr[1].mask  = dbg.ddata & MSK32(31,2);
                vwpr.arr[1].load  = BIT32(dbg.ddata,1);
                vwpr.arr[1].store = BIT32(dbg.ddata,0);
              break;
              case 0xC:
                vwpr.arr[2].addr = dbg.ddata & MSK32(31,2);
                vwpr.arr[2].exec = BIT32(dbg.ddata,0);
              break;
              case 0xD:
                vwpr.arr[2].mask  = dbg.ddata & MSK32(31,2);
                vwpr.arr[2].load  = BIT32(dbg.ddata,1);
                vwpr.arr[2].store = BIT32(dbg.ddata,0);
              break;
              case 0xE:
                vwpr.arr[3].addr = dbg.ddata & MSK32(31,2);
                vwpr.arr[3].exec = BIT32(dbg.ddata,0); 
              break;
              case 0xF:
                vwpr.arr[3].mask  = dbg.ddata & MSK32(31,2);
                vwpr.arr[3].load  = BIT32(dbg.ddata,1);
                vwpr.arr[3].store = BIT32(dbg.ddata,0);
              break;
              default:;
            }
            //-- disabled due to bug in XST
            //--                  i := conv_integer(dbg.daddr(4 downto 3)); 
            //--                  if dbg.daddr(2) = '0' then
            //--                    vwpr(i).addr := dbg.ddata(31 downto 2);
            //--                    vwpr(i).exec := dbg.ddata(0); 
            //--                  else
            //--                    vwpr(i).mask := dbg.ddata(31 downto 2);
            //--                    vwpr(i).load := dbg.ddata(1);
            //--                    vwpr(i).store := dbg.ddata(0);              
            //--                  end if;
          break;
          default:;
        }
      break;
      default:;
    }// end 1-st switch
  }
}

//****************************************************************************
word iu3::asr17_gen (registers &r)
{
  word asr17 = 0;
  int32 fpu2;

  asr17 = hindex<<28;
  if (CFG_BP == 2) asr17 |= (r.w.s.dbp<<27);
  if (CFG_NOTAG == 0)  asr17 |= (1<<26);  //  -- CASA/TADD present
  if (clk2x > 8)
  {
    asr17 |= (((clk2x-8)&0x3)<<15);
    asr17 |= (1<<17); 
  }else if (clk2x > 0)
    asr17 |= (clk2x<<15);
  
  asr17 |= (r.w.s.dwt<<14);
  if (CFG_SVT == 1)   asr17 |= (r.w.s.svt<<13);
  if (CFG_LDDEL == 2) asr17 |= (1<<12);
  
  if ((CFG_FPU > 0) && (CFG_FPU < 8))        fpu2 = 1;
  else if ((CFG_FPU >= 8) && (CFG_FPU < 15)) fpu2 = 3;                          
  else if (CFG_FPU == 15)                    fpu2 = 2;
  else                                       fpu2 = 0;
  
  asr17 |= (fpu2<<10);                       
  if (CFG_MAC == 1) asr17 |= (1<<9);
  if (CFG_V8 != 0)  asr17 |= (1<<8);
  asr17 |= ((CFG_NWP&0x7)<<5);
  asr17 |= ((CFG_NWIN-1)&0x1F);       
  return(asr17);
}

//****************************************************************************
void iu3::diagread( l3_debug_in_type &dbgi,//   : in l3_debug_in_type;
                        registers &r,//      : in registers;
                        iregfile_out_type &rfo,//   : in  iregfile_out_type;
                        fpc_out_type &fpo,//   : in  fpc_out_type;
                        dsu_registers &dsur,//   : in dsu_registers;
                        irestart_register &ir,//     : in irestart_register;
                        watchpoint_registers &wpr,//    : in watchpoint_registers;
                        dcache_out_type &dco,//   : in  dcache_out_type;                          
                        tracebuf_out_type &tbufo,//  : in tracebuf_out_type;
                        word &data)// : out word) is
{
  uint32 cwp;// : std_logic_vector(4 downto 0);
  int32 i;// : integer range 0 to 3;    

#if 1
if(iClkCnt==2271)
bool st = true;
#endif

  data = 0;
  cwp = BITS32(r.w.s.cwp,NWINLOG2-1,0);
  switch ((dbgi.daddr>>20)&0x7)
  {
    case 1: //-- trace buffer
      if (TRACEBUF)
      {
        if ((dbgi.daddr>>16)&0x1) //-- trace buffer control reg
        {
          if (TRACEBUF) data = dsur.tbufcnt;
        }else
        {
          switch ((dbgi.daddr>>2)&0x3)
          {
            case 0: data = (uint32)(tbufo.data[1]>>32); break;
            case 1: data = (uint32)tbufo.data[1]; break;
            case 2: data = (uint32)(tbufo.data[0]>>32); break;
            default: data = (uint32)tbufo.data[0];
          }
        }
      }
    break;
    case 3: //-- IU reg file
      if(((dbgi.daddr>>12)&0x1) == 0)
      {
        data = rfo.data1; 
        if ( BIT32(dbgi.daddr,11) && (is_fpga[fabtech] == 0))
          data = rfo.data2;
      }else 
        data = fpo.dbg.data;
    break;
    case 4: //-- IU regs
      switch (BITS32(dbgi.daddr,7,6))
      {
        case 0: //-- IU regs Y - TBUF ctrl reg
          switch (BITS32(dbgi.daddr,5,2))
          {
            case 0:
              data = r.w.s.y;
            break;
            case 1:
              data = (IMPL<<28) | (VER<<24) |
                     (r.w.s.icc<<20) | (r.w.s.ec<<13) | (r.w.s.ef<<12) | (r.w.s.pil<<8) |
                     (r.w.s.s<<7) | (r.w.s.ps<<6) | (r.w.s.et<<5) | cwp;
            break;
            case 2:
              data = r.w.s.wim;
            break;
            case 3:
              data = (r.w.s.tba<<12) | (r.w.s.tt<<4);
            break;
            case 4:
              data &= ~MSK32(31,CFG_PCLOW);
              data |= r.f.pc;
            break;
            case 5:
              data &= ~MSK32(31,CFG_PCLOW);
              data |= ir.addr;
            break;
            case 6: //-- FSR
              data = fpo.dbg.data;
            break;
            case 7: //-- CPSR 
            break;
            case 8: //-- TT reg
              data = (dsur.err<<12) | (dsur.tt<<4);
            break;
            case 9: //-- ASI reg
              data = dsur.asi;
            break;
            default:;
          }
        break;
        case 1:
          if (BIT32(dbgi.daddr,5) == 0)         //-- %ASR17
          {
            if (BITS32(dbgi.daddr,4,2) == 1)       //-- %ASR17
              data = asr17_gen(r);
            else if (MACEN && (BITS32(dbgi.daddr,4,2)== 2)) //-- %ASR18
              data = r.w.s.asr18;
          }else                                   //-- %ASR24 - %ASR31
          {
            i = BITS32(dbgi.daddr,4,3);
            if (BIT32(dbgi.daddr,2) == 0)
            {
              data &= ~0xfffffffd;
              data |= (wpr.arr[i].addr&MSK32(31,CFG_PCLOW));
              data |= wpr.arr[i].exec;
            }else
            {
              data = (wpr.arr[i].mask&MSK32(31,CFG_PCLOW));
              data |= (wpr.arr[i].load<<1);
              data |= wpr.arr[i].store; 
            }
          }
        break;
        default:;
      }
    break;
    case 7:
      data = r.x.data.arr[r.x.set];
    break;
    default:;
  }
}

//****************************************************************************
void iu3::itrace( registers &r,//    : in registers;
                      dcache_out_type &dco,//   : in  dcache_out_type;
                      dsu_registers &dsur,// : in dsu_registers;
                      dsu_registers &vdsu,// : in dsu_registers;
                      word &res,//  : in word;
                      uint32 exc,//  : in std_ulogic;
                      l3_debug_in_type &dbgi,// : in l3_debug_in_type;
                      uint32 error,// : in std_ulogic;
                      uint32 trap,//  : in std_ulogic;                          
                      uint32 &tbufcnt,// : out std_logic_vector(TBUFBITS-1 downto 0); 
                      tracebuf_in_type &di)//  : out tracebuf_in_type) is
{
  uint32 meminst;
  di.addr   = 0;
  di.data[1] = 0;
  di.data[0] = 0;
  di.enable = 0;
  di.write  = 0;
  tbufcnt   = vdsu.tbufcnt;
  meminst   = BIT32(r.x.ctrl.inst,31) & BIT32(r.x.ctrl.inst,30);
  if (TRACEBUF)
  {
    di.addr = dsur.tbufcnt;
    di.data[1] |= (((uint64)(!r.x.ctrl.pv))<<(126-64));
    di.data[1] |= (((uint64)(BITS32(dbgi.timer,29,0)))<<(96-64));
    di.data[1] |= ((uint64)res);
    di.data[0] |= (((uint64)BITS32(r.x.ctrl.pc,31,CFG_PCLOW))<<34);
    di.data[0] |= (((uint64)trap)<<33);
    di.data[0] |= (((uint64)error)<<32);
    di.data[0] |= ((uint64)r.x.ctrl.inst);
    if ((dbgi.tenable == 0) || (r.x.rstate == dsu2))
    {
      if ((dbgi.dsuen & dbgi.denable) && (BITS32(dbgi.daddr,23,20)==0x1) && (BIT32(dbgi.daddr,16)==0) )
      {
        di.enable = 1; 
        di.addr &= ~MSK32(TBUFBITS-1,0);
        di.addr |= BITS32(dbgi.daddr,TBUFBITS-1+4,4);
        if (dbgi.dwrite == 1)
        {
          switch (BITS32(dbgi.daddr,3,2))
          {
            case 0: di.write |= (1<<3); break;
            case 1: di.write |= (1<<2); break;
            case 2: di.write |= (1<<1); break;
            default: di.write |= 1; break;
          }
          di.data[1] = (((uint64)dbgi.ddata)<<32) | ((uint64)dbgi.ddata);
          di.data[0] = (((uint64)dbgi.ddata)<<32) | ((uint64)dbgi.ddata);
        }
      }
    }else if (!r.x.ctrl.annul & (r.x.ctrl.pv | meminst) & !r.x.debug)
    {
      di.enable = 1;
      di.write  = MSK32(3,0);
      tbufcnt   = BITS32(dsur.tbufcnt+1,TBUFBITS-1,0);
    }
    di.diag = dco.testen<<3;
    if (dco.scanen) di.enable = 0;
  }
}


//****************************************************************************
void iu3::dbg_cache( uint32 holdn,//    : in std_ulogic;
                        l3_debug_in_type &dbgi,//     :  in l3_debug_in_type;
                        registers &r,//        : in registers;
                        dsu_registers &dsur,//     : in dsu_registers;
                        word &mresult,//  : in word;
                        dc_in_type &dci,//      : in dc_in_type;
                        word &mresult2,// : out word;
                        dc_in_type &dci2)//     : out dc_in_type)
{
#if 1
  if(iClkCnt>=63)
  bool st =true;
#endif
  mresult2   = mresult;
  dci2       = dci;
  dci2.dsuen = 0; 
  if (DBGUNIT)
  {
    if (r.x.rstate == dsu2)
    {
      dci2.asi = dsur.asi;
      if ( (BITS32(dbgi.daddr,22,20)==0x7) && (dbgi.dsuen == 1))
      {
        dci2.dsuen  = (dbgi.denable | r.m.dci.dsuen) & !(BIT32(dsur.crdy,2));
        dci2.enaddr = dbgi.denable;
        dci2.size   = 0x2;
        dci2.read   = 1;
        dci2.write  = 0;
        if (dbgi.denable & !r.m.dci.enaddr)
          mresult2 = dbgi.daddr&MSK32(19,2);
        else
          mresult2 = dbgi.ddata;            

        if (dbgi.dwrite)
        {
          dci2.read  = 0;
          dci2.write = 1;
        }
      }
    }
  }
}


//****************************************************************************
void iu3::fpexack( registers &r,// : in registers;
                       uint32 &fpexc)// : out std_ulogic) is
{
  fpexc = 0;
#if (CFG_FPU!=0)
  if (r.x.ctrl.tt == TT_FPEXC) fpexc = 1;
#endif
}

//****************************************************************************
void iu3::diagrdy( uint32 denable,// : in std_ulogic;
                       dsu_registers &dsur,// : in dsu_registers;
                       dc_in_type &dci,//   : in dc_in_type;
                       icache_out_type &ico,// : in icache_out_type;
                       uint32 &crdy)// : out std_logic_vector(2 downto 1))
{
  crdy = BIT32(dsur.crdy,1)<<2;
  if (dci.dsuen == 1)
  {
    switch(BITS32(dsur.asi,4,0))
    {
      case ASI_ITAG:
      case ASI_IDATA:
      case ASI_UINST:
      case ASI_SINST:
        crdy &= ~0x4;
        crdy |= ( (ico.diagrdy & !BIT32(dsur.crdy,2)) << 2);
      break;
      case ASI_DTAG:
      case ASI_MMUSNOOP_DTAG:
      case ASI_DDATA:
      case ASI_UDATA:
      case ASI_SDATA:
        crdy &= ~0x2;
        crdy |= ( (!denable & dci.enaddr & !BIT32(dsur.crdy,1)) <<1 );
      break;
      default:;
        crdy &= ~0x4;
        crdy |= ( (dci.enaddr & denable) <<2 );
    }
  }
}

//****************************************************************************
// calculate register file address
void iu3::regaddr(uint32 cwp,// : std_logic_vector; 
                  uint32 reg,// : std_logic_vector(4 downto 0);
	                rfatype &rao)// : out rfatype)
{
  rfatype ra;
  const uint32 globals = CFG_NWIN & MSK32(RFBITS-5, 0);

  ra = 0;
  ra = reg & MSK32(4,0);
  if (BITS32(reg,4,3) == 0 )
  {
    ra |= (globals<<4);
  }else
  {
    uint32 tmp = BIT32(ra,4);
    ra &= ~MSK32(NWINLOG2+3,4);
    ra |= ( ((cwp + tmp)<<4) & MSK32(NWINLOG2+3,4) );
    if (BITS32(ra,RFBITS-1,4) == globals)
      ra &= ~MSK32(RFBITS-1,4);
  }
  rao = ra;
}

//****************************************************************************
// branch adder
uint32 iu3::branch_address(word &inst,// : word; 
                           pctype &pc)// : pctype) return std_logic_vector is
{
  pctype tmp=0;
  pctype baddr, caddr;

  caddr = 0;
  caddr = inst<<2;
  caddr = caddr + pc;
  baddr = 0;
  baddr = BIT32(inst,21) ? MSK32(31,24) : 0x0; 
  baddr |= (BITS32(inst,21,0)<<2);
  baddr = baddr + pc;
  if (BIT32(inst,30)) tmp = caddr;
  else                tmp = baddr;
  return(tmp);
}

//****************************************************************************
// evaluate branch condition
uint32 iu3::branch_true( uint32 icc,// : std_logic_vector(3 downto 0); 
                             uint32 inst)// : word) 
{
  uint32 n, z, v, c, branch;
  uint32 i28;

  n = (icc&0x8) ? 1: 0;
  z = (icc&0x4) ? 1: 0;
  v = (icc&0x2) ? 1: 0;
  c = (icc&0x1) ? 1: 0;
  i28 = BIT32(inst,28);
  switch (BITS32(inst,27,25))
  {
    case 0:  branch = i28 ^ 0; break;			//-- bn, ba
    case 1:  branch = i28 ^ z; break;      		//-- be, bne
    case 2:  branch = i28 ^ (z | (n ^ v)); break;     //-- ble, bg
    case 3:  branch = i28 ^ (n ^ v); break; 	        //-- bl, bge
    case 4:  branch = i28 ^ (c | z); break;   	        //-- bleu, bgu
    case 5:  branch = i28 ^ c; break;		        //-- bcs, bcc 
    case 6:  branch = i28 ^ n; break;		        //-- bneg, bpos
    default: branch = i28 ^ v;		        //-- bvs, bvc   
  }
  return(branch);
}

//****************************************************************************
// detect RETT instruction in the pipeline and set the local psr.su and psr.et
void iu3::su_et_select( registers &r,// : in registers; 
                            uint32 xc_ps,
                            uint32 xc_s,
                            uint32 xc_et,// : in std_ulogic;
		                        uint32 &su,// : out std_ulogic,
                            uint32 &et)// : out std_ulogic )
{
  if ( ((r.a.ctrl.rett | r.e.ctrl.rett | r.m.ctrl.rett | r.x.ctrl.rett) == 1)
    && (r.x.annul_all == 0) )

  {
    su = xc_ps;
    et = 1;
  }else
  {
    su = xc_s; 
    et = xc_et;
  }
}

//****************************************************************************
// detect watchpoint trap
uint32 iu3::wphit( registers &r,// : registers;
                       watchpoint_registers &wpr,// : watchpoint_registers; 
                       l3_debug_in_type &debug)// : l3_debug_in_type)  return std_ulogic is
{
  uint32 exc=0;
  for (int32 i=1; i<=CFG_NWP; i++)
  {
    if ((wpr.arr[i-1].exec & r.a.ctrl.pv & !r.a.ctrl.annul) == 1)
    {
      if (((wpr.arr[i-1].addr ^ r.a.ctrl.pc) & wpr.arr[i-1].mask) == 0)
        exc = 1;
    }
  }

  if (DBGUNIT)
  {
    if (debug.dsuen & !r.a.ctrl.annul)
      exc = exc | (r.a.ctrl.pv & ((debug.dbreak & debug.bwatch) | r.a.step));
  }
  return(exc);
}

//****************************************************************************
// 32-bit shifter
word iu3::shift3( registers &r,// : registers; 
                      word &aluin1)// : word) return word is
{
  uint64 shiftin;
  uint64 shiftout;
  uint32 cnt = r.e.shcnt;

  if (r.e.shleft == 1)
  {
    shiftin = ((uint64)aluin1)<<31;
  }else
  {
    shiftin = (uint64)aluin1;
    shiftin |= (r.e.sari?0xFFFFFFFF00000000:0x0);
  }
  shiftout = shiftin>>cnt;

  return (uint32)shiftout;
}

//****************************************************************************
word iu3::shift2( registers &r,// : registers;
                      word &aluin1)// : word) return word is
{
  uint32 cnt;
  uint32 ushiftin;//[31:0]
  int64  sshiftin;//[32:0]
  word resleft, resright;

  cnt = r.e.shcnt;
  ushiftin = (uint32)(aluin1);
  sshiftin = ((int64)aluin1)&0x00000000FFFFFFFF;
  if (r.e.shleft == 1)
  {
    resleft = ushiftin<<cnt;
    return(resleft);
  }else
  {
    if (r.e.sari == 1) 
    {
      if (aluin1&0x80000000) sshiftin |= 0xFFFFFFFF00000000;
    }
    sshiftin = sshiftin>>cnt;
    resright = (uint32)(sshiftin&0xFFFFFFFF);
    return(resright);
//--      else 
//--  ushiftin := SHIFT_RIGHT(ushiftin, cnt);
//--        return(std_logic_vector(ushiftin));
//--      end if;
  }
}

//****************************************************************************
word iu3::shift(registers &r,// : registers; 
                    word aluin1,// : word;
	                  uint32 shiftcnt,// : std_logic_vector(4 downto 0); 
                    uint32 sari)// : std_ulogic ) return word is
{
  uint64 shiftin = aluin1;
  if (r.e.shleft == 1)
    shiftin = (((uint64)aluin1)<<31);
  else
  {
    shiftin |= (sari ? 0xFFFFFFFF00000000: 0x0);
  }
  if (((shiftcnt>>4)&0x1) == 1) shiftin = shiftin>>16;
  if (((shiftcnt>>3)&0x1) == 1) shiftin = shiftin>>8;
  if (((shiftcnt>>2)&0x1) == 1) shiftin = shiftin>>4;
  if (((shiftcnt>>1)&0x1) == 1) shiftin = shiftin>>2;
  if (((shiftcnt>>0)&0x1) == 1) shiftin = shiftin>>1;
  return (uint32)shiftin;
}

//****************************************************************************
// Check for illegal and privileged instructions
void iu3::exception_detect( registers &r,// : registers; 
                                watchpoint_registers &wpr,// : watchpoint_registers; 
                                l3_debug_in_type &dbgi,// : l3_debug_in_type;
	                              uint32 &trap,// : out std_ulogic; 
                                uint32 &tt)// : out std_logic_vector(5 downto 0)) is
{
  uint32 illegal_inst, privileged_inst;// : std_ulogic;
  uint32 cp_disabled, fp_disabled, fpop;// : std_ulogic;
  uint32 op;// : std_logic_vector(1 downto 0);
  uint32 op2;// : std_logic_vector(2 downto 0);
  uint32 op3;// : std_logic_vector(5 downto 0);
  uint32 rd;//  : std_logic_vector(4 downto 0);
  word inst;// : word;
  uint32 wph;// : std_ulogic;

  inst = r.a.ctrl.inst;
  trap = r.a.ctrl.trap;
  tt   = r.a.ctrl.tt;
  if (r.a.ctrl.annul == 0)
  {
    op  = (inst>>30&0x3);//(31 downto 30);
    op2 = (inst>>22)&0x7;//(24 downto 22);
    op3 = (inst>>19)&0x3F;//(24 downto 19);
    rd  = (inst>>25)&0x1F;
    illegal_inst    = 0;
    privileged_inst = 0;
    cp_disabled     = 0; 
    fp_disabled     = 0;
    fpop            = 0;
    switch (op)
    {
    case CALL: break;
    case FMT2:
      switch (op2)
      {
      case SETHI:
      case BICC:  break;
      case FBFCC:
	      if (FPEN) fp_disabled = !r.w.s.ef;
	      else      fp_disabled = 1;
	    break;
      case CBCCC:
	      if ((!CPEN) || (r.w.s.ec == 0)) cp_disabled = 1;
	    break;
      default:  illegal_inst = 1;
      }
    break;
    case FMT3:
      switch(op3)
      {
        case IAND: case ANDCC: case ANDN: case ANDNCC: case IOR: case ORCC: case ORN: case ORNCC: case IXOR:
        case XORCC: case IXNOR: case XNORCC: case ISLL: case ISRL: case ISRA: case MULSCC: case IADD: case ADDX:
	      case ADDCC: case ADDXCC: case ISUB: case SUBX: case SUBCC: case SUBXCC: case FLUSH: case JMPL: case TICC:
	      case SAVE: case RESTORE: case RDY:
	      break;
        case TADDCC:
        case TADDCCTV:
        case TSUBCC:
        case TSUBCCTV:
	        if (CFG_NOTAG == 1) illegal_inst = 1;
	      break;
        case UMAC:
        case SMAC:
	        if (!MACEN) illegal_inst = 1;
	      break;
        case UMUL:
        case SMUL: 
        case UMULCC:
        case SMULCC:
	        if (!MULEN) illegal_inst = 1;
	      break;
        case UDIV:
        case SDIV:
        case UDIVCC:
        case SDIVCC:
	        if (!DIVEN) illegal_inst = 1;
	      break;
        case RETT: 
          illegal_inst    = r.a.et;
          privileged_inst = !r.a.su;
        break;
        case RDPSR:
        case RDTBR:
        case RDWIM: 
          privileged_inst = !r.a.su;
        break;
        case WRY: break;
        case WRPSR:
	        privileged_inst = !r.a.su; 
	      break;
        case WRWIM:
        case WRTBR:
          privileged_inst = !r.a.su;
        break;
        case FPOP1:
        case FPOP2:
	        if (FPEN)
	        {
	          fp_disabled = !r.w.s.ef;
	          fpop = 1;
	        }else
	        {
	          fp_disabled = 1;
	          fpop = 0;
	        }
	      break;
        case CPOP1:
        case CPOP2:
	        if ((!CPEN) || (r.w.s.ec == 0)) cp_disabled = 1;
	      break;
        default: illegal_inst = 1;
      }
    break;
    default:	//-- LDST
      switch(op3)
      {
        case LDD: case ISTD:
          illegal_inst = rd&0x1;	//-- trap if odd destination register
        break;
        case LD: case LDUB: case LDSTUB: case LDUH: case LDSB: case LDSH: 
        case ST: case STB: case STH: case SWAP:
	      break;
        case LDDA: case STDA:
	        illegal_inst = ((inst>>13)&0x1) | (rd&0x1);
	        privileged_inst = !r.a.su;
	      break;
        case LDA: case LDUBA: case LDSTUBA: case LDUHA: case LDSBA: case LDSHA: 
        case STA: case STBA: case STHA: case SWAPA:
	        illegal_inst = (inst>>13)&0x1;
	        privileged_inst = !r.a.su;
	      break;
        case CASA:
	        if (CASAEN)
	        {
	          illegal_inst = (inst>>13)&0x1; 
	          if (((inst>>5)&0xFF) != 0x0A) privileged_inst = !r.a.su;
	        }else (illegal_inst == 1);
	      break;
        case LDDF: case STDF: case LDF: case LDFSR: case STF: case STFSR:
	        if (FPEN) fp_disabled = !r.w.s.ef;
	        else      fp_disabled = 1;
	      break;
        case STDFQ:
	        privileged_inst = !r.a.su; 
	        if ((!FPEN) || (r.w.s.ef == 0)) fp_disabled = 1;
	      break;
        case STDCQ:
	        privileged_inst = !r.a.su;
	        if ((!CPEN) || (r.w.s.ec ==0)) cp_disabled = 1;
	      break;
        case LDC: case LDCSR: case LDDC: case STC: case STCSR: case STDC:
	        if ((!CPEN) || (r.w.s.ec == 0)) cp_disabled = 1;
	      break;
        default: illegal_inst = 1;
      }
    }

    wph = wphit(r, wpr, dbgi);
    
    trap = 1;
    if (r.a.ctrl.trap)        tt = TT_IAEX;
    else if (privileged_inst) tt = TT_PRIV; 
    else if (illegal_inst)    tt = TT_IINST;
    else if (fp_disabled)     tt = TT_FPDIS;
    else if (cp_disabled)     tt = TT_CPDIS;
    else if (wph)             tt = TT_WATCH;
    else if (r.a.wovf)        tt = TT_WINOF;
    else if (r.a.wunf)        tt = TT_WINUF;
    else if (r.a.ticc)        tt = TT_TICC;
    else          { trap = 0; tt = 0; }
  }
}

//****************************************************************************
// instructions that write the condition codes (psr.icc)
void iu3::wicc_y_gen( word inst,// : word; 
                      registers &r,// : registers; 
                      mul32_out_type &mulo,//  : in  mul32_out_type;
                      div32_out_type &divo,//  : in  div32_out_type;
                      uint32 &wicc,// : out std_ulogic
                      uint32 &wy)// : out std_ulogic) is
{
  wicc = 0;
  wy   = 0;
  if (BITS32(inst,31,30) == FMT3)
  {
    switch (BITS32(inst,24,19))
    {
      case SUBCC: case TSUBCC: case TSUBCCTV: case ADDCC: case ANDCC: case ORCC: case XORCC: case ANDNCC:
      case ORNCC: case XNORCC: case TADDCC: case TADDCCTV: case ADDXCC: case SUBXCC: case WRPSR:
        wicc = 1;
      break;
      case WRY:
        if (BITS32(r.d.inst.arr[r.d.set],29,25) == 0) wy = 1;
      break;
      case MULSCC:
        wicc = 1;
        wy   = 1;
      break;
      case UMAC: 
      case SMAC:
        if (MACEN) wy = 1;
      break;
      case UMULCC:
      case SMULCC:
        if (MULEN && (((mulo.nready==1) && (r.d.cnt!= 0)) || (MULTYPE!=0)))
        {
          wicc = 1;
          wy   = 1;
        }
      break;
      case UMUL:
      case SMUL:
        if (MULEN && (((mulo.nready==1) && (r.d.cnt!=0)) || (MULTYPE != 0)))
          wy = 1;
      break;
      case UDIVCC:
      case SDIVCC:
        if (DIVEN && (divo.nready==1) && (r.d.cnt!=0))
          wicc = 1;
      break;
      default:;
    }
  }
}

//****************************************************************************
// select cwp 
void iu3::cwp_gen(uint32 rstn,//  : in  std_ulogic;
                      registers &r, 
                      registers &v,// : registers; 
                      uint32 annul,
                      uint32 wcwp,// : std_ulogic;
                      cwptype ncwp,// : cwptype;
		                  cwptype &cwp)// : out cwptype) is
{
  if ((r.x.rstate == trap) || (r.x.rstate == dsu2) || !rstn) cwp = v.w.s.cwp;
  else if ((wcwp == 1) && (annul == 0))                      cwp = ncwp;
  else if (r.m.wcwp == 1)                                    cwp = BITS32(r.m.result,NWINLOG2-1,0);
  else                                                       cwp = r.d.cwp;
}

//****************************************************************************
// generate wcwp in ex stage
void iu3::cwp_ex(registers &r,// : in registers;
                     uint32 &wcwp)// : out std_ulogic) is
{
  if( (BITS32(r.e.ctrl.inst,31,30) == FMT3) &&
      (BITS32(r.e.ctrl.inst,24,19)== WRPSR) )
    wcwp = !r.e.ctrl.annul;
  else 
    wcwp = 0;
}

//****************************************************************************
// generate next cwp & window under- and overflow traps
void iu3::cwp_ctrl(registers &r,// : in registers;
                       uint32 xc_wim,// : in std_logic_vector(NWIN-1 downto 0);
	                     word inst,// : word;
                       cwptype &de_cwp,// : out cwptype;
                       uint32 &wovf_exc,
                       uint32 &wunf_exc, 
                       uint32 &wcwp)// : out std_ulogic) is
{
  uint32 op;// : std_logic_vector(1 downto 0);
  uint32 op3;// : std_logic_vector(5 downto 0);
  word wim;// : word;
  cwptype ncwp;// : cwptype;

  op  = BITS32(inst,31,30);
  op3 = BITS32(inst,24,19); 
  wovf_exc = 0;
  wunf_exc = 0;
  wim      = 0; 
  wim      = xc_wim;
  ncwp     = r.d.cwp;
  wcwp     = 0;
  if ((op==FMT3) && ((op3==RETT) || (op3==RESTORE) || (op3==SAVE)))
  {
    wcwp = 1;
    if (op3 == SAVE)
    {
      if ((!CWPOPT) && (r.d.cwp == CWPMIN)) ncwp = CWPMAX;
      else                                  ncwp = (r.d.cwp - 1) & MSK32(NWINLOG2-1,0);
    }else
    {
      if ((!CWPOPT) && (r.d.cwp == CWPMAX)) ncwp = CWPMIN;
      else                                  ncwp = (r.d.cwp + 1) & MSK32(NWINLOG2-1,0);
    }
    if (BIT32(wim,ncwp))
    {
      if (op3 == SAVE)  wovf_exc = 1;
      else              wunf_exc = 1;
    }
  }
  de_cwp = ncwp;
}

//****************************************************************************
// generate register read address 1
void iu3::rs1_gen(registers &r,// : registers;
                      word inst,// : word;
                      uint32 &rs1,// : out std_logic_vector(4 downto 0);
	                    uint32 &rs1mod)// : out std_ulogic) is
{
  uint32 op;// : std_logic_vector(1 downto 0);
  uint32 op3;// : std_logic_vector(5 downto 0);

  op  = BITS32(inst,31,30);
  op3 = BITS32(inst,24,19); 
  rs1 = BITS32(inst,18,14);
  rs1mod = 0;
  if (op == LDST)
  {
    if ( ((r.d.cnt == 0x1) && (BIT32(op3,2) & !BIT32(op3,3))) ||
        (r.d.cnt == 0x2) )
    {
      rs1mod = 1;
      rs1 = BITS32(inst,29,25);
    }
    if ((r.d.cnt == 0x2) && (BITS32(op3,3,0) == 0x7))
      rs1 |= 0x1;
  }
}

//****************************************************************************
// load/icc interlock detection
int32 iu3::icc_valid(registers &r)// : registers) return std_logic is
{
  uint32 not_valid = 0;// : std_logic;
  if (MULEN || DIVEN)
  {
    not_valid = r.m.ctrl.wicc & (BIT32(r.m.ctrl.cnt,0) | r.m.mul);
  }
  not_valid = not_valid | (r.a.ctrl.wicc | r.e.ctrl.wicc);
  return !not_valid;
}

//****************************************************************************
void iu3::bp_miss_ex( registers &r,// : registers; 
                          uint32 icc,// : std_logic_vector(3 downto 0); 
	                        uint32 &ex_bpmiss, 
                          uint32 &ra_bpannul)// : out std_logic) is
{
  uint32 miss;// : std_logic;
  miss       = !r.e.ctrl.annul & r.e.bp & !branch_true(icc, r.e.ctrl.inst);
  ra_bpannul = miss & BIT32(r.e.ctrl.inst,29);
  ex_bpmiss  = miss;
}

//****************************************************************************
void iu3::bp_miss_ra(registers &r,// : registers; 
                         uint32 &ra_bpmiss,
                         uint32 &de_bpannul)// : out std_logic) is
{
  uint32 miss;// : std_logic;

  miss = ((!r.a.ctrl.annul) & r.a.bp & icc_valid(r) & !branch_true(r.m.icc, r.a.ctrl.inst));
  de_bpannul = miss & BIT32(r.a.ctrl.inst,29);
  ra_bpmiss  = miss;
}

//****************************************************************************
void iu3::lock_gen( registers &r,// : registers; 
                        uint32 inBPRED,
                        uint32 rs2, 
                        uint32 rd,// : std_logic_vector(4 downto 0);
	                      rfatype &rfa1, 
                        rfatype &rfa2, 
                        rfatype &rfrd,// : rfatype; 
                        word inst,// : word; 
                        uint32 fpc_lock, 
                        uint32 mulinsn, 
                        uint32 divinsn, 
                        uint32 de_wcwp,// : std_ulogic;
                        uint32 &lldcheck1, 
                        uint32 &lldcheck2, 
                        uint32 &lldlock, 
                        uint32 &lldchkra, 
                        uint32 &lldchkex, 
                        uint32 &bp, 
                        uint32 &nobp, 
                        uint32 &de_fins_hold)// : out std_ulogic) is
{
  uint32 op;// : std_logic_vector(1 downto 0);
  uint32 op2;// : std_logic_vector(2 downto 0);
  uint32 op3;// : std_logic_vector(5 downto 0);
  uint32 cond;// : std_logic_vector(3 downto 0);
  uint32 rs1;//  : std_logic_vector(4 downto 0);
  uint32 i, ldcheck1, ldcheck2, ldchkra, ldchkex, ldcheck3;// : std_ulogic;
  uint32 ldlock, icc_check, bicc_hold, chkmul, y_check;// : std_logic;
  uint32 icc_check_bp, y_hold, mul_hold, bicc_hold_bp, fins, call_hold;//  : std_ulogic;
  bool lddlock;// : boolean;
  uint32 de_fins_holdx;// : std_ulogic;

  op   = BITS32(inst,31,30);
  op3  = BITS32(inst,24,19);
  op2  = BITS32(inst,24,22);
  cond = BITS32(inst,28,25);
  rs1  = BITS32(inst,18,14);
  i    = BIT32(inst,13);
  lddlock  = false;
  ldcheck1 = 0; 
  ldcheck2 = 0;
  ldcheck3 = 0;
  ldlock   = 0;
  ldchkra  = 1;
  ldchkex  = 1;
  icc_check  = 0;
  bicc_hold  = 0;
  y_check    = 0; 
  y_hold     = 0;
  bp         = 0;
  mul_hold   = 0;
  icc_check_bp = 0;
  nobp = 0;
  fins = 0;
  call_hold = 0;
  if (r.d.annul == 0)
  {
    switch (op)
    {
      case CALL:
        call_hold = 1;
        nobp      = inBPRED;
      break;
      case FMT2:
        if ((op2 == BICC) && (BITS32(cond,2,0) != 0))
          icc_check_bp = 1;
        if (op2 == BICC) nobp = inBPRED;
      break;
      case FMT3:
        ldcheck1 = 1;
        ldcheck2 = !i;
        switch(op3)
        {
          case TICC:
            if (BITS32(cond,2,0) != 0) icc_check = 1;
            nobp = inBPRED;
          break;
          case RDY:
            ldcheck1 = 0;
            ldcheck2 = 0;
            if (MACPIPE) y_check = 1;
          break;
          case RDWIM: case RDTBR:
            ldcheck1 = 0;
            ldcheck2 = 0;
          break;
          case RDPSR:
            ldcheck1  = 0;
            ldcheck2  = 0;
            icc_check = 1;
            if (MULEN) icc_check = 1;
          break;
          case SDIV: case SDIVCC: case UDIV: case UDIVCC:
            if (DIVEN) y_check = 1;
          break;
          case FPOP1: case FPOP2: 
            ldcheck1 = 0;
            ldcheck2 = 0;
            fins     = inBPRED;
          break;
          case JMPL: 
            call_hold = 1;
            nobp      = inBPRED;
          break;
          default:;
        }
      break;
      case LDST:
        ldcheck1 = 1;
        ldchkra  = 0;
        switch (r.d.cnt)
        {
          case 0:
            if ( (CFG_LDDEL == 2) && BIT32(op3,2) && !BIT32(op3,5) ) 
              ldcheck3 = 1;
            ldcheck2 = !i;
            ldchkra  = 1;
          break;
          case 1:
            ldcheck2 = !i;
            if ( BIT32(op3,5) & BIT32(op3,2) & !BIT32(op3,3) ) // STF/STC
            { 
              ldcheck1 = 0;
              ldcheck2 = 0;
            }
          break;
          default:
            ldchkex = 0;
            if (CASAEN && (BITS32(op3,5,3)== 0x7))
            {
              ldcheck2 = 1;
            }else if (BIT32(op3,5) || (!BIT32(op3,5) && (BITS32(op3,3,1)==0x6)) ) // LDST
            {
              ldcheck1 = 0;
              ldcheck2 = 0;
            }
          break;
        }
        if (BITS32(op3,2,0)==0x3) lddlock = true;
        if (BIT32(op3,5)) fins = inBPRED; // no BP on FPU/CP LD/ST
      break;
      default:;
    }
  }

  if (MULEN || DIVEN)
  {
    chkmul = mulinsn;
    mul_hold = (r.a.mulstart & r.a.ctrl.wicc) | (r.m.ctrl.wicc & (BIT32(r.m.ctrl.cnt,0) | r.m.mul));
  }else chkmul = 0;
  if (DIVEN)
  {
    y_hold = y_check & (r.a.ctrl.wy | r.e.ctrl.wy);
    chkmul = chkmul | divinsn;
  }

  bicc_hold    = icc_check & !icc_valid(r);
  bicc_hold_bp = icc_check_bp & !icc_valid(r);

  if ( ((r.a.ctrl.ld | chkmul) & r.a.ctrl.wreg & ldchkra) &
       ((ldcheck1 & (r.a.ctrl.rd == rfa1)) |
        (ldcheck2 & (r.a.ctrl.rd == rfa2)) |
        (ldcheck3 & (r.a.ctrl.rd == rfrd))) )
    ldlock = 1;

  if ( ((r.e.ctrl.ld | r.e.mac) & r.e.ctrl.wreg & ldchkex) & 
        ((CFG_LDDEL==2) | (MACPIPE & r.e.mac) | ((MULTYPE==3) & r.e.mul)) &
       ((ldcheck1 & (r.e.ctrl.rd == rfa1)) |
        (ldcheck2 & (r.e.ctrl.rd == rfa2))) )
    ldlock = 1; 

  de_fins_holdx = inBPRED & fins & (r.a.bp | r.e.bp); // skip BP on FPU inst in branch target address
  de_fins_hold  = de_fins_holdx;
  ldlock = ldlock | y_hold | fpc_lock | (inBPRED & r.a.bp & BIT32(r.a.ctrl.inst,29) & de_wcwp) | de_fins_holdx;
  
  if ( ((icc_check_bp & inBPRED) == 1) && ((r.a.nobp | mul_hold) == 0) )
    bp = bicc_hold_bp;
  else ldlock = ldlock | bicc_hold | bicc_hold_bp;
  lldcheck1 = ldcheck1;
  lldcheck2 = ldcheck2;
  lldlock   = ldlock;
  lldchkra  = ldchkra; 
  lldchkex  = ldchkex;
}

//****************************************************************************
void iu3::fpbranch(word inst,// : in word; 
                       uint32 fcc,//  : in std_logic_vector(1 downto 0);
                       uint32 &branch)// : out std_ulogic) is
{
  uint32 cond;// : std_logic_vector(3 downto 0);
  uint32 fbres;// : std_ulogic;
  cond = BITS32(inst,28,25);//(28 downto 25);
  switch (BITS32(cond,2,0))
  {
    case 0:   fbres = 0;	                          break;		// fba, fbn
    case 1:   fbres = BIT32(fcc,1) | BIT32(fcc,0);	break;
    case 2:   fbres = BIT32(fcc,1) ^ BIT32(fcc,0);	break;
    case 3:   fbres = BIT32(fcc,0);	                break;
    case 4:   fbres = !BIT32(fcc,1) & BIT32(fcc,0);	break;
    case 5:   fbres = BIT32(fcc,1);	                break;
    case 6:   fbres = BIT32(fcc,1) & !BIT32(fcc,0);	break;
    default:  fbres = BIT32(fcc,1) & BIT32(fcc,0);
  }
  branch = BIT32(cond,3) ^ fbres;     
}

//****************************************************************************
// PC generation
void iu3::ic_ctrl(uint32 rstn,//  : in  std_ulogic;
                  uint32 inBPRED,
                      registers &r,// : registers; 
                      mul32_out_type &mulo,//  : in  mul32_out_type;
                      div32_out_type &divo,//  : in  div32_out_type;
                      word inst,// : word; "de_inst"
                      uint32 annul_all, //"v.x.annul_all"
                      uint32 ldlock, //"de_ldlock"
                      uint32 branch_true, //" branch_true(de_icc, de_inst)"
	                    uint32 fbranch_true, //"de_fbranch"
                      uint32 cbranch_true, //"de_cbranch"
                      uint32 fccv,//"fpo.ccv"
                      uint32 cccv,// : in std_ulogic; "cpo.ccv"
	                    uint32 &cnt,// : out std_logic_vector(1 downto 0); "v.d.cnt"
	                    pctype &de_pc,// : out pctype; "v.d.pc"
                      uint32 &de_branch, // "de_branch"
                      uint32 &ctrl_annul, // "v.a.ctrl.annul"
                      uint32 &de_annul, // "v.d.annul"
                      uint32 &jmpl_inst, // "v.a.jmpl"
                      uint32 &inull, // "de_inull"
	                    uint32 &de_pv, /// "v.d.pv"
                      uint32 &ctrl_pv, // "v.a.ctrl.pv"
                      uint32 &de_hold_pc, // "de_hold_pc"
                      uint32 &ticc_exception, // "v.a.ticc"
                      uint32 &rett_inst, // "v.a.ctrl.rett"
                      uint32 &mulstart, // "v.a.mulstart"
	                    uint32 &divstart,// : out std_ulogic; "v.a.divstart"
                      uint32 rabpmiss, // "ra_bpmiss"
                      uint32 exbpmiss)// : std_logic) is "ex_bpmiss"
{
  uint32 op;// : std_logic_vector(1 downto 0);
  uint32 op2;// : std_logic_vector(2 downto 0);
  uint32 op3;// : std_logic_vector(5 downto 0);
  uint32 cond;// : std_logic_vector(3 downto 0);
  uint32 hold_pc, annul_current, annul_next, branch, annul, pv;// : std_ulogic;
  uint32 de_jmpl, inhibit_current;// : std_ulogic;

  branch        = 0;
  annul_next    = 0;
  annul_current = 0;
  pv            = 1;
  hold_pc       = 0;
  ticc_exception = 0;
  rett_inst     = 0;
  op            = BITS32(inst,31,30);//(31 downto 30);
  op3           = BITS32(inst,24,19);//(24 downto 19); 
  op2           = BITS32(inst,24,22);//(24 downto 22);
  cond          = BITS32(inst,28,25);//(28 downto 25); 
  annul         = BIT32(inst,29);
  de_jmpl       = 0;
  cnt           = 0;
  mulstart      = 0;
  divstart      = 0;
  inhibit_current = 0;
#if 1 
if(iClkCnt>=1241)
  bool st = true;
#endif
  if (r.d.annul == 0)
  {
    switch (BITS32(inst,31,30))
    {
      case CALL:
        branch = 1;
	      if (r.d.inull == 1)
	      {
	        hold_pc       = 1;
	        annul_current = 1;
 	      }
 	    break;
      case FMT2:
        if ( (op2==BICC) || (FPEN && (op2==FBFCC)) || (CPEN && (op2==CBCCC)) )
        {
          if (FPEN && (op2 == FBFCC))
          {
	          branch = fbranch_true;
	          if (fccv != 1) 
	          {
	            hold_pc       = 1;
	            annul_current = 1;
	          }
          }else if (CPEN && (op2 == CBCCC))
          {
	          branch = cbranch_true;
	          if (cccv != 1) 
	          {
	            hold_pc       = 1;
	            annul_current = 1;
	          }
	        }else 
	          branch = branch_true | (inBPRED & orv(cond) & !icc_valid(r));
	        
	        if (hold_pc == 0)
	        {
  	        if (branch == 1)
  	        {
              if ((cond==BA) && (annul==1)) annul_next = 1;
            }else 
              annul_next = annul_next | annul;
            if (r.d.inull==1) // contention with JMPL
            {
              hold_pc       = 1;
              annul_current = 1;
              annul_next    = 0;
            }
          }
        }
      break;
      case FMT3:
        switch(op3)
        {
          case UMUL: case SMUL: case UMULCC: case SMULCC:
	          if (MULEN && (MULTYPE != 0)) mulstart = 1;
	          if (MULEN && (MULTYPE == 0))
	          {
              switch (r.d.cnt)
              {
                case 0:
 	                cnt      = 0x1;
 	                hold_pc  = 1;
 	                pv       = 0;
 	                mulstart = 1;
 	              break;
                case 1:
 	                if (mulo.nready) cnt = 0x0;
                  else
                  {
                    cnt = 0x1;
                    pv  = 0;
                    hold_pc = 1;
                  }
                break;
                default:;
	            }
	          }
	        break;
          case UDIV: case SDIV: case UDIVCC: case SDIVCC:
	          if (DIVEN)
	          {
              switch (r.d.cnt)
              {
                case 0:
 	                hold_pc = 1;
 	                pv      = 0;
	                if (r.d.divrdy == 0)
	                {
 	                  cnt      = 1;
 	                  divstart = 1;
	                }
	              break;
                case 1:
 	                if (divo.nready == 1) cnt = 0x0; 
                  else 
                  {
                    cnt = 1;
                    pv = 0;
                    hold_pc = 1;
                  }
                break;
                default:;
	            }
	          }
	        break;
          case TICC:
	          if (branch_true == 1) ticc_exception = 1;
	        break;
          case RETT:
            rett_inst = 1; //su := sregs.ps; 
          break;
          case JMPL:
            de_jmpl = 1;
          break;
          case WRY:
            if (PWRD1)
            {
              if (BITS32(inst,29,25) == 0x13)// %ASR19
              {
                switch (r.d.cnt)
                {
                  case 0:
                    pv  = 0;
                    cnt = 0x0;
                    hold_pc = 1;
                    if (r.x.ipend == 1) cnt = 0x1;
                  break;
                  case 1: 
                    cnt = 0x0;
                  break;
                  default:;
                }
              }
            }
          break;
          default:;
        }
      break;
      default:  // LDST 
        switch (r.d.cnt)
        {
          case 0:
            if (BIT32(op3,2) | (BITS32(op3,1,0) == 0x3)) // ST/LDST/SWAP/LDD/CASA
            {
 	            cnt     = 0x1;
 	            hold_pc = 1;
 	            pv      = 0;
            }
          break;
          case 1:
            if ( (BITS32(op3,2,0)==0x7) || (BITS32(op3,3,0)==0xD) ||
	                (CASAEN && (BITS32(op3,5,4)==0x3)) ||	// CASA
                 ((CPEN||FPEN) && BIT32(op3,5) && (BITS32(op3,2,0)==0x6)) ) // LDD/STD/LDSTUB/SWAP
	          {
 	            cnt     = 0x2;
 	            pv      = 0;
 	            hold_pc = 1;
	          }else
 	            cnt = 0x0;
          break;
          case 2:
 	          cnt = 0x0;
 	        break;
          default:;
	      }
	    break;
    }
  }

  if (ldlock == 1)
  {
    cnt        = r.d.cnt;
    annul_next = 0;
    pv         = 1;
  }
  hold_pc = (hold_pc | ldlock) & !annul_all;

  if (exbpmiss & r.a.ctrl.annul & r.d.pv & !hold_pc)
  {
	  annul_next = 1;
	  pv         = 0;
  }
  if (exbpmiss & !r.a.ctrl.annul & r.d.pv)
  {
	  annul_next    = 1;
	  pv            = 0;
	  annul_current = 1;
  }
  if (exbpmiss & !r.a.ctrl.annul & !r.d.pv & !hold_pc)
  {
	  annul_next = 1;
	  pv         = 0;
  }
  if ((exbpmiss & BIT32(r.e.ctrl.inst,29) & !r.a.ctrl.annul & !r.d.pv ) 
	  && (r.d.cnt == 0x1))
	{
	  annul_next    = 1;
	  annul_current = 1;
	  pv            = 0;
  }
  if (exbpmiss & BIT32(r.e.ctrl.inst,29) & r.a.ctrl.annul & r.d.pv)
  {
      annul_next = 1;
      pv         = 0;
      inhibit_current = 1;
  }
  if (rabpmiss & !BIT32(r.a.ctrl.inst,29) & !r.d.annul & r.d.pv & !hold_pc)
  {
	  annul_next = 1;
	  pv         = 0;
  }
  if (rabpmiss & BIT32(r.a.ctrl.inst,29) & !r.d.annul & r.d.pv )
  {
	  annul_next      = 1;
	  pv              = 0;
	  inhibit_current = 1;
  }
#if 1
  if(iClkCnt>=21)
  bool st = true;
#endif
  if (hold_pc) de_pc = r.d.pc;
  else         de_pc = r.f.pc;

  annul_current = annul_current | (ldlock & !inhibit_current) | annul_all;
  ctrl_annul    = r.d.annul | annul_all | annul_current | inhibit_current;
  pv            = pv & !((r.d.inull & !hold_pc) | annul_all);
  jmpl_inst     = de_jmpl & !annul_current & !inhibit_current;
  annul_next    = (r.d.inull & !hold_pc) | annul_next | annul_all;
  if ((annul_next == 1) || (rstn == 0))
    cnt = 0; 

  de_hold_pc = hold_pc;
  de_branch  = branch;
  de_annul   = annul_next;
  de_pv      = pv;
  ctrl_pv    = r.d.pv & !((r.d.annul & !r.d.pv) | annul_all | annul_current);
  inull      = (!rstn) | r.d.inull | hold_pc | annul_all;
}

//****************************************************************************
// register write address generation
void iu3::rd_gen( registers &r,// : registers; 
                      mul32_out_type &mulo,//  : in  mul32_out_type;
                      div32_out_type &divo,//  : in  div32_out_type;
                      word inst,// : word; 
                      uint32 &wreg, 
                      uint32 &ld,// : out std_ulogic; 
	                    uint32 &rdo)// : out std_logic_vector(4 downto 0)) is
{
  uint32 write_reg;// : std_ulogic;
  uint32 op;// : std_logic_vector(1 downto 0);
  uint32 op2;// : std_logic_vector(2 downto 0);
  uint32 op3;// : std_logic_vector(5 downto 0);
  uint32 rd;//  : std_logic_vector(4 downto 0);

  op    = BITS32(inst,31,30);//(31 downto 30);
  op2   = BITS32(inst,24,22);//(24 downto 22);
  op3   = BITS32(inst,24,19);//(24 downto 19);
  write_reg = 0;
  rd    = BITS32(inst,29,25);//(29 downto 25);
  ld    = 0;

  switch(op)
  {
    case CALL:
      write_reg = 1;
      rd        = 0x0F;    // CALL saves PC in r[15] (%o7)
    break;
    case FMT2:
        if(op2==SETHI) write_reg = 1;
    break;
    case FMT3:
      switch (op3)
      {
        case UMUL: case SMUL: case UMULCC: case SMULCC:
          if (MULEN)
          {
            if (((mulo.nready == 1) && (r.d.cnt != 0x0)) || (MULTYPE != 0))
              write_reg = 1; 
          }else write_reg = 1;
        break;
        case UDIV: case SDIV: case UDIVCC: case SDIVCC:
          if (DIVEN)
          {
            if ((divo.nready == 1) && (r.d.cnt != 0x0))
              write_reg = 1; 
          }else write_reg = 1;
        break;
        case RETT: case WRPSR: case WRY: case WRWIM: case WRTBR: case TICC: case FLUSH: break;
        case FPOP1: case FPOP2: break;
        case CPOP1: case CPOP2: break;
        default: write_reg = 1; break;
      }
    break;
    default:   // LDST
      ld = !BIT32(op3,2);
      if ( (BIT32(op3,2)==0) && !((CPEN || FPEN) && (BIT32(op3,5) == 1)) )
        write_reg = 1;
      switch (op3)
      {
        case SWAP: case SWAPA: case LDSTUB: case LDSTUBA: case CASA:
          if (r.d.cnt==0x0) 
          {
            write_reg = 1;
            ld        = 1;
          }
        break;
        default:;
      }
      if (r.d.cnt == 0x1)
      {
        switch(op3)
        {
          case LDD: case LDDA: case LDDC: case LDDF: 
            rd |= 0x1; 
          break;
          default:;
        }
    	}
    break;
  }

  if (rd==0x0) write_reg = 0;
  wreg = write_reg;
  rdo  = rd;
}

//****************************************************************************
// immediate data generation
word iu3::imm_data (registers &r,// : registers;
                        word insn)// : word) 	return word is
{
  word immediate_data = 0;
  word inst = insn;// : word;

  switch (BITS32(inst,31,30))
  {
    case FMT2:
      immediate_data = BITS32(inst,21,0)<<10;
    break;
    default:	// LDST
      immediate_data = BIT32(inst,12) ? MSK32(31,13): 0x0;
      immediate_data |= BITS32(inst,12,0);
    break;
  }
  return(immediate_data);
}

//****************************************************************************
// read special registers
word iu3::get_spr (registers &r)// : registers) return word is
{
  word spr = 0;
  switch (BITS32(r.e.ctrl.inst,24,19))
  {
    case RDPSR:
      spr = (IMPL<<28) | (VER<<24) 
      | (r.m.icc<<20) | (0x0<<14) | (r.w.s.ec<<13) | (r.w.s.ef<<12)
      | (r.w.s.pil<<8) | (r.e.su<<7) | (r.w.s.ps<<6) | (r.e.et<<5);
      spr |= BITS32(r.e.cwp,NWINLOG2-1,0);
    break;
    case RDTBR:
      spr = (r.w.s.tba<<12) | (r.w.s.tt<<4) | (0x0);
    break;
    case RDWIM:
      spr = BITS32(r.w.s.wim, CFG_NWIN-1, 0);
    break;
    default:;
  }
  return(spr);
}

//****************************************************************************
// immediate data select
bool iu3::imm_select(word inst)// : word) return boolean is
{
  bool imm = false;
  switch (BITS32(inst,31,30))
  {
    case FMT2:
      switch (BITS32(inst,24,22))
      {
        case SETHI: imm = true; break;
        default:;
      }
    break;
    case FMT3:
      switch (BITS32(inst,24,19))
      {
        case RDWIM:
        case RDPSR:
        case RDTBR: imm = true; break;
        default:    if (BIT32(inst,13)) imm = true;
      }
    break;
    case LDST:
      if (BIT32(inst,13)) imm = true;
    break;
    default:;
  }
  return(imm);
}

//****************************************************************************
// EXE operation
void iu3::alu_op( registers &r,// : in registers; 
                      word iop1, 
                      word iop2,// : in word; 
                      uint32 me_icc,// : std_logic_vector(3 downto 0);
	                    uint32 my, 
	                    uint32 ldbp,// : std_ulogic; 
	                    word &aop1, 
	                    word &aop2,// : out word; 
	                    uint32 &aluop,//  : out std_logic_vector(2 downto 0);
	                    uint32 &alusel,// : out std_logic_vector(1 downto 0); 
	                    uint32 &aluadd,// : out std_ulogic;
	                    uint32 &shcnt,// : out std_logic_vector(4 downto 0); 
	                    uint32 &sari, 
	                    uint32 &shleft, 
	                    uint32 &ymsb, 
	                    uint32 &mulins, 
	                    uint32 &divins, 
	                    uint32 &mulstep, 
	                    uint32 &macins, 
	                    uint32 &ldbp2, 
	                    uint32 &invop2)// : out std_ulogic) is
{
  uint32 op;// : std_logic_vector(1 downto 0);
  uint32 op2;// : std_logic_vector(2 downto 0);
  uint32 op3;// : std_logic_vector(5 downto 0);
//  uint32 rd;//  : std_logic_vector(4 downto 0);
  uint32 icc;// : std_logic_vector(3 downto 0);
  uint32 y0;//  : std_ulogic;

  op     = BITS32(r.a.ctrl.inst,31,30);
  op2    = BITS32(r.a.ctrl.inst,24,22);
  op3    = BITS32(r.a.ctrl.inst,24,19);
  aop1   = iop1; 
  aop2   = iop2;
  ldbp2  = ldbp;
  aluop  = EXE_NOP;
  alusel = EXE_RES_MISC;
  aluadd = 1; 
  shcnt  = iop2&0x1F;
  sari   = 0;
  shleft = 0;
  invop2 = 0;
  ymsb   = iop1&0x1;
  mulins = 0;
  divins = 0;
  mulstep = 0;
  macins = 0;

  if (r.e.ctrl.wy == 1)      y0 = my;
  else if (r.m.ctrl.wy == 1) y0 = r.m.y&0x1;
  else if (r.x.ctrl.wy == 1) y0 = r.x.y&0x1;
  else                       y0 = r.w.s.y&0x1;

  if (r.e.ctrl.wicc == 1)      icc = me_icc;
  else if (r.m.ctrl.wicc == 1) icc = r.m.icc;
  else if (r.x.ctrl.wicc == 1) icc = r.x.icc;
  else                         icc = r.w.s.icc;

  switch (op)
  {
    case CALL:
      aluop = EXE_LINK;
    break;
    case FMT2:
      if (op2==SETHI) aluop = EXE_PASS2;
    break;
    case FMT3:
      switch (op3)
      {
        case IADD: case ADDX: case ADDCC: case ADDXCC: case TADDCC: case TADDCCTV:
        case SAVE: case RESTORE: case TICC: case JMPL: case RETT:
          alusel = EXE_RES_ADD;
        break;
        case ISUB: case SUBX: case SUBCC: case SUBXCC: case TSUBCC: case TSUBCCTV:
	        alusel = EXE_RES_ADD;
	        aluadd = 0;
	        aop2   = ~iop2;
	        invop2 = 1;
	      break;
        case MULSCC: 
          alusel = EXE_RES_ADD;
	        aop1   = ((BIT32(icc,3) ^ BIT32(icc,1))<<31) | BITS32(iop1,31,1);
	        if (y0==0) 
	        {
	          aop2  = 0;
	          ldbp2 = 0;
	        }
	        mulstep = 1;
	      break;
        case UMUL: case UMULCC: case SMUL: case SMULCC:
	        if (MULEN) mulins = 1;
	      break;
        case UMAC: case SMAC:
	        if (MACEN)
	        {
	          mulins = 1;
	          macins = 1;
	        }
	      break;
        case UDIV: case UDIVCC: case SDIV: case SDIVCC:
	        if (DIVEN)
	        {
	          aluop  = EXE_DIV;
	          alusel = EXE_RES_LOGIC;
	          divins = 1;
          }
        break;
        case IAND: case ANDCC: 
          aluop  = EXE_AND;
          alusel = EXE_RES_LOGIC;
        break;
        case ANDN: case ANDNCC:
          aluop  = EXE_ANDN;
          alusel = EXE_RES_LOGIC;
        break;
        case IOR: case ORCC:
          aluop  = EXE_OR;
          alusel = EXE_RES_LOGIC;
        break;
        case ORN: case ORNCC:
          aluop  = EXE_ORN;
          alusel = EXE_RES_LOGIC;
        break;
        case IXNOR: case XNORCC:
          aluop  = EXE_XNOR;
          alusel = EXE_RES_LOGIC;
        break;
        case XORCC: case IXOR: case WRPSR: case WRWIM: case WRTBR: case WRY:
      	  aluop  = EXE_XOR;
      	  alusel = EXE_RES_LOGIC;
      	break;
        case RDPSR: case RDTBR: case RDWIM:
          aluop = EXE_SPR;
        break;
        case RDY:
          aluop = EXE_RDY;
        break;
        case ISLL:
          aluop  = EXE_SLL;
          alusel = EXE_RES_SHIFT; 
          shleft = 1; 
		      shcnt  = (~iop2)&0x1F;
		      invop2 = 1;
		    break;
        case ISRL:
          aluop  = EXE_SRL;
          alusel = EXE_RES_SHIFT; 
        break;
        case ISRA:
          aluop  = EXE_SRA;
          alusel = EXE_RES_SHIFT;
          sari   = BIT32(iop1,31);
        break;
        case FPOP1: case FPOP2:
        default:;
      }
    break;
    default:	// LDST
      switch (r.a.ctrl.cnt)
      {
        case 0:
          alusel = EXE_RES_ADD;
        break;
        case 1:
	        switch(op3)
	        {
	          case LDD: case LDDA: case LDDC:
	            alusel = EXE_RES_ADD;
	          break;
	          case LDDF:
	            alusel = EXE_RES_ADD;
	          break;
	          case SWAP: case SWAPA: case LDSTUB: case LDSTUBA: case CASA:
	            alusel = EXE_RES_ADD;
	          break;
	          case STF:
	          case STDF:
	          default:
              aluop = EXE_PASS1;
	            if (BIT32(op3,2))
	            {
	              if (BITS32(op3,1,0) == 0x1)      aluop = EXE_STB;
	              else if (BITS32(op3,1,0) == 0x2) aluop = EXE_STH;
              }
            break;
          }
        break;
        case 2:
          aluop = EXE_PASS1;
          if (BIT32(op3,2)) // ST
          {
            if (BIT32(op3,3) & !BIT32(op3,5) & !BIT32(op3,1))// LDSTUB
              aluop = EXE_ONES;
          }
	        if (CASAEN && (r.m.casa == 1))
	        {
	          alusel = EXE_RES_ADD;
	          aluadd = 0;
	          aop2   = ~iop2;
	          invop2 = 1;
	        }
        default:;
      }
    break;
  }
}

//****************************************************************************
uint32 iu3::ra_inull_gen( registers &r, 
                              registers &v)// : registers) return std_ulogic is
{
  uint32 de_inull = 0;// : std_ulogic;
  if ((v.e.jmpl | v.e.ctrl.rett) & !v.e.ctrl.annul & !(r.e.jmpl & !r.e.ctrl.annul)) 
    de_inull = 1;
  if ((v.a.jmpl | v.a.ctrl.rett) & !v.a.ctrl.annul & !(r.a.jmpl & !r.a.ctrl.annul))
    de_inull = 1;
  return(de_inull);
}

//****************************************************************************
// operand generation
void iu3::op_mux( registers &r,// : in registers; 
                      word rfd, 
                      word ed, 
                      word md, 
                      word xd, 
                      word im,// : in word; 
	                    uint32 rsel,// : in std_logic_vector(2 downto 0); 
	                    uint32 &ldbp,// : out std_ulogic; 
	                    word &d,// : out word; 
	                    uint32 id)// : std_logic) is
{
  ldbp = 0;
  switch (rsel)
  {
    case 0: d = rfd; break;
    case 1: d = ed; break;
    case 2: 
      d = md;
      if (CFG_LDDEL == 1) ldbp = r.m.ctrl.ld;
    break;
    case 3: d = xd; break;
    case 4: d = im; break;
    case 5: d = 0; break;
    case 6: d = r.w.result; break;
    default: d = 0;//(others => '-');   // WARNING!!!!!!! in a testbench value will be "-----------"
  }
  if (CASAEN && (r.a.ctrl.cnt == 0x2) && (r.m.casa & !id))
    ldbp = 1;
}

//****************************************************************************
void iu3::op_find(registers &r,// : in registers; 
                      uint32 ldchkra,// : std_ulogic; 
                      uint32 ldchkex,// : std_ulogic;
	                    uint32 rs1,// : std_logic_vector(4 downto 0); 
	                    rfatype ra,// : rfatype; 
	                    bool im,// : boolean; 
	                    uint32 &rfe,// : out std_ulogic; 
	                    uint32 &osel,// : out std_logic_vector(2 downto 0); 
	                    uint32 ldcheck)// : std_ulogic) is
{
  rfe = 0;
  if (im)               osel = 0x4;
  else if (rs1 == 0x0)  osel = 0x5;	// %g0
  else if (((r.a.ctrl.wreg & ldchkra)==1) && (ra == r.a.ctrl.rd)) osel = 0x1;
  else if (((r.e.ctrl.wreg & ldchkex)==1) && (ra == r.e.ctrl.rd)) osel = 0x2;
  else if ((r.m.ctrl.wreg == 1) && (ra == r.m.ctrl.rd))           osel = 0x3;
  else if ((CFG_IRFWT == 0) && (r.x.ctrl.wreg==1) && (ra == r.x.ctrl.rd)) osel = 0x6;
  else
  {
    osel = 0x0;
    rfe  = ldcheck;
  }
}

//****************************************************************************
// generate carry-in for alu
void iu3::cin_gen(registers &r,// : registers; 
                      uint32 me_cin,// : in std_ulogic; 
                      uint32 &cin)// : out std_ulogic) is
{
  uint32 op;// : std_logic_vector(1 downto 0);
  uint32 op3;// : std_logic_vector(5 downto 0);
  uint32 ncin;// : std_ulogic;

  op  = BITS32(r.a.ctrl.inst,31,30);
  op3 = BITS32(r.a.ctrl.inst,24,19);
  if (r.e.ctrl.wicc == 1) ncin = me_cin;
  else                    ncin = BIT32(r.m.icc,0); 
  cin = 0;
  switch(op)
  {
    case FMT3:
      switch(op3)
      {
        case ISUB: case SUBCC: case TSUBCC: case TSUBCCTV:  cin = 1; break;
        case ADDX: case ADDXCC:                             cin = ncin; break;
        case SUBX: case SUBXCC:                             cin = !ncin; break;
        default:;
      }
    break;
    case LDST:
      if (CASAEN && (r.m.casa == 1) && (r.a.ctrl.cnt == 0x2))
        cin = 1;
    break;
    default:;
  }
}

//****************************************************************************
void iu3::logic_op(registers &r,// : registers; 
                        mul32_out_type &mulo,//  : in  mul32_out_type;
                        word aluin1, 
                        word aluin2, 
                        word mey,// : word; 
	                      uint32 ymsb,// : std_ulogic; 
	                      word &logicres, 
	                      word &y)// : out word) is
{
  word logicout;// : word;
  switch (r.e.aluop)
  {
    case EXE_AND:  logicout = aluin1 & aluin2; break;
    case EXE_ANDN: logicout = aluin1 & ~aluin2; break;
    case EXE_OR:   logicout = aluin1 | aluin2; break;
    case EXE_ORN:  logicout = aluin1 | ~aluin2; break;
    case EXE_XOR:  logicout = aluin1 ^ aluin2; break;
    case EXE_XNOR: logicout = aluin1 ^ ~aluin2; break;
    case EXE_DIV:
      if (DIVEN) logicout = aluin2;
      else       logicout = 0;//(others => '-');
    break;
    default: logicout = 0;//(others => '-');
  }
  if (r.e.ctrl.wy & r.e.mulstep)      y = (ymsb<<31) | BITS32(r.m.y,31,1);
  else if (r.e.ctrl.wy == 1)          y = logicout;
  else if (r.m.ctrl.wy == 1)          y = mey; 
  else if (MACPIPE && (r.x.mac == 1)) y = (word)BITS64(mulo.result,63,32);
  else if (r.x.ctrl.wy == 1)          y = r.x.y; 
  else                                y = r.w.s.y;
  logicres = logicout;
}

//****************************************************************************
void iu3::misc_op(registers &r,// : registers; 
                      mul32_out_type &mulo,//  : in  mul32_out_type;
                      watchpoint_registers wpr,// : watchpoint_registers; 
                      word aluin1, 
                      word aluin2, 
                      word ldata, 
                      word mey,// : word; 
        	            word &mout, 
        	            word &edata)// : out word) is
{
  word miscout, bpdata;// : word;
  int32 wpi;// : integer;

  wpi     = 0;
  miscout = r.e.ctrl.pc&MSK32(31,CFG_PCLOW);
  edata   = aluin1;
  bpdata  = aluin1;
  if ( (r.x.ctrl.wreg & r.x.ctrl.ld & !r.x.ctrl.annul) &&
       (r.x.ctrl.rd == r.e.ctrl.rd) && (BITS32(r.e.ctrl.inst,31,30)==LDST) &&
       (r.e.ctrl.cnt != 0x2) )
  {
    bpdata = ldata;
  }

  switch(r.e.aluop)
  {
    case EXE_STB:   
      miscout = ((bpdata&0xFF)<<24) | ((bpdata&0xFF)<<16) | ((bpdata&0xFF)<<8) | (bpdata&0xFF);
	    edata = miscout;
	  break;
    case EXE_STH:
      miscout = ((bpdata&0xFFFF)<<16) | (bpdata&0xFFFF);
	    edata   = miscout;
	  break;
    case EXE_PASS1:
      miscout = bpdata;
      edata   = miscout;
    break;
    case EXE_PASS2: 
      miscout = aluin2;
    break;
    case EXE_ONES:
      miscout = 0xFFFFFFFF;
	    edata   = miscout;
	  break;
    case EXE_RDY:
      if (MULEN && (r.m.ctrl.wy == 1)) miscout = mey;
      else                             miscout = r.m.y;
      if ((CFG_NWP > 0) && (BITS32(r.e.ctrl.inst,18,17) == 0x3))
      {
        wpi = BITS32(r.e.ctrl.inst,16,15);
        if (BIT32(r.e.ctrl.inst,14) == 0) 
          miscout = (wpr.arr[wpi].addr) | (0x0<<1) | wpr.arr[wpi].exec;
        else miscout = (wpr.arr[wpi].mask) | (wpr.arr[wpi].load<<1) | wpr.arr[wpi].store;
      }
      if ((BITS32(r.e.ctrl.inst,18,17)==0x2) && (BIT32(r.e.ctrl.inst,14)==1)) //%ASR17
        miscout = asr17_gen(r);
      if (MACEN)
      {
        if (BITS32(r.e.ctrl.inst,18,14) == 0x12) //%ASR18
        {
          if ((r.m.mac & !MACPIPE) || (r.x.mac & MACPIPE))
            miscout = (word)BITS64(mulo.result,31,0);	// data forward of asr18
          else miscout = r.w.s.asr18;
        }else
        {
          if ((r.m.mac & !MACPIPE) | (r.x.mac & MACPIPE))
            miscout = (word)BITS64(mulo.result,63,32);   // data forward Y
        }
      }
    break;
    case EXE_SPR:
      miscout = get_spr(r);
    break;
    default:;
  }
  mout = miscout;
}

//****************************************************************************
void iu3::alu_select(registers &r,// : registers; 
                          uint64 addout,// : std_logic_vector(32 downto 0);
	                        word op1, 
	                        word op2,// : word; 
	                        word shiftout, 
	                        word logicout, 
	                        word miscout,// : word; 
	                        word &res,// : out word; 
	                        uint32 me_icc,// : std_logic_vector(3 downto 0);
	                        uint32 &icco,// : out std_logic_vector(3 downto 0); 
	                        uint32 &divz, 
	                        uint32 &mzero)// : out std_ulogic) is
{
  uint32 op;// : std_logic_vector(1 downto 0);
  uint32 op3;// : std_logic_vector(5 downto 0);
  uint32 icc;// : std_logic_vector(3 downto 0);
  word aluresult;// : word;
  uint32 azero;// : std_logic;
  
  op  = BITS32(r.e.ctrl.inst,31,30);
  op3 = BITS32(r.e.ctrl.inst,24,19);
  icc = 0;
  if (BITS64(addout,32,1) == 0) azero = 1;
  else                           azero = 0;
  mzero = azero;
#if 1
  if(iClkCnt>=152)
  bool s =true;
#endif
  
  switch(r.e.alusel)
  {
    case EXE_RES_ADD:
      aluresult = (word)BITS64(addout,32,1);
      if (r.e.aluadd == 0)
      {
        icc = ( (!BIT32(op1,31) & !BIT32(op2,31)) | 	// Carry
		      ((word)BIT64(addout,32) & (!BIT32(op1,31) | !BIT32(op2,31))) ) ? 0x1: 0x0;
		      
        icc |= ( (BIT32(op1,31) & BIT32(op2,31) & !(word)BIT64(addout,32)) | 	// Overflow
          ((word)BIT64(addout,32) & !BIT32(op1,31) & !BIT32(op2,31)) ) ? 0x2: 0x0;
      }else
      {
        icc = ( (BIT32(op1,31) & BIT32(op2,31)) | 	// Carry
		      (!(word)BIT64(addout,32) & (BIT32(op1,31) | BIT32(op2,31))) ) ? 0x1: 0x0;
        icc |= ( (BIT32(op1,31) & BIT32(op2,31) & !(word)BIT64(addout,32)) | 	// Overflow
		      ((word)BIT64(addout,32) & !BIT32(op1,31) & !BIT32(op2,31)) ) ? 0x2: 0x0;
      }
      if (CFG_NOTAG == 0)
      {
        switch(op)
        {
          case FMT3:
            switch (op3)
            {
              case TADDCC: case TADDCCTV:
                icc |= (BIT32(op1,0) | BIT32(op1,1) | BIT32(op2,0) | BIT32(op2,1) | BIT32(icc,1)) ? 0x2 : 0x0;
              break;
              case TSUBCC: case TSUBCCTV:
                icc |= (BIT32(op1,0) | BIT32(op1,1) | !BIT32(op2,0) | !BIT32(op2,1) | BIT32(icc,1)) ? 0x2 : 0x0;
              break;
              default:;
            }
          break;
          default:;
        }
      }
      icc |= ((azero&0x1)<<2);
    break;
    case EXE_RES_SHIFT:
      aluresult = shiftout;
    break;
    case EXE_RES_LOGIC:
      aluresult = logicout;
      if (aluresult == 0) icc |= 0x4;
    break;
    default:
       aluresult = miscout;
  }
  if (r.e.jmpl == 1) 
    aluresult = r.e.ctrl.pc&MSK32(31,2);
  icc |= (BIT32(aluresult,31)<<3);
  divz = BIT32(icc,2);
  if (r.e.ctrl.wicc == 1)
  {
    if ((op == FMT3) && (op3 == WRPSR)) icco = BITS32(logicout,23,20);
    else                                icco = icc;
  }else if (r.m.ctrl.wicc == 1)   icco = me_icc;
  else if (r.x.ctrl.wicc == 1)    icco = r.x.icc;
  else                            icco = r.w.s.icc;
  res = aluresult;
}

//****************************************************************************
void iu3::dcache_gen(registers &r, 
                          registers &v,// : registers; 
                          dc_in_type &dci,// : out dc_in_type; 
	                        uint32 &link_pc, 
	                        uint32 &jump, 
	                        uint32 &force_a2, 
	                        uint32 &load, 
	                        uint32 &mcasa)// : out std_ulogic) is
{
  uint32 op;// : std_logic_vector(1 downto 0);
  uint32 op3;// : std_logic_vector(5 downto 0);
  uint32 su;// : std_ulogic;
  
  op  = BITS32(r.e.ctrl.inst,31,30);
  op3 = BITS32(r.e.ctrl.inst,24,19);
  dci.Signed = 0;
  dci.lock   = 0;
  dci.dsuen  = 0;
  dci.size   = SZWORD;
  mcasa      = 0;
  if (op == LDST)
  {
    switch (op3)
    {
      case LDUB: case LDUBA:
        dci.size = SZBYTE;
      break;
      case LDSTUB: case LDSTUBA:
        dci.size = SZBYTE;
        dci.lock = 1; 
      break;
      case LDUH: case LDUHA:
        dci.size = SZHALF;
      break;
      case LDSB: case LDSBA: 
        dci.size = SZBYTE; dci.Signed = 1; 
      break;
      case LDSH: case LDSHA:
        dci.size   = SZHALF;
        dci.Signed = 1;
      break;
      case LD: case LDA: case LDF: case LDC:
        dci.size = SZWORD;
      break;
      case SWAP: case SWAPA:
        dci.size = SZWORD;
        dci.lock = 1; 
      break;
      case CASA:
        if (CASAEN) 
        {
          dci.size = SZWORD;
          dci.lock = 1;
        }
      break;
      case LDD: case LDDA: case LDDF: case LDDC:
        dci.size = SZDBL;
      break;
      case STB: case STBA:
        dci.size = SZBYTE;
      break;
      case STH: case STHA:
        dci.size = SZHALF;
      break;
      case ST: case STA: case STF:
        dci.size = SZWORD;
      break;
      case ISTD: case STDA:
        dci.size = SZDBL;
      break;
      case STDF: case STDFQ:
        if (FPEN) dci.size = SZDBL;
      break;
      case STDC: case STDCQ:
        if (CPEN) dci.size = SZDBL;
      break;
      default:
        dci.size   = SZWORD;
        dci.lock   = 0;
        dci.Signed = 0;
    }
  }
    
  link_pc   = 0;
  jump      = 0;
  force_a2  = 0;
  load      = 0;
  dci.write = 0;
  dci.enaddr = 0;
  dci.read  = !BIT32(op3,2);

// load/store control decoding

  if (r.e.ctrl.annul == 0)
  {
    switch(op)
    {
      case CALL:
        link_pc = 1;
      break;
      case FMT3:
	      if (r.e.ctrl.trap == 0)
	      {
          switch(op3)
          {
            case JMPL:
              jump    = 1;
              link_pc = 1; 
            break;
            case RETT:
              jump = 1;
            break;
            default:;
          }
	      }
	    break;
      case LDST:
        switch(r.e.ctrl.cnt)
        {
	        case 0:
            dci.read = BIT32(op3,3) | !BIT32(op3,2);	// LD/LDST/SWAP/CASA
            load = BIT32(op3,3) | !BIT32(op3,2);
            // WARNING!!!!!! What is a fuck condition?????
            dci.enaddr = !BIT32(op3,2) | BIT32(op3,2) | (BIT32(op3,3) & BIT32(op3,2));
          break;
          case 1:
            force_a2 = !BIT32(op3,2);	// LDD
            load     = !BIT32(op3,2);
            dci.enaddr = !BIT32(op3,2);
            if (BITS32(op3,3,2) == 0x1)		// ST/STD
	            dci.write = 1;
            if ( (CASAEN && (BITS32(op3,5,4) == 0x3)) ||	// CASA
            	   (BITS32(op3,3,2) == 0x3) )		// LDST/SWAP
	            dci.enaddr = 1;
          break;
          case 2: 					// STD/LDST/SWAP/CASA
            dci.write = 1;
          break;
          default:;
	      }
	      if (r.e.ctrl.trap | (v.x.ctrl.trap & !v.x.ctrl.annul))
	        dci.enaddr = 0;
        if (CASAEN && (BITS32(op3,5,4) == 0x3)) mcasa = 1;
      break;
      default:;
    }
  }

#if 1
if(iClkCnt>=63)
bool sr = true;
#endif

  if (r.x.ctrl.rett & !r.x.ctrl.annul) su = r.w.s.ps;
  else                                 su = r.w.s.s;
  if (su == 1) dci.asi = 0x0B;//"00001011";
  else         dci.asi = 0x0A;//"00001010";
  if (BIT32(op3,4) & (!BIT32(op3,5) | !CPEN))
    dci.asi = BITS32(r.e.ctrl.inst,12,5);
}

//****************************************************************************
void iu3::fpstdata(registers &r,// : in registers; 
                        word edata, 
                        word eres,// : in word; 
                        uint32 fpstdata,// : in std_logic_vector(31 downto 0);
                        word &edata2, 
                        word &eres2)// : out word) is
{
  uint32 op;// : std_logic_vector(1 downto 0);
  uint32 op3;// : std_logic_vector(5 downto 0);
  edata2 = edata;
  eres2  = eres;
  op     = BITS32(r.e.ctrl.inst,31,30);
  op3    = BITS32(r.e.ctrl.inst,24,19);
  if (FPEN)
  {
    if (FPEN && (op==LDST) && (BITS32(op3,5,4)==0x2) && BIT32(op3,2) && (r.e.ctrl.cnt != 0))
    {
      edata2 = fpstdata;
      eres2  = fpstdata;
    }
  }
  if (CASAEN && (r.m.casa==1) && (r.e.ctrl.cnt==0x2))
  {
    edata2 = r.e.op1;
    eres2  = r.e.op1;
  }
}

//****************************************************************************
word iu3::ld_align(dcdtype &data,// : dcdtype; 
                        uint32 set,// : std_logic_vector(DSETMSB downto 0);
	                      uint32 size, 
	                      uint32 laddr,// : std_logic_vector(1 downto 0); 
	                      uint32 Signed)// : std_ulogic) return word is
{
  word align_data, rdata;// : word;
  align_data = data.arr[set];
  rdata      = 0;
  switch (size)
  {
    case 0x0: 			// byte read
      switch(laddr)
      {
        case 0:
	        rdata = BITS32(align_data,31,24);
	        if (Signed) rdata |= (BIT32(align_data,31) ? MSK32(31,8) : 0x0);
	      break;
        case 1:
	        rdata = BITS32(align_data,23,16);
	        if (Signed) rdata |= (BIT32(align_data,23) ? MSK32(31,8): 0x0);
	      break;
        case 2:
	        rdata = BITS32(align_data,15,8);
	        if (Signed) rdata |= (BIT32(align_data,15) ? MSK32(31,8): 0x0);
	      break;
        default:
	        rdata = BITS32(align_data,7,0);
	        if (Signed) rdata |= (BIT32(align_data,7) ? MSK32(31,8): 0x0);
      }
    break;
    case 0x1: 			// half-word read
      if (BIT32(laddr,1))
      {
	      rdata = BITS32(align_data,15,0);
	      if (Signed) rdata |= (BIT32(align_data,15) ? MSK32(31,15) : 0x0);
      }else
      {
	      rdata = BITS32(align_data,31,16);
	      if (Signed) rdata |= (BIT32(align_data,31) ? MSK32(31,15) : 0x0);
      }
    break;
    default: 			// single and double word read
      rdata = align_data;
  }
  return(rdata);
}

//****************************************************************************
void iu3:: mem_trap(uint32 rstn,//  : in  std_ulogic;
                        dcache_out_type &dco,//   : in  dcache_out_type;
                        fpc_out_type &fpo,//   : in  fpc_out_type;
                        fpc_out_type &cpo,//   : in  fpc_out_type;
                        registers &r,// : registers; 
                        watchpoint_registers &wpr,// : watchpoint_registers;
                        uint32 annul,
                        uint32 holdn,// : in std_ulogic;
                        uint32 &trapout,
                        uint32 &iflush, 
                        uint32 &nullify, 
                        uint32 &werrout,// : out std_ulogic;
                        uint32 &tt)// : out std_logic_vector(5 downto 0)) is
{
//  uint32 cwp;//   : std_logic_vector(NWINLOG2-1 downto 0);
  uint32 cwpx;//  : std_logic_vector(5 downto NWINLOG2);
  uint32 op;// : std_logic_vector(1 downto 0);
  uint32 op2;// : std_logic_vector(2 downto 0);
  uint32 op3;// : std_logic_vector(5 downto 0);
  uint32 nalign_d;// : std_ulogic;
  uint32 trap, werr;// : std_ulogic;
#if 1
if(iClkCnt>=118)
bool st = true;
#endif
  op   = BITS32(r.m.ctrl.inst,31,30);
  op2  = BITS32(r.m.ctrl.inst,24,22);
  op3  = BITS32(r.m.ctrl.inst,24,19);
  cwpx = r.m.result & MSK32(5,NWINLOG2);
  cwpx &= ~0x20;
  iflush  = 0;
  trap    = r.m.ctrl.trap;
  nullify = annul;
  tt      = r.m.ctrl.tt;
  werr    = (dco.werr | r.m.werr) & !r.w.s.dwt;
  nalign_d = r.m.nalign | BIT32(r.m.result,2); 
  if ((trap == 1) && (r.m.ctrl.pv == 1) && (op == LDST))
    nullify = 1;
  if (((annul | trap) != 1) && (r.m.ctrl.pv == 1))
  {
    if (werr & holdn) 
    {
      trap  = 1;
      tt    = TT_DSEX;
      werr  = 0;
      if (op == LDST) nullify = 1;
    }
  }
  if ((annul | trap) != 1) 
  {
    switch(op)
    {
      case FMT2:
        switch (op2)
        {
          case FBFCC:
            if (FPEN && (fpo.exc == 1))
            { 
              trap = 1;
              tt   = TT_FPEXC;
            }
          break;
          case CBCCC:
    	      if (CPEN && (cpo.exc == 1)) 
    	      {
    	        trap = 1;
    	        tt   = TT_CPEXC;
    	      }
    	    break;
          default:;
        }
      break;
      case FMT3:
        switch (op3)
        {
	        case WRPSR:
	          if (orv(cwpx) == 1) 
	          {
	            trap = 1;
	            tt   = TT_IINST;
	          }
	        break;
	        case UDIV: case SDIV: case UDIVCC: case SDIVCC:
            if (DIVEN && (r.m.divz == 1))
            {
              trap = 1;
              tt   = TT_DIV;
            }
          break;
	        case JMPL: case RETT:
	          if (r.m.nalign == 1) 
	          {
	            trap = 1;
	            tt   = TT_UNALA;
	          }
	        break;
          case TADDCCTV: case TSUBCCTV:
	          if ((CFG_NOTAG == 0) && BIT32(r.m.icc,1) )
	          {
	            trap = 1;
	            tt   = TT_TAG;
	          }
	        break;
	        case FLUSH:
	          iflush = 1;
	        break;
	        case FPOP1: case FPOP2:
            if (FPEN && (fpo.exc == 1)) 
            {
              trap = 1;
              tt   = TT_FPEXC;
            }
          break;
	        case CPOP1: case CPOP2:
    	      if (CPEN && (cpo.exc == 1))
    	      {
    	        trap = 1;
    	        tt   = TT_CPEXC;
    	      }
    	    break;
          default:;
        }
      break;
      case LDST:
        if (r.m.ctrl.cnt == 0)
        {
          switch(op3)
          {
            case LDDF: case STDF: case STDFQ:
	            if (FPEN)
	            {
	              if (nalign_d == 1)
	              {
	                trap = 1;
	                tt   = TT_UNALA;
	                nullify = 1;
                }else if (fpo.exc & r.m.ctrl.pv)
                {
                  trap = 1;
                  tt   = TT_FPEXC;
                  nullify = 1;
                }
	            }
	          break;
	          case LDDC: case STDC: case STDCQ:
	            if (CPEN)
	            {
	              if (nalign_d == 1)
	              {
	                trap = 1;
	                tt   = TT_UNALA;
	                nullify = 1;
    	          }else if ((cpo.exc & r.m.ctrl.pv) == 1) 
    	          {
    	            trap = 1;
    	            tt   = TT_CPEXC;
    	            nullify = 1;
    	          }
	            }
	          break;
	          case LDD: case ISTD: case LDDA: case STDA:
	            if (BITS32(r.m.result,2,0) != 0)
	            {
	              trap = 1;
	              tt   = TT_UNALA;
	              nullify = 1;
	            }
	          break;
 	          case LDF: case LDFSR: case STFSR: case STF:
	            if (FPEN && (r.m.nalign == 1))
	            {
	                trap = 1;
	                tt   = TT_UNALA;
	                nullify = 1;
               }else if (FPEN && ((fpo.exc & r.m.ctrl.pv) == 1))
               {
                  trap = 1;
                  tt   = TT_FPEXC;
                  nullify = 1;
               }    
            break;
 	          case LDC: case LDCSR: case STCSR: case STC:
	            if (CPEN & (r.m.nalign == 1))
	            {
	              trap = 1;
	              tt   = TT_UNALA;
	              nullify = 1;
    	        }else if (CPEN && ((cpo.exc & r.m.ctrl.pv) == 1))
    	        { 
    	          trap = 1;
    	          tt   = TT_CPEXC;
    	          nullify = 1;
    	        }
    	      break;
 	          case LD: case LDA: case ST: case STA: case SWAP: case SWAPA: case CASA:
	            if (BITS32(r.m.result,1,0) != 0)
	            {
	              trap = 1;
	              tt   = TT_UNALA;
	              nullify = 1;
	            }
	          break;
            case LDUH: case LDUHA: case LDSH: case LDSHA: case STH: case STHA:
	            if (BIT32(r.m.result,0) != 0)
	            {
	              trap = 1;
	              tt   = TT_UNALA;
	              nullify = 1;
	            }
	          break;
            default:;
          }
          for (int32 i=1; i<=CFG_NWP; i++)// in 1 to NWP loop
          {
            if ( (((wpr.arr[i-1].load & !BIT32(op3,2)) | (wpr.arr[i-1].store & BIT32(op3,2))) == 1) &&
                 (((wpr.arr[i-1].addr ^ (r.m.result&MSK32(31,2))) & wpr.arr[i-1].mask) == 0) )
            {
              trap = 1;
              tt   = TT_WATCH;
              nullify = 1;
            }
          }
	      }
	    break;
      default:;
    }
  }
  if (!rstn || (r.x.rstate == dsu2)) werr = 0;
  trapout = trap;
  werrout = werr;
}

//****************************************************************************
void iu3::irq_trap(registers &r,//       : in registers;
                     irestart_register &ir,//      : in irestart_register;
                     uint32 irl,//     : in std_logic_vector(3 downto 0);
                     uint32 annul,//   : in std_ulogic;
                     uint32 pv,//      : in std_ulogic;
                     uint32 trap,//    : in std_ulogic;
                     uint32 tt,//      : in std_logic_vector(5 downto 0);
                     uint32 nullify,// : in std_ulogic;
                     uint32 &irqen,//   : out std_ulogic;
                     uint32 &irqen2,//  : out std_ulogic;
                     uint32 &nullify2,// : out std_ulogic;
                     uint32 &trap2,
                     uint32 &ipend,//  : out std_ulogic;
                     uint32 &tt2)//      : out std_logic_vector(5 downto 0)) is
{
  uint32 op;// : std_logic_vector(1 downto 0);
  uint32 op3;// : std_logic_vector(5 downto 0);
  uint32 pend;// : std_ulogic;
  
  nullify2 = nullify;
  trap2    = trap;
  tt2      = tt; 
  op     = (r.m.ctrl.inst>>30)&0x3;//(31 downto 30);
  op3    = (r.m.ctrl.inst>>19)&0x3F;//(24 downto 19);
  irqen  = 1;
  irqen2 = r.m.irqen;

  if ((annul | trap) == 0)
  {
    if ((op == FMT3) && (op3 == WRPSR)) irqen = 0;
  }

  if ((irl == 0xF) || (irl > r.w.s.pil))
    pend = r.m.irqen & r.m.irqen2 & r.w.s.et & !ir.pwd;
  else pend = 0;
  ipend = pend;

  if (!annul & pv & !trap & pend)
  {
    trap2 = 1;
    tt2   = (0x1<<4) | irl;
    if (op == LDST) nullify2 = 1;
  }
}

//****************************************************************************
void iu3::irq_intack(registers &r,// : in registers;
                          uint32 holdn,// : in std_ulogic; 
                          uint32 &intack)//: out std_ulogic) is 
{
  if ( (r.x.rstate==trap)&&(BITS32(r.w.s.tt,7,4)==0x1) ) intack = 1;
  else                                                   intack = 0;
}

//****************************************************************************
// write special registers
void iu3::sp_write( mul32_out_type &mulo,//  : in  mul32_out_type;
                        registers &r,// : registers;
                        watchpoint_registers &wpr,// : watchpoint_registers;
                        special_register_type &s,// : out special_register_type; 
                        watchpoint_registers &vwpr)// : out watchpoint_registers) is
{
  uint32 op;// : std_logic_vector(1 downto 0);
  uint32 op2;// : std_logic_vector(2 downto 0);
  uint32 op3;// : std_logic_vector(5 downto 0);
  uint32 rd;//  : std_logic_vector(4 downto 0);
//  int32 i;//   : integer range 0 to 3;

  op  = BITS32(r.x.ctrl.inst,31,30);
  op2 = BITS32(r.x.ctrl.inst,24,22);
  op3 = BITS32(r.x.ctrl.inst,24,19);
  s   = r.w.s;
  rd  = BITS32(r.x.ctrl.inst,29,25);
  vwpr = wpr;
    
  switch(op)
  {  
    case FMT3:
      switch(op3)
      {  
        case WRY:
          if (rd == 0x00)                           s.y = r.x.result;
          else if (MACEN && (rd == 0x12))           s.asr18 = r.x.result;
          else if (rd == 0x11)
          {
	          if (CFG_BP==2) s.dbp = BIT32(r.x.result,27);
	          s.dwt = BIT32(r.x.result,14);
            if (CFG_SVT==1) s.svt = BIT32(r.x.result,13);
          }else if (BITS32(rd,4,3) == 0x3) // %ASR24 - %ASR31
          {
	          switch (BITS32(rd,2,0))//(2 downto 0) is
	          {
	            case 0://when "000" => 
                vwpr.arr[0].addr = r.x.result&MSK32(31,2);
                vwpr.arr[0].exec = BIT32(r.x.result,0); 
              break;
	            case 1://when "001" => 
                vwpr.arr[0].mask = r.x.result&MSK32(31,2);
                vwpr.arr[0].load = BIT32(r.x.result,1);
                vwpr.arr[0].store = BIT32(r.x.result,0);
              break;
	            case 2://when "010" => 
                vwpr.arr[1].addr = r.x.result&MSK32(31,2);
                vwpr.arr[1].exec = BIT32(r.x.result,0);
              break;
	            case 3://when "011" => 
                vwpr.arr[1].mask = r.x.result&MSK32(31,2);
                vwpr.arr[1].load = BIT32(r.x.result,1);
                vwpr.arr[1].store = BIT32(r.x.result,0);
              break;
	            case 4://when "100" => 
                vwpr.arr[2].addr = r.x.result&MSK32(31,2);
                vwpr.arr[2].exec = BIT32(r.x.result,0); 
              break;
	            case 5://when "101" => 
                vwpr.arr[2].mask = r.x.result&MSK32(31,2);
                vwpr.arr[2].load = BIT32(r.x.result,1);
                vwpr.arr[2].store = BIT32(r.x.result,0);
              break;
	            case 6://"110" => 
                vwpr.arr[3].addr = r.x.result&MSK32(31,2);
                vwpr.arr[3].exec = BIT32(r.x.result,0); 
              break;
	            default:   // "111"
                vwpr.arr[3].mask = r.x.result&MSK32(31,2);
                vwpr.arr[3].load = BIT32(r.x.result,1);
                vwpr.arr[3].store = BIT32(r.x.result,0);
            }
          }
        break;
        case WRPSR:
          s.cwp = BITS32(r.x.result,NWINLOG2-1,0);
          s.icc = BITS32(r.x.result,23,20);
          s.ec  = BIT32(r.x.result,13);
          if (FPEN) s.ef  = BIT32(r.x.result,12);
          s.pil = BITS32(r.x.result,11,8);
          s.s   = BIT32(r.x.result,7);
          s.ps  = BIT32(r.x.result,6);
          s.et  = BIT32(r.x.result,5);
        break;
        case WRWIM:
          s.wim = BITS32(r.x.result,CFG_NWIN-1,0);
        break;
        case WRTBR:
          s.tba = BITS32(r.x.result,31,12);
        break;
        case SAVE:
          if ((!CWPOPT) && (r.w.s.cwp==CWPMIN))           s.cwp = CWPMAX;
          else                                            s.cwp = r.w.s.cwp - 1;
        break;
        case RESTORE:
          if ((!CWPOPT) && (r.w.s.cwp==CWPMAX))           s.cwp = CWPMIN;
          else                                            s.cwp = r.w.s.cwp + 1;
        break;
        case RETT:
          if ((!CWPOPT) && (r.w.s.cwp==CWPMAX))           s.cwp = CWPMIN;
          else                                            s.cwp = r.w.s.cwp + 1;
          s.s  = r.w.s.ps;
          s.et = 1;
        break;
        default:;
      }
    break;
    default:;
  }
  if (r.x.ctrl.wicc == 1) s.icc = r.x.icc;
  if (r.x.ctrl.wy == 1)   s.y = r.x.y;
  if (MACPIPE && (r.x.mac == 1))
  {
    s.asr18 = (word)BITS64(mulo.result,31,0);
	  s.y = (word)BITS64(mulo.result,63,32);
  }
}

//****************************************************************************
uint32 iu3::npc_find (registers &r)// : registers) return std_logic_vector is
{
  uint32 npc;// : std_logic_vector(2 downto 0);
  npc = 0x3;//"011";
  if (r.m.ctrl.pv)      npc = 0x0;//"000";
  else if (r.e.ctrl.pv) npc = 0x1;//"001";
  else if (r.a.ctrl.pv) npc = 0x2;//"010";
  else if (r.d.pv)      npc = 0x3;//"011";
  else if (CFG_V8 != 0) npc = 0x4;//"100";
  return(npc);
}

//****************************************************************************
word iu3::npc_gen(registers &r)// : registers) return word is
{
  uint32 npc;// : std_logic_vector(31 downto 0);
  npc =  r.a.ctrl.pc&MSK32(31,2);
  switch (r.x.npc)
  {
    case 0: npc = r.x.ctrl.pc&MSK32(31,2); break;
    case 1: npc = r.m.ctrl.pc&MSK32(31,2); break;
    case 2: npc = r.e.ctrl.pc&MSK32(31,2); break;
    case 3: npc = r.a.ctrl.pc&MSK32(31,2); break;
    default:
	    if (CFG_V8 != 0) npc = r.d.pc&MSK32(31,2);
  }
  return(npc);
}

//****************************************************************************
void iu3::mul_res(registers &r,// : registers;
                      mul32_out_type &mulo,//  : in  mul32_out_type;
                      div32_out_type &divo,//  : in  div32_out_type;
                      word asr18in,// : word;
                      word &result, 
                      word &y, 
                      word &asr18,// : out word; 
	                    uint32 &icc)// : out std_logic_vector(3 downto 0)) is
{
  uint32 op;//  : std_logic_vector(1 downto 0);
  uint32 op3;// : std_logic_vector(5 downto 0);
  op     = BITS32(r.m.ctrl.inst,31,30);
  op3    = BITS32(r.m.ctrl.inst,24,19);
  result = r.m.result;
  y      = r.m.y;
  icc    = r.m.icc;
  asr18  = asr18in;
  switch(op)
  {
    case FMT3:
      switch(op3)
      {
        case UMUL: case SMUL:
    	    if (MULEN)
    	    {
    	      result = (word)BITS64(mulo.result,31,0);
            y = (word)BITS64(mulo.result,63,32);
          }
        break;
        case UMULCC: case SMULCC:
    	    if (MULEN)
    	    {
            result = (word)BITS64(mulo.result,31,0);
            icc    = mulo.icc;
            y      = (word)BITS64(mulo.result,63,32);
          }
        break;
        case UMAC: case SMAC:
	        if (MACEN && !MACPIPE)
	        {
	          result = (word)BITS64(mulo.result,31,0);
	          asr18  = (word)BITS64(mulo.result,31,0);
            y      = (word)BITS64(mulo.result,63,32);
	        }
	      break;
        case UDIV: case SDIV:
    	    if (DIVEN)result = (word)divo.result;
        break;
        case UDIVCC: case SDIVCC:
    	    if (DIVEN)
    	    {
            result = (word)divo.result;
            icc    = divo.icc;
          }
        break;
        default:;
      }
    break;
    default:;
  }
}

//****************************************************************************
uint32 iu3::powerdwn(registers &r,// : registers;
                          uint32 trap,// : std_ulogic;
                          pwd_register_type &rp)// : pwd_register_type) return std_ulogic is
{
  uint32 op;// : std_logic_vector(1 downto 0);
  uint32 op3;// : std_logic_vector(5 downto 0);
  uint32 rd;//  : std_logic_vector(4 downto 0);
  uint32 pd;//  : std_ulogic;

  op  = BITS32(r.x.ctrl.inst,31,30);
  op3 = BITS32(r.x.ctrl.inst,24,19);
  rd  = BITS32(r.x.ctrl.inst,29,25);
  pd  = 0;
  if (!(r.x.ctrl.annul | trap) & r.x.ctrl.pv)
  {
    if ((op==FMT3) && (op3==WRY) && (rd == 0x13)) pd = 1;
    pd = pd | rp.pwd;
  }
  return(pd);
}

//****************************************************************************
//****************************************************************************
void iu3::Update
(
  SClock clk,//   : in  std_ulogic;
  uint32 rstn,//  : in  std_ulogic;
  uint32 holdn,// : in  std_ulogic;
  icache_in_type &ici,//   : out icache_in_type;
  icache_out_type &ico,//   : in  icache_out_type;
  dcache_in_type &dci,//   : out dcache_in_type;
  dcache_out_type &dco,//   : in  dcache_out_type;
  iregfile_in_type &rfi,//   : out iregfile_in_type;
  iregfile_out_type &rfo,//   : in  iregfile_out_type;
  l3_irq_in_type &irqi,//  : in  l3_irq_in_type;
  l3_irq_out_type &irqo,//  : out l3_irq_out_type;
  l3_debug_in_type &dbgi,//  : in  l3_debug_in_type;
  l3_debug_out_type &dbgo,//  : out l3_debug_out_type;
  mul32_in_type &muli,//  : out mul32_in_type;
  mul32_out_type &mulo,//  : in  mul32_out_type;
  div32_in_type &divi,//  : out div32_in_type;
  div32_out_type &divo,//  : in  div32_out_type;
  fpc_out_type &fpo,//   : in  fpc_out_type;
  fpc_in_type &fpi,//   : out fpc_in_type;
  fpc_out_type &cpo,//   : in  fpc_out_type;
  fpc_in_type &cpi,//   : out fpc_in_type;
  tracebuf_out_type &tbo,//   : in  tracebuf_out_type;
  tracebuf_in_type &tbi,//   : out tracebuf_in_type;
  SClock sclk//   : in  std_ulogic
)
{

  if(CFG_BP==0)      BPRED = 0;
  else if(CFG_BP==1) BPRED = 1;
  else               BPRED = !rbR.Q.w.s.dbp;

  v = r = rbR.Q;
  vwpr = wpr.Q;
  vdsu = dsur.Q;
  vp = rp.Q;
  xc_fpexack = 0;
  sidle = 0;
  fpcdbgwr = 0; 
  vir = ir.Q; 
  xc_rstn = rstn;
    
  //-----------------------------------------------------------------------
  //-- WRITE STAGE
  //-----------------------------------------------------------------------

  //--    wr_rf1_data := rfo.data1; wr_rf2_data := rfo.data2;
  //--    if irfwt = 0 then
  //--      if r.w.wreg = '1' then
  //--	if r.a.rfa1 = r.w.wa then wr_rf1_data := r.w.result; end if;
  //--	if r.a.rfa2 = r.w.wa then wr_rf2_data := r.w.result; end if;
  //--      end if;
  //--    end if;


  //-----------------------------------------------------------------------
  //-- EXCEPTION STAGE
  //-----------------------------------------------------------------------

  xc_exception = 0;
  xc_halt = 0;
  icnt = 0;
  fcnt = 0;
  xc_waddr = 0;
  xc_waddr = r.x.ctrl.rd;
  xc_trap = r.x.mexc || r.x.ctrl.trap;
  v.x.nerror = rp.Q.error;
    
  if (r.x.mexc==1)                xc_vectt = TT_DAEX;
  else if (r.x.ctrl.tt==TT_TICC)  xc_vectt = (1<<7) | BITS32(r.x.result,6,0);
  else                            xc_vectt = r.x.ctrl.tt;
  
  if (r.w.s.svt==0) xc_trap_address = (((r.w.s.tba<<8)|xc_vectt)<<4); 
  else              xc_trap_address = ((r.w.s.tba<<8)<<4); 

  xc_wreg = 0;
  v.x.annul_all = 0; 
  
  if (r.x.ctrl.ld == 1)
  {
    if (CFG_LDDEL==2) xc_result = ld_align(r.x.data, r.x.set, r.x.dci.size, r.x.laddr, r.x.dci.Signed);
    else              xc_result = r.x.data.arr[0];
  }else if (MACEN && MACPIPE && (r.x.mac == 1))
  {
    xc_result = (word)BITS64(mulo.result,31,0);
  }else xc_result = r.x.result;
  
  xc_df_result = xc_result;
#if(CFG_DSU==1)
    dbgm = dbgexc(r, dbgi, xc_trap, xc_vectt);
    if ((dbgi.dsuen & dbgi.dbreak)==0) v.x.debug = 0;
#else
    dbgm = 0;
    v.x.debug = 0;
#endif
#if (CFG_PWD!=0)
  pwrd = powerdwn(r, xc_trap, rp.Q);
#else
  pwrd = 0;
#endif
    
  switch (r.x.rstate)
  {
    case run:
      if (!r.x.ctrl.annul & r.x.ctrl.pv & !r.x.debug)  icnt = holdn;

      if (dbgm == 1)
      {
        v.x.annul_all = 1;
        vir.addr      = r.x.ctrl.pc;
        v.x.rstate    = dsu1;
        v.x.debug     = 1;
        v.x.npc       = npc_find(r);
        vdsu.tt       = xc_vectt;
        vdsu.err      = dbgerr(r, dbgi, xc_vectt);
      }else if ((pwrd == 1) && (ir.Q.pwd == 0))
      {
        v.x.annul_all = 1;
        vir.addr      = r.x.ctrl.pc;
        v.x.rstate    = dsu1;
        v.x.npc       = npc_find(r);
        vp.pwd        = 1;
      }else if ((r.x.ctrl.annul | xc_trap) == 0)
      {
        xc_wreg = r.x.ctrl.wreg;
        sp_write(mulo, r, wpr.Q, v.w.s, vwpr);        
        vir.pwd = 0;
        if (r.x.ctrl.pv & !r.x.debug)
        {
	        icnt = holdn;
	        if( (BITS32(r.x.ctrl.inst,31,30) == FMT3) &&
		          ((BITS32(r.x.ctrl.inst,24,19) == FPOP1) || 
		          (BITS32(r.x.ctrl.inst,24,19) == FPOP2)) )
	          fcnt = holdn;
        }
      }else if (!r.x.ctrl.annul & xc_trap)
      {
        xc_exception  = 1;
        xc_result     = r.x.ctrl.pc&MSK32(31,2);
        xc_wreg       = 1; 
        v.w.s.tt      = xc_vectt;
        v.w.s.ps      = r.w.s.s;
        v.w.s.s       = 1;
        v.x.annul_all = 1;
        v.x.rstate    = trap;
        xc_waddr      = 0;
        xc_waddr      =  (BITS32(r.w.s.cwp,NWINLOG2-1,0)<<4)|0x1;
	      v.x.npc       = npc_find(r);
        fpexack(r, xc_fpexack);
        if (r.w.s.et == 0)
        {
          //--	  v.x.rstate := dsu1; xc_wreg := '0'; vp.error := '1';
	        xc_wreg = 0;
        }
      }
    break;
    case trap:
      xc_result = npc_gen(r);
      xc_wreg   = 1;
      xc_waddr  = 0;
      xc_waddr  =  (BITS32(r.w.s.cwp,NWINLOG2-1,0)<<4) | 0x2;
      if (r.w.s.et)
      {
	      v.w.s.et    = 0;
	      v.x.rstate  = run;
        if (!CWPOPT && (r.w.s.cwp == CWPMIN)) v.w.s.cwp = CWPMAX;
        else                                  v.w.s.cwp = r.w.s.cwp - 1 ;
      }else
      {
	      v.x.rstate = dsu1; 
	      xc_wreg    = 0;
	      vp.error   = 1;
      }
    break;
    case dsu1:
      xc_exception    = 1;
      v.x.annul_all   = 1;
      xc_trap_address = r.f.pc;
      if (DBGUNIT || PWRD2 || ((CFG_NCPU-1) != 0))
      {
        xc_trap_address = ir.Q.addr;
        vir.addr = npc_gen(r)&MSK32(31,CFG_PCLOW);
        v.x.rstate = dsu2;
      }
      if (DBGUNIT) v.x.debug = r.x.debug;
    break;
    case dsu2:
      xc_exception  = 1;
      v.x.annul_all = 1;
      xc_trap_address = r.f.pc;
      if (DBGUNIT || PWRD2 || ((CFG_NCPU-1) != 0))
      {
	      sidle = (rp.Q.pwd | rp.Q.error) & ico.idle & dco.idle & !r.x.debug;
        if(DBGUNIT)
        {
	        if (dbgi.reset)
	        {
            if ((CFG_NCPU-1)) vp.pwd = !irqi.run;
            else              vp.pwd = 0;
            vp.error = 0;
	        }
          if (dbgi.dsuen & dbgi.dbreak) v.x.debug = 1;
          diagwr(r, dsur.Q, ir.Q, dbgi, wpr.Q, v.w.s, vwpr, vdsu.asi, xc_trap_address,
                 vir.addr, vdsu.tbufcnt, xc_wreg, xc_waddr, xc_result, fpcdbgwr);
	        xc_halt = dbgi.halt;
        }
	      if (r.x.ipend) vp.pwd = 0;
        if ((rp.Q.error | rp.Q.pwd | r.x.debug | xc_halt) == 0)
        {
	        v.x.rstate    = run;
	        v.x.annul_all = 0; 
	        vp.error      = 0;
          xc_trap_address = ir.Q.addr;
          v.x.debug     = 0;
          vir.pwd       = 1;
        }
        if ((CFG_NCPU-1) && (irqi.rst == 1))
        {
	        vp.pwd   = 0;
	        vp.error = 0;
	      }
      }
    default:;
  }

  irq_intack(r, holdn, v.x.intack);          
  itrace(r, dco, dsur.Q, vdsu, xc_result, xc_exception, dbgi, rp.Q.error, xc_trap, tbufcntx, tbufi);
  vdsu.tbufcnt = tbufcntx;

  v.w.except = xc_exception;
  v.w.result = xc_result;
  if (r.x.rstate == dsu2) v.w.except = 0;
  v.w.wa   = xc_waddr;
  v.w.wreg = xc_wreg & holdn;

  rfi.wdata   = xc_result;
  rfi.waddr   = xc_waddr;
  rfi.wren    = (xc_wreg & holdn) & !dco.scanen;
  irqo.intack = r.x.intack & holdn;
  irqo.irl    = BITS32(r.w.s.tt,3,0);
  irqo.pwd    = rp.Q.pwd;
  irqo.fpen   = r.w.s.ef;
  irqo.idle   = 0;
  dbgo.halt   = xc_halt;
  dbgo.pwd    = rp.Q.pwd;
  dbgo.idle   = sidle;
  dbgo.icnt   = icnt;
  dbgo.fcnt   = fcnt;
  dbgo.optype = (BITS32(r.x.ctrl.inst,31,30)<<4) | BITS32(r.x.ctrl.inst,24,21);
  dci.intack  = r.x.intack & holdn;
    
  if (xc_rstn == 0)
  {
    v.w.except = 0; 
    v.w.s.et  = 0; 
    v.w.s.svt = 0; 
    v.w.s.dwt = 0;
    v.w.s.ef  = 0; // -- needed for AX
    if (need_extra_sync_reset[fabtech])
    {
      v.w.s.cwp = 0;
      v.w.s.icc = 0;
    }
    v.w.s.dbp     = 0;
    v.x.annul_all = 1;
    v.x.rstate    = run;
    vir.pwd       = 0; 
    vp.pwd        = 0; 
    v.x.debug     = 0; 
    //--vp.error = 0;
    v.x.nerror    = 0;
    if (CFG_SVT)
      v.w.s.tt = 0;
    if (DBGUNIT)
    {
      if (dbgi.dsuen & dbgi.dbreak)
      {
        v.x.rstate = dsu1;
        v.x.debug  = 1;
      }
    }
    if ((CFG_NCPU-1) && (irqi.run == 0) && (rstn == 0))
    {
      v.x.rstate = dsu1; 
      vp.pwd     = 1;
    }
  }
  
  if (!FPEN) v.w.s.ef = 0;

  //-----------------------------------------------------------------------
  //-- MEMORY STAGE
  //-----------------------------------------------------------------------

  v.x.ctrl      = r.m.ctrl; 
  v.x.dci       = r.m.dci;
  v.x.ctrl.rett = r.m.ctrl.rett & !r.m.ctrl.annul;
  v.x.mac       = r.m.mac; 
  v.x.laddr     = BITS32(r.m.result,1,0);
  v.x.ctrl.annul = r.m.ctrl.annul | v.x.annul_all; 
  
  if (CASAEN && (r.m.casa == 1) && (r.m.ctrl.cnt == 0))
  {
    v.x.ctrl.inst &= ~MSK32(4,0);
    v.x.ctrl.inst |= BITS32(r.a.ctrl.inst,4,0); //-- restore rs2 for trace log
  }

  mul_res(r, mulo, divo, v.w.s.asr18, v.x.result, v.x.y, me_asr18, me_icc);
  mem_trap(rstn, dco, fpo, cpo, r, wpr.Q, v.x.ctrl.annul, holdn, v.x.ctrl.trap, me_iflush,
            me_nullify, v.m.werr, v.x.ctrl.tt);
  me_newtt = v.x.ctrl.tt;

  irq_trap( r, ir.Q, irqi.irl, v.x.ctrl.annul, v.x.ctrl.pv, v.x.ctrl.trap, me_newtt, me_nullify,
            v.m.irqen, v.m.irqen2, me_nullify2, v.x.ctrl.trap,
            v.x.ipend, v.x.ctrl.tt);   
    
  if (r.m.ctrl.ld | !dco.mds)
  {
    for (int32 i=0; i<=CFG_DSETS-1; i++)
      v.x.data.arr[i] = dco.data.arr[i];
    v.x.set = dco.set;
    if (dco.mds == 0)
    {
      me_size   = r.x.dci.size; 
      me_laddr  = r.x.laddr; 
      me_signed = r.x.dci.Signed;
    }else
    {
      me_size   = v.x.dci.size; 
      me_laddr  = v.x.laddr; 
      me_signed = v.x.dci.Signed;
    }
    if (CFG_LDDEL != 2)
      v.x.data.arr[0] = ld_align(v.x.data, v.x.set, me_size, me_laddr, me_signed);
  }
  v.x.mexc = dco.mexc;

  v.x.icc = me_icc;
  v.x.ctrl.wicc = r.m.ctrl.wicc & !v.x.annul_all;
    
  if (MACEN && ((v.x.ctrl.annul | v.x.ctrl.trap)==0))
    v.w.s.asr18 = me_asr18;

  if (r.x.rstate == dsu2)
  {
    me_nullify2 = 0;
    v.x.set     = dco.set;
  }
  
  dci.maddress = r.m.result;
  dci.enaddr   = r.m.dci.enaddr;
  dci.asi      = r.m.dci.asi;
  dci.size     = r.m.dci.size;
  dci.lock     = r.m.dci.lock & !r.m.ctrl.annul;
  dci.read     = r.m.dci.read;
  dci.write    = r.m.dci.write;
  dci.flush    = me_iflush;
  dci.flushl   = 0;
  dci.dsuen    = r.m.dci.dsuen;
  dci.msu      = r.m.su;
  dci.esu      = r.e.su;
  dbgo.ipend   = v.x.ipend;


  //-----------------------------------------------------------------------
  //-- EXECUTE STAGE
  //-----------------------------------------------------------------------

  v.m.ctrl      = r.e.ctrl;
  ex_op1        = r.e.op1; 
  ex_op2        = r.e.op2;
  v.m.ctrl.rett = r.e.ctrl.rett & !r.e.ctrl.annul;
  v.m.ctrl.wreg = r.e.ctrl.wreg & !v.x.annul_all;    
  ex_ymsb       = r.e.ymsb;
  mul_op2       = ex_op2;
  ex_shcnt      = r.e.shcnt;
  v.e.cwp       = r.a.cwp;
  ex_sari       = r.e.sari;
  v.m.su        = r.e.su;
  if (MULTYPE == 3) v.m.mul = r.e.mul;
  else              v.m.mul = 0;

  if (CFG_LDDEL == 1)
  {
    if (r.e.ldbp1 == 1)
    {
      ex_op1  = r.x.data.arr[0]; 
      ex_sari = BIT32(r.x.data.arr[0],1) & BIT32(r.e.ctrl.inst,19) & BIT32(r.e.ctrl.inst,20);
    }
    if (r.e.ldbp2 == 1)
    {
      ex_op2    = r.x.data.arr[0];
      ex_ymsb   = BIT32(r.x.data.arr[0],0); 
      mul_op2   = ex_op2;
      ex_shcnt  = BITS32(r.x.data.arr[0],4,0);
      if (r.e.invop2 == 1)
      {
        ex_op2    = !ex_op2;
        ex_shcnt  = !ex_shcnt;
      }
    }
  }

  ex_add_res = ((uint64(ex_op1)<<1)|0x1) + ((uint64(ex_op2)<<1)|r.e.alucin);

  if (BITS32(ex_add_res,2,1)==0) v.m.nalign = 0;
  else                           v.m.nalign = 1;

  dcache_gen(r, v, ex_dci, ex_link_pc, ex_jump, ex_force_a2, ex_load, v.m.casa);
  ex_jump_address = ((pctype)BITS64(ex_add_res,32,CFG_PCLOW+1)) << CFG_PCLOW;
  
  logic_op(r, mulo, ex_op1, ex_op2, v.x.y, ex_ymsb, ex_logic_res, v.m.y);
  ex_shift_res    = shift(r, ex_op1, ex_shcnt, ex_sari);
  
  misc_op(r, mulo, wpr.Q, ex_op1, ex_op2, xc_df_result, v.x.y, ex_misc_res, ex_edata);
    ex_add_res   |= (ex_force_a2<<3);    
  
  alu_select(r, ex_add_res, ex_op1, ex_op2, ex_shift_res, ex_logic_res,
             ex_misc_res, ex_result, me_icc, v.m.icc, v.m.divz, v.m.casaz);    
  dbg_cache(holdn, dbgi, r, dsur.Q, ex_result, ex_dci, ex_result2, v.m.dci);
  fpstdata(r, ex_edata, ex_result2, fpo.data, ex_edata2, v.m.result);
  cwp_ex(r, v.m.wcwp);    

  if (CASAEN && (r.e.ctrl.cnt == 0x2) && ((r.m.casa & !v.m.casaz)==1))
    me_nullify2 = 1;

  dci.nullify  = me_nullify2;
  
  v.m.ctrl.annul  = v.m.ctrl.annul | v.x.annul_all;
  v.m.ctrl.wicc   = r.e.ctrl.wicc & !v.x.annul_all; 
  v.m.mac         = r.e.mac;
  if (DBGUNIT && (r.x.rstate == dsu2)) v.m.ctrl.ld = 1;
  dci.eenaddr     = v.m.dci.enaddr;
  dci.eaddress    = (uint32)BITS64(ex_add_res,32,1);
  dci.edata       = ex_edata2;
  bp_miss_ex(r, me_icc, ex_bpmiss, ra_bpannul);

  //-----------------------------------------------------------------------
  //-- REGFILE STAGE
  //-----------------------------------------------------------------------

  v.e.ctrl        = r.a.ctrl; 
  v.e.jmpl        = r.a.jmpl & !r.a.ctrl.trap;
  v.e.ctrl.annul  = r.a.ctrl.annul | ra_bpannul | v.x.annul_all;
  v.e.ctrl.rett   = r.a.ctrl.rett & !r.a.ctrl.annul & !r.a.ctrl.trap;
  v.e.ctrl.wreg   = r.a.ctrl.wreg & !(ra_bpannul | v.x.annul_all);    
  v.e.su          = r.a.su; 
  v.e.et          = r.a.et;
  v.e.ctrl.wicc   = r.a.ctrl.wicc & !(ra_bpannul | v.x.annul_all);
  
  exception_detect(r, wpr.Q, dbgi, v.e.ctrl.trap, v.e.ctrl.tt);
  op_mux(r, rfo.data1, v.m.result, v.x.result, xc_df_result, 0, 
         r.a.rsel1, v.e.ldbp1, ra_op1, 0);
  op_mux(r, rfo.data2,  v.m.result, v.x.result, xc_df_result, r.a.imm, 
         r.a.rsel2, ex_ldbp2, ra_op2, 1);
  alu_op(r, ra_op1, ra_op2, v.m.icc, BIT32(v.m.y,0), ex_ldbp2, v.e.op1, v.e.op2,
         v.e.aluop, v.e.alusel, v.e.aluadd, v.e.shcnt, v.e.sari, v.e.shleft,
         v.e.ymsb, v.e.mul, ra_div, v.e.mulstep, v.e.mac, v.e.ldbp2, v.e.invop2);
  cin_gen(r, BIT32(v.m.icc,0), v.e.alucin);
  bp_miss_ra(r, ra_bpmiss, de_bpannul);
  v.e.bp = r.a.bp & !ra_bpmiss;

  //-----------------------------------------------------------------------
  //-- DECODE STAGE
  //-----------------------------------------------------------------------

  if (CFG_ISETS > 1) de_inst = r.d.inst.arr[r.d.set];
  else               de_inst = r.d.inst.arr[0];

  de_icc  = r.m.icc;
  v.a.cwp = r.d.cwp;
  su_et_select(r, v.w.s.ps, v.w.s.s, v.w.s.et, v.a.su, v.a.et);
  wicc_y_gen(de_inst, r, mulo, divo, v.a.ctrl.wicc, v.a.ctrl.wy);
  cwp_ctrl(r, v.w.s.wim, de_inst, de_cwp, v.a.wovf, v.a.wunf, de_wcwp);
  if ( CASAEN && (BITS32(de_inst,31,30)==LDST) && (BITS32(de_inst,24,19)==CASA) )
  {
    switch(r.d.cnt)
    {
      case 0:
      case 1:
        de_inst &= ~MSK32(4,0); //-- rs2=0
      break;
      default:;
    }
  }

  rs1_gen(r, de_inst, v.a.rs1, de_rs1mod); 
  de_rs2    = BITS32(de_inst,4,0);
  de_raddr1 = 0;
  de_raddr2 = 0;
  
  if (RS1OPT)
  {
    if (de_rs1mod==1)
    {
      uint32 tmp = (BITS32(de_inst,29,26)<<1) | BIT32(v.a.rs1,0);
      regaddr(r.d.cwp, tmp, de_raddr1);
    }else
    {
      uint32 tmp = (BITS32(de_inst,18,15)<<1) | BIT32(v.a.rs1,0);
      regaddr(r.d.cwp, tmp, de_raddr1);
    }
  }else
    regaddr(r.d.cwp, v.a.rs1, de_raddr1);
  
  regaddr(r.d.cwp, de_rs2, de_raddr2);
  v.a.rfa1 = de_raddr1; 
  v.a.rfa2 = de_raddr2; 

  rd_gen(r, mulo,divo, de_inst, v.a.ctrl.wreg, v.a.ctrl.ld, de_rd);
  regaddr(de_cwp, de_rd, v.a.ctrl.rd);
  
  fpbranch(de_inst, fpo.cc, de_fbranch);
  fpbranch(de_inst, cpo.cc, de_cbranch);
  v.a.imm = imm_data(r, de_inst);
  
  lock_gen(r, BPRED, de_rs2, de_rd, v.a.rfa1, v.a.rfa2, v.a.ctrl.rd, de_inst, 
           fpo.ldlock, v.e.mul, ra_div, de_wcwp, v.a.ldcheck1, v.a.ldcheck2, de_ldlock, 
           v.a.ldchkra, v.a.ldchkex, v.a.bp, v.a.nobp, de_fins_hold);
  ic_ctrl(rstn,BPRED, r, mulo, divo, de_inst, v.x.annul_all, de_ldlock, branch_true(de_icc, de_inst), 
          de_fbranch, de_cbranch, fpo.ccv, cpo.ccv, v.d.cnt, v.d.pc, de_branch,
          v.a.ctrl.annul, v.d.annul, v.a.jmpl, de_inull, v.d.pv, v.a.ctrl.pv,
          de_hold_pc, v.a.ticc, v.a.ctrl.rett, v.a.mulstart, v.a.divstart, 
          ra_bpmiss, ex_bpmiss);

  v.a.bp    = v.a.bp & !v.a.ctrl.annul;
  v.a.nobp  = v.a.nobp & !v.a.ctrl.annul;
  cwp_gen(rstn, r, v, v.a.ctrl.annul, de_wcwp, de_cwp, v.d.cwp);    
  
  v.d.inull = ra_inull_gen(r, v);

  op_find(r, v.a.ldchkra, v.a.ldchkex, v.a.rs1, v.a.rfa1, 
          false, v.a.rfe1, v.a.rsel1, v.a.ldcheck1);
  op_find(r, v.a.ldchkra, v.a.ldchkex, de_rs2, v.a.rfa2, 
          imm_select(de_inst), v.a.rfe2, v.a.rsel2, v.a.ldcheck2);

  de_branch_address = branch_address(de_inst, r.d.pc);

  v.a.ctrl.wicc  = v.a.ctrl.wicc & !v.a.ctrl.annul;
  v.a.ctrl.wreg  = v.a.ctrl.wreg & !v.a.ctrl.annul;
  v.a.ctrl.rett  = v.a.ctrl.rett & !v.a.ctrl.annul;
  v.a.ctrl.wy    = v.a.ctrl.wy & !v.a.ctrl.annul;

  v.a.ctrl.trap = r.d.mexc;
  v.a.ctrl.tt   = 0;
  v.a.ctrl.inst = de_inst;
  v.a.ctrl.pc   = r.d.pc;
  v.a.ctrl.cnt  = r.d.cnt;
  v.a.step      = r.d.step;

  if (holdn == 0)
  {
    de_raddr1 &= ~MSK32(RFBITS-1,0);
    de_raddr1 |= BITS32(r.a.rfa1,RFBITS-1,0);
    de_raddr2 &= ~MSK32(RFBITS-1,0);
    de_raddr2 |= BITS32(r.a.rfa2,RFBITS-1,0);
    de_ren1 = r.a.rfe1;
    de_ren2 = r.a.rfe2;
  }else
  {
    de_ren1 = v.a.rfe1;
    de_ren2 = v.a.rfe2;
  }

  if (DBGUNIT)
  {
    if (((dbgi.denable & !dbgi.dwrite) == 1) && (r.x.rstate == dsu2))
    {
      de_raddr1 = BITS32(dbgi.daddr,RFBITS+1,2);
      de_ren1   = 1;
    }
    v.d.step = dbgi.step & !r.d.annul;      
  }

  rfi.raddr1  = de_raddr1; 
  rfi.raddr2  = de_raddr2;
  rfi.ren1    = de_ren1 & !dco.scanen;
  rfi.ren2    = de_ren2 & !dco.scanen;
  rfi.diag    = dco.testen<<3;
  ici.inull   = de_inull;
  ici.flush   = me_iflush;
  v.d.divrdy  = divo.nready;
  ici.flushl  = 0;
  ici.pnull   = 0;
  ici.fline   = 0;
  dbgo.bpmiss = bpmiss & holdn;

  if (xc_rstn == 0)
  {
    v.d.cnt = 0;
    if (need_extra_sync_reset[fabtech] != 0)
      v.d.cwp = 0;
  }


  //-----------------------------------------------------------------------
  //-- FETCH STAGE
  //-----------------------------------------------------------------------

  bpmiss  = ex_bpmiss | ra_bpmiss;
  npc     = r.f.pc; 
  fe_pc   = r.f.pc;
  if (ra_bpmiss == 1) fe_pc = r.d.pc;
  if (ex_bpmiss == 1) fe_pc = r.a.ctrl.pc;
  fe_npc = fe_pc + (1<<CFG_PCLOW);    //-- Address incrementer

#if 1
  if(iClkCnt>=1241)
    bool stop = true;
#endif
  if (xc_rstn == 0)
  {
    v.f.pc      = 0;
    v.f.branch  = 0;
    if (DYNRST) v.f.pc = (v.f.pc&MSK32(11,0)) | (irqi.rstvec);//<<12);
    else        v.f.pc = (v.f.pc&MSK32(11,0)) | (CFG_RSTADDR<<12);
  }else if (xc_exception == 1) 	                    //-- exception
  {
    v.f.branch = 1;
    v.f.pc     = xc_trap_address;
    npc        = v.f.pc;
  }else if (de_hold_pc == 1)
  {
    v.f.pc     = r.f.pc;
    v.f.branch = r.f.branch;
    if (bpmiss == 1)
    {
      v.f.pc      = fe_npc;
      v.f.branch  = 1;
      npc         = v.f.pc;
    }else if (ex_jump == 1)
    {
      v.f.pc     = ex_jump_address; 
      v.f.branch = 1;
      npc        = v.f.pc;
    }
  }else if (ex_jump & !bpmiss)
  {
    v.f.pc     = ex_jump_address;
    v.f.branch = 1;
    npc        = v.f.pc;
  }else if (de_branch & !bpmiss)
  {
    v.f.pc     = branch_address(de_inst, r.d.pc);
    v.f.branch = 1;
    npc        = v.f.pc;
  }else
  {
    v.f.branch = bpmiss;
    v.f.pc     = fe_npc; 
    npc        = v.f.pc;
  }

  ici.dpc     = r.d.pc & MSK32(31,2);
  ici.fpc     = r.f.pc & MSK32(31,2);
  ici.rpc     = npc & MSK32(31,2);
  ici.fbranch = r.f.branch;
  ici.rbranch = v.f.branch;
  ici.su      = v.a.su;
  ici.fline   = 0;
  ici.flushl  = 0;

  if ((ico.mds & de_hold_pc) == 0)
  {
    for (int32 i=0; i<=CFG_ISETS-1; i++)
      v.d.inst.arr[i] = ico.data.arr[i];	//-- latch instruction
    v.d.set  = ico.set;                   //-- latch instruction
    v.d.mexc = ico.mexc;				          //-- latch instruction
  }

  //-----------------------------------------------------------------------
  //-----------------------------------------------------------------------

  if (DBGUNIT)                            //-- DSU diagnostic read    
  {
    diagread(dbgi, r, rfo, fpo, dsur.Q, ir.Q, wpr.Q, dco, tbo, diagdata);
    diagrdy(dbgi.denable, dsur.Q, r.m.dci, ico, vdsu.crdy);
  }

  //-----------------------------------------------------------------------
  //-- OUTPUTS
  //-----------------------------------------------------------------------

  rin         = v; 
  wprin       = vwpr;
  dsuin       = vdsu;
  irin        = vir;
  muli.start  = r.a.mulstart & !r.a.ctrl.annul;
  muli.Signed = BIT32(r.e.ctrl.inst,19);
  muli.op1    = (((uint64)(BIT32(ex_op1,31) & BIT32(r.e.ctrl.inst,19))) <<32) | (uint64)ex_op1;
  muli.op2    = (((uint64)(BIT32(mul_op2,31) & BIT32(r.e.ctrl.inst,19))) <<32) | (uint64)mul_op2;
  muli.mac    = BIT32(r.e.ctrl.inst,24);
  if (MACPIPE) muli.acc = (((uint64)BITS32(r.w.s.y,7,0))<<32);
  else         muli.acc = (((uint64)BITS32(r.x.y,7,0))<<32);
  muli.acc    |= ((uint64)r.w.s.asr18);
  muli.flush  = r.x.annul_all;
  divi.start  = r.a.divstart & !r.a.ctrl.annul;
  divi.Signed = BIT32(r.e.ctrl.inst,19);
  divi.flush  = r.x.annul_all;
  divi.op1    = (((uint64)(BIT32(ex_op1,31) & BIT32(r.e.ctrl.inst,19)))<<32) | (uint64)ex_op1;
  divi.op2    = (((uint64)(BIT32(ex_op2,31) & BIT32(r.e.ctrl.inst,19)))<<32) | (uint64)ex_op2;
  if ((r.a.divstart & !r.a.ctrl.annul) == 1) dsign = BIT32(r.a.ctrl.inst,19);
  else                                       dsign = BIT32(r.e.ctrl.inst,19);
  divi.y = (((uint64)(BIT32(r.m.y,31) & dsign))<<32) | (uint64)r.m.y;
  rpin   = vp;

  if (DBGUNIT)
  {
    dbgo.dsu     = 1;
    dbgo.dsumode = r.x.debug;
    dbgo.crdy    = BIT32(dsur.Q.crdy,2);
    dbgo.data    = diagdata;
    if (TRACEBUF) 
      tbi = tbufi;
    else
    {
      tbi.addr   = 0;
      tbi.data[1]=tbi.data[0] = 0;
      tbi.enable = 0;
      tbi.write  = 0;
      tbi.diag   = 0;
    }
  }else
  {
    dbgo.dsu  = 0;
    dbgo.data = 0;
    dbgo.crdy = 0;
    dbgo.dsumode = 0;
    tbi.addr     = 0;
    tbi.data[1]=tbi.data[0] = 0;
    tbi.enable   = 0;
    tbi.write    = 0;
    tbi.diag     = 0;
  }
  dbgo.error = dummy & !r.x.nerror;
  dbgo.wbhold = 0; //--dco.wbhold;
  dbgo.su = r.w.s.s;
  dbgo.istat.chold = 0;
  dbgo.istat.cmiss = 0;
  dbgo.istat.mhold = 0;
  dbgo.istat.tmiss = 0;
  dbgo.dstat.chold = 0;
  dbgo.dstat.cmiss = 0;
  dbgo.dstat.mhold = 0;
  dbgo.dstat.tmiss = 0;


//-- pragma translate_off
  if (FPEN)
  {
//-- pragma translate_on
    vfpi.flush   = v.x.annul_all; 
    vfpi.exack   = xc_fpexack; 
    vfpi.a_rs1   = r.a.rs1; 
    vfpi.d.inst  = de_inst;
    vfpi.d.cnt   = r.d.cnt; 
    vfpi.d.annul = v.x.annul_all | de_bpannul | r.d.annul | de_fins_hold;
    vfpi.d.trap  = r.d.mexc;
    vfpi.d.pc    = r.d.pc & MSK32(31,CFG_PCLOW); 
    vfpi.d.pv    = r.d.pv;
    vfpi.a.pc     = r.a.ctrl.pc & MSK32(31,CFG_PCLOW);
    vfpi.a.inst   = r.a.ctrl.inst; 
    vfpi.a.cnt    = r.a.ctrl.cnt;
    vfpi.a.trap   = r.a.ctrl.trap;
    vfpi.a.annul  = r.a.ctrl.annul | (ex_bpmiss & BIT32(r.e.ctrl.inst,29));
    vfpi.a.pv     = r.a.ctrl.pv;
    vfpi.e.pc     = r.e.ctrl.pc & MSK32(31,CFG_PCLOW);
    vfpi.e.inst   = r.e.ctrl.inst;
    vfpi.e.cnt    = r.e.ctrl.cnt;
    vfpi.e.trap   = r.e.ctrl.trap;
    vfpi.e.annul  = r.e.ctrl.annul;
    vfpi.e.pv     = r.e.ctrl.pv;
    vfpi.m.pc     = r.m.ctrl.pc & MSK32(31,CFG_PCLOW);
    vfpi.m.inst   = r.m.ctrl.inst;
    vfpi.m.cnt    = r.m.ctrl.cnt;
    vfpi.m.trap   = r.m.ctrl.trap;
    vfpi.m.annul  = r.m.ctrl.annul;
    vfpi.m.pv     = r.m.ctrl.pv;
    vfpi.x.pc     = r.x.ctrl.pc & MSK32(31,CFG_PCLOW);
    vfpi.x.inst   = r.x.ctrl.inst;
    vfpi.x.cnt    = r.x.ctrl.cnt;
    vfpi.x.trap   = xc_trap;
    vfpi.x.annul  = r.x.ctrl.annul;
    vfpi.x.pv     = r.x.ctrl.pv;
    vfpi.lddata   = xc_df_result;    //--xc_result;
    if (r.x.rstate == dsu2) vfpi.dbg.enable = dbgi.denable;
    else                    vfpi.dbg.enable = 0;      
    vfpi.dbg.write = fpcdbgwr;
    vfpi.dbg.fsr   = BIT32(dbgi.daddr,22); //-- IU reg access
    vfpi.dbg.addr  = BITS32(dbgi.daddr,6,2);
    vfpi.dbg.data  = dbgi.ddata;
    fpi = vfpi;
    cpi = vfpi;  //-- dummy, just to kill some warnings ...
//-- pragma translate_off
  }
//-- pragma translate_on
  rp.CLK = sclk;
  rp.D = rpin; 
  if (rstn == 0) rp.D.error = 0;

  rbR.CLK = clk;
  if (holdn == 1)
  {
    rbR.D = rin;
  }else
  {
    rbR.D.d.divrdy = rin.d.divrdy;
    rbR.D.x.ipend  = rin.x.ipend;
	  rbR.D.m.werr   = rin.m.werr;
	  if ((holdn | ico.mds) == 0)
    {
      rbR.D.d.inst = rin.d.inst;
      rbR.D.d.mexc = rin.d.mexc; 
      rbR.D.d.set  = rin.d.set;
    }
	  if ((holdn | dco.mds) == 0)
    {
      rbR.D.x.data = rin.x.data;
      rbR.D.x.mexc = rin.x.mexc; 
      rbR.D.x.set  = rin.x.set;
    }
  }
  if (!rstn)
  {
    rbR.D.w.s.cwp = 0;
    rbR.D.w.s.tba = 0;
    rbR.D.w.s.y   = 0;
    rbR.D.w.s.icc = 0;
    rbR.D.w.s.pil = 0;
    rbR.D.w.s.wim = 0;
    rbR.D.w.s.asr18 = 0;
    rbR.D.x.y     = 0;
    rbR.D.x.icc   = 0;

    rbR.D.w.s.s  = 1;
    rbR.D.w.s.ps = 1; 
    if (need_extra_sync_reset[fabtech] != 0)
    {
      memset(rbR.D.d.inst.arr, 0, CFG_ISETS*sizeof(word));
	    rbR.D.x.mexc = 0;
    }
  }


  if(DBGUNIT)
  {
    dsur.CLK = clk;
    if(holdn) dsur.D = dsuin;
    else      dsur.D.crdy = dsuin.crdy;
    if (!rstn)// WARNING: !!! removed "need_extra_reset()"
    {
      dsur.D.err = 0;
      dsur.D.tbufcnt = 0;
      dsur.D.tt = 0;
      dsur.D.asi = 0;
      dsur.D.crdy = 0;
    }
  }else
  {
    dsur.D.err     = dsur.Q.err     = 0;
    dsur.D.tbufcnt = dsur.Q.tbufcnt = 0;
    dsur.D.tt      = dsur.Q.tt      = 0;
    dsur.D.asi     = dsur.Q.asi     = 0;
    dsur.D.crdy    = dsur.Q.crdy    = 0;
  }

  if(DBGUNIT | PWRD2)
  {
    ir.CLK = clk;
    if (holdn == 1) ir.D = irin;
  }else
  {
    ir.D.pwd  = ir.Q.pwd  = 0;
    ir.D.addr = ir.Q.addr = 0;
  }
  
  wpr.CLK = clk;
  for (int32 i=0; i<=3; i++)
  {
    if (CFG_NWP > i)
    {
      if (holdn == 1) wpr.D.arr[i] = wprin.arr[i];
      if (rstn == 0)
      {
        wpr.D.arr[i].exec = 0;
        wpr.D.arr[i].load = 0;
        wpr.D.arr[i].store = 0;
	    }
    }else
      wpr.D.arr[i] = wpr.Q.arr[i] = wpr_none;
  }

  //-- pragma translate_off
#if (CFG_DISAS==1)
    trc : process(clk)
      variable valid : boolean;
      variable op : std_logic_vector(1 downto 0);
      variable op3 : std_logic_vector(5 downto 0);
      variable fpins, fpld : boolean;    
    begin
    if (disas = 1) and rising_edge(clk) and (rstn = '1') then
      if (fpu /= 0) then
        op := r.x.ctrl.inst(31 downto 30); op3 := r.x.ctrl.inst(24 downto 19);
        fpins := (op = FMT3) and ((op3 = FPOP1) or (op3 = FPOP2));
        fpld := (op = LDST) and ((op3 = LDF) or (op3 = LDDF) or (op3 = LDFSR));
      else
        fpins := false; fpld := false;
      end if;
      valid := (((not r.x.ctrl.annul) and r.x.ctrl.pv) = '1') and 
	(not ((fpins or fpld) and (r.x.ctrl.trap = '0')));
      valid := valid and (holdn = '1');
      if rising_edge(clk) and (rstn = '1') then
        print_insn (index, r.x.ctrl.pc(31 downto 2) & "00", r.x.ctrl.inst, 
                  rin.w.result, valid, r.x.ctrl.trap = '1', rin.w.wreg = '1', false);
      end if;
    end if;
    end process;
#endif
  //-- pragma translate_on

#if (CFG_DISAS < 2) 
  dummy = 1;
#endif

#if (CFG_DISAS > 1)
  disasen <= '1' when disas /= 0 else '0';
  cpu_index <= conv_std_logic_vector(index, 4);
  x0 : cpu_disasx
  port map (clk, rstn, dummy, r.x.ctrl.inst, r.x.ctrl.pc(31 downto 2),
            rin.w.result, cpu_index, rin.w.wreg, r.x.ctrl.annul, holdn,
            r.x.ctrl.pv, r.x.ctrl.trap, disasen);
#endif
}


