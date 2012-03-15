//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#pragma once

struct gptimer_in_type
{
  uint32 dhalt;//    : std_ulogic;
  uint32 extclk;//   : std_ulogic;
  uint32 wdogen;//   : std_ulogic;
};


struct gptimer_out_type
{
  uint32 tick;//     : std_logic_vector(0 to 7);
  uint32 timer1;//   : std_logic_vector(31 downto 0);
  uint32 wdogn;//    : std_ulogic;
  uint32 wdog;//    : std_ulogic;
};


class gptimer
{
  friend class dbg;
  private:
    uint32 pindex;
    uint32 paddr;
    uint32 pmask;
  private:
    struct timer_reg
    {
      uint32 enable	: 1;// std_ulogic;	-- enable counter
      uint32 load		: 1;// std_ulogic;	-- load counter
      uint32 restart	: 1;// std_ulogic;	-- restart counter
      uint32 irqpen  	: 1;// std_ulogic;	-- interrupt pending
      uint32 irqen   	: 1;// std_ulogic;	-- interrupt enable
      uint32 irq   	: 1;// std_ulogic;	-- interrupt pulse
      uint32 chain 	: 1;// std_ulogic;	-- chain with previous timer
      uint32 value;// 	:  std_logic_vector(nbits-1 downto 0);
      uint32 reload;//	:  std_logic_vector(nbits-1 downto 0);
    };

    struct timer_reg_vector
    {
      timer_reg arr[CFG_GPT_NTIM+1];// (Natural range <> ) of timer_reg;
    };

    struct registers
    {
      uint32 scaler : CFG_GPT_SW;//	:  std_logic_vector(sbits-1 downto 0);
      uint32 reload : CFG_GPT_SW;// 	:  std_logic_vector(sbits-1 downto 0);
      uint32 tick   : 1;//         :  std_ulogic;
      uint32 tsel;//   	:  integer range 0 to ntimers;
      timer_reg_vector timers;//	:  timer_reg_vector(1 to ntimers);
      uint32 dishlt : 1;//       :  std_ulogic;   
      uint32 wdogn  : 1;//       :  std_ulogic;   
      uint32 wdog   : 1;//      :  std_ulogic;   
    };

    registers rin;
    TDFF<registers> r;


    uint32 scaler : CFG_GPT_SW+1;// : std_logic_vector(sbits downto 0);
    uint32 readdata;
    uint32 timer1;// : std_logic_vector(31 downto 0);
    uint32 res;
    uint32 addin;// : std_logic_vector(nbits-1 downto 0);
    registers v;// : registers;
    uint32 z : 1;//std_ulogic;
    timer_reg_vector vtimers;// : timer_reg_vector(0 to ntimers);
    uint32 xirq;// : std_logic_vector(NAHBIRQ-1 downto 0);
    uint32 nirq : CFG_GPT_NTIM;// : std_logic_vector(0 to ntimers-1);
    uint32 tick : 8;//std_logic_vector(1 to 7);

  public:
    gptimer(uint32 pindex_=APB_TIMER, uint32 paddr_=3, uint32 pmask_=0xfff);
  
  
    void Update( uint32 rst,//    : in  std_ulogic;
                 SClock clk,//    : in  std_ulogic;
                 apb_slv_in_type &apbi,//   : in  apb_slv_in_type;
                 apb_slv_out_type &apbo,//   : out apb_slv_out_type;
                 gptimer_in_type &gpti,//   : in  gptimer_in_type;
                 gptimer_out_type &gpto);//   : out gptimer_out_type
                 
    void ClkUpdate()
    {
      r.ClkUpdate();
    }
};
