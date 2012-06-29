//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#pragma once

#include "stdtypes.h"
#include "dff.h"
#include "leon3_ml605\config.h"
#include "leon3_ml605\amba\amba.h"

class apbctrl
{
  friend class dbg;
  private:
    uint32 hindex;
    uint32 haddr;
    uint32 hmask;
    uint32 IOAREA;
    uint32 IOMSK;
    #define apbmax  19
    #define VERSION 0

  private:
    struct reg_type
    {
      uint32 haddr   : apbmax+1;//std_logic_vector(apbmax downto 0);   -- address bus
      uint32 hwrite  : 1;//std_logic;  		     -- read/write
      uint32 hready  : 1;//std_logic;  		     -- ready
      uint32 penable : 1;//std_logic;
      uint32 psel    : 1;//std_logic;
      uint32 prdata;//  : std_logic_vector(31 downto 0);   -- read data
      uint32 pwdata;//  : std_logic_vector(31 downto 0);   -- write data
      uint32 state   : 2;//std_logic_vector(1 downto 0);   -- state
      uint32 cfgsel  : 1;//std_ulogic;
    };

    reg_type rin;// : reg_type;
    TDFF<reg_type> r;
    apb_slv_in_type lapbi;//  : apb_slv_in_type;

    reg_type v;// : reg_type;
    uint32 psel[32];//   : std_logic_vector(0 to 31);   
    uint32 pwdata;// : std_logic_vector(31 downto 0);
    uint32 apbaddr : apbmax+1;//std_logic_vector(apbmax downto 0);
    uint32 apbaddr2;// : std_logic_vector(31 downto 0);
    uint32 pirq;// : std_logic_vector(NAHBIRQ-1 downto 0);
    uint32 nslave  : APB_SLAVES_MAX;// : integer range 0 to nslaves-1;
    uint32 bnslave[4];//std_logic_vector(3 downto 0);

  public:
    apbctrl(uint32 hind_=AHB_SLAVE_APBBRIDGE, uint32 addr_=0x000, uint32 mask_=0xfff);
    ~apbctrl();

    void Update( uint32 rst,//     : in  std_ulogic;
                 SClock clk,//     : in  std_ulogic;
                 ahb_slv_in_type in_ahbi,//    : in  ahb_slv_in_type;
                 ahb_slv_out_type &out_ahbo,//    : out ahb_slv_out_type;
                 apb_slv_in_type &out_apbi,//    : out apb_slv_in_type;
                 apb_slv_out_vector &in_apbo );//    : in  apb_slv_out_vector

    void ClkUpdate()
    {
      r.ClkUpdate();
    }
};
