//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

class irqmp
{
  friend class dbg;
  private:
    uint32 pindex;
    uint32 paddr;
    uint32 pmask;
  private:
    struct mask_type
    {
      uint32 arr[CFG_NCPU];//(0 to ncpu-1) of std_logic_vector(15 downto 1);
    };
    struct mask2_type
    {
      uint32 arr[CFG_NCPU];// is array (0 to ncpu-1) of std_logic_vector(15 downto 0);
    };
    struct irl_type
    {
      uint32 arr[CFG_NCPU];// is array (0 to ncpu-1) of std_logic_vector(3 downto 0);
    };
    struct irl2_type
    {
      uint32 arr[CFG_NCPU];// is array (0 to ncpu-1) of std_logic_vector(4 downto 0);
    };

    struct reg_type
    {
      mask_type imask;//		: mask_type;
      uint32 ilevel	  : 16;//std_logic_vector(15 downto 1);
      uint32 ipend		: 16;// std_logic_vector(15 downto 1);
      mask_type iforce;//	: mask_type;
      uint32 ibroadcast	: 16;//std_logic_vector(15 downto 1);
      irl_type irl;//    	: irl_type;
      uint32 cpurst	  : CFG_NCPU;//std_logic_vector(ncpu-1 downto 0);
    };

    struct ereg_type
    {
      mask2_type imask;//		: mask2_type;
      uint32 ipend		: 16;//std_logic_vector(15 downto 0);
      irl2_type irl;//    	: irl2_type;
    };
    
    reg_type rin;// : reg_type;
    TDFF<reg_type> r;
    ereg_type r2in;// : ereg_type;
    TDFF<ereg_type> r2;


    reg_type v;// : reg_type;
    mask_type temp;// : mask_type;
    uint32 prdata;// : std_logic_vector(31 downto 0);
    uint32 tmpirq : 16;//std_logic_vector(15 downto 0);
    uint32 tmpvar : 16;//std_logic_vector(15 downto 1);
    uint32 cpurun	: CFG_NCPU;//std_logic_vector(ncpu-1 downto 0);
    ereg_type v2;// : ereg_type;
    uint32 irl2 : 4;//std_logic_vector(3 downto 0);
    uint32 ipend2 : CFG_NCPU;//std_logic_vector(ncpu-1 downto 0);
    mask2_type temp2;// : mask2_type;
    uint32 neirq;// : integer;

  
  public:
    irqmp(uint32 pindex_=APB_IRQ_CONTROL, uint32 paddr_=2, uint32 pmask_=0xfff);
  
    uint32 prioritize(uint32 b);// : std_logic_vector(15 downto 0)) return std_logic_vector
  
    void Update( uint32 rst,//    : in  std_ulogic;
                 SClock clk,//    : in  std_ulogic;
                 apb_slv_in_type &apbi,//   : in  apb_slv_in_type;
                 apb_slv_out_type &apbo,//   : out apb_slv_out_type;
                 irq_out_vector &irqi,//   : in  irq_out_vector(0 to ncpu-1);
                 irq_in_vector &irqo);//   : out irq_in_vector(0 to ncpu-1)
                 
    void ClkUpdate()
    {
      r.ClkUpdate();
      r2.ClkUpdate();
    }
};
