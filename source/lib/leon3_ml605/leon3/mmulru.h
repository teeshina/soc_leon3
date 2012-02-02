#pragma once

class mmulru
{
  friend class dbg;
  private:
    uint32 entries;
  private:

    struct lru_rtype
    {
      uint32 bar   : 2;//std_logic_vector(1 downto 0);
      uint32 clear;// : std_logic_vector(M_ENT_MAX-1 downto 0);
      
      //-- pragma translate_off
      uint32 reinit : 1;//std_logic;
      uint32 pos;//    : std_logic_vector(entries_log-1 downto 0);
      //-- pragma translate_on
    };
    
    lru_rtype c;//   : lru_rtype;
    TDFF<lru_rtype> r;//   : lru_rtype;
    mmulruei_a *lruei;// : mmulruei_a (entries-1 downto 0);
    mmulrueo_a *lrueo;// : mmulrueo_a (entries-1 downto 0);

    lru_rtype v;// : lru_rtype;
    uint32 reinit : 1;//std_logic;
    uint32 v_lruei_clk : 1;//std_logic;
    uint32 pos;// : std_logic_vector(entries_log-1 downto 0);
    uint32 touch : 1;// std_logic;
    
    mmulrue *l1[M_ENT_MAX];

  public:
    mmulru(uint32 entries_=8);
    ~mmulru();
  
    void Update(uint32 rst,//   : in std_logic;
                SClock clk,//   : in std_logic;
                mmulru_in_type &lrui,//  : in mmulru_in_type;
                mmulru_out_type &lruo);//  : out mmulru_out_type 
    
    void ClkUpdate()
    {
      r.ClkUpdate();
      for (int32 i=entries-1; i>=0; i--)
        l1[i]->ClkUpdate();
    }
};
