#pragma once


typedef uint32 word;// is std_logic_vector(31 downto 0);
typedef uint32 pctype;// is std_logic_vector(31 downto PCLOW);
typedef uint32 rfatype;// is std_logic_vector(RFBITS-1 downto 0);
typedef uint32 cwptype;// is std_logic_vector(NWINLOG2-1 downto 0);
struct icdtype// is array (0 to isets-1) of word;
{ 
  word arr[CFG_ISETS];
};
struct dcdtype// is array (0 to dsets-1) of word;
{
  word arr[CFG_DSETS];
};

struct dc_in_type
{
  uint32 Signed;
  uint32 enaddr;
  uint32 read;
  uint32 write;
  uint32 lock;
  uint32 dsuen;// : std_ulogic;
  uint32 size;// : std_logic_vector(1 downto 0);
  uint32 asi;//  : std_logic_vector(7 downto 0);    
};
  
struct pipeline_ctrl_type
{
  pctype pc;//    : pctype;
  word inst;//  : word;
  uint32 cnt;//   : std_logic_vector(1 downto 0);
  rfatype rd;//    : rfatype;
  uint32 tt;//    : std_logic_vector(5 downto 0);
  uint32 trap;//  : std_ulogic;
  uint32 annul;// : std_ulogic;
  uint32 wreg;//  : std_ulogic;
  uint32 wicc;//  : std_ulogic;
  uint32 wy;//    : std_ulogic;
  uint32 ld;//    : std_ulogic;
  uint32 pv;//    : std_ulogic;
  uint32 rett;//  : std_ulogic;
};

struct fetch_reg_type
{
  pctype pc;//     : pctype;
  uint32 branch;// : std_ulogic;
};
  
struct decode_reg_type
{
  pctype pc;//    : pctype;
  icdtype inst;//  : icdtype;
  cwptype cwp;//   : cwptype;
  uint32 set;//   : std_logic_vector(ISETMSB downto 0);
  uint32 mexc;//  : std_ulogic;
  uint32 cnt;//   : std_logic_vector(1 downto 0);
  uint32 pv;//    : std_ulogic;
  uint32 annul;// : std_ulogic;
  uint32 inull;// : std_ulogic;
  uint32 step;//  : std_ulogic;        
  uint32 divrdy;//: std_ulogic;        
};
  
struct regacc_reg_type
{
  pipeline_ctrl_type ctrl;//  : pipeline_ctrl_type;
  uint32 rs1;//   : std_logic_vector(4 downto 0);
  rfatype rfa1;
  rfatype rfa2;// : rfatype;
  uint32 rsel1;
  uint32 rsel2;// : std_logic_vector(2 downto 0);
  uint32 rfe1;
  uint32 rfe2;// : std_ulogic;
  cwptype cwp;//   : cwptype;
  word imm;//   : word;
  uint32 ldcheck1;// : std_ulogic;
  uint32 ldcheck2;// : std_ulogic;
  uint32 ldchkra;// : std_ulogic;
  uint32 ldchkex;// : std_ulogic;
  uint32 su;// : std_ulogic;
  uint32 et;// : std_ulogic;
  uint32 wovf;// : std_ulogic;
  uint32 wunf;// : std_ulogic;
  uint32 ticc;// : std_ulogic;
  uint32 jmpl;// : std_ulogic;
  uint32 step;//  : std_ulogic;            
  uint32 mulstart;// : std_ulogic;            
  uint32 divstart;// : std_ulogic;            
  uint32 bp;
  uint32 nobp;// : std_ulogic;
};
  
struct execute_reg_type
{
  pipeline_ctrl_type ctrl;//   : pipeline_ctrl_type;
  word op1;//    : word;
  word op2;//    : word;
  uint32 aluop;//  : std_logic_vector(2 downto 0);  	-- Alu operation
  uint32 alusel;// : std_logic_vector(1 downto 0);  	-- Alu result select
  uint32 aluadd;// : std_ulogic;
  uint32 alucin;// : std_ulogic;
  uint32 ldbp1;
  uint32 ldbp2;// : std_ulogic;
  uint32 invop2;// : std_ulogic;
  uint32 shcnt;//  : std_logic_vector(4 downto 0);  	-- shift count
  uint32 sari;//   : std_ulogic;				-- shift msb
  uint32 shleft;// : std_ulogic;				-- shift left/right
  uint32 ymsb;//   : std_ulogic;				-- shift left/right
  uint32 rd;// 	   : std_logic_vector(4 downto 0);
  uint32 jmpl;//   : std_ulogic;
  uint32 su;//     : std_ulogic;
  uint32 et;//     : std_ulogic;
  cwptype cwp;//    : cwptype;
  uint32 icc;//    : std_logic_vector(3 downto 0);
  uint32 mulstep;//: std_ulogic;            
  uint32 mul;//    : std_ulogic;            
  uint32 mac;//    : std_ulogic;
  uint32 bp;//     : std_ulogic;
};
  
