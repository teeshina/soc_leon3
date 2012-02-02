#include "lheaders.h"

//#define tech;//          : integer := 0;
//#define arch       CFG_MULARCH//          : integer := 0;
#define a_width 33//       : positive := 2;                      -- multiplier word width
#define b_width 33//       : positive := 2;                      -- multiplicand word width
#define num_stages (CFG_MULPIPE+1)//    : natural := 2;                 -- number of pipeline stages
#define stall_mode CFG_MULPIPE//    : natural range 0 to 1 := 1      -- '0': non-stallable; '1': stallable
 

//******************************************************************
void gen_mult_pipe::Update( SClock clk,//     : in  std_logic;          -- register clock
                            uint32 en,//      : in  std_logic;          -- register enable
                            uint32 TC,//      : in  std_logic;          -- '0' : unsigned, '1' : signed
                            uint64 A,//       : in  std_logic_vector(a_width-1 downto 0);  -- multiplier
                            uint64 B,//       : in  std_logic_vector(b_width-1 downto 0);  -- multiplicand
                            uint64 *product)// : out std_logic_vector(a_width+b_width-1 downto 0));  -- product
{
  // 33 bits * 33 bits = 66 bits
  if (TC == 1)
  {
    // WARNING !!!! maximal signed value is 65 bits (not 66 as in a case of unsiged operation)
    //              int33(A) * int33(B) = int66(prod)
    int64 x1 = BITS64(A, a_width-1,0);
    if(x1&0x100000000) x1 |= 0xFFFFFFFE00000000;
    int64 x2 = BITS64(B, b_width-1,0);
    if(x2&0x100000000) x2 |= 0xFFFFFFFE00000000;
    int64 tmpa = x1*(x2&0x3);
    int64 tmpb = x1*(x2>>2);
    prod[1] = ((tmpa>>2) + tmpb)>>62;
    prod[0] = (((tmpa>>2) + tmpb)<<2) + (tmpa&0x3);
    prod[1] &= 0x3; // 64 + 2 bits. [65:64] bits are always identical
  }else
  {
    // prod[0] = A * B;
    uint64 x1 = BITS64(A, a_width-1,0);
    uint64 x2 = BITS64(B, b_width-1,0);
    uint64 tmpa = x1*(x2&0x3);
    uint64 tmpb = x1*(x2>>2);
    prod[1] = ((tmpa>>2) + tmpb)>>62;
    prod[0] = (((tmpa>>2) + tmpb)<<2) + (tmpa&0x3);
  }

  p_i.CLK = clk;
#if (num_stages == 2)
  if((stall_mode==0) || (en==1))
  {
    p_i.D.arr2[1][0] = prod[0];
    p_i.D.arr2[1][1] = prod[1];
  }
#elif (num_stages > 2)
  if ((CFG_MULPIPE == 0) || (en == 1))
  {
    for (int32 i=1; i<=CFG_MULPIPE; i++)
    {
      p_i.D.arr2[i][0] = p_i.Q.arr2[i-1][0] ;
      p_i.D.arr2[i][1] = p_i.Q.arr2[i-1][1];
    }
    p_i.D.arr2[0][0] =  prod[0];
    p_i.D.arr2[0][1] = prod[1];
  }
#endif

  product[0] = p_i.Q.arr2[num_stages-1][0];
  product[1] = p_i.Q.arr2[num_stages-1][1];
}

