#pragma once

//#define DBG_mul32
//#define DBG_div32
//#define DBG_mmutlbcam
//#define DBG_mmutlb
//#define DBG_mmu_icache
//#define DBG_mmu_dcache
//#define DBG_mmu_acache
//#define DBG_iu3
//#define DBG_mmutw
//#define DBG_mmulrue
//#define DBG_mmulru
//#define DBG_mmu
//#define DBG_mmu_cache
//#define DBG_cachemem
//#define DBG_regfile_3p
//#define DBG_tbufmem
//#define DBG_leon3s
#define DBG_dsu3x

extern char* PutToStr(char *p, uint32 v, int size, char *comment=NULL, bool inv=false);
extern char* PutToStr(char *p, uint64 v, int size, char *comment=NULL, bool inv=false);
extern char* PutToStr(char *p, uint32* bus, int size, char *comment=NULL, bool inv=false);
extern char* PutToStr(char *p, char *str);

extern leon3mp topLeon3mp;


class dbg
{
  private:
    char chStr[2*64386];
    char *pStr;
    char chArr[256];

    LibInitData sLibInitData;
    std::ofstream *posBench[TB_TOTAL];
  public:
    
  public:
    dbg();
    ~dbg();
    
    void Update(SystemOnChipIO &ioData);
    // library:
    void Init(LibInitData *p);
    void Close();

    // testbench generators:
    
    void jtagcom_tb(SystemOnChipIO &ioData);
    void ahbmst_tb(SystemOnChipIO &ioData);
    void ahbjtag_tb(SystemOnChipIO &ioData);
    void ahbctrl_tb(SystemOnChipIO &ioData);
    void leon3s_tb(SystemOnChipIO &ioData);
    void dsu3x_tb(SystemOnChipIO &ioData);

#ifdef DBG_mul32
    uint32 holdnx;// : std_logic;
    mul32_in_type muli;//  : mul32_in_type;
    mul32_out_type mulo;//  : mul32_out_type;

    mul32 tst_mul32;
    void mul32_tb(SystemOnChipIO &io);
#endif
#ifdef DBG_div32
    uint32 holdnx;//   : in  std_ulogic;
    div32_in_type divi;//    : in  div32_in_type;
    div32_out_type divo;//    : out div32_out_type

    div32 tst_div32;
    void div32_tb(SystemOnChipIO &io);
#endif
#ifdef DBG_mmutlbcam
    mmutlbcam_in_type tlbcami;
    mmutlbcam_out_type tlbcamo;
    
    mmutlbcam tst_mmutlbcam;
    void mmutlbcam_tb(SystemOnChipIO &io);
#endif
#ifdef DBG_mmutlb
    mmutlb_in_type in_tlbi;//  : in  mmutlb_in_type;
    mmutlb_out_type ch_tlbo;//  : out mmutlb_out_type;
    mmutw_out_type in_two;//   : in  mmutw_out_type;
    mmutw_in_type ch_twi;//   : out mmutw_in_type

    mmutlb tst_mmutlb;
    void mmutlb_tb(SystemOnChipIO &io);
#endif
#ifdef DBG_mmu_icache
    icache_in_type      ici;
    icache_out_type     ico;
    dcache_in_type      dci;
    dcache_out_type     dco;
    memory_ic_in_type   mcii;
    memory_ic_out_type  mcio;
    mmudc_in_type       mmudci;
    mmuic_in_type       mmuici;
    mmuic_out_type      mmuico;
    icram_in_type       icramin;
    icram_out_type      icramo;
    uint32 fpuholdn;

    mmu_icache tst_mmu_icache;
    void mmu_icache_tb(SystemOnChipIO &io);
#endif
#ifdef DBG_mmu_dcache
    dcache_in_type in_dci;// : in  dcache_in_type;
    dcache_out_type ch_dco;// : out dcache_out_type;
    icache_out_type in_ico;// : in  icache_out_type;
    memory_dc_in_type ch_mcdi;// : out memory_dc_in_type;
    memory_dc_out_type in_mcdo;// : in  memory_dc_out_type;
    ahb_slv_in_type in_ahbsi;// : in  ahb_slv_in_type;
    dcram_in_type ch_dcrami;// : out dcram_in_type;
    dcram_out_type in_dcramo;// : in  dcram_out_type;
    uint32 fpuholdn;// : in  std_logic;
    mmudc_in_type ch_mmudci;// : out mmudc_in_type;
    mmudc_out_type in_mmudco;// : in mmudc_out_type;