struct memory_reg_type
{
  pipeline_ctrl_type ctrl;//   : pipeline_ctrl_type;
  word result;// : word;
  word y;//      : word;
  uint32 icc;//    : std_logic_vector(3 downto 0);
  uint32 nalign;// : std_ulogic;
  dc_in_type dci;//	   : dc_in_type;
  uint32 werr;//   : std_ulogic;
  uint32 wcwp;//   : std_ulogic;
  uint32 irqen;//  : std_ulogic;
  uint32 irqen2;// : std_ulogic;
  uint32 mac;//    : std_ulogic;
  uint32 divz;//   : std_ulogic;
  uint32 su;//     : std_ulogic;
  uint32 mul;//    : std_ulogic;
  uint32 casa;//   : std_ulogic;
  uint32 casaz;//  : std_ulogic;
};

enum exception_state {run, trap, dsu1, dsu2};
  
struct exception_reg_type
{
  pipeline_ctrl_type ctrl;//   : pipeline_ctrl_type;
  word result;// : word;
  word y;//      : word;
  uint32 icc       : 4;//    : std_logic_vector( 3 downto 0);
  uint32 annul_all : 1;// : std_ulogic;
  dcdtype data;//   : dcdtype;
  uint32 set;//    : std_logic_vector(DSETMSB downto 0);
  uint32 mexc      : 1;//   : std_ulogic;
  dc_in_type dci;//	   : dc_in_type;
  uint32 laddr     : 2;//  : std_logic_vector(1 downto 0);
  exception_state rstate;// : exception_state;
  uint32 npc       : 3;//    : std_logic_vector(2 downto 0);
  uint32 intack;// : std_ulogic;
  uint32 ipend;//  : std_ulogic;
  uint32 mac       : 1;//    : std_ulogic;
  uint32 debug     : 1;//  : std_ulogic;
  uint32 nerror    : 1;// : std_ulogic;
};

struct dsu_registers
{
  uint32 tt       : 8;//      : std_logic_vector(7 downto 0);
  uint32 err      : 1;//     : std_ulogic;
  uint32 tbufcnt;// : std_logic_vector(TBUFBITS-1 downto 0);
  uint32 asi;//     : std_logic_vector(7 downto 0);
  uint32 crdy;//    : std_logic_vector(2 downto 1);  -- diag cache access ready
};

struct irestart_register
{
  pctype addr;//   : pctype;
  uint32 pwd    : 1;//    : std_ulogic;
};
  
struct pwd_register_type
{
  uint32 pwd    : 1;//    : std_ulogic;
  uint32 error  : 1;//  : std_ulogic;
};

struct special_register_type
{
  cwptype cwp;//    : cwptype;                             -- current window pointer
  uint32 icc : 4;//    : std_logic_vector(3 downto 0);	  -- integer condition codes
  uint32 tt  : 8;//     : std_logic_vector(7 downto 0);	  -- trap type
  uint32 tba : 20;//    : std_logic_vector(19 downto 0);	  -- trap base address
  uint32 wim : CFG_NWIN;//    : std_logic_vector(NWIN-1 downto 0);   -- window invalid mask
  uint32 pil : 4;//    : std_logic_vector(3 downto 0);	  -- processor interrupt level
  uint32 ec  : 1;//     : std_ulogic;			   -- enable CP 
  uint32 ef  : 1;//     : std_ulogic;			   -- enable FP 
  uint32 ps  : 1;//     : std_ulogic;			   -- previous supervisor flag
  uint32 s   : 1;//      : std_ulogic;			   -- supervisor flag
  uint32 et  : 1;//     : std_ulogic;			   -- enable traps
  word y;//      : word;
  word asr18;//  : word;
  uint32 svt  : 1;//    : std_ulogic;			   -- enable traps
  uint32 dwt  : 1;//    : std_ulogic;			   -- disable write error trap
  uint32 dbp  : 1;//    : std_ulogic;			   -- disable branch prediction
};
  