//******************************************************************
void techmult::Update(uint64 a,//       : in std_logic_vector(a_width-1 downto 0);  
                      uint64 b,//       : in std_logic_vector(b_width-1 downto 0);
                      SClock clk,//     : in std_logic;     
                      uint32 en,//      : in std_logic;     
                      uint32 sign,//    : in std_logic;     
                      uint64 *product)// : out std_logic_vector(a_width+b_width-1 downto 0));
{

  
  pres  = 0;
  sonly = 0;
  gnd = 0;
  vcc = 1;

#if (num_stages == 1)
  
#if (CFG_MULARCH == 0)//--inferred
      mixed_mul(a, b, sign, product);
      pres = 1;
#elif (CFG_MULARCH==1)-- modgen
      m1717 : if (a_width = 17) and (b_width = 17) generate
        m17 : mul_17_17 generic map (mulpipe => 0)
          port map (clk, vcc, a, b, product);
        pres <= '1'; sonly <= '1';
      end generate;
      m3317 : if (a_width = 33) and (b_width = 17) generate
        m33 : mul_33_17 port map (a, b, product);
        pres <= '1'; sonly <= '1';
      end generate;
      m339 : if (a_width = 33) and (b_width = 9) generate
        m33 : mul_33_9 port map (a, b, product);
        pres <= '1'; sonly <= '1';
      end generate;
      m3333 : if (a_width = 33) and (b_width = 33) generate
        m33 : mul_33_33 generic map (mulpipe => 0)
    port map (clk, vcc, a, b, product);
        pres <= '1'; sonly <= '1';
      end generate;
      mgen  : if not(((a_width = 17) and (b_width = 17)) or
                     ((a_width = 33) and (b_width = 33)) or
                     ((a_width = 33) and (b_width = 17)) or
                     ((a_width = 33) and (b_width = 9)))
      generate
        product <= mixed_mul(a, b, sign);
        pres <= '1';
      end generate;
#elif (CFG_MULARCH==2)//--techspec
    axd : if (tech = axdsp) and (a_width = 33) and (b_width = 33) generate
         m33 : axcel_mul_33x33_signed generic map (pipe => 0)
    port map (a, b, vcc, clk, product);
        pres <= '1'; sonly <= '1';
      end generate;
#elif(CFG_MULARCH==3) //-- designware

      dwm : mul_dw
        generic map (a_width => a_width, b_width => b_width, 
          num_stages => 1, stall_mode => 0)
          port map (a => a, b => b, clk => clk, en => en, sign => sign, product => product);
        pres <= '1';
    end generate;
#endif

#elif (num_stages == 2)
  #if (CFG_MULARCH == 0) //-- inferred
    dwm.Update(clk, en, sign, a, b, product);
    pres = 1;
  #else
    arch1 : if (arch = 1) generate  -- modgen
      m1717 : if (a_width = 17) and (b_width = 17) generate
        m17 : mul_17_17 generic map (mulpipe => 1)
    port map (clk, en, a, b, product);
        pres <= '1'; sonly <= '1';
      end generate;
      m3333 : if (a_width = 33) and (b_width = 33) generate
        m33 : mul_33_33 generic map (mulpipe => 1)
    port map (clk, en, a, b, product);
        pres <= '1'; sonly <= '1';
      end generate;
    end generate;
    arch2 : if (arch = 2) generate  --techspec
    axd : if (tech = axdsp) and (a_width = 33) and (b_width = 33) generate
         m33 : axcel_mul_33x33_signed generic map (pipe => 1)
    port map (a, b, en, clk, product);
        pres <= '1'; sonly <= '1';
      end generate;
    end generate;
    arch3 : if (arch = 3) generate  -- designware
      dwm : mul_dw
        generic map (a_width => a_width, b_width => b_width, 
    num_stages => num_stages, stall_mode => stall_mode)
        port map (a => a, b => b, clk => clk, en => en, sign => sign, 
      product => product);
        pres <= '1';
    end generate;
  end generate;
  #endif
#elif (num_stages > 2)

#if (CFG_MULARCH == 0) //-- inferred
      //generic map (a_width => a_width, b_width => b_width, num_stages => num_stages, stall_mode => stall_mode)
      dwm.Update(clk, en, sign, a, b, product);
      
      pres = 1;
#elif (CFG_MULARCH == 3) //-- designware
      dwm : mul_dw
        generic map (a_width => a_width, b_width => b_width, 
    num_stages => num_stages, stall_mode => stall_mode)
        port map (a => a, b => b, clk => clk, en => en, sign => sign, 
      product => product);
        pres <= '1';
#endif

#endif

};