    mmu_dcache tst_mmu_dcache;
    void mmu_dcache_tb(SystemOnChipIO &io);
#endif
#ifdef DBG_mmu_acache
    memory_ic_in_type in_mcii;//   : in  memory_ic_in_type;
    memory_ic_out_type ch_mcio;//   : out memory_ic_out_type;
    memory_dc_in_type in_mcdi;//   : in  memory_dc_in_type;
    memory_dc_out_type ch_mcdo;//   : out memory_dc_out_type;
    memory_mm_in_type in_mcmmi;//  : in  memory_mm_in_type;
    memory_mm_out_type ch_mcmmo;//  : out memory_mm_out_type;
    ahb_mst_in_type in_ahbi;//   : in  ahb_mst_in_type;
    ahb_mst_out_type ch_ahbo;//   : out ahb_mst_out_type;
    ahb_slv_out_vector in_ahbso;//  : in  ahb_slv_out_vector;
    uint32 in_hclken;

    mmu_acache tst_mmu_acache;
    void mmu_acache_tb(SystemOnChipIO &io);
#endif
#ifdef DBG_mmutw
    mmctrl_type1        in_mmctrl1;// : in  mmctrl_type1;
    mmutw_in_type       in_twi;//     : in  mmutw_in_type;
    mmutw_out_type      ch_two;//     : out mmutw_out_type;
    memory_mm_out_type  in_mcmmo;//   : in  memory_mm_out_type;
    memory_mm_in_type   ch_mcmmi;//   : out memory_mm_in_type

    mmutw tst_mmutw;
    void mmutw_tb(SystemOnChipIO &io);
#endif
#ifdef DBG_mmulrue
    mmulrue_in_type in_lruei;//  : in mmulrue_in_type;
    mmulrue_out_type ch_lrueo;//  : out mmulrue_out_type );

    mmulrue *ptst_mmulrue;
    void mmulrue_tb(SystemOnChipIO &io);
#endif
#ifdef DBG_mmulru
    mmulru_in_type in_lrui;//  : in mmulrue_in_type;
    mmulru_out_type ch_lruo;//  : out mmulrue_out_type );

    mmulru *ptst_mmulru;
    void mmulru_tb(SystemOnChipIO &io);
#endif
#ifdef DBG_mmu
    mmudc_in_type in_mmudci;// : in  mmudc_in_type;
    mmudc_out_type ch_mmudco;// : out mmudc_out_type;
    mmuic_in_type in_mmuici;// : in  mmuic_in_type;
    mmuic_out_type ch_mmuico;// : out mmuic_out_type;
    memory_mm_out_type in_mcmmo;//  : in  memory_mm_out_type;
    memory_mm_in_type ch_mcmmi;

    mmu tst_mmu;
    void mmu_tb(SystemOnChipIO &io);
#endif
#ifdef DBG_mmu_cache
  icache_in_type in_ici;//   : in  icache_in_type;
  icache_out_type ch_ico;//   : out icache_out_type;
  dcache_in_type in_dci;//   : in  dcache_in_type;
  dcache_out_type ch_dco;//   : out dcache_out_type;
  ahb_mst_in_type in_ahbi;//  : in  ahb_mst_in_type;
  ahb_mst_out_type ch_ahbo;//  : out ahb_mst_out_type;
  ahb_slv_in_type in_ahbsi;// : in  ahb_slv_in_type;
  ahb_slv_out_vector in_ahbso;//  : in  ahb_slv_out_vector;            
  cram_in_type ch_crami;// : out cram_in_type;
  cram_out_type in_cramo;// : in  cram_out_type;
  uint32 in_fpuholdn;// : in  std_ulogic;
  uint32 in_hclken;// : in std_ulogic
  
