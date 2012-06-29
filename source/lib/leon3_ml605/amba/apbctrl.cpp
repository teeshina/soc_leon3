//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GNU2
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************
#include "id.h"
#include "apbctrl.h"

//#define hindex      : integer := 0;
//#define haddr       : integer := 0;
//#define hmask       : integer := 16#fff#;
//#define nslaves     APB_SLAVES_MAX//: integer range 1 to NAPBSLV := NAPBSLV;
//#define debug       : integer range 0 to 2 := 2;
//#define icheck      : integer range 0 to 1 := 1;
//#define enbusmon    0//: integer range 0 to 1 := 0;
//#define asserterr   : integer range 0 to 1 := 0;
//#define assertwarn  : integer range 0 to 1 := 0;
//#define pslvdisable : integer := 0;
//#define mcheck      : integer range 0 to 1 := 1;
//#define ccheck      : integer range 0 to 1 := 1

//****************************************************************************
apbctrl::apbctrl(uint32 hind_, uint32 addr_, uint32 mask_)
{
  hindex = hind_;
  haddr = addr_;
  hmask = mask_;
  IOAREA = BITS32(haddr,11,0);
  IOMSK  = BITS32(hmask,11,0);
}
apbctrl::~apbctrl()
{
}

//****************************************************************************
void apbctrl::Update( uint32 rst,//     : in  std_ulogic;
                       SClock clk,//     : in  std_ulogic;
                       ahb_slv_in_type ahbi,//    : in  ahb_slv_in_type;
                       ahb_slv_out_type &ahbo,//    : out ahb_slv_out_type;
                       apb_slv_in_type &apbi,//    : out apb_slv_in_type;
                       apb_slv_out_vector &apbo )//    : in  apb_slv_out_vector
{
  ((ahb_device_reg*)(&ahbo.hconfig.arr[0]))->vendor  = 1;
  ((ahb_device_reg*)(&ahbo.hconfig.arr[0]))->device  = 6;
  ((ahb_device_reg*)(&ahbo.hconfig.arr[0]))->version = VERSION;
  ((ahb_device_reg*)(&ahbo.hconfig.arr[0]))->cfgver  = 0;
  ((ahb_device_reg*)(&ahbo.hconfig.arr[0]))->interrupt = 0;
  ((ahb_membar_type*)(&ahbo.hconfig.arr[4]))->memaddr  = haddr;
  ((ahb_membar_type*)(&ahbo.hconfig.arr[4]))->addrmask = hmask;
  ((ahb_membar_type*)(&ahbo.hconfig.arr[4]))->area_id  = CFGAREA_TYPE_AHB_MEM;
  ((ahb_membar_type*)(&ahbo.hconfig.arr[4]))->prefetch = 0;
  ((ahb_membar_type*)(&ahbo.hconfig.arr[4]))->cache    = 0;


  v = r.Q;
  v.psel = 0;
  v.penable = 0;
  for(int32 i=0; i<32; i++) psel[i] = 0;
  pirq = 0; 

  //-- detect start of cycle
  if (ahbi.hready)
  {
    if( ((ahbi.htrans==HTRANS_NONSEQ) || (ahbi.htrans==HTRANS_SEQ)) && BIT32(ahbi.hsel,hindex) )
    {
      v.hready = 0;
      v.hwrite = ahbi.hwrite; 
      v.haddr &= ~MSK32(apbmax,0);
      v.haddr |= BITS32(ahbi.haddr,apbmax,0); 
      v.state = 0x1;
      v.psel  = !ahbi.hwrite;
    }
  }
  
  switch(r.Q.state)
  {
    case 0: break;		//-- idle
    case 1:
      if (r.Q.hwrite == 0) v.penable = 1; 
      else                 v.pwdata = ahbi.hwdata;//ahbreadword(ahbi.hwdata, r.haddr(4 downto 2)); end if;
      v.psel  = 1;
      v.state = 0x2;
    break;
    default:
      if (r.Q.penable == 0)
      {
        v.psel = 1;
        v.penable = 1;
      }
      v.state = 0;
      v.hready = 1;
  }

  for(int32 i=0; i<APB_SLAVES_MAX; i++) psel[i] = 0;
  
  for (int32 i=0; i<=APB_SLAVES_MAX-1; i++)
  {
    if((BITS32(apbo.arr[i].pconfig.arr[1],1,0)==0x1) &&
      ((BITS32(apbo.arr[i].pconfig.arr[1],31,20) & BITS32(apbo.arr[i].pconfig.arr[1],15,4))== 
      (BITS32(r.Q.haddr,19,8) & BITS32(apbo.arr[i].pconfig.arr[1],15,4))))
    {
      psel[i] = 1;
    }
  }
  
  bnslave[0] = psel[1] | psel[3] | psel[5] | psel[7] |
               psel[9] | psel[11] | psel[13] | psel[15];
  bnslave[1] = psel[2] | psel[3] | psel[6] | psel[7] |
               psel[10] | psel[11] | psel[14] | psel[15];
  bnslave[2] = psel[4] | psel[5] | psel[6] | psel[7] |
               psel[12] | psel[13] | psel[14] | psel[15];
  bnslave[3] = psel[8] | psel[9] | psel[10] | psel[11] |
               psel[12] | psel[13] | psel[14] | psel[15];
  nslave = (bnslave[3]<<3)|(bnslave[2]<<2)|(bnslave[1]<<1)|(bnslave[0]);

  if (BITS32(r.Q.haddr,19,12) == 0xff)
  {
    v.cfgsel = 1;
    for(int32 i=0; i<APB_SLAVES_MAX; i++) psel[i] = 0;
    v.penable = 0;
  }else v.cfgsel = 0;

  v.prdata = apbo.arr[nslave].prdata;

  if (r.Q.cfgsel == 1)
  {
    v.prdata = apbo.arr[BITS32(r.Q.haddr,6,3)].pconfig.arr[BITS32(r.Q.haddr,2,2)];
    if (APB_SLAVES_MAX <= BITS32(r.Q.haddr,6,3))
      v.prdata = 0;
  }

  for (int32 i=0; i<=APB_SLAVES_MAX-1;i++)  pirq = pirq | apbo.arr[i].pirq;

  //-- AHB respons
  ahbo.hready = r.Q.hready;
  ahbo.hrdata = r.Q.prdata;//ahbdrivedata(r.prdata);
  ahbo.hirq   = pirq;

  if (!rst)
  {
    v.penable = 0;
    v.hready = 1;
    v.psel = 0;
    v.state = 0;
    v.hwrite = 0;
    v.haddr = 0;
  };

  rin = v;

  //-- drive APB bus
  apbaddr2 = 0;
  apbaddr2 = BITS32(r.Q.haddr,apbmax,0);
  apbi.paddr   = apbaddr2;
  apbi.pwdata  = r.Q.pwdata;
  apbi.pwrite  = r.Q.hwrite;
  apbi.penable = r.Q.penable;
  apbi.pirq    = ahbi.hirq;
  apbi.testen  = ahbi.testen;
  apbi.testoen = ahbi.testoen;
  apbi.scanen  = ahbi.scanen;
  apbi.testrst = ahbi.testrst;

  apbi.psel = 0;
  for (int32 i=0; i<=APB_SLAVES_MAX-1; i++)
    apbi.psel |= ((psel[i] & r.Q.psel)<<i);
    
  lapbi.paddr   = apbaddr2;
  lapbi.pwdata  = r.Q.pwdata;
  lapbi.pwrite  = r.Q.hwrite;
  lapbi.penable = r.Q.penable;
  lapbi.pirq    = ahbi.hirq;

  for (int32 i=0; i<=APB_SLAVES_MAX-1; i++) 
  {
    lapbi.psel &= ~(1<<i);
    lapbi.psel |= ((psel[i] & r.Q.psel)<<i);
  }

  
  ahbo.hindex  = hindex;
  //ahbo.hconfig = hconfig; // filled at the begin!!!
  ahbo.hcache  = 0;
  ahbo.hsplit  = 0;
  ahbo.hresp   = HRESP_OKAY;

  r.CLK = clk;
  r.D = rin;

#if (enbusmon != 0)
    mon :  apbmon 
      generic map(
        asserterr   => asserterr,
        assertwarn  => assertwarn,
        pslvdisable => pslvdisable,
        napb        => APB_SLAVES_MAX)
      port map(
        rst         => rst,
        clk         => clk,
        apbi        => lapbi,
        apbo        => apbo,
        err         => open);
#endif
/*
  diag : process
  type apb_memarea_type is record
     start : std_logic_vector(31 downto 20);
     stop  : std_logic_vector(31 downto 20);
  end record;
  type memmap_type is array (0 to APB_SLAVES_MAX-1) of apb_memarea_type;
  variable k : integer;
  variable mask : std_logic_vector(11 downto 0);
  variable device : std_logic_vector(11 downto 0);
  variable devicei : integer;
  variable vendor : std_logic_vector( 7 downto 0);
  variable vendori : integer;
  variable iosize : integer;
  variable iounit : string(1 to 5) := "byte ";
  variable memstart : std_logic_vector(11 downto 0) := IOAREA and IOMSK;
  variable L1 : line := new string'("");
  variable memmap : memmap_type;
  begin
    wait for 3 ns;
    if debug > 0 then
      print("apbctrl: APB Bridge at " & tost(memstart) & "00000 rev 1");
    end if;
    for i in 0 to APB_SLAVES_MAX-1 loop
      vendor := apbo(i).pconfig(0)(31 downto 24); 
      vendori := conv_integer(vendor);
      if vendori /= 0 then
        if debug > 1 then
          device := apbo(i).pconfig(0)(23 downto 12); 
          devicei := conv_integer(device);
          std.textio.write(L1, "apbctrl: slv" & tost(i) & ": " &                
           iptable(vendori).vendordesc  & iptable(vendori).device_table(devicei));
          std.textio.writeline(OUTPUT, L1);
          mask := apbo(i).pconfig(1)(15 downto 4);
          k := 0;
          while (k<15) and (mask(k) = 0) loop k := k+1; end loop;      
          iosize := 256 * 2**k; iounit := "byte ";
          if (iosize > 1023) then iosize := iosize/1024; iounit := "kbyte"; end if;
          print("apbctrl:       I/O ports at " & 
            tost(memstart & (apbo(i).pconfig(1)(31 downto 20) and
                             apbo(i).pconfig(1)(15 downto 4))) &
		"00, size " & tost(iosize) & " " & iounit);
          if mcheck /= 0 then
            memmap(i).start := (apbo(i).pconfig(1)(31 downto 20) and
                                apbo(i).pconfig(1)(15 downto 4));
            memmap(i).stop := memmap(i).start + 2**k;
          end if;
        end if;
	assert (apbo(i).pindex = i) or (icheck = 0)
	report "APB slave index error on slave " & tost(i) &
          ". Detected index value " & tost(apbo(i).pindex) severity failure;
        if mcheck /= 0 then
          for j in 0 to i loop
            if memmap(i).start /= memmap(i).stop then
              assert ((memmap(i).start >= memmap(j).stop) or
                      (memmap(i).stop <= memmap(j).start) or (i = j))
                report "APB slave " & tost(i) & " memory area" &  
                " intersects with APB slave " & tost(j) & " memory area."
                severity failure;
            end if;
          end loop;
        end if;
      else
        for j in 0 to NAPBCFG-1 loop
          assert (apbo(i).pconfig(j) = zx or ccheck = 0)
            report "APB slave " & tost(i) & " appears to be disabled, " &
            "but the config record is not driven to zero"
            severity warning;
        end loop;
      end if;
    end loop;
    if APB_SLAVES_MAX < NAPBSLV then
      for i in nslaves to NAPBSLV-1 loop
        for j in 0 to NAPBCFG-1 loop
          assert (apbo(i).pconfig(j) = zx or ccheck = 0)
            report "APB slave " & tost(i) & " is outside the range of decoded " &
            "slave indexes but the config record is not driven to zero"
            severity warning;
        end loop;  -- j
      end loop;  -- i
    end if;
    wait;
  end process;
-- pragma translate_on



*/
}