struct write_reg_type
{
  special_register_type s;//      : special_register_type;
  word result;// : word;
  rfatype wa;//     : rfatype;
  uint32 wreg   : 1;//   : std_ulogic;
  uint32 except : 1;// : std_ulogic;
};

struct registers
{
  fetch_reg_type      f;//  : fetch_reg_type;
  decode_reg_type     d;//  : decode_reg_type;
  regacc_reg_type     a;//  : regacc_reg_type;
  execute_reg_type    e;//  : execute_reg_type;
  memory_reg_type     m;//  : memory_reg_type;
  exception_reg_type  x;//  : exception_reg_type;
  write_reg_type      w;//  : write_reg_type;
};

struct exception_type
{
  uint32 pri   : 1;//  : std_ulogic;
  uint32 ill   : 1;//  : std_ulogic;
  uint32 fpdis : 1;//  : std_ulogic;
  uint32 cpdis : 1;//  : std_ulogic;
  uint32 wovf  : 1;//  : std_ulogic;
  uint32 wunf  : 1;//  : std_ulogic;
  uint32 ticc  : 1;//  : std_ulogic;
};

struct watchpoint_register 
{
  uint32 addr;//    : std_logic_vector(31 downto 2);  -- watchpoint address
  uint32 mask;//    : std_logic_vector(31 downto 2);  -- watchpoint mask
  uint32 exec  : 1;//    : std_ulogic;			    -- trap on instruction
  uint32 load  : 1;//    : std_ulogic;			    -- trap on load
  uint32 store : 1;//   : std_ulogic;			    -- trap on store
};

struct watchpoint_registers
{
  watchpoint_register arr[4];// (0 to 3) of watchpoint_register;
};

const watchpoint_register wpr_none = {0,0,0,0,0};//: watchpoint_register := (zero32(31 downto 2), zero32(31 downto 2), '0', '0', '0');




//******************************
class iu3
{
  friend class dbg;
  private:
    uint32 hindex;
    uint32 BPRED;// : std_logic;
    registers       r;
    TDFF<registers>       rbR;
    registers             rin;// : registers;
    TDFF<watchpoint_registers> wpr;
    watchpoint_registers  wprin;// : watchpoint_registers;
    TDFF<dsu_registers>   dsur;
    dsu_registers         dsuin;// : dsu_registers;
    TDFF<irestart_register> ir;
    irestart_register     irin;// : irestart_register;
    TDFF<pwd_register_type>  rp;
    pwd_register_type     rpin;// : pwd_register_type;

    uint32 dummy;// : std_ulogic;
    uint32 cpu_index;// : std_logic_vector(3 downto 0);
    uint32 disasen;// : std_ulogic;
    
    // process "comb":
    registers             v;// 	: registers;
    pwd_register_type     vp;// 	: pwd_register_type;
    watchpoint_registers  vwpr;// : watchpoint_registers;
    dsu_registers         vdsu;// : dsu_registers;
    uint32  fe_pc, fe_npc;// :  std_logic_vector(31 downto PCLOW);
    uint32  npc;// 	: std_logic_vector(31 downto PCLOW);
    uint32  de_raddr1, de_raddr2;// : std_logic_vector(9 downto 0);
    uint32  de_rs2, de_rd;// : std_logic_vector(4 downto 0);
    uint32  de_hold_pc, de_branch, de_ldlock;// : std_ulogic;
    cwptype de_cwp, de_cwp2;// : cwptype;
    uint32  de_inull;// : std_ulogic;
    uint32  de_ren1, de_ren2;// : std_ulogic;
    uint32  de_wcwp;// : std_ulogic;
    word    de_inst;// : word;
    pctype  de_branch_address;// : pctype;
    uint32  de_icc;// : std_logic_vector(3 downto 0);
    uint32  de_fbranch, de_cbranch;// : std_ulogic;
    uint32  de_rs1mod;// : std_ulogic;
    uint32  de_bpannul;// : std_ulogic;
    uint32  de_fins_hold;// : std_ulogic;