  mmu_cache tst_mmu_cache;
  void mmu_cache_tb(SystemOnChipIO &io);
#endif
#ifdef DBG_iu3
    uint32 holdnx;
    icache_in_type      ici;
    icache_out_type     ico;
    dcache_in_type      dci;
    dcache_out_type     dco;
    iregfile_in_type    rfi;
    iregfile_out_type   rfo;
    l3_irq_in_type      irqi;
    l3_irq_out_type     irqo;
    l3_debug_in_type    dbgi;
    l3_debug_out_type   dbgo;
    mul32_in_type       muli;
    mul32_out_type      mulo;
    div32_in_type       divi;
    div32_out_type      divo;
    fpc_in_type         fpi;
    fpc_out_type        fpo;
    fpc_in_type         cpi;
    fpc_out_type        cpo;
    tracebuf_in_type    tbi;
    tracebuf_out_type   tbo;

    iu3 tst_iu3;
    void iu3_tb(SystemOnChipIO &io);
#endif

#ifdef DBG_cachemem
  cram_in_type in_crami;// : in  cram_in_type;
  cram_out_type ch_cramo;// : out cram_out_type;

  cachemem tst_cachemem;
  void cachemem_tb(SystemOnChipIO &io);
#endif
#ifdef DBG_regfile_3p
  uint32 in_waddr;//  : in  std_logic_vector((abits -1) downto 0);
  uint32 in_wdata;//  : in  std_logic_vector((dbits -1) downto 0);
  uint32 in_we;//     : in  std_ulogic;
  uint32 in_raddr1;// : in  std_logic_vector((abits -1) downto 0);
  uint32 in_re1;//    : in  std_ulogic;
  uint32 ch_rdata1;// : out std_logic_vector((dbits -1) downto 0);
  uint32 in_raddr2;// : in  std_logic_vector((abits -1) downto 0);
  uint32 in_re2;//    : in  std_ulogic;
  uint32 ch_rdata2;// : out std_logic_vector((dbits -1) downto 0);
  uint32 in_testin;//   : in std_logic_vector(3 downto 0) := "0000");

  regfile_3p tst_regfile_3p;
  void regfile_3p_tb(SystemOnChipIO &io);
#endif
#ifdef DBG_tbufmem
  tracebuf_in_type in_tbi;
  tracebuf_out_type ch_tbo;
  
  tbufmem tst_tbufmem;
  void tbufmem_tb(SystemOnChipIO &io);
#endif
  // leon3s debug:
#ifdef DBG_leon3s
  ahb_mst_in_type in_ahbi;//   : in  ahb_mst_in_type;
  ahb_mst_out_type ch_ahbo;//   : out ahb_mst_out_type;
  ahb_slv_in_type in_ahbsi;//  : in  ahb_slv_in_type;
  ahb_slv_out_vector in_ahbso;//  : in  ahb_slv_out_vector;
  l3_irq_in_type in_irqi;//   : in  l3_irq_in_type;
  l3_irq_out_type ch_irqo;//   : out l3_irq_out_type;
  l3_debug_in_type in_dbgi;//   : in  l3_debug_in_type;
  l3_debug_out_type ch_dbgo;//   : out l3_debug_out_type
  
  leon3s tst_leon3s;
#endif
#ifdef DBG_dsu3x
  ahb_mst_in_type in_ahbmi;//  : in  ahb_mst_in_type;
  ahb_slv_in_type in_ahbsi;//  : in  ahb_slv_in_type;
  ahb_slv_out_type ch_ahbso;//  : out ahb_slv_out_type;
  l3_debug_out_vector in_dbgi;//   : in l3_debug_out_vector(0 to NCPU-1);
  l3_debug_in_vector ch_dbgo;//   : out l3_debug_in_vector(0 to NCPU-1);
  dsu_in_type in_dsui;//   : in dsu_in_type;
  dsu_out_type ch_dsuo;//   : out dsu_out_type;
  uint32 in_hclken;// : in std_ulogic
  
  dsu3x tst_dsu3x;
#endif
};

