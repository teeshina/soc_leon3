#pragma once

// set by CFG_MCTRL_SDEN
#define SDRAM_ENABLE

class mctrl
{
  friend class dbg;
  private:
    uint32 hindex;//    : integer := 0;
    uint32 pindex;//    : integer := 0;
    uint32 romaddr;//   : integer := 16#000#;
    uint32 rommask;//   : integer := 16#E00#;
    uint32 ioaddr;//    : integer := 16#200#;
    uint32 iomask;//    : integer := 16#E00#;
    uint32 ramaddr;//   : integer := 16#400#;
    uint32 rammask;//   : integer := 16#C00#;
    uint32 paddr;//     : integer := 0;
    uint32 pmask;//     : integer := 16#fff#;
    uint32 wprot;//     : integer := 0;
    uint32 invclk;//    : integer := 0;
    uint32 fast;//      : integer := 0;
    uint32 romasel;//   : integer := 28;
    static const uint32 SDRASEL=29;//sdrasel;//   : integer := 29;
    uint32 srbanks;//   : integer := 4;
    uint32 ram8;//      : integer := 0;
    uint32 ram16;//     : integer := 0;
//SH    uint32 sden;//      : integer := 0;
    uint32 sepbus;//    : integer := 0;
    uint32 sdbits;//    : integer := 32;
    uint32 sdlsb;//     : integer := 2;          -- set to 12 for the GE-HPE board
    uint32 oepol;//     : integer := 0;
    uint32 syncrst;//   : integer := 0;
    uint32 pageburst;// : integer := 0;
    uint32 scantest;//  : integer := 0;
    uint32 mobile;//    : integer := 0
  
    ahb_config_type hconfig;
    apb_config_type pconfig;
    
    static const int REVISION = 1;
    static const int prom     = 1;//: integer := 1;
    static const int memory   = 0;//: integer := 0;

    static const int rom = 0;//: integer := 0;
    static const int io  = 1;//: integer := 1;
    static const int ram = 2;//: integer := 2;
    enum memcycletype {idle, berr, bread, bwrite, bread8, bwrite8, bread16, bwrite16};

    bool RAMSEL5;// : boolean := srbanks = 5;
//    bool SDRAMEN;// : boolean := (sden /= 0);
    bool BUS16EN;// : boolean := (ram16 /= 0);
    bool BUS8EN;//  : boolean := (ram8 /= 0);
    bool WPROTEN;// : boolean := (wprot /= 0);
    bool WENDFB;//  : boolean := false;
    bool SDSEPBUS;//: boolean := (sepbus /= 0);
    bool BUS64;//   : boolean := (sdbits = 64);

  private:
    // memory configuration register 1 type
    struct mcfg1type
    {
      uint32 romrws;//           : std_logic_vector(3 downto 0);
      uint32 romwws;//           : std_logic_vector(3 downto 0);
      uint32 romwidth;//         : std_logic_vector(1 downto 0);
      uint32 romwrite;//         : std_logic;

      uint32 ioen;//             : std_logic;
      uint32 iows;//             : std_logic_vector(3 downto 0);
      uint32 bexcen;//           : std_logic;
      uint32 brdyen;//           : std_logic;
      uint32 iowidth;//          : std_logic_vector(1 downto 0);
    };

    // memory configuration register 2 type
    struct mcfg2type
    {
      uint32 ramrws;//           : std_logic_vector(1 downto 0);
      uint32 ramwws;//           : std_logic_vector(1 downto 0);
      uint32 ramwidth;//         : std_logic_vector(1 downto 0);
      uint32 rambanksz;//        : std_logic_vector(3 downto 0);
      uint32 rmw;//              : std_logic;
      uint32 brdyen;//           : std_logic;
      uint32 srdis;//            : std_logic;
      uint32 sdren;//            : std_logic;
    };

    // memory status register type
    
