//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#include "stdtypes.h"
#include "dff.h"
#include "leon3_ml605\config.h"
#include "leon3_ml605\amba\amba.h"
#include "leon3_ml605\uart\uart.h"

class apbuart
{
  friend class dbg;
  private:
    uint32 pindex;
    uint32 paddr;
    uint32 pmask;
  private:
    enum rxfsmtype {r_idle, r_startbit, r_data, r_cparity, r_stopbit};
    enum txfsmtype {t_idle, t_data, t_cparity, t_stopbit};

    struct fifo
    {
      uint32 arr[CFG_APBUART_FIFOSZ]; //of std_logic_vector(7 downto 0)
    };
    struct uartregs
    {
      uint32 rxen   	:  1;//std_ulogic;	-- receiver enabled
      uint32 txen   	:  1;//std_ulogic;	-- transmitter enabled
      uint32 rirqen 	:  1;//std_ulogic;	-- receiver irq enable
      uint32 tirqen 	:  1;//std_ulogic;	-- transmitter irq enable
      uint32 parsel 	:  1;//std_ulogic;	-- parity select
      uint32 paren  	:  1;//std_ulogic;	-- parity select
      uint32 flow   	:  1;//std_ulogic;	-- flow control enable
      uint32 loopb   	:  1;//std_ulogic;	-- loop back mode enable
      uint32 debug         : 1;// std_ulogic;  -- debug mode enable
      uint32 rsempty   	:  1;//std_ulogic;	-- receiver shift register empty (internal)
      uint32 tsempty   	:  1;//std_ulogic;	-- transmitter shift register empty
      uint32 tsemptyirqen  :  1;//std_ulogic;  -- generate irq when tx shift register is empty
      uint32 Break  	:  1;//std_ulogic;	-- break detected
      uint32 breakirqen    :  1;//std_ulogic;  -- generate irq when break has been received
      uint32 ovf    	:  1;//std_ulogic;	-- receiver overflow
      uint32 parerr    	:  1;//std_ulogic;	-- parity error
      uint32 frame     	:  1;//std_ulogic;	-- framing error
      uint32 ctsn      	:  2;//std_logic_vector(1 downto 0); -- clear to send
      uint32 rtsn      	:  1;//std_ulogic;	-- request to send
      uint32 extclken  	:  1;//std_ulogic;	-- use external baud rate clock
      uint32 extclk    	:  1;//std_ulogic;	-- rising edge detect register
      fifo rhold;// 	:  fifo;
      uint32 rshift	:  8;//std_logic_vector(7 downto 0);
      uint32 tshift	:  11;//std_logic_vector(10 downto 0);
      fifo thold;// 	:  fifo;
      uint32 irq       	:  1;//std_ulogic;	-- tx/rx interrupt (internal)
      uint32 irqpend       :  1;//std_ulogic;  -- pending irq for delayed rx irq
      uint32 delayirqen    :  1;//std_ulogic;  -- enable delayed rx irq
      uint32 tpar       	:  1;//std_ulogic;	-- tx data parity (internal)
      txfsmtype txstate;//	:  txfsmtype;
      uint32 txclk 	:  3;//std_logic_vector(2 downto 0);  -- tx clock divider
      uint32 txtick     	:  1;//std_ulogic;	-- tx clock (internal)
      rxfsmtype rxstate;//	:  rxfsmtype;
      uint32 rxclk 	:  3;//std_logic_vector(2 downto 0); -- rx clock divider
      uint32 rxdb  	:  2;//std_logic_vector(1 downto 0);  -- rx delay
      uint32 dpar       	:  1;//std_ulogic;	-- rx data parity (internal)
      uint32 rxtick     	:  1;//std_ulogic;	-- rx clock (internal)
      uint32 tick     	:  1;//std_ulogic;	-- rx clock (internal)
      uint32 scaler	:  CFG_APBUART_SBITS;//std_logic_vector(sbits-1 downto 0);
      uint32 brate 	:  CFG_APBUART_SBITS;//std_logic_vector(sbits-1 downto 0);
      uint32 rxf    	:  5;//std_logic_vector(4 downto 0); --  rx data filtering buffer
      uint32 txd        	:  1;//std_ulogic;	-- transmitter data
      uint32 rfifoirqen    :  1;//std_ulogic;  -- receiver fifo interrupt enable
      uint32 tfifoirqen    :  1;//std_ulogic;  -- transmitter fifo interrupt enable
      uint32 irqcnt        :  6;//std_logic_vector(5 downto 0); -- delay counter for rx irq
     //--fifo counters
      uint32 rwaddr;//        :  std_logic_vector(log2x(fifosize) - 1 downto 0);
      uint32 rraddr;//        :  std_logic_vector(log2x(fifosize) - 1 downto 0);
      uint32 traddr;//        :  std_logic_vector(log2x(fifosize) - 1 downto 0);
      uint32 twaddr;//        :  std_logic_vector(log2x(fifosize) - 1 downto 0);
      uint32 rcnt;//          :  std_logic_vector(log2x(fifosize) downto 0);
      uint32 tcnt;//          :  std_logic_vector(log2x(fifosize) downto 0);
    };


    uartregs rin;
    TDFF<uartregs> r;//

    uint32 rdata;// : std_logic_vector(31 downto 0);
    uint32 scaler : CFG_APBUART_SBITS;//std_logic_vector(sbits-1 downto 0);
    uint32 rxclk : 3;
    uint32 txclk : 3;//std_logic_vector(2 downto 0);
    uint32 rxd   : 1;
    uint32 ctsn  : 1;//std_ulogic;
    uint32 irq;//   : std_logic_vector(NAHBIRQ-1 downto 0);
    uint32 paddress : 8;//std_logic_vector(7 downto 2);
    uartregs v;// : uartregs;
    uint32 thalffull : 1;//std_ulogic;
    uint32 rhalffull : 1;//std_ulogic;
    uint32 rfull : 1;//std_ulogic;
    uint32 tfull : 1;//std_ulogic;
    uint32 dready : 1;//std_ulogic;
    uint32 thempty : 1;//std_ulogic;
    //line L1;// : line;
    char CH;// : character;
    bool FIRST;// : boolean := true;
    //variable pt : time := 0 ns;


  public:
    apbuart(uint32 pindex_=APB_UART_CFG, uint32 paddr_=0x1, uint32 pmask_=0xfff);
    ~apbuart();

    void Update(  uint32 rst,//    : in  std_ulogic;
                  SClock clk,//    : in  std_ulogic;
                  apb_slv_in_type &in_apbi,//   : in  apb_slv_in_type;
                  apb_slv_out_type &out_apbo,//   : out apb_slv_out_type;
                  uart_in_type &in_uarti,//  : in  uart_in_type;
                  uart_out_type &out_uarto);//  : out uart_out_type);

    void ClkUpdate()
    {
      r.ClkUpdate();
    }
};
