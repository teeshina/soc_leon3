#pragma once

//******************************************************************
class ram16_sx_sx
{
  friend class dbg;
  private:
    uint32 abits;// = 9;  // default = 10
    uint32 dbits;// = 36; // default = 8
    uint32 size;

    uint64 *rfd;
    uint64 wbValueA;
    uint64 wbValueB;

  public:
    ram16_sx_sx(uint32 abits_=10, uint32 dbits_=8)
    {
      abits = abits_;
      dbits = dbits_;
      size = (0x1<<abits)-1;
      rfd = (uint64*)malloc(sizeof(uint64)*size);
      memset(rfd, 0, sizeof(uint64)*size);
    }
    ~ram16_sx_sx()
    {
      free(rfd);
    }
    
    void Update( uint64 &DOA,// : out std_logic_vector (dbits-1 downto 0);
                 uint64 &DOB,// : out std_logic_vector (dbits-1 downto 0);
                 uint32 ADDRA,// : in std_logic_vector (abits-1 downto 0);
                 SClock CLKA,// : in std_ulogic;
                 uint64 DIA,// : in std_logic_vector (dbits-1 downto 0);
                 uint32 ENA,// : in std_ulogic;
                 uint32 WEA,// : in std_ulogic;
                 uint32 ADDRB,// : in std_logic_vector (abits-1 downto 0);
                 SClock CLKB,// : in std_ulogic;
                 uint64 DIB,// : in std_logic_vector (dbits-1 downto 0);
                 uint32 ENB,// : in std_ulogic;
                 uint32 WEB// : in std_ulogic
                )
    {
      if(CLKA.eClock_z == SClock::CLK_POSEDGE)
      {
        if (ENA)
        {
          if (WEA) rfd[ADDRA] = DIA;
          wbValueA = rfd[ADDRA];
        }else
          wbValueA = LSBMSK64(dbits);
      }
      DOA = wbValueA;

      if (CLKB.eClock_z == SClock::CLK_POSEDGE)
      {
        if (ENB)
        {
          if (WEB)
            rfd[ADDRB] = DIB;
          wbValueB = rfd[ADDRB];
        }else
          wbValueB = LSBMSK64(dbits);//(others => '1');
      }
      DOB = wbValueB;
    }
};


//******************************************************************
class syncram
{
  friend class dbg;
  private:
    uint32 ENA;
    uint32 WRENA;
    uint32 WRADR;
    uint32 WRDATA;
    uint64 WRDATA64;
    SClock CLK;
    uint32 abits;
    uint32 dbits;
    uint32 size;
    TDFF<uint32>ra;//std_logic_vector((abits -1) downto 0);
    uint32 *memarr;
    uint64 *memarr64;
    uint32 datamsk;
    uint64 datamsk64;
  public:
    syncram( uint32 abits_= 10, uint32 dbits_ = 8 )
    {
      abits = abits_;
      dbits = dbits_;
      size = (0x1<<abits);
      if(dbits<=32) {memarr = (uint32*)malloc(sizeof(uint32)*size); datamsk = MSK32(dbits-1,0); }
      else          {memarr64 = (uint64*)malloc(sizeof(uint64)*size); datamsk64 = MSK64(dbits-1,0); }
    }
    ~syncram() 
    {
      if(dbits<=32) free(memarr);
      else          free(memarr64);
    }
    
    void Update( SClock clk,//      : in std_ulogic;
                uint32 address,//  : in std_logic_vector((abits -1) downto 0);
                uint32 datain,//   : in std_logic_vector((dbits -1) downto 0);
                uint32 &dataout,//  : out std_logic_vector((dbits -1) downto 0);
                uint32 enable,
                uint32 write,//    : in std_ulogic
                uint32 testin=0
              )
    {
      CLK = clk;
      ENA = 1;//enable; // This wire is using in "virtex" target
      WRENA = write;
      WRADR = address;
      WRDATA = datain;
      
      ra.CLK = clk;
      ra.D = address;
      dataout = memarr[ra.Q]&datamsk;
    }
    void Update( SClock clk,//      : in std_ulogic;
                uint32 address,//  : in std_logic_vector((abits -1) downto 0);
                uint64 datain,//   : in std_logic_vector((dbits -1) downto 0);
                uint64 &dataout,//  : out std_logic_vector((dbits -1) downto 0);
                uint32 enable,
                uint32 write,//    : in std_ulogic
                uint32 testin=0
              )
    {
      CLK = clk;
      ENA = 1;//enable; // This wire is using in "virtex" target
      WRENA = write;
      WRADR = address;
      WRDATA64 = datain;
      
      ra.CLK = clk;
      ra.D = address;
      dataout = memarr64[ra.Q]&datamsk64;
    }
    
