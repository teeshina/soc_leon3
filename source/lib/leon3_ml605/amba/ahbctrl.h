#pragma once

class ahbctrl
{
  friend class dbg;
  
  private:
    struct nmstarr { int arr[4]; };//[1:3]
    struct nvalarr { bool arr[4]; };//[1:3]
    
    struct reg_type
    {
      uint32 hmaster;//      : integer range 0 to nahbmx -1;
      uint32 hmasterd;//     : integer range 0 to nahbmx -1;
      uint32 hslave;//       : integer range 0 to gen_nahbs-1;
      uint32 hmasterlock  : 1;//  : std_ulogic;
      uint32 hmasterlockd : 1;// : std_ulogic;
      uint32 hready       : 1;//       : std_ulogic;
      uint32 defslv       : 1;//       : std_ulogic;
      uint32 htrans       : 2;//       : std_logic_vector(1 downto 0);
      uint32 hsize        : 3;//        : std_logic_vector(2 downto 0);
      uint32 haddr        : 16;//        : std_logic_vector(15 downto 2); 
      uint32 cfgsel       : 1;//       : std_ulogic;
      uint32 cfga11       : 1;//       : std_ulogic;
      uint32 hrdatam;//      : std_logic_vector(31 downto 0); 
      uint32 hrdatas;//      : std_logic_vector(31 downto 0);
      uint32 beat         : 4;//         : std_logic_vector(3 downto 0);
      uint32 defmst       : 1;//       : std_ulogic;
      uint32 ldefmst      : 1;//      : std_ulogic;
      uint32 lsplmst;//      : integer range 0 to nahbmx-1;
      
    };
    
    reg_type rin;// : reg_type;
    TDFF<reg_type> r;
    
    struct l0_type { uint32 arr[16]; }; //[2:0]arr[0:15]
    struct l1_type { uint32 arr[8]; };  //[3:0]arr[0:7]
    struct l2_type { uint32 arr[4]; };  //[4:0]arr[0:3]
    struct l3_type { uint32 arr[2]; };  //[5:0]arr[0:1]
    
  private:
    uint32 tz(uint32 in_vect,
              uint32 in_length);
                   
    uint32 lz(uint32 vect_in, int vec_length);


    void selmast( reg_type &in_r,//      : in reg_type;
                  ahb_mst_out_vector &in_msto,//   : in ahb_mst_out_vector;
                  uint32 in_rsplit,// : in std_logic_vector(0 to nahbmx-1);
                  uint32 &out_mast,//   : out integer range 0 to nahbmx-1;
                  uint32 &out_defmst// : out std_ulogic;
                 );
                 
    uint32 rsplitin;// : std_logic_vector(0 to nahbmx-1);  
    TDFF<uint32> rsplit;// : std_logic_vector(0 to nahbmx-1);

    //-- pragma translate_off
    ahb_mst_in_type lmsti;// : ahb_mst_in_type;
    ahb_slv_in_type lslvi;// : ahb_slv_in_type;
    //-- pragma translate_on

    reg_type v;// : reg_type;
    uint32 nhmaster, hmaster;// : integer range 0 to nahbmx -1;
    uint32 hgrant  : AHB_MASTERS_MAX;//std_logic_vector(0 to NAHBMST-1);   -- bus grant
    uint32 hsel;//    : std_logic_vector(0 to 31);   -- slave select
    uint32 hmbsel  : AHB_MEM_ID_WIDTH;//std_logic_vector(0 to NAHBAMR-1);
    uint32 nslave  : 5;//natural range 0 to 31;
    uint32 vsplit;//  : std_logic_vector(0 to nahbmx-1);
    uint32 bnslave : 4;//std_logic_vector(3 downto 0);
    uint32 area    : 2;//std_logic_vector(1 downto 0);
    uint32 hready  : 1;//std_ulogic;
    uint32 defslv  : 1;//std_ulogic;
    uint32 cfgsel  : 1;//std_ulogic;
    uint32 hcache  : 1;//std_ulogic;
    uint32 hresp   : 2;//std_logic_vector(1 downto 0);
    uint32 hrdata;//  : std_logic_vector(AHBDW-1 downto 0);
    uint32 haddr;//   : std_logic_vector(31 downto 0);
    uint32 hirq;//    : std_logic_vector(NAHBIRQ-1 downto 0);
    uint32 arb     : 1;//std_ulogic;
    uint32 hconfndx : 3;//integer range 0 to 7;
    ahb_slv_in_type vslvi;//   : ahb_slv_in_type;
    uint32 defmst;//std_ulogic;
    uint32 tmpv;//     : std_logic_vector(0 to nahbmx-1);

    
  public:
  
    void Update(
      int32 inNRst,//     : in  std_ulogic;
      SClock inClk,//     : in  std_ulogic;
      ahb_mst_in_type    &msti,//    : out ahb_mst_in_type;
      ahb_mst_out_vector &msto,//    : in  ahb_mst_out_vector;
      ahb_slv_in_type    &slvi,//    : out ahb_slv_in_type;
      ahb_slv_out_vector &slvo//    : in  ahb_slv_out_vector;
    );
    
    void ClkUpdate()
    {
      r.ClkUpdate();
      rsplit.ClkUpdate();
    }
};


