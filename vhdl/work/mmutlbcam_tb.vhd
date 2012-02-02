library ieee;
use ieee.std_logic_1164.all;
library std;
use std.textio.all;
library ieee;
use ieee.std_logic_1164.all;
library grlib;
use grlib.amba.all;
use grlib.stdlib.all;
library techmap;
use techmap.gencomp.all;
library gaisler;
use gaisler.libiu.all;
use gaisler.libcache.all;
use gaisler.leon3.all;
use gaisler.mmuconfig.all;
use gaisler.mmuiface.all;
use gaisler.libmmu.all;
library work;
use work.config.all;
use work.util_tb.all;

entity mmutlbcam_tb is
  constant CLK_HPERIOD : time := 10 ps;
  constant STRING_SIZE : integer := 304; -- string size = index of the last element
end mmutlbcam_tb;
architecture behavior of mmutlbcam_tb is
  -- tlb cams
  component mmutlbcam 
    generic ( 
      tlb_type  : integer range 0 to 3 := 1;
      mmupgsz   : integer range 0 to 5  := 0
    );
    port (
      rst     : in std_logic;
      clk     : in std_logic;
      tlbcami : in mmutlbcam_in_type;
      tlbcamo : out mmutlbcam_out_type
      );
  end component;

  -- input/output signals:
  signal inNRst       : std_logic:= '0';
  signal inClk        : std_logic:= '0';
  signal tlbcami      : mmutlbcam_in_type;
  signal tlbcamo      : mmutlbcam_out_type;
  signal ch_tlbcamo   : mmutlbcam_out_type;
  signal tmp_blvl_decode : std_logic_vector(3 downto 0);
  signal tmp_pagesize    : std_logic_vector(1 downto 0);
  signal tmp_h_c         : std_logic;
  signal tmp_hm          : std_logic;
  signal tmp_vaddr_r : std_logic_vector(31 downto 0);
  signal tmp_vaddr_i : std_logic_vector(31 downto 0);
  signal tmp_h_i1  : std_logic;
  signal tmp_h_i2  : std_logic;
  signal tmp_h_i3  : std_logic;
  signal tmp_h_su_cnt : std_logic;
  signal tmp_hf       : std_logic;



  
  signal U: std_ulogic_vector(STRING_SIZE-1 downto 0);
  signal S: std_logic_vector(STRING_SIZE-1 downto 0);
  shared variable iClkCnt : integer := 0;