    void ClkUpdate()
    {
      if(CLK.eClock==SClock::CLK_POSEDGE)
      {
        if(dbits<=32)
        {
          if(ENA&WRENA) memarr[WRADR] = WRDATA;
        }else
        {
          if(ENA&WRENA) memarr64[WRADR] = WRDATA64;
        }
      }
      ra.ClkUpdate();
    }
};


//******************************************************************
class syncram64
{
  friend class dbg;
  private:
    uint32 abits;
    syncram *x0;
    syncram *x1;

  public:
    syncram64(uint32 abits_)
    { 
      uint32 abits = abits_;
      x0 = new syncram(abits,32);
      x1 = new syncram(abits,32);
    }
    ~syncram64()
    {
      free(x0);
      free(x1);
    }
    
    void Update(  SClock clk,//     : in  std_ulogic;
                  uint32 address,// : in  std_logic_vector (abits -1 downto 0);
                  uint64 datain,//  : in  std_logic_vector (63 downto 0);
                  uint64 &dataout,// : out std_logic_vector (63 downto 0);
                  uint32 enable,//  : in  std_logic_vector (1 downto 0);
                  uint32 write,//   : in  std_logic_vector (1 downto 0);
                  uint32 testin=0);//  : in  std_logic_vector (3 downto 0) := "0000");
                  
    void ClkUpdate()
    {
      x0->ClkUpdate();
      x1->ClkUpdate();
    }
};


//******************************************************************
//-- synchronous 2-port ram, common clock
class generic_syncram_2p
{
  friend class dbg;
  private:

    uint32 abits;// : integer := 8;
    uint32 dbits;// : integer := 32;
    uint32 sepclk;//: integer := 0
    uint32 size;
    SClock WRCLK;
    uint32 WRENA;
    uint32 WRADR;
    uint32 WRDATA;
    TDFF<uint32> RDDATA;
    

    uint32 *memarr;
    uint32 wbValue;

  public:
    generic_syncram_2p(uint32 abits_=8, uint32 dbits_=32, uint32 sepclk_=0)
    {
      abits = abits_;
      dbits = dbits_;
      sepclk = sepclk_;
      size = (0x1<<abits);
      memarr = (uint32*)malloc(sizeof(uint32)*size);
    }
    ~generic_syncram_2p()
    { free(memarr); }
  
    void Update( SClock rclk,// : in std_ulogic;
                SClock wclk,// : in std_ulogic;
                uint32 rdaddress,//: in std_logic_vector (abits -1 downto 0);
                uint32 wraddress,//: in std_logic_vector (abits -1 downto 0);
                uint32 data,//: in std_logic_vector (dbits -1 downto 0);
                uint32 wren,// : in std_ulogic;
                uint32 &q//: out std_logic_vector (dbits -1 downto 0)
              )
    {
      WRCLK = wclk;
      WRENA = wren;
      WRADR = wraddress;
      WRDATA = data;

      if (sepclk==0)  RDDATA.CLK = wclk;  
      else            RDDATA.CLK = rclk;
      RDDATA.D = memarr[rdaddress];
      
      q = RDDATA.Q;
    }

    void ClkUpdate()
    {
      if(WRCLK.eClock==SClock::CLK_POSEDGE)
      {
        if(WRENA) memarr[WRADR] = WRDATA;
      }
      RDDATA.ClkUpdate();
    }
};

