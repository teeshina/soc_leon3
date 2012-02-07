#pragma once

class ahbctrl
{
  friend class dbg;
  
  private:
    struct nmstarr { int arr[4]; };//[1:3]
    struct nvalarr { bool arr[4]; };//[1:3]
    
    struct reg_type
    {
      //int hmaster;//      : integer range 0 to nahbmx -1; rbSelMaster[0]
      //int hmasterd;//     : integer range 0 to nahbmx -1; rbSelMaster[1]
      int hslave;//       : integer range 0 to gen_nahbs-1;
      //int hmasterlock;//  : std_ulogic; rMasterLock[0]
      //int hmasterlockd;// : std_ulogic; rMasterLock[1]
      int hready;//       : std_ulogic;
      int defslv;//       : std_ulogic;
      int htrans;//       : std_logic_vector(1 downto 0);
      //int hsize;//        : std_logic_vector(2 downto 0);
      int haddr;//        : std_logic_vector(15 downto 2); 
      int cfgsel;//       : std_ulogic;
      int cfga11;//       : std_ulogic;
      int hrdatam;//      : std_logic_vector(31 downto 0); 
      int hrdatas;//      : std_logic_vector(31 downto 0);
      int beat;//         : std_logic_vector(3 downto 0);
//      int defmst;//       : std_ulogic; rDefmstEna
//      int ldefmst;//      : std_ulogic; rSplitDefmstEna
//      int lsplmst;//      : integer range 0 to nahbmx-1; rbSplitMaster
      
    };
    TDFF<reg_type> r;
    TDFF<uint32> rMasterLock[2];
    TDFF<uint32> rbSelMaster[2];
    TDFF<uint32> rDefmstEna;
    TDFF<uint32> rSplitDefmstEna;
    TDFF<uint32> rbSplit[AHB_MASTERS_MAX];
    TDFF<uint32> rbSplitMaster;
    TDFF<uint32> rbMstSize; //hsize  : std_logic_vector(2 downto 0);

    
    static const int primst = 0;// AHB_MASTERS_MAX downto 0 = [16:0]
    struct l0_type { int arr[16]; }; //[2:0]arr[0:15]
    struct l1_type { int arr[8]; };  //[3:0]arr[0:7]
    struct l2_type { int arr[4]; };  //[4:0]arr[0:3]
    struct l3_type { int arr[2]; };  //[5:0]arr[0:1]
    
  private:
    uint32 tz(uint32 inVect[],
              uint32 inVectLength);
                   
    uint32 lz(uint32 vect_in, int vec_length);

    uint32 wbAhbBusRequest[AHB_MASTERS_MAX];
    uint32 wbAhbSplit[AHB_MASTERS_MAX];
    void selmast( uint32 inAhbMaster,
                  uint32 inAhbBusRequest[],
                  uint32 inAhbSplit[],// : in std_logic_vector(0 to nahbmx-1);
                  uint32& outSelMaster,//   : out integer range 0 to nahbmx-1;
                  uint32& outDefmstEna);// : out std_ulogic)
    
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
      rMasterLock[0].ClkUpdate();
      rMasterLock[1].ClkUpdate();
      rbSelMaster[0].ClkUpdate();
      rbSelMaster[1].ClkUpdate();
      rDefmstEna.ClkUpdate();
      rSplitDefmstEna.ClkUpdate();
      for (uint32 i=0; i<AHB_MASTERS_MAX; i++)
        rbSplit[i].ClkUpdate(); 
      rbSplitMaster.ClkUpdate();
      rbMstSize.ClkUpdate();
    }
};