    // local registers
    struct reg_type
    {
      uint32 address;//          : std_logic_vector(31 downto 0);  -- memory address
      uint32 data;//             : std_logic_vector(31 downto 0);  -- latched memory data
      uint32 writedata;//        : std_logic_vector(31 downto 0);
      uint32 writedata8;//       : std_logic_vector(15 downto 0);  -- lsb write data buffer
      uint64 sdwritedata;//      : std_logic_vector(63 downto 0);
      uint32 readdata;//         : std_logic_vector(31 downto 0);
      uint32 brdyn;//            : std_logic;
      uint32 ready;//            : std_logic;
      uint32 ready8;//           : std_logic;
      uint32 bdrive;//           : std_logic_vector(3 downto 0);
      uint32 nbdrive;//          : std_logic_vector(3 downto 0);
      uint32 ws;//               : std_logic_vector(3 downto 0);
      uint32 romsn;//      : std_logic_vector(1 downto 0);
      uint32 ramsn;//      : std_logic_vector(4 downto 0);
      uint32 ramoen;//     : std_logic_vector(4 downto 0);
      uint32 size;//       : std_logic_vector(1 downto 0);
      uint32 busw;//       : std_logic_vector(1 downto 0);
      uint32 oen;//              : std_logic;
      uint32 iosn;//       : std_logic_vector(1 downto 0);
      uint32 read;//             : std_logic;
      uint32 wrn;//              : std_logic_vector(3 downto 0);
      uint32 writen;//           : std_logic;
      memcycletype bstate;//           : memcycletype;
      uint32 area;//       : std_logic_vector(0 to 2);
      mcfg1type mcfg1;//      : mcfg1type;
      mcfg2type mcfg2;//      : mcfg2type;
      uint32 bexcn;//            : std_logic;   -- latched external bexcn
      uint32 echeck;//           : std_logic;
      uint32 brmw;//             : std_logic;
      uint32 haddr;//            : std_logic_vector(31 downto 0);
      uint32 hsel;//             : std_logic;
      uint32 srhsel;//           : std_logic;
      uint32 sdhsel;//           : std_logic;
      uint32 hwrite;//           : std_logic;
      uint32 hburst;//           : std_logic_vector(2 downto 0);
      uint32 htrans;//           : std_logic_vector(1 downto 0);
      uint32 hresp;//      : std_logic_vector(1 downto 0);
      uint32 sa;//         : std_logic_vector(14 downto 0);
      uint64 sd;//         : std_logic_vector(63 downto 0);
      uint32 mben;//         : std_logic_vector(3 downto 0);
    };


    TDFF<reg_type> r;
    reg_type ri;
    sdram_mctrl_out_type  sdmo;// : sdram_mctrl_out_type;
    sdram_in_type         sdi;//  : sdram_in_type;
    sdram_out_type        lsdo;//   : sdram_out_type;

    // vectored output enable to data pads 
    TDFF<uint32>rbdrive;// : std_logic_vector(31 downto 0);
    uint32 ribdrive;// : std_logic_vector(31 downto 0);
    TDFF<uint64>rrsbdrive;// : std_logic_vector(63 downto 0);
    TDFF<uint64>rsbdrive;// : std_logic_vector(63 downto 0);
    uint64 risbdrive;// : std_logic_vector(63 downto 0);
    uint32 arst;//     : std_ulogic;

  private:
    reg_type v;// : reg_type;    -- local variables for registers
    uint32 start;// : std_logic;
    uint32 dataout;// : std_logic_vector(31 downto 0); -- data from memory
    uint32 regsd;// : std_logic_vector(31 downto 0);   -- data from registers
    uint32 memdata;// : std_logic_vector(31 downto 0);   -- data to memory
    uint32 rws;// : std_logic_vector(3 downto 0);    -- read waitstates
    uint32 wws;// : std_logic_vector(3 downto 0);    -- write waitstates
    uint32 wsnew;// : std_logic_vector(3 downto 0);    -- write waitstates
    uint32 adec;// : std_logic_vector(1 downto 0);
    uint32 rams;// : std_logic_vector(4 downto 0);
    uint32 bready, leadin;// : std_logic;
    uint32 csen;// : std_logic;      -- Generate chip selects
    uint32 aprot;//   : std_logic_vector(14 downto 0); --
    uint32 wrn;//   : std_logic_vector(3 downto 0); --
    uint32 bexc, addrerr;// : std_logic;
    uint32 ready;// : std_logic;
    uint32 writedata;// : std_logic_vector(31 downto 0);
    uint32 bwdata;// : std_logic_vector(31 downto 0);
    uint32 merrtype;//  : std_logic_vector(2 downto 0); -- memory error type
    uint32 noerror;// : std_logic;
    uint32 area;//  : std_logic_vector(0 to 2);
    uint32 bdrive;// : std_logic_vector(3 downto 0);
    uint32 ramsn;// : std_logic_vector(4 downto 0);
    uint32 romsn, busw;// : std_logic_vector(1 downto 0);
    uint32 iosn;// : std_logic;
    uint32 lock;// : std_logic;
    uint32 wprothitx;// : std_logic;
    uint32 brmw;// : std_logic;
    uint32 bidle;//: std_logic;
    uint32 haddr;//   : std_logic_vector(31 downto 0);
    uint32 hsize;//   : std_logic_vector(1 downto 0);
    uint32 hwrite;//  : std_logic;
    uint32 hburst;//  : std_logic_vector(2 downto 0);
    uint32 htrans;//  : std_logic_vector(1 downto 0);
    uint32 sdhsel, srhsel, hready;//  : std_logic;
    uint32 vbdrive;// : std_logic_vector(31 downto 0);
    uint64 vsbdrive;// : std_logic_vector(63 downto 0);
    uint32 bdrive_sel;// : std_logic_vector(3 downto 0);
    uint32 haddrsel;//   : std_logic_vector(31 downto 13);


