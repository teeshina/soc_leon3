
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;--"unsgined", SHIFT_RIGHT(), etc
library std;
use std.textio.all;
library ieee;
library grlib;
use grlib.stdlib.all;
use grlib.sparc.all;
library techmap;
use techmap.gencomp.all;
library gaisler;
use gaisler.leon3.all;
use gaisler.libiu.all;
use gaisler.libcache.all;
use gaisler.arith.all;
use grlib.sparc_disas.all;
library work;
use work.config.all;
use work.util_tb.all;

entity iu3_tb is
  constant CLK_HPERIOD : time := 10 ps;
  constant STRING_SIZE : integer := 3003; -- string size = index of the last element

 
  constant ISETMSB : integer := log2x(CFG_ISETS)-1;
  constant DSETMSB : integer := log2x(CFG_DSETS)-1;
  constant RFBITS : integer range 6 to 10 := log2(CFG_NWIN+1) + 4;
  constant NWINLOG2   : integer range 1 to 5 := log2(CFG_NWIN);
  constant CWPOPT : boolean := (CFG_NWIN = (2**NWINLOG2));
  constant CWPMIN : std_logic_vector(NWINLOG2-1 downto 0) := (others => '0');
  constant CWPMAX : std_logic_vector(NWINLOG2-1 downto 0) := 
  	conv_std_logic_vector(CFG_NWIN-1, NWINLOG2);
  constant FPEN   : boolean := (CFG_FPU /= 0);
  constant CPEN   : boolean := false;
  constant MULEN  : boolean := (CFG_V8 /= 0);
  constant MULTYPE: integer := (CFG_V8 / 16);
  constant DIVEN  : boolean := (CFG_V8 /= 0);
  constant MACEN  : boolean := (CFG_MAC = 1);
  constant MACPIPE: boolean := (CFG_MAC = 1) and (CFG_V8/2 = 1);
  constant IMPL   : integer := 15;
  constant VER    : integer := 3;
  constant DBGUNIT : boolean := (CFG_DSU = 1);
  constant TRACEBUF   : boolean := (CFG_ITBSZ /= 0);
  constant TBUFBITS : integer := 10 + log2(CFG_ITBSZ) - 4;
  constant PWRD1  : boolean := false; --(pwd = 1) and not (index /= 0);
  constant PWRD2  : boolean := CFG_PWD /= 0; --(pwd = 2) or (index /= 0);
  constant RS1OPT : boolean := (is_fpga(inferred) /= 0);
  constant DYNRST : boolean := (CFG_RSTADDR = 16#FFFFF#);
  
  constant CASAEN : boolean := (CFG_NOTAG = 0) and (CFG_LDDEL = 1);
-- execute stage operations

  constant EXE_AND   : std_logic_vector(2 downto 0) := "000";
  constant EXE_XOR   : std_logic_vector(2 downto 0) := "001"; -- must be equal to EXE_PASS2
  constant EXE_OR    : std_logic_vector(2 downto 0) := "010";
  constant EXE_XNOR  : std_logic_vector(2 downto 0) := "011";
  constant EXE_ANDN  : std_logic_vector(2 downto 0) := "100";
  constant EXE_ORN   : std_logic_vector(2 downto 0) := "101";
  constant EXE_DIV   : std_logic_vector(2 downto 0) := "110";

  constant EXE_PASS1 : std_logic_vector(2 downto 0) := "000";
  constant EXE_PASS2 : std_logic_vector(2 downto 0) := "001";
  constant EXE_STB   : std_logic_vector(2 downto 0) := "010";
  constant EXE_STH   : std_logic_vector(2 downto 0) := "011";
  constant EXE_ONES  : std_logic_vector(2 downto 0) := "100";
  constant EXE_RDY   : std_logic_vector(2 downto 0) := "101";
  constant EXE_SPR   : std_logic_vector(2 downto 0) := "110";
  constant EXE_LINK  : std_logic_vector(2 downto 0) := "111";

  constant EXE_SLL   : std_logic_vector(2 downto 0) := "001";
  constant EXE_SRL   : std_logic_vector(2 downto 0) := "010";
  constant EXE_SRA   : std_logic_vector(2 downto 0) := "100";

  constant EXE_NOP   : std_logic_vector(2 downto 0) := "000";

-- EXE result select

  constant EXE_RES_ADD   : std_logic_vector(1 downto 0) := "00";
  constant EXE_RES_SHIFT : std_logic_vector(1 downto 0) := "01";
  constant EXE_RES_LOGIC : std_logic_vector(1 downto 0) := "10";
  constant EXE_RES_MISC  : std_logic_vector(1 downto 0) := "11";

-- Load types

  constant SZBYTE    : std_logic_vector(1 downto 0) := "00";
  constant SZHALF    : std_logic_vector(1 downto 0) := "01";
  constant SZWORD    : std_logic_vector(1 downto 0) := "10";
  constant SZDBL     : std_logic_vector(1 downto 0) := "11";

  
  subtype word is std_logic_vector(31 downto 0);
  subtype pctype is std_logic_vector(31 downto CFG_PCLOW);
  subtype rfatype is std_logic_vector(RFBITS-1 downto 0);
  subtype cwptype is std_logic_vector(NWINLOG2-1 downto 0);
  type icdtype is array (0 to CFG_ISETS-1) of word;
  type dcdtype is array (0 to CFG_DSETS-1) of word;

  type dc_in_type is record
    signed, enaddr, read, write, lock , dsuen : std_ulogic;
    size : std_logic_vector(1 downto 0);
    asi  : std_logic_vector(7 downto 0);    
  end record;
  
  type pipeline_ctrl_type is record
    pc    : pctype;
    inst  : word;
    cnt   : std_logic_vector(1 downto 0);
    rd    : rfatype;
    tt    : std_logic_vector(5 downto 0);
    trap  : std_ulogic;
    annul : std_ulogic;
    wreg  : std_ulogic;
    wicc  : std_ulogic;
    wy    : std_ulogic;
    ld    : std_ulogic;
    pv    : std_ulogic;
    rett  : std_ulogic;
  end record;
  
  type fetch_reg_type is record
    pc     : pctype;
    branch : std_ulogic;
  end record;
  
  type decode_reg_type is record
    pc    : pctype;
    inst  : icdtype;
    cwp   : cwptype;
    set   : std_logic_vector(ISETMSB downto 0);
    mexc  : std_ulogic;
    cnt   : std_logic_vector(1 downto 0);
    pv    : std_ulogic;
    annul : std_ulogic;
    inull : std_ulogic;
    step  : std_ulogic;        
    divrdy: std_ulogic;        
  end record;
  
  type regacc_reg_type is record
    ctrl  : pipeline_ctrl_type;
    rs1   : std_logic_vector(4 downto 0);
    rfa1, rfa2 : rfatype;
    rsel1, rsel2 : std_logic_vector(2 downto 0);
    rfe1, rfe2 : std_ulogic;
    cwp   : cwptype;
    imm   : word;
    ldcheck1 : std_ulogic;
    ldcheck2 : std_ulogic;
    ldchkra : std_ulogic;
    ldchkex : std_ulogic;
    su : std_ulogic;
    et : std_ulogic;
    wovf : std_ulogic;
    wunf : std_ulogic;
    ticc : std_ulogic;
    jmpl : std_ulogic;
    step  : std_ulogic;            
    mulstart : std_ulogic;            
    divstart : std_ulogic;            
    bp, nobp : std_ulogic;
  end record;
  
  type execute_reg_type is record
    ctrl   : pipeline_ctrl_type;
    op1    : word;
    op2    : word;
    aluop  : std_logic_vector(2 downto 0);  	-- Alu operation
    alusel : std_logic_vector(1 downto 0);  	-- Alu result select
    aluadd : std_ulogic;
    alucin : std_ulogic;
    ldbp1, ldbp2 : std_ulogic;
    invop2 : std_ulogic;
    shcnt  : std_logic_vector(4 downto 0);  	-- shift count
    sari   : std_ulogic;				-- shift msb
    shleft : std_ulogic;				-- shift left/right
    ymsb   : std_ulogic;				-- shift left/right
    rd 	   : std_logic_vector(4 downto 0);
    jmpl   : std_ulogic;
    su     : std_ulogic;
    et     : std_ulogic;
    cwp    : cwptype;
    icc    : std_logic_vector(3 downto 0);
    mulstep: std_ulogic;            
    mul    : std_ulogic;            
    mac    : std_ulogic;
    bp     : std_ulogic;
  end record;
  
  type memory_reg_type is record
    ctrl   : pipeline_ctrl_type;
    result : word;
    y      : word;
    icc    : std_logic_vector(3 downto 0);
    nalign : std_ulogic;
    dci	   : dc_in_type;
    werr   : std_ulogic;
    wcwp   : std_ulogic;
    irqen  : std_ulogic;
    irqen2 : std_ulogic;
    mac    : std_ulogic;
    divz   : std_ulogic;
    su     : std_ulogic;
    mul    : std_ulogic;
    casa   : std_ulogic;
    casaz  : std_ulogic;
  end record;

  type exception_state is (run, trap, dsu1, dsu2);
  
  type exception_reg_type is record
    ctrl   : pipeline_ctrl_type;
    result : word;
    y      : word;
    icc    : std_logic_vector( 3 downto 0);
    annul_all : std_ulogic;
    data   : dcdtype;
    set    : std_logic_vector(DSETMSB downto 0);
    mexc   : std_ulogic;
    dci	   : dc_in_type;
    laddr  : std_logic_vector(1 downto 0);
    rstate : exception_state;
    npc    : std_logic_vector(2 downto 0);
    intack : std_ulogic;
    ipend  : std_ulogic;
    mac    : std_ulogic;
    debug  : std_ulogic;
    nerror : std_ulogic;
  end record;

  type dsu_registers is record
    tt      : std_logic_vector(7 downto 0);
    err     : std_ulogic;
    tbufcnt : std_logic_vector(TBUFBITS-1 downto 0);
    asi     : std_logic_vector(7 downto 0);
    crdy    : std_logic_vector(2 downto 1);  -- diag cache access ready
  end record;

  type irestart_register is record
    addr   : pctype;
    pwd    : std_ulogic;
  end record;
  
  type pwd_register_type is record
    pwd    : std_ulogic;
    error  : std_ulogic;
  end record;

  type special_register_type is record
    cwp    : cwptype;                             -- current window pointer
    icc    : std_logic_vector(3 downto 0);	  -- integer condition codes
    tt     : std_logic_vector(7 downto 0);	  -- trap type
    tba    : std_logic_vector(19 downto 0);	  -- trap base address
    wim    : std_logic_vector(CFG_NWIN-1 downto 0);   -- window invalid mask
    pil    : std_logic_vector(3 downto 0);	  -- processor interrupt level
    ec     : std_ulogic;			   -- enable CP 
    ef     : std_ulogic;			   -- enable FP 
    ps     : std_ulogic;			   -- previous supervisor flag
    s      : std_ulogic;			   -- supervisor flag
    et     : std_ulogic;			   -- enable traps
    y      : word;
    asr18  : word;
    svt    : std_ulogic;			   -- enable traps
    dwt    : std_ulogic;			   -- disable write error trap
    dbp    : std_ulogic;			   -- disable branch prediction
  end record;
  
  type write_reg_type is record
    s      : special_register_type;
    result : word;
    wa     : rfatype;
    wreg   : std_ulogic;
    except : std_ulogic;
  end record;

  type registers is record
    f  : fetch_reg_type;
    d  : decode_reg_type;
    a  : regacc_reg_type;
    e  : execute_reg_type;
    m  : memory_reg_type;
    x  : exception_reg_type;
    w  : write_reg_type;
  end record;

  type exception_type is record
    pri   : std_ulogic;
    ill   : std_ulogic;
    fpdis : std_ulogic;
    cpdis : std_ulogic;
    wovf  : std_ulogic;
    wunf  : std_ulogic;
    ticc  : std_ulogic;
  end record;

  type watchpoint_register is record
    addr    : std_logic_vector(31 downto 2);  -- watchpoint address
    mask    : std_logic_vector(31 downto 2);  -- watchpoint mask
    exec    : std_ulogic;			    -- trap on instruction
    load    : std_ulogic;			    -- trap on load
    store   : std_ulogic;			    -- trap on store
  end record;

  type watchpoint_registers is array (0 to 3) of watchpoint_register;

  constant wpr_none : watchpoint_register := (
	zero32(31 downto 2), zero32(31 downto 2), '0', '0', '0');

  
end iu3_tb;
architecture behavior of iu3_tb is
  -- input/output signals:
  signal inNRst   : std_logic:= '0';
  signal inClk    : std_logic:= '0';
  signal holdnx : std_logic;
  signal rfi    : iregfile_in_type;
  signal rfo    : iregfile_out_type;
  signal ici : icache_in_type;
  signal ico : icache_out_type;
  signal dci : dcache_in_type;
  signal dco : dcache_out_type;
  
  signal tbi    : tracebuf_in_type;
  signal tbo    : tracebuf_out_type;
  signal fpi    : fpc_in_type;
  signal fpo    : fpc_out_type;
  signal cpi    : fpc_in_type;
  signal cpo    : fpc_out_type;
  signal irqi   : l3_irq_in_type;
  signal irqo   : l3_irq_out_type;
  signal dbgi   : l3_debug_in_type;
  signal dbgo   : l3_debug_out_type;
  
  signal muli  : mul32_in_type;
  signal mulo  : mul32_out_type;
  signal divi  : div32_in_type;
  signal divo  : div32_out_type;

  signal ch_rfi    : iregfile_in_type;
  signal ch_dci : dcache_in_type;
  signal ch_ici : icache_in_type;
  signal ch_tbi    : tracebuf_in_type;
  signal ch_irqo   : l3_irq_out_type;
  signal ch_dbgo   : l3_debug_out_type;
  signal ch_muli  : mul32_in_type;
  signal ch_divi  : div32_in_type;

  signal t_fe_pc  : std_logic_vector(29 downto 0);
  signal t_fe_npc  : std_logic_vector(29 downto 0);
  signal t_npc  : std_logic_vector(29 downto 0);
  signal t_de_raddr1  : std_logic_vector(9 downto 0);
  signal t_de_addr2  : std_logic_vector(9 downto 0);
  signal t_de_rs2  : std_logic_vector(4 downto 0);
  signal t_de_rd  : std_logic_vector(4 downto 0);
  signal t_de_hold_pc  : std_logic;
  signal t_de_branch  : std_logic;
  signal t_de_ldlock  : std_logic;

  signal in_r : registers;
  signal in_dsur :  dsu_registers;
  signal in_ir   :  irestart_register;
  signal in_wpr  :  watchpoint_registers;

  signal in_trap : std_ulogic;
  signal in_rp : pwd_register_type;
  signal ch_pd : std_ulogic;
                     					
  signal U: std_ulogic_vector(STRING_SIZE-1 downto 0);
  signal S: std_logic_vector(STRING_SIZE-1 downto 0);
  shared variable iClkCnt : integer := 0;
  shared variable iErrCnt : integer := 0;

begin

  -- Process of clock generation
  procClkgen : process
  begin
      inClk <= '0' after CLK_HPERIOD, '1' after 2*CLK_HPERIOD;
      wait for 2*CLK_HPERIOD;
  end process procClkgen;

  -- Process of reading  
  procReadingFile : process
    file InputFile:TEXT is "e:/iu3_tb.txt";
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
--  fpo <= fpc_out_none;
  fpo.data <= (others => '0');
  fpo.exc <= '0';
  fpo.cc <= "00";
  fpo.ccv <= '0';
  fpo.ldlock <= '0';
  fpo.holdn <= '0';
--  fpo.dbg.data;//           : fpc_debug_out_type;             -- FP debug signals    

  cpo <= fpc_out_none;

  inNRst <= S(0);
  holdnx <= S(1);
  ico.data(0) <= S(33 downto 2);
  ico.data(1) <= S(65 downto 34);
  ico.data(2) <= S(97 downto 66);
  ico.data(3) <= S(129 downto 98);
  ico.set <= S(131 downto 130);
  ico.mexc <= S(132);
  ico.hold <= S(133);
  ico.flush <= S(134);
  ico.diagrdy <= S(135);
  ico.diagdata <= S(167 downto 136);
  ico.mds <= S(168);
  ico.cfg <= S(200 downto 169);
  ico.idle <= S(201);
  ico.cstat.cmiss <= S(202);
  ico.cstat.tmiss <= S(203);
  ico.cstat.chold <= S(204);
  ico.cstat.mhold <= S(205);
  dco.data(0) <= S(237 downto 206);
  dco.data(1) <= S(269 downto 238);
  dco.data(2) <= S(301 downto 270);
  dco.data(3) <= S(333 downto 302);
  dco.set <= S(335 downto 334);
  dco.mexc <= S(336);
  dco.hold <= S(337);
  dco.mds <= S(338);
  dco.werr <= S(339);
  dco.icdiag.addr <= S(371 downto 340);
  dco.icdiag.enable <= S(372);
  dco.icdiag.read <= S(373);
  dco.icdiag.tag <= S(374);
  dco.icdiag.ctx <= S(375);
  dco.icdiag.flush <= S(376);
  dco.icdiag.ilramen <= S(377);
  dco.icdiag.cctrl.burst <= S(378);
  dco.icdiag.cctrl.dfrz <= S(379);
  dco.icdiag.cctrl.ifrz <= S(380);
  dco.icdiag.cctrl.dsnoop <= S(381);
  dco.icdiag.cctrl.dcs <= S(383 downto 382);
  dco.icdiag.cctrl.ics <= S(385 downto 384);
  dco.icdiag.pflush <= S(386);
  dco.icdiag.pflushaddr <= S(406 downto 387);
  dco.icdiag.pflushtyp <= S(407);
  dco.icdiag.ilock <= S(411 downto 408);
  dco.icdiag.scanen <= S(412);
  dco.cache <= S(413);
  dco.idle <= S(414);
  dco.scanen <= S(415);
  dco.testen <= S(416);
  rfo.data1 <= S(448 downto 417);
  rfo.data2 <= S(480 downto 449);
  irqi.irl <= S(484 downto 481);
  irqi.rst <= S(485);
  irqi.run <= S(486);
  irqi.rstvec <= S(506 downto 487);
  irqi.iact <= S(507);
  irqi.index <= S(511 downto 508);
  dbgi.dsuen <= S(512);
  dbgi.denable <= S(513);
  dbgi.dbreak <= S(514);
  dbgi.step <= S(515);
  dbgi.halt <= S(516);
  dbgi.reset <= S(517);
  dbgi.dwrite <= S(518);
  dbgi.daddr <= S(540 downto 519);
  dbgi.ddata <= S(572 downto 541);
  dbgi.btrapa <= S(573);
  dbgi.btrape <= S(574);
  dbgi.berror <= S(575);
  dbgi.bwatch <= S(576);
  dbgi.bsoft <= S(577);
  dbgi.tenable <= S(578);
  dbgi.timer <= S(609 downto 579);
  mulo.ready <= S(610);
  mulo.nready <= S(611);
  mulo.icc <= S(615 downto 612);
  mulo.result <= S(679 downto 616);
  divo.ready <= S(680);
  divo.nready <= S(681);
  divo.icc <= S(685 downto 682);
  divo.result <= S(717 downto 686);
  tbo.data(63 downto 0) <= S(781 downto 718);
  tbo.data(127 downto 64) <= S(845 downto 782);
  ch_ici.rpc <= S(877 downto 846);
  ch_ici.fpc <= S(909 downto 878);
  ch_ici.dpc <= S(941 downto 910);
  ch_ici.rbranch <= S(942);
  ch_ici.fbranch <= S(943);
  ch_ici.inull <= S(944);
  ch_ici.su <= S(945);
  ch_ici.flush <= S(946);
  ch_ici.flushl <= S(947);
  ch_ici.fline <= S(976 downto 948);
  ch_ici.pnull <= 'U';--S(977);
  ch_dci.asi <= S(985 downto 978);
  ch_dci.maddress <= S(1017 downto 986);
  ch_dci.eaddress <= S(1049 downto 1018);
  ch_dci.edata <= S(1081 downto 1050);
  ch_dci.size <= S(1083 downto 1082);
  ch_dci.enaddr <= S(1084);
  ch_dci.eenaddr <= S(1085);
  ch_dci.nullify <= S(1086);
  ch_dci.lock <= S(1087);
  ch_dci.read <= S(1088);
  ch_dci.write <= S(1089);
  ch_dci.flush <= S(1090);
  ch_dci.flushl <= 'U';--S(1091);
  ch_dci.dsuen <= S(1092);
  ch_dci.msu <= S(1093);
  ch_dci.esu <= S(1094);
  ch_dci.intack <= S(1095);
  ch_rfi.raddr1 <= S(1105 downto 1096);
  ch_rfi.raddr2 <= S(1115 downto 1106);
  ch_rfi.waddr <= S(1125 downto 1116);
  ch_rfi.wdata <= S(1157 downto 1126);
  ch_rfi.ren1 <= S(1158);
  ch_rfi.ren2 <= S(1159);
  ch_rfi.wren <= S(1160);
  ch_rfi.diag <= S(1164 downto 1161);
  ch_irqo.intack <= S(1165);
  ch_irqo.irl <= S(1169 downto 1166);
  ch_irqo.pwd <= S(1170);
  ch_irqo.fpen <= 'U';--S(1171);
  ch_dbgo.data <= S(1203 downto 1172);
  ch_dbgo.crdy <= S(1204);
  ch_dbgo.dsu <= S(1205);
  ch_dbgo.dsumode <= S(1206);
  ch_dbgo.error <= S(1207);
  ch_dbgo.halt <= S(1208);
  ch_dbgo.pwd <= S(1209);
  ch_dbgo.idle <= S(1210);
  ch_dbgo.ipend <= S(1211);
  ch_dbgo.icnt <= S(1212);
  --ch_dbgo.fcnt <= S(1213);
  --ch_dbgo.optype <= S(1219 downto 1214);
  --ch_dbgo.bpmiss <= S(1220);
  --ch_dbgo.istat.cmiss <= S(1221);
  --ch_dbgo.istat.tmiss <= S(1222);
  --ch_dbgo.istat.chold <= S(1223);
  --ch_dbgo.istat.mhold <= S(1224);
  --ch_dbgo.dstat.cmiss <= S(1225);
  --ch_dbgo.dstat.tmiss <= S(1226);
  --ch_dbgo.dstat.chold <= S(1227);
  --ch_dbgo.dstat.mhold <= S(1228);
  --ch_dbgo.wbhold <= S(1229);
  --ch_dbgo.su <= S(1230);
  ch_muli.op1 <= S(1263 downto 1231);
  ch_muli.op2 <= S(1296 downto 1264);
  ch_muli.flush <= S(1297);
  ch_muli.signed <= S(1298);
  ch_muli.start <= S(1299);
  ch_muli.mac <= S(1300);
  ch_muli.acc <= S(1340 downto 1301);
  ch_divi.y <= S(1373 downto 1341);
  ch_divi.op1 <= S(1406 downto 1374);
  ch_divi.op2 <= S(1439 downto 1407);
  ch_divi.flush <= S(1440);
  ch_divi.Signed <= S(1441);
  ch_divi.start <= S(1442);
  ch_tbi.addr <= S(1454 downto 1443);
  ch_tbi.data(63 downto 0) <= S(1518 downto 1455);
  ch_tbi.data(127 downto 64) <= S(1582 downto 1519);
  ch_tbi.enable <= S(1583);
  ch_tbi.write <= S(1587 downto 1584);
  ch_tbi.diag <= S(1591 downto 1588);
  in_r.d.pc <= S(1621 downto 1592);
  in_r.d.inst(0) <= S(1653 downto 1622);
  in_r.d.inst(1) <= S(1685 downto 1654);
  in_r.d.cwp <= S(1688 downto 1686);
  in_r.d.set <= S(1689 downto 1689);
  in_r.d.mexc <= S(1690);
  in_r.d.cnt <= S(1692 downto 1691);
  in_r.d.pv <= S(1693);
  in_r.d.annul <= S(1694);
  in_r.d.inull <= S(1695);
  in_r.d.step <= S(1696);
  in_r.d.divrdy <= S(1697);
  in_r.a.ctrl.pc <= S(1727 downto 1698);
  in_r.a.ctrl.inst <= S(1759 downto 1728);
  in_r.a.ctrl.cnt <= S(1761 downto 1760);
  in_r.a.ctrl.rd <= S(1769 downto 1762);
  in_r.a.ctrl.tt <= S(1775 downto 1770);
  in_r.a.ctrl.trap <= S(1776);
  in_r.a.ctrl.annul <= S(1777);
  in_r.a.ctrl.wreg <= S(1778);
  in_r.a.ctrl.wicc <= S(1779);
  in_r.a.ctrl.wy <= S(1780);
  in_r.a.ctrl.ld <= S(1781);
  in_r.a.ctrl.pv <= S(1782);
  in_r.a.ctrl.rett <= S(1783);
  in_r.a.rs1 <= S(1788 downto 1784);
  in_r.a.rfa1 <= S(1796 downto 1789);
  in_r.a.rfa2 <= S(1804 downto 1797);
  in_r.a.rsel1 <= S(1807 downto 1805);
  in_r.a.rsel2 <= S(1810 downto 1808);
  in_r.a.rfe1 <= S(1811);
  in_r.a.rfe2 <= S(1812);
  in_r.a.cwp <= S(1815 downto 1813);
  in_r.a.imm <= S(1847 downto 1816);
  in_r.a.ldcheck1 <= S(1848);
  in_r.a.ldcheck2 <= S(1849);
  in_r.a.ldchkra <= S(1850);
  in_r.a.ldchkex <= S(1851);
  in_r.a.su <= S(1852);
  in_r.a.et <= S(1853);
  in_r.a.wovf <= S(1854);
  in_r.a.wunf <= S(1855);
  in_r.a.ticc <= S(1856);
  in_r.a.jmpl <= S(1857);
  in_r.a.step <= S(1858);
  in_r.a.mulstart <= S(1859);
  in_r.a.divstart <= S(1860);
  in_r.a.bp <= S(1861);
  in_r.a.nobp <= S(1862);
  in_r.e.ctrl.pc <= S(1892 downto 1863);
  in_r.e.ctrl.inst <= S(1924 downto 1893);
  in_r.e.ctrl.cnt <= S(1926 downto 1925);
  in_r.e.ctrl.rd <= S(1934 downto 1927);
  in_r.e.ctrl.tt <= S(1940 downto 1935);
  in_r.e.ctrl.trap <= S(1941);
  in_r.e.ctrl.annul <= S(1942);
  in_r.e.ctrl.wreg <= S(1943);
  in_r.e.ctrl.wicc <= S(1944);
  in_r.e.ctrl.wy <= S(1945);
  in_r.e.ctrl.ld <= S(1946);
  in_r.e.ctrl.pv <= S(1947);
  in_r.e.ctrl.rett <= S(1948);
  in_r.e.op1 <= S(1980 downto 1949);
  in_r.e.op2 <= S(2012 downto 1981);
  in_r.e.aluop <= S(2015 downto 2013);
  in_r.e.alusel <= S(2017 downto 2016);
  in_r.e.aluadd <= S(2018);
  in_r.e.alucin <= S(2019);
  in_r.e.ldbp1 <= S(2020);
  in_r.e.ldbp2 <= S(2021);
  in_r.e.invop2 <= S(2022);
  in_r.e.shcnt <= S(2027 downto 2023);
  in_r.e.sari <= S(2028);
  in_r.e.shleft <= S(2029);
  in_r.e.ymsb <= S(2030);
  in_r.e.rd <= S(2035 downto 2031);
  in_r.e.jmpl <= S(2036);
  in_r.e.su <= S(2037);
  in_r.e.et <= S(2038);
  in_r.e.cwp <= S(2041 downto 2039);
  in_r.e.icc <= S(2045 downto 2042);
  in_r.e.mulstep <= S(2046);
  in_r.e.mul <= S(2047);
  in_r.e.mac <= S(2048);
  in_r.e.bp <= S(2049);
  in_r.m.ctrl.pc <= S(2079 downto 2050);
  in_r.m.ctrl.inst <= S(2111 downto 2080);
  in_r.m.ctrl.cnt <= S(2113 downto 2112);
  in_r.m.ctrl.rd <= S(2121 downto 2114);
  in_r.m.ctrl.tt <= S(2127 downto 2122);
  in_r.m.ctrl.trap <= S(2128);
  in_r.m.ctrl.annul <= S(2129);
  in_r.m.ctrl.wreg <= S(2130);
  in_r.m.ctrl.wicc <= S(2131);
  in_r.m.ctrl.wy <= S(2132);
  in_r.m.ctrl.ld <= S(2133);
  in_r.m.ctrl.pv <= S(2134);
  in_r.m.ctrl.rett <= S(2135);
  in_r.m.result <= S(2167 downto 2136);
  in_r.m.y <= S(2199 downto 2168);
  in_r.m.icc <= S(2203 downto 2200);
  in_r.m.nalign <= S(2204);
  in_r.m.dci.signed <= S(2205);
  in_r.m.dci.enaddr <= S(2206);
  in_r.m.dci.read <= S(2207);
  in_r.m.dci.write <= S(2208);
  in_r.m.dci.lock <= S(2209);
  in_r.m.dci.dsuen <= S(2210);
  in_r.m.dci.size <= S(2212 downto 2211);
  in_r.m.dci.asi <= S(2220 downto 2213);
  in_r.m.werr <= S(2221);
  in_r.m.wcwp <= S(2222);
  in_r.m.irqen <= S(2223);
  in_r.m.irqen2 <= S(2224);
  in_r.m.mac <= S(2225);
  in_r.m.divz <= S(2226);
  in_r.m.su <= S(2227);
  in_r.m.mul <= S(2228);
  in_r.m.casa <= S(2229);
  in_r.m.casaz <= S(2230);
  in_r.x.ctrl.pc <= S(2260 downto 2231);
  in_r.x.ctrl.inst <= S(2292 downto 2261);
  in_r.x.ctrl.cnt <= S(2294 downto 2293);
  in_r.x.ctrl.rd <= S(2302 downto 2295);
  in_r.x.ctrl.tt <= S(2308 downto 2303);
  in_r.x.ctrl.trap <= S(2309);
  in_r.x.ctrl.annul <= S(2310);
  in_r.x.ctrl.wreg <= S(2311);
  in_r.x.ctrl.wicc <= S(2312);
  in_r.x.ctrl.wy <= S(2313);
  in_r.x.ctrl.ld <= S(2314);
  in_r.x.ctrl.pv <= S(2315);
  in_r.x.ctrl.rett <= S(2316);
  in_r.x.result <= S(2348 downto 2317);
  in_r.x.y <= S(2380 downto 2349);
  in_r.x.icc <= S(2384 downto 2381);
  in_r.x.annul_all <= S(2385);
  in_r.x.data(0) <= S(2417 downto 2386);
  in_r.x.data(1) <= S(2449 downto 2418);
  in_r.x.set <= S(2450 downto 2450);
  in_r.x.mexc <= S(2451);
  in_r.x.dci.signed <= S(2452);
  in_r.x.dci.enaddr <= S(2453);
  in_r.x.dci.read <= S(2454);
  in_r.x.dci.write <= S(2455);
  in_r.x.dci.lock <= S(2456);
  in_r.x.dci.dsuen <= S(2457);
  in_r.x.dci.size <= S(2459 downto 2458);
  in_r.x.dci.asi <= S(2467 downto 2460);
  in_r.x.laddr <= S(2469 downto 2468);
  --  in_r.x.rstate <= S(2471 downto 2470);
  in_r.x.npc <= S(2474 downto 2472);
  in_r.x.intack <= S(2475);
  in_r.x.ipend <= S(2476);
  in_r.x.mac <= S(2477);
  in_r.x.debug <= S(2478);
  in_r.x.nerror <= S(2479);
  fpo.dbg.data <= S(2511 downto 2480);
  in_r.f.pc <= S(2541 downto 2512);
  in_r.w.s.cwp <= S(2544 downto 2542);
  in_r.w.s.icc <= S(2548 downto 2545);
  in_r.w.s.tt <= S(2556 downto 2549);
  in_r.w.s.tba <= S(2576 downto 2557);
  in_r.w.s.wim <= S(2584 downto 2577);
  in_r.w.s.pil <= S(2588 downto 2585);
  in_r.w.s.ec <= S(2589);
  in_r.w.s.ef <= S(2590);
  in_r.w.s.ps <= S(2591);
  in_r.w.s.s <= S(2592);
  in_r.w.s.et <= S(2593);
  in_r.w.s.y <= S(2625 downto 2594);
  in_r.w.s.asr18 <= S(2657 downto 2626);
  in_r.w.s.svt <= S(2658);
  in_r.w.s.dwt <= S(2659);
  in_r.w.s.dbp <= S(2660);
  in_r.w.result <= S(2692 downto 2661);
  in_wpr(0).addr <= S(2722 downto 2693);
  in_wpr(0).mask <= S(2752 downto 2723);
  in_wpr(0).exec <= S(2753);
  in_wpr(0).load <= S(2754);
  in_wpr(0).store <= S(2755);
  in_wpr(1).addr <= S(2785 downto 2756);
  in_wpr(1).mask <= S(2815 downto 2786);
  in_wpr(1).exec <= S(2816);
  in_wpr(1).load <= S(2817);
  in_wpr(1).store <= S(2818);
  in_wpr(2).addr <= S(2848 downto 2819);
  in_wpr(2).mask <= S(2878 downto 2849);
  in_wpr(2).exec <= S(2879);
  in_wpr(2).load <= S(2880);
  in_wpr(2).store <= S(2881);
  in_wpr(3).addr <= S(2911 downto 2882);
  in_wpr(3).mask <= S(2941 downto 2912);
  in_wpr(3).exec <= S(2942);
  in_wpr(3).load <= S(2943);
  in_wpr(3).store <= S(2944);
  in_ir.addr <= S(2974 downto 2945);
  in_ir.pwd <= S(2975);
  in_dsur.asi <= S(2983 downto 2976);
  in_dsur.tbufcnt <= S(2991 downto 2984);
  in_dsur.err <= S(2992);
  in_dsur.tt <= S(3000 downto 2993);
  in_dsur.crdy <= S(3002 downto 3001);

  
--  in_r.x.rstate <= dsu2;


  tt : iu3 generic map 
  (
    CFG_NWIN,--nwindows,
    CFG_ISETS,--isets,
    CFG_DSETS,--dsets,
    CFG_FPU,--fpu,
    CFG_V8,--v8,
    0,--cp,
    CFG_MAC,--mac,
    CFG_DSU,--dsu,
    CFG_NWP,--nwp,
    CFG_PCLOW,--pclow,
	  CFG_NOTAG,--notag,
	  0,--hindex,
	  CFG_LDDEL,--lddel,
	  1,--IRFWT,
	  CFG_DISAS,--disas,
	  CFG_ITBSZ,--tbuf,
	  CFG_PWD,--pwd,
	  CFG_SVT,--svt,
	  CFG_RSTADDR,--rstaddr,
	  (CFG_NCPU-1),--smp,
	  0,--fabtech,
	  0,--clk2x,
	  CFG_BP--bp
	)port map 
	(
    inClk,
    inNRst,
    holdnx,
    ici, ico, dci, dco, rfi, rfo,
    irqi, irqo, dbgi, dbgo, muli, mulo, divi, divo, fpo, fpi, cpo, cpi, tbo, tbi, inClk);

  states : process(inClk, S(2471 downto 2470))
  begin
    if(S(2471 downto 2470)="00")     then in_r.x.rstate <= run;
    elsif (S(2471 downto 2470)="01") then in_r.x.rstate <= trap;
    elsif (S(2471 downto 2470)="10") then in_r.x.rstate <= dsu1;
    else                                  in_r.x.rstate <= dsu2;
    end if;
  end process states;
  
  comb : process(inNRst,inClk, in_r, dbgi, in_wpr, mulo, divo, dco, fpo,cpo)
   	variable pd : std_ulogic; 
  begin
	  if(iClkCnt=152) then
  	  --print("break");
	  end if;

    
    if(rising_edge(inClk) and (iClkCnt>14)) then
     if(ch_ici/=ici) then print("Err: ici");  iErrCnt:=iErrCnt+1; end if;
     if(ch_dci/=dci) then print("Err: dci");  iErrCnt:=iErrCnt+1; end if;
     if(ch_rfi/=rfi) then print("Err: rfi");  iErrCnt:=iErrCnt+1; end if;
     if(ch_tbi/=tbi) then print("Err: tbi");  iErrCnt:=iErrCnt+1; end if;
     if(ch_dbgo/=dbgo) then print("Err: dbgo");  iErrCnt:=iErrCnt+1; end if;
     if(ch_irqo/=irqo) then print("Err: irqo");  iErrCnt:=iErrCnt+1; end if;
     if(ch_muli/=muli) then print("Err: muli");  iErrCnt:=iErrCnt+1; end if;
     if(ch_divi/=divi) then print("Err: divi");  iErrCnt:=iErrCnt+1; end if;
     if(ch_rfi/=rfi) then print("Err: rfi");  iErrCnt:=iErrCnt+1; end if;
--     if(ch_cpi/=cpi) then print("Err: cpi");  iErrCnt:=iErrCnt+1; end if;
    end if;
  end process comb;
  

procCheck : process (inClk)
begin
  if(rising_edge(inClk) and (iClkCnt>6)) then
--    if(ch_dbgm/=ch_ico.idle)then print("Err: ico.idle");  iErrCnt:=iErrCnt+1; end if;
  end if;
end process procCheck;

  
end;
