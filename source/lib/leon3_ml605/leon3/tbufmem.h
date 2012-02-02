#pragma once



class tbufmem
{
  friend class dbg;
  private:
    uint32 kbytes;
    uint32 ADDRBITS;
    uint32 enable;// : std_logic_vector(1 downto 0);
    syncram64 *pclRam64[2];
  public:
    tbufmem(uint32 kbytes_=4);
    ~tbufmem();
    
    void Update(SClock clk,// : in std_ulogic;
                tracebuf_in_type &tbi,//  : in tracebuf_in_type;
                tracebuf_out_type &tbo);
    
    
    void ClkUpdate()
    {
      pclRam64[0]->ClkUpdate();
      pclRam64[1]->ClkUpdate();
    }
};