begin

  -- Process of clock generation
  procClkgen : process
  begin
      inClk <= '0' after CLK_HPERIOD, '1' after 2*CLK_HPERIOD;
      wait for 2*CLK_HPERIOD;
  end process procClkgen;

  -- Process of reading  
  procReadingFile : process
    file InputFile:TEXT is "e:/mmutlbcam_tb.txt";
    variable rdLine: line;  
    variable strLine : string(STRING_SIZE downto 1);
  begin
    while not endfile(InputFile) loop
      readline(InputFile, rdLine);
      read(rdLine, strLine);
      U <= StringToUVector(strLine);
      S <= StringToSVector(strLine);
  
      wait until rising_edge(inClk);
      --wait until falling_edge(inClk);
      iClkCnt := iClkCnt + 1;
    end loop;
  end process procReadingFile;


  -- Input signals:
  inNRst                  <= S(0);
  tlbcami.mmctrl.e        <= S(1);
  tlbcami.mmctrl.nf       <= S(2);
  tlbcami.mmctrl.pso      <= S(3);
  tlbcami.mmctrl.pagesize <= S(5 downto 4);
  tlbcami.mmctrl.ctx      <= S(13 downto 6);
  tlbcami.mmctrl.tlbdis   <= S(14);
  tlbcami.mmctrl.bar      <= S(16 downto 15);
  tlbcami.mmctrl.ctxp     <= S(46 downto 17);
  
  tlbcami.tagin.TYP  <= S(49 downto 47);
  tlbcami.tagin.I1   <= S(57 downto 50);
  tlbcami.tagin.I2   <= S(63 downto 58);
  tlbcami.tagin.I3   <= S(69 downto 64);
  tlbcami.tagin.CTX  <= S(77 downto 70);
  tlbcami.tagin.M    <= S(78);
  
  tlbcami.tagwrite.ET     <= S(80 downto 79);
  tlbcami.tagwrite.ACC    <= S(83 downto 81);
  tlbcami.tagwrite.M      <= S(84);
  tlbcami.tagwrite.R      <= S(85);
  tlbcami.tagwrite.SU     <= S(86);
  tlbcami.tagwrite.VALID  <= S(87);
  tlbcami.tagwrite.LVL    <= S(89 downto 88);
  tlbcami.tagwrite.I1     <= S(97 downto 90);
  tlbcami.tagwrite.I2     <= S(103 downto 98);
  tlbcami.tagwrite.I3     <= S(109 downto 104);
  tlbcami.tagwrite.CTX    <= S(117 downto 110);
  tlbcami.tagwrite.PPN    <= S(141 downto 118);
  tlbcami.tagwrite.C      <= S(142);
  
  tlbcami.trans_op  <= S(143);
  tlbcami.flush_op  <= S(144);
  tlbcami.write_op  <= S(145);
  tlbcami.wb_op     <= S(146);
  tlbcami.mmuen     <= S(147);
  tlbcami.mset      <= S(148);

  -- Output:
  ch_tlbcamo.pteout      <= S(180 downto 149);
  ch_tlbcamo.LVL         <= S(182 downto 181);
  ch_tlbcamo.hit         <= S(183);
  ch_tlbcamo.ctx         <= S(191 downto 184);
  ch_tlbcamo.valid       <= S(192);
  ch_tlbcamo.vaddr       <= S(224 downto 193);
  ch_tlbcamo.NEEDSYNC    <= S(225);
  ch_tlbcamo.WBNEEDSYNC  <= S(226);
  
  -- internal register:
  tmp_blvl_decode <= S(230 downto 227);
  tmp_blvl_decode <= S(230 downto 227);
  tmp_pagesize <= S(232 downto 231);
  tmp_h_c <= S(233);
  tmp_hm <= S(234);
  tmp_vaddr_r <= S(266 downto 235);
  tmp_vaddr_i <= S(298 downto 267);
  tmp_h_i1 <= S(299);
  tmp_h_i2 <= S(300);
  tmp_h_i3 <= S(301);
  tmp_h_su_cnt <= S(302);
  tmp_hf <= S(303);


  
  tt : mmutlbcam generic map 
  (
    tlb_type => 0,
    mmupgsz => 0
  )port map 
  (
    inNRst,
    inClk,
    tlbcami,
    tlbcamo
  );

procCheck : process (inClk)
  variable iErrCnt : integer := 0;
begin
  if(rising_edge(inClk) and (iClkCnt>2)) then
    if(ch_tlbcamo.pteout /= tlbcamo.pteout) then print("Err: pteout");  iErrCnt:=iErrCnt+1; end if;
    if(ch_tlbcamo.LVL /= tlbcamo.LVL) then print("Err: LVL");  iErrCnt:=iErrCnt+1; end if;
    if(ch_tlbcamo.hit /= tlbcamo.hit) then print("Err: hit");  iErrCnt:=iErrCnt+1; end if;
    if(ch_tlbcamo.ctx /= tlbcamo.ctx) then print("Err: ctx");  iErrCnt:=iErrCnt+1; end if;
    if(ch_tlbcamo.valid /= tlbcamo.valid) then print("Err: valid");  iErrCnt:=iErrCnt+1; end if;
    if(ch_tlbcamo.vaddr /= tlbcamo.vaddr) then print("Err: vaddr");  iErrCnt:=iErrCnt+1; end if;
    if(ch_tlbcamo.NEEDSYNC /= tlbcamo.NEEDSYNC) then print("Err: NEEDSYNC");  iErrCnt:=iErrCnt+1; end if;
    if(ch_tlbcamo.WBNEEDSYNC /= tlbcamo.WBNEEDSYNC) then print("Err: WBNEEDSYNC");  iErrCnt:=iErrCnt+1; end if;
  end if;
end process procCheck;

  
end;
