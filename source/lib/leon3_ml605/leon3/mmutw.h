#pragma once

class mmutw
{
  friend class dbg;
  private:
    struct write_buffer_type      //-- write buffer 
    {
      uint32 addr;
      uint32 data;//  : std_logic_vector(31 downto 0);
      uint32 read : 1;//       : std_logic;
    };

    enum states {idle, waitm, pte_state, lv1, lv2, lv3, lv4};
    struct tw_rtype
    {
      states state;//       : states;
      write_buffer_type wb;//          : write_buffer_type;
      uint32 req         : 1;//std_logic;
      uint32 walk_op     : 1;//std_logic;
      
      //--#dump
      //-- pragma translate_off
      uint32 finish      : 1;//std_logic;
      uint32 index       ;//: std_logic_vector(31-2 downto 0);
      uint32 lvl         : 2;//std_logic_vector(1 downto 0);
      uint32 fault_mexc  : 1;//std_logic;
      uint32 fault_trans : 1;//std_logic;
      uint32 fault_lvl   : 2;//std_logic_vector(1 downto 0);
      uint32 pte : 1;
      uint32 ptd : 1;
      uint32 inv : 1;
      uint32 rvd : 1;//std_logic;
      uint32 goon : 1;
      uint32 found : 1;//std_logic;
      uint32 base;//        : std_logic_vector(31 downto 0);
      //-- pragma translate_on
    };
    tw_rtype c;// : tw_rtype;
    TDFF<tw_rtype> r;// : tw_rtype;
    
    tw_rtype v;//           : tw_rtype;
    uint32 finish      : 1;//std_logic;
    uint32 index;//       //: std_logic_vector(31-2 downto 0);
    uint32 lvl         : 2;//std_logic_vector(1 downto 0);
    uint32 fault_mexc  : 1;//std_logic;
    uint32 fault_trans : 1;//std_logic;
    uint32 fault_inv   : 1;//std_logic;
    uint32 fault_lvl   : 2;//std_logic_vector(1 downto 0);
    uint32 pte : 1;
    uint32 ptd : 1;
    uint32 inv : 1;
    uint32 rvd : 1;//std_logic;
    uint32 goon : 1;
    uint32 found : 1;// std_logic;
    uint32 base;//        : std_logic_vector(31 downto 0);
    uint32 pagesize;// : integer range 0 to 3;

  public:
    void Update(uint32 rst,//     : in  std_logic;
                SClock clk,//     : in  std_logic;
                mmctrl_type1 &mmctrl1,// : in  mmctrl_type1;
                mmutw_in_type &twi,//     : in  mmutw_in_type;
                mmutw_out_type &two,//     : out mmutw_out_type;
                memory_mm_out_type &mcmmo,//   : in  memory_mm_out_type;
                memory_mm_in_type &mcmmi);//   : out memory_mm_in_type
    
    void ClkUpdate()
    {
      r.ClkUpdate();
    }
};