    word ra_op1, ra_op2;// : word;
    uint32 ra_div;// : std_ulogic;
    uint32 ra_bpmiss;// : std_ulogic;
    uint32 ra_bpannul;// : std_ulogic;

    uint32 ex_jump, ex_link_pc;// : std_ulogic;
    pctype ex_jump_address;// : pctype;
    uint64 ex_add_res;// : std_logic_vector(32 downto 0);
    word ex_shift_res, ex_logic_res, ex_misc_res;// : word;
    word ex_edata, ex_edata2;// : word;
    dc_in_type ex_dci;// : dc_in_type;
    uint32 ex_force_a2, ex_load, ex_ymsb;// : std_ulogic;
    word ex_op1, ex_op2, ex_result, ex_result2, mul_op2;// : word;
    uint32 ex_shcnt;// : std_logic_vector(4 downto 0);
    uint32 ex_dsuen;// : std_ulogic;
    uint32 ex_ldbp2;// : std_ulogic;
    uint32 ex_sari;// : std_ulogic;
    uint32 ex_bpmiss;// : std_ulogic;
    
    uint32 me_inull, me_nullify, me_nullify2;// : std_ulogic;
    uint32 me_iflush;// : std_ulogic;
    uint32 me_newtt;// : std_logic_vector(5 downto 0);
    word me_asr18;// : word;
    uint32 me_signed;// : std_ulogic;
    uint32 me_size, me_laddr;// : std_logic_vector(1 downto 0);
    uint32 me_icc;// : std_logic_vector(3 downto 0);

    
    word xc_result;// : word;
    word xc_df_result;// : word;
    uint32 xc_waddr;// : std_logic_vector(9 downto 0);
    uint32 xc_exception, xc_wreg;// : std_ulogic;
    pctype xc_trap_address;// : pctype;
    uint32 xc_vectt;// : std_logic_vector(7 downto 0);
    uint32 xc_trap;// : std_ulogic;
    uint32 xc_fpexack;// : std_ulogic;
    uint32 xc_rstn, xc_halt;// : std_ulogic;
    
    //--  variable wr_rf1_data, wr_rf2_data : word;
    word diagdata;// : word;
    tracebuf_in_type tbufi;// : tracebuf_in_type;
    uint32  dbgm;// : std_ulogic;
    uint32  fpcdbgwr;// : std_ulogic;
    fpc_in_type vfpi;// : fpc_in_type;
    uint32  dsign;// : std_ulogic;
    uint32  pwrd, sidle;// : std_ulogic;
    irestart_register vir;// : irestart_register;
    uint32  icnt;// : std_ulogic;
    uint32  fcnt;// : std_ulogic;
    uint32  tbufcntx;// : std_logic_vector(TBUFBITS-1 downto 0);
    uint32  bpmiss; // : std_ulogic;

  public:
    // check dbgexc() function:
    registers            in_r;
    registers            in_v;
    dsu_registers        in_dsur;
    irestart_register    in_ir;
    watchpoint_registers in_wpr;
  
    //special_register_type ch_s;// : special_register_type; 



	  
  public:
    iu3(uint32 hindex_=AHB_MASTER_LEON3);

