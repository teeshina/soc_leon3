//****************************************************************************
// Author:      GNSS Sensor Limited
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#pragma once

class ahbrom
{
  friend class dbg;
  private:
    uint32 hindex;
    uint32 haddr;
    uint32 hmask;
    TDFF<uint32> addr;
  public:
    ahbrom(uint32 hindex_=AHB_SLAVE_ROM,uint32 haddr_=0x000, uint32 hmask_=0xfff)
    {
      hindex = hindex_;
      haddr = haddr_;
      hmask = hmask_;
    }
    void Update(uint32 rst,//     : in  std_ulogic;
                SClock clk,//     : in  std_ulogic;
                ahb_slv_in_type &in_ahbsi,//   : in  ahb_slv_in_type;
                ahb_slv_out_type &out_ahbso);//   : out ahb_slv_out_type
    
    void ClkUpdate()
    {
      addr.ClkUpdate();
    }

};