//******************************************************************
class syncram_2p
{
  friend class dbg;
  private:
    uint32 wrfst;// : integer := 0;
    uint32 testen;// : integer := 0;
    uint32 words;// : integer := 0
    TDFF<uint32> r_raddr;
    TDFF<uint32> r_waddr;
    TDFF<uint32> r_datain;
    TDFF<uint32> r_write;
    TDFF<uint32> r_renable;

    generic_syncram_2p *pSyncRam_2p;
    
  public:
    syncram_2p(uint32 abits_=6, uint32 dbits_=8, uint32 sepclk_=0, uint32 wrfst_=0)
    {
      wrfst = wrfst_;
      pSyncRam_2p = new generic_syncram_2p(abits_, dbits_, sepclk_);
    }
    ~syncram_2p()
    { free(pSyncRam_2p); }
  
    void Update(SClock rclk,//     : in std_ulogic;
                uint32 renable,//  : in std_ulogic;
                uint32 raddress,// : in std_logic_vector((abits -1) downto 0);
                uint32 &dataout,//  : out std_logic_vector((dbits -1) downto 0);
                SClock wclk,//     : in std_ulogic;
                uint32 write,//    : in std_ulogic;
                uint32 waddress,// : in std_logic_vector((abits -1) downto 0);
                uint32 datain,//   : in std_logic_vector((dbits -1) downto 0);
                uint32 testin=0);//   : in std_logic_vector(3 downto 0) := "0000");
    void ClkUpdate()
    {
      r_raddr.ClkUpdate();
      r_waddr.ClkUpdate();
      r_datain.ClkUpdate();
      r_write.ClkUpdate();
      r_renable.ClkUpdate();
      pSyncRam_2p->ClkUpdate();
    }
    
};

//******************************************************************
class generic_regfile_3p
{
  friend class dbg;
  private:
    uint32 abits;// : integer := 6;
    uint32 dbits;// : integer := 32;
    uint32 wrfst;// : integer := 0;
    uint32 numregs;// : integer := 40);
    
    SClock WRCLK;

    uint32 *memarr;// : mem;
    TDFF<uint32> ra1;
    TDFF<uint32> ra2;
    TDFF<uint32> wa;//  : std_logic_vector((abits -1) downto 0);
    TDFF<uint32> din;//  : std_logic_vector((dbits -1) downto 0);
    TDFF<uint32> wr;//  : std_ulogic;

  public:
    generic_regfile_3p( uint32 abits_=6, uint32 dbits_=32, uint32 wrfst_=0, uint32 numregs_=40)
    {
      abits = abits_;
      dbits = dbits_;
      wrfst = wrfst_;
      numregs = numregs_;
      memarr = (uint32*)malloc(numregs*sizeof(uint32));
    }
    ~generic_regfile_3p()
    { free(memarr); }
  
    void Update( SClock wclk,//   : in  std_ulogic;
                 uint32 waddr,//  : in  std_logic_vector((abits -1) downto 0);
                 uint32 wdata,//  : in  std_logic_vector((dbits -1) downto 0);
                 uint32 we,//     : in  std_ulogic;
                 SClock rclk,//   : in  std_ulogic;
                 uint32 raddr1,// : in  std_logic_vector((abits -1) downto 0);
                 uint32 re1,//    : in  std_ulogic;
                 uint32 &rdata1,// : out std_logic_vector((dbits -1) downto 0);
                 uint32 raddr2,// : in  std_logic_vector((abits -1) downto 0);
                 uint32 re2,//    : in  std_ulogic;
                 uint32 &rdata2// : out std_logic_vector((dbits -1) downto 0)
               );
    
    void ClkUpdate()
    {
      if(WRCLK.eClock == SClock::CLK_POSEDGE)
      {
        if(wa.Q<numregs)
        {
          if(wr.Q) memarr[wa.Q] = din.Q;
        }
      }
      ra1.ClkUpdate();
      ra2.ClkUpdate();
      wa.ClkUpdate();
      din.ClkUpdate();
      wr.ClkUpdate();
    }
};