    void Update
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
    );
    
    void ClkUpdate()
    {
      rp.ClkUpdate();
      rbR.ClkUpdate();
      dsur.ClkUpdate();
      ir.ClkUpdate();
      wpr.ClkUpdate();
    }

    // 1:
    uint32 dbgexc(registers& r,
                  l3_debug_in_type &dbgi,
                  uint32 &trap,// : std_ulogic;
                  uint32 &tt );// : std_logic_vector(7 downto 0)) return std_ulogic is

    // 2:
    uint32 dbgerr(registers &r,// : registers;
                  l3_debug_in_type &dbgi,// : l3_debug_in_type;
                  uint32 tt);// : std_logic_vector(7 downto 0))  return std_ulogic is

    // 3:
    void diagwr(  registers &r,//    : in registers;
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
                  uint32 &fpcwr);// : out std_ulogic);

    // 4:
    word asr17_gen ( registers &r);// : in registers) return word is

    // 5:
    void diagread( l3_debug_in_type &dbgi,//   : in l3_debug_in_type;
                   registers &r,//      : in registers;
                   iregfile_out_type &rfo,//   : in  iregfile_out_type;
                   fpc_out_type &fpo,//   : in  fpc_out_type;
                   dsu_registers &dsur,//   : in dsu_registers;
                   irestart_register &ir,//     : in irestart_register;
                   watchpoint_registers &wpr,//    : in watchpoint_registers;
                   dcache_out_type &dco,//   : in  dcache_out_type;                          
                   tracebuf_out_type &tbufo,//  : in tracebuf_out_type;
                   word &data);// : out word) is

    // 6:
    void itrace(registers &r,//    : in registers;
                dcache_out_type &dco,//   : in  dcache_out_type;
                dsu_registers &dsur,// : in dsu_registers;
                dsu_registers &vdsu,// : in dsu_registers;
                word &res,//  : in word;
                uint32 exc,//  : in std_ulogic;
                l3_debug_in_type &dbgi,// : in l3_debug_in_type;
                uint32 error,// : in std_ulogic;
                uint32 trap,//  : in std_ulogic;                          
                uint32 &tbufcnt,// : out std_logic_vector(TBUFBITS-1 downto 0); 
                tracebuf_in_type &di);//  : out tracebuf_in_type) is

    // 7:
    void dbg_cache( uint32 holdn,//    : in std_ulogic;
                    l3_debug_in_type &dbgi,//     :  in l3_debug_in_type;
                    registers &r,//        : in registers;
                    dsu_registers &dsur,//     : in dsu_registers;
                    word &mresult,//  : in word;
                    dc_in_type &dci,//      : in dc_in_type;
                    word &mresult2,// : out word;
                    dc_in_type &dci2);//     : out dc_in_type)

    // 8:
    void fpexack( registers &r,// : in registers;
                  uint32 &fpexc);// : out std_ulogic) is

    // 9:
    void diagrdy( uint32 denable,// : in std_ulogic;
                  dsu_registers &dsur,// : in dsu_registers;
                  dc_in_type &dci,//   : in dc_in_type;
                  icache_out_type &ico,// : in icache_out_type;
                  uint32 &crdy);// : out std_logic_vector(2 downto 1)) is                   

    // 10:
    void regaddr( uint32 cwp,// : std_logic_vector; 
                  uint32 reg,// : std_logic_vector(4 downto 0);
	                rfatype &rao);// : out rfatype)

    // 11:
    uint32 branch_address(word &inst,// : word; 
                      pctype &pc);// : pctype) return std_logic_vector is

    // 12:
    uint32 branch_true( uint32 icc,// : std_logic_vector(3 downto 0); 
                        uint32 inst);// : word) 

    // 13:
    void su_et_select( registers &r,// : in registers; 
                       uint32 xc_ps,
                       uint32 xc_s,
                       uint32 xc_et,// : in std_ulogic;
		                   uint32 &su,// : out std_ulogic,
                       uint32 &et);// : out std_ulogic )

    // 14:
    uint32 wphit( registers &r,// : registers;
                  watchpoint_registers &wpr,// : watchpoint_registers; 
                  l3_debug_in_type &debug);// : l3_debug_in_type)  return std_ulogic is

    // 15:
    word shift3(registers &r,// : registers; 
                word &aluin1);// : word) return word is

    // 16:
    word shift2(registers &r,// : registers;
                word &aluin1);// : word) return word is

    // 17:
    word shift( registers &r,// : registers; 
                word aluin1,// : word;
	              uint32 shiftcnt,// : std_logic_vector(4 downto 0); 
                uint32 sari);// : std_ulogic ) return word is

    // 18:
    void exception_detect(registers &r,// : registers; 
                          watchpoint_registers &wpr,// : watchpoint_registers; 
                          l3_debug_in_type &dbgi,// : l3_debug_in_type;
	                        uint32 &trap,// : out std_ulogic; 
                          uint32 &tt);// : out std_logic_vector(5 downto 0)) is

    // 19:
    void wicc_y_gen(word inst,// : word; 
                    registers &r,// : registers; 
                    mul32_out_type &mulo,//  : in  mul32_out_type;
                    div32_out_type &divo,//  : in  div32_out_type;
                    uint32 &wicc,// : out std_ulogic
                    uint32 &wy);// : out std_ulogic) is

    // 20:
    void cwp_gen( uint32 rstn,//  : in  std_ulogic;
                  registers &r, 
                  registers &v,// : registers; 
                  uint32 annul,
                  uint32 wcwp,// : std_ulogic;
                  cwptype ncwp,// : cwptype;
		              cwptype &cwp);// : out cwptype) is

    // 21:
    void cwp_ex(registers &r,// : in registers;
                uint32 &wcwp);// : out std_ulogic) is

    // 22:
    void cwp_ctrl(registers &r,// : in registers;
                  uint32 xc_wim,// : in std_logic_vector(NWIN-1 downto 0);
	                word inst,// : word;
                  cwptype &de_cwp,// : out cwptype;
                  uint32 &wovf_exc,
                  uint32 &wunf_exc, 
                  uint32 &wcwp);// : out std_ulogic) is

    // 23:
    void rs1_gen(registers &r,// : registers;
                 word inst,// : word;
                 uint32 &rs1,// : out std_logic_vector(4 downto 0);
	               uint32 &rs1mod);// : out std_ulogic) is

    // 24:
    int32 icc_valid(registers &r);// : registers) return std_logic is

    // 25:
    void bp_miss_ex(registers &r,// : registers; 
                    uint32 icc,// : std_logic_vector(3 downto 0); 
	                  uint32 &ex_bpmiss, 
                    uint32 &ra_bpannul);// : out std_logic) is

    // 26:
    void bp_miss_ra(registers &r,// : registers; 
                    uint32 &ra_bpmiss,
                    uint32 &de_bpannul);// : out std_logic) is

    // 27:
    void lock_gen(registers &r,// : registers; 
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
                uint32 &de_fins_hold);// : out std_ulogic) is

    // 28:
    void fpbranch(word inst,// : in word; 
                  uint32 fcc,//  : in std_logic_vector(1 downto 0);
                  uint32 &branch);// : out std_ulogic) is

    // 29:
    void ic_ctrl( uint32 rstn,
                  uint32 inBPRED,
                  registers &r,// : registers; 
                  mul32_out_type &mulo,//  : in  mul32_out_type;
                  div32_out_type &divo,//  : in  div32_out_type;
                  word inst,// : word; 
                  uint32 annul_all, 
                  uint32 ldlock, 
                  uint32 branch_true, 
	                uint32 fbranch_true, 
                  uint32 cbranch_true, 
                  uint32 fccv,
                  uint32 cccv,// : in std_ulogic; 
	                uint32 &cnt,// : out std_logic_vector(1 downto 0); 
	                pctype &de_pc,// : out pctype;
                  uint32 &de_branch, 
                  uint32 &ctrl_annul, 
                  uint32 &de_annul, 
                  uint32 &jmpl_inst, 
                  uint32 &inull, 
	                uint32 &de_pv, 
                  uint32 &ctrl_pv, 
                  uint32 &de_hold_pc, 
                  uint32 &ticc_exception, 
                  uint32 &rett_inst, 
                  uint32 &mulstart,
	                uint32 &divstart,// : out std_ulogic; 
                  uint32 rabpmiss, 
                  uint32 exbpmiss);// : std_logic) is
    
    // 30:
    void rd_gen(registers &r,// : registers; 
                mul32_out_type &mulo,//  : in  mul32_out_type;
                div32_out_type &divo,//  : in  div32_out_type;
                word inst,// : word; 
                uint32 &wreg, 
                uint32 &ld,// : out std_ulogic; 
	              uint32 &rdo);// : out std_logic_vector(4 downto 0)) is

    // 31:
    word imm_data (registers &r,// : registers;
                     word insn);// : word) 	return word is

    // 32:
    word get_spr (registers &r);// : registers) return word is
    
    // 33:
    bool imm_select(word inst);// : word) return boolean is
    
    // 34:
    void alu_op(registers &r,// : in registers; 
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
	              uint32 &invop2);// : out std_ulogic) is
    
    // 35:
    uint32 ra_inull_gen(registers &r, 
                        registers &v);// : registers) return std_ulogic is
         
    // 36:               
    void op_mux(registers &r,// : in registers; 
                word rfd, 
                word ed, 
                word md, 
                word xd, 
                word im,// : in word; 
	              uint32 rsel,// : in std_logic_vector(2 downto 0); 
	              uint32 &ldbp,// : out std_ulogic; 
	              word &d,// : out word; 
	              uint32 id);// : std_logic) is

    // 37:
    void op_find(registers &r,// : in registers; 
                  uint32 ldchkra,// : std_ulogic; 
                  uint32 ldchkex,// : std_ulogic;
	                uint32 rs1,// : std_logic_vector(4 downto 0); 
	                rfatype ra,// : rfatype; 
	                bool im,// : boolean; 
	                uint32 &rfe,// : out std_ulogic; 
	                uint32 &osel,// : out std_logic_vector(2 downto 0); 
	                uint32 ldcheck);// : std_ulogic) is
    
    // 38:         
    void cin_gen(registers &r,// : registers; 
                  uint32 me_cin,// : in std_ulogic; 
                  uint32 &cin);// : out std_ulogic) is

    // 39:
    void logic_op(registers &r,// : registers; 
                  mul32_out_type &mulo,//  : in  mul32_out_type;
                  word aluin1, 
                  word aluin2, 
                  word mey,// : word; 
	                uint32 ymsb,// : std_ulogic; 
	                word &logicres, 
	                word &y);// : out word) is
	       
    // 40:
    void misc_op( registers &r,// : registers; 
                  mul32_out_type &mulo,//  : in  mul32_out_type;
                  watchpoint_registers wpr,// : watchpoint_registers; 
	                word aluin1, 
	                word aluin2, 
	                word ldata, 
	                word mey,// : word; 
          	      word &mout, 
          	      word &edata);// : out word) is

    // 41:          	      
    void alu_select(registers &r,// : registers; 
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
	                  uint32 &mzero);// : out std_ulogic) is
    // 42:           
    void dcache_gen(registers &r, 
                    registers &v,// : registers; 
                    dc_in_type &dci,// : out dc_in_type; 
	                  uint32 &link_pc, 
	                  uint32 &jump, 
	                  uint32 &force_a2, 
	                  uint32 &load, 
	                  uint32 &mcasa);// : out std_ulogic) is

    // 43:
    void fpstdata(registers &r,// : in registers; 
                  word edata, 
                  word eres,// : in word; 
                  uint32 fpstdata,// : in std_logic_vector(31 downto 0);
                  word &edata2, 
                  word &eres2);// : out word) is

    // 44:
    word ld_align(dcdtype &data,// : dcdtype; 
                  uint32 set,// : std_logic_vector(DSETMSB downto 0);
	                uint32 size, 
	                uint32 laddr,// : std_logic_vector(1 downto 0); 
	                uint32 Signed);// : std_ulogic) return word is

    // 45:
    void mem_trap(uint32 rstn,//  : in  std_ulogic;
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
                  uint32 &tt);// : out std_logic_vector(5 downto 0)) is

    // 46:
    void irq_trap(registers &r,//       : in registers;
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
                 uint32 &tt2);//      : out std_logic_vector(5 downto 0)) is

    // 47:
    void irq_intack(registers &r,// : in registers;
                    uint32 holdn,// : in std_ulogic; 
                    uint32 &intack);//: out std_ulogic) is 

    // 48:
    void sp_write(mul32_out_type &mulo,//  : in  mul32_out_type;
                  registers &r,// : registers;
                  watchpoint_registers &wpr,// : watchpoint_registers;
                  special_register_type &s,// : out special_register_type; 
                  watchpoint_registers &vwpr);// : out watchpoint_registers) is

    // 49:
    uint32 npc_find (registers &r);// : registers) return std_logic_vector is

    // 50:
    word npc_gen(registers &r);// : registers) return word is

    // 51:
    void mul_res( registers &r,// : registers;
                  mul32_out_type &mulo,//  : in  mul32_out_type;
                  div32_out_type &divo,//  : in  div32_out_type;
                  word asr18in,// : word;
                  word &result, 
                  word &y, 
                  word &asr18,// : out word; 
	                uint32 &icc);// : out std_logic_vector(3 downto 0)) is

    // 52:
    uint32 powerdwn(registers &r,// : registers;
                    uint32 trap,// : std_ulogic;
                    pwd_register_type &rp);// : pwd_register_type) return std_ulogic is


};