  public:
    mctrl()
    {
      memset((void*)&hconfig, 0, sizeof(ahb_config_type));
      memset((void*)&pconfig, 0, sizeof(apb_config_type));
      
      ((ahb_device_reg*)&hconfig.arr[0])->vendor  = VENDOR_ESA;
      ((ahb_device_reg*)&hconfig.arr[0])->device  = ESA_MCTRL;
      ((ahb_device_reg*)&hconfig.arr[0])->version = REVISION;
      
      ((ahb_membar_type*)&hconfig.arr[4])->memaddr  = romaddr;
      ((ahb_membar_type*)&hconfig.arr[4])->prefetch = 1;
      ((ahb_membar_type*)&hconfig.arr[4])->cache    = 1;
      ((ahb_membar_type*)&hconfig.arr[4])->addrmask = rommask;
      ((ahb_membar_type*)&hconfig.arr[4])->one      = 1;

      ((ahb_membar_type*)&hconfig.arr[5])->memaddr  = ioaddr;
      ((ahb_membar_type*)&hconfig.arr[5])->prefetch = 0;
      ((ahb_membar_type*)&hconfig.arr[5])->cache    = 0;
      ((ahb_membar_type*)&hconfig.arr[5])->addrmask = iomask;
      ((ahb_membar_type*)&hconfig.arr[5])->one      = 1;

      ((ahb_membar_type*)&hconfig.arr[6])->memaddr  = ramaddr;
      ((ahb_membar_type*)&hconfig.arr[6])->prefetch = 1;
      ((ahb_membar_type*)&hconfig.arr[6])->cache    = 1;
      ((ahb_membar_type*)&hconfig.arr[6])->addrmask = rammask;
      ((ahb_membar_type*)&hconfig.arr[6])->one      = 1;


      ((ahb_device_reg*)&pconfig.arr[0])->vendor  = VENDOR_ESA;
      ((ahb_device_reg*)&pconfig.arr[0])->device  = ESA_MCTRL;
      ((ahb_device_reg*)&pconfig.arr[0])->version = REVISION;
      
      ((apb_iobar_type*)&pconfig.arr[1])->memaddr   = paddr;
      ((apb_iobar_type*)&pconfig.arr[1])->addrmask  = pmask;
      ((apb_iobar_type*)&pconfig.arr[1])->one       = 1;
      
      RAMSEL5   = (srbanks == 5);
//      SDRAMEN   = (sden != 0);
      BUS16EN   = (ram16 != 0);
      BUS8EN    = (ram8 != 0);
      WPROTEN   = (wprot != 0);
      WENDFB    = false;
      SDSEPBUS  = (sepbus != 0);
      BUS64     = (sdbits == 64);
    }
    
    void Update(uint32 rst,
                uint32 clk,
                memory_in_type memi,//      : in  memory_in_type;
                memory_out_type& memo,//      : out memory_out_type;
                ahb_slv_in_type ahbsi,//     : in  ahb_slv_in_type;
                ahb_slv_out_type& ahbso,//     : out ahb_slv_out_type;
                apb_slv_in_type apbi,//      : in  apb_slv_in_type;
                apb_slv_out_type& apbo,//      : out apb_slv_out_type;
                wprot_out_type wpo,//       : in  wprot_out_type;
                sdram_out_type& sdo//       : out sdram_out_type

                );
    void ClkUpdate()
    {
      r.ClkUpdate();
      rbdrive.ClkUpdate();
      rrsbdrive.ClkUpdate();
      rsbdrive.ClkUpdate();
    }
};
