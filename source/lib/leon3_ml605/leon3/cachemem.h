#pragma once

class cachemem
{
  friend class dbg;
  private:
    typedef uint64 dtdatain_vector;// is std_logic_vector(DTWIDTH downto 0);
    struct dtdatain_type
    {
      dtdatain_vector arr[MAXSETS];//is array (0 to MAXSETS-1) of dtdatain_vector;
    };
    typedef uint64 itdatain_vector;// is std_logic_vector(ITWIDTH downto 0);
    struct itdatain_type
    {
      itdatain_vector arr[MAXSETS];//is array (0 to MAXSETS-1) of itdatain_vector;
    };
    
    typedef uint64 itdataout_vector;// is std_logic_vector(ITWIDTH-1 downto 0);
    struct itdataout_type
    {
      itdataout_vector arr[MAXSETS];// is array (0 to MAXSETS-1) of itdataout_vector;
    };
    typedef uint32 iddataout_vector;// is std_logic_vector(IDWIDTH -1 downto 0);
    struct iddataout_type
    {
      iddataout_vector arr[MAXSETS];// is array (0 to MAXSETS-1) of iddataout_vector;
    };
    typedef uint32 dtdataout_vector;// is std_logic_vector(DTWIDTH-1 downto 0);
    struct dtdataout_type
    {
      dtdataout_vector arr[MAXSETS];// is array (0 to MAXSETS-1) of dtdataout_vector;
    };
    typedef uint32 dddataout_vector;// is std_logic_vector(DDWIDTH -1 downto 0);
    struct dddataout_type
    {
      dddataout_vector arr[MAXSETS];// is array (0 to MAXSETS-1) of dddataout_vector;
    };
    

    uint32 itaddr;//    : std_logic_vector(IOFFSET_BITS + ILINE_BITS -1 downto ILINE_BITS);
    uint32 idaddr;//    : std_logic_vector(IOFFSET_BITS + ILINE_BITS -1 downto 0);
    uint32 ildaddr;//   : std_logic_vector(ILRAM_BITS-3 downto 0);

    itdatain_type  itdatain;//  : itdatain_type; 
    itdataout_type itdataout;// : itdataout_type;
    uint32         iddatain;//  : std_logic_vector(IDWIDTH -1 downto 0);
    iddataout_type iddataout;// : iddataout_type;
    uint32         ildataout;// : std_logic_vector(31 downto 0);

    uint32 itenable;//  : std_ulogic;
    uint32 idenable;//  : std_ulogic;
    uint32 itwrite;//   : std_logic_vector(0 to MAXSETS-1);
    uint32 idwrite;//   : std_logic_vector(0 to MAXSETS-1);

    uint32 dtaddr;//    : std_logic_vector(DOFFSET_BITS + DLINE_BITS -1 downto DLINE_BITS);
    uint32 dtaddr2;//   : std_logic_vector(DOFFSET_BITS + DLINE_BITS -1 downto DLINE_BITS);
    uint32 ddaddr;//    : std_logic_vector(DOFFSET_BITS + DLINE_BITS -1 downto 0);
    uint32 ldaddr;//    : std_logic_vector(DLRAM_BITS-1 downto 2);
    
    dtdatain_type  dtdatain;//  : dtdatain_type; 
    dtdatain_type  dtdatain2;// : dtdatain_type;
    dtdatain_type  dtdatain3;// : dtdatain_type;
    dtdatain_type  dtdatainu;// : dtdatain_type;
    dtdataout_type dtdataout;// : dtdataout_type;
    dtdataout_type dtdataout2;//: dtdataout_type;
    dtdataout_type dtdataout3;//: dtdataout_type;
    cdatatype      dddatain;//  : cdatatype;
    dddataout_type dddataout;// : dddataout_type;
    uint32 lddatain, ldataout;//  : std_logic_vector(31 downto 0);

    uint32 dtenable;//  : std_logic_vector(0 to MAXSETS-1);
    uint32 dtenable2;// : std_logic_vector(0 to MAXSETS-1);
    uint32 ddenable;//  : std_logic_vector(0 to MAXSETS-1);
    uint32 dtwrite;//   : std_logic_vector(0 to MAXSETS-1);
    uint32 dtwrite2;//  : std_logic_vector(0 to MAXSETS-1);
    uint32 dtwrite3;//  : std_logic_vector(0 to MAXSETS-1);
    uint32 ddwrite;//   : std_logic_vector(0 to MAXSETS-1);

    uint32 vcc, gnd;//  : std_ulogic;
    
    // instructions cache:
    syncram *itags0[CFG_ISETS];
    syncram *idata0[CFG_ISETS];
    // data cache:
    syncram    *dtags0[CFG_DSETS];
    syncram_2p *dtags1[CFG_DSETS];
    syncram    *ddata0[CFG_DSETS];

  public:
    cachemem();
    ~cachemem();
    
    void Update(SClock clk,//   : in  std_ulogic;
                cram_in_type &crami,// : in  cram_in_type;
                cram_out_type &cramo,// : out cram_out_type;
                SClock sclk);
    
    void ClkUpdate()
    {
      for (int32 i=0; i<CFG_ISETS; i++)
      {
        itags0[i]->ClkUpdate();
        idata0[i]->ClkUpdate();
      }
      for (int32 i=0; i<CFG_DSETS; i++)
      {
        dtags0[i]->ClkUpdate();
        dtags1[i]->ClkUpdate();
        ddata0[i]->ClkUpdate();
      }
    }
};

