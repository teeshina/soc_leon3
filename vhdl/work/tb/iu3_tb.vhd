
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
  constant STRING_SIZE : integer := 3177; -- string size = index of the last element

 
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
  signal t_inClk  : std_logic;
  signal t_cramo_data0  : std_logic_vector(31 downto 0);
  signal t_mcio_data  : std_logic_vector(31 downto 0);
  signal t_xc_result  : std_logic_vector(31 downto 0);
  signal t_ex_op1  : std_logic_vector(31 downto 0);
  signal t_v_a_rsel1  : std_logic_vector(7 downto 0);
  signal t_r_a_rsel1  : std_logic_vector(7 downto 0);
  signal t_ra_op1  : std_logic_vector(31 downto 0);
  
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
      --wait for CLK_HPERIOD/2;
      wait until rising_edge(inClk);
      iClkCnt := iClkCnt + 1;
      if(iClkCnt=3704) then
        print("Break: " & tost(iClkCnt));
      end if;
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
  dco.icdiag.scanen <= S(408);
  dco.cache <= S(409);
  dco.idle <= S(410);
  dco.scanen <= S(411);
  dco.testen <= S(412);
  rfo.data1 <= S(444 downto 413);
  rfo.data2 <= S(476 downto 445);
  irqi.irl <= S(480 downto 477);
  irqi.rst <= S(481);
  irqi.run <= S(482);
  irqi.rstvec <= S(502 downto 483);
  irqi.iact <= S(503);
  irqi.index <= S(507 downto 504);
  dbgi.dsuen <= S(508);
  dbgi.denable <= S(509);
  dbgi.dbreak <= S(510);
  dbgi.step <= S(511);
  dbgi.halt <= S(512);
  dbgi.reset <= S(513);
  dbgi.dwrite <= S(514);
  dbgi.daddr <= S(536 downto 515);
  dbgi.ddata <= S(568 downto 537);
  dbgi.btrapa <= S(569);
  dbgi.btrape <= S(570);
  dbgi.berror <= S(571);
  dbgi.bwatch <= S(572);
  dbgi.bsoft <= S(573);
  dbgi.tenable <= S(574);
  dbgi.timer <= S(605 downto 575);
  mulo.ready <= S(606);
  mulo.nready <= S(607);
  mulo.icc <= S(611 downto 608);
  mulo.result <= S(675 downto 612);
  divo.ready <= S(676);
  divo.nready <= S(677);
  divo.icc <= S(681 downto 678);
  divo.result <= S(713 downto 682);
  tbo.data(63 downto 0) <= S(777 downto 714);
  tbo.data(127 downto 64) <= S(841 downto 778);
  ch_ici.rpc <= S(873 downto 842);
  ch_ici.fpc <= S(905 downto 874);
  ch_ici.dpc <= S(937 downto 906);
  ch_ici.rbranch <= S(938);
  ch_ici.fbranch <= S(939);
  ch_ici.inull <= S(940);
  ch_ici.su <= S(941);
  ch_ici.flush <= S(942);
  ch_ici.flushl <= S(943);
  ch_ici.fline <= S(972 downto 944);
  ch_ici.pnull <= S(973);
  ch_dci.asi <= S(981 downto 974);
  ch_dci.maddress <= S(1013 downto 982);
  ch_dci.eaddress <= S(1045 downto 1014);
  ch_dci.edata <= S(1077 downto 1046);
  ch_dci.size <= S(1079 downto 1078);
  ch_dci.enaddr <= S(1080);
  ch_dci.eenaddr <= S(1081);
  ch_dci.nullify <= S(1082);
  ch_dci.lock <= S(1083);
  ch_dci.read <= S(1084);
  ch_dci.write <= S(1085);
  ch_dci.flush <= S(1086);
  ch_dci.flushl <= S(1087);
  ch_dci.dsuen <= S(1088);
  ch_dci.msu <= S(1089);
  ch_dci.esu <= S(1090);
  ch_dci.intack <= S(1091);
  ch_rfi.raddr1 <= S(1101 downto 1092);
  ch_rfi.raddr2 <= S(1111 downto 1102);
  ch_rfi.waddr <= S(1121 downto 1112);
  ch_rfi.wdata <= S(1153 downto 1122);
  ch_rfi.ren1 <= S(1154);
  ch_rfi.ren2 <= S(1155);
  ch_rfi.wren <= S(1156);
  ch_rfi.diag <= S(1160 downto 1157);
  ch_irqo.intack <= S(1161);
  ch_irqo.irl <= S(1165 downto 1162);
  ch_irqo.pwd <= S(1166);
  ch_irqo.fpen <= S(1167);
  ch_irqo.idle <= S(1168);
  ch_dbgo.data <= S(1200 downto 1169);
  ch_dbgo.crdy <= S(1201);
  ch_dbgo.dsu <= S(1202);
  ch_dbgo.dsumode <= S(1203);
  ch_dbgo.error <= S(1204);
  ch_dbgo.halt <= S(1205);
  ch_dbgo.pwd <= S(1206);
  ch_dbgo.idle <= S(1207);
  ch_dbgo.ipend <= S(1208);
  ch_dbgo.icnt <= S(1209);
  ch_dbgo.fcnt <= S(1210);
  ch_dbgo.optype <= S(1216 downto 1211);
  ch_dbgo.bpmiss <= S(1217);
  ch_dbgo.istat.cmiss <= S(1218);
  ch_dbgo.istat.tmiss <= S(1219);
  ch_dbgo.istat.chold <= S(1220);
  ch_dbgo.istat.mhold <= S(1221);
  ch_dbgo.dstat.cmiss <= S(1222);
  ch_dbgo.dstat.tmiss <= S(1223);
  ch_dbgo.dstat.chold <= S(1224);
  ch_dbgo.dstat.mhold <= S(1225);
  ch_dbgo.wbhold <= S(1226);
  ch_dbgo.su <= S(1227);
  ch_muli.op1 <= S(1260 downto 1228);
  ch_muli.op2 <= S(1293 downto 1261);
  ch_muli.flush <= S(1294);
  ch_muli.signed <= S(1295);
  ch_muli.start <= S(1296);
  ch_muli.mac <= S(1297);
  --ch_muli.acc <= S(1337 downto 1298);
  ch_divi.y <= S(1370 downto 1338);
  ch_divi.op1 <= S(1403 downto 1371);
  ch_divi.op2 <= S(1436 downto 1404);
  ch_divi.flush <= S(1437);
  ch_divi.Signed <= S(1438);
  ch_divi.start <= S(1439);
  ch_tbi.addr <= S(1451 downto 1440);
  ch_tbi.data(63 downto 0) <= S(1515 downto 1452);
  ch_tbi.data(127 downto 64) <= S(1579 downto 1516);
  ch_tbi.enable <= S(1580);
  ch_tbi.write <= S(1584 downto 1581);
  ch_tbi.diag <= S(1588 downto 1585);
  in_r.d.pc <= S(1618 downto 1589);
  in_r.d.inst(0) <= S(1650 downto 1619);
  in_r.d.inst(1) <= S(1682 downto 1651);
  in_r.d.cwp <= S(1685 downto 1683);
  in_r.d.set <= S(1686 downto 1686);
  in_r.d.mexc <= S(1687);
  in_r.d.cnt <= S(1689 downto 1688);
  in_r.d.pv <= S(1690);
  in_r.d.annul <= S(1691);
  in_r.d.inull <= S(1692);
  in_r.d.step <= S(1693);
  in_r.d.divrdy <= S(1694);
  in_r.a.ctrl.pc <= S(1724 downto 1695);
  in_r.a.ctrl.inst <= S(1756 downto 1725);
  in_r.a.ctrl.cnt <= S(1758 downto 1757);
  in_r.a.ctrl.rd <= S(1766 downto 1759);
  in_r.a.ctrl.tt <= S(1772 downto 1767);
  in_r.a.ctrl.trap <= S(1773);
  in_r.a.ctrl.annul <= S(1774);
  in_r.a.ctrl.wreg <= S(1775);
  in_r.a.ctrl.wicc <= S(1776);
  in_r.a.ctrl.wy <= S(1777);
  in_r.a.ctrl.ld <= S(1778);
  in_r.a.ctrl.pv <= S(1779);
  in_r.a.ctrl.rett <= S(1780);
  in_r.a.rs1 <= S(1785 downto 1781);
  in_r.a.rfa1 <= S(1793 downto 1786);
  in_r.a.rfa2 <= S(1801 downto 1794);
  in_r.a.rsel1 <= S(1804 downto 1802);
  in_r.a.rsel2 <= S(1807 downto 1805);
  in_r.a.rfe1 <= S(1808);
  in_r.a.rfe2 <= S(1809);
  in_r.a.cwp <= S(1812 downto 1810);
  in_r.a.imm <= S(1844 downto 1813);
  in_r.a.ldcheck1 <= S(1845);
  in_r.a.ldcheck2 <= S(1846);
  in_r.a.ldchkra <= S(1847);
  in_r.a.ldchkex <= S(1848);
  in_r.a.su <= S(1849);
  in_r.a.et <= S(1850);
  in_r.a.wovf <= S(1851);
  in_r.a.wunf <= S(1852);
  in_r.a.ticc <= S(1853);
  in_r.a.jmpl <= S(1854);
  in_r.a.step <= S(1855);
  in_r.a.mulstart <= S(1856);
  in_r.a.divstart <= S(1857);
  in_r.a.bp <= S(1858);
  in_r.a.nobp <= S(1859);
  in_r.e.ctrl.pc <= S(1889 downto 1860);
  in_r.e.ctrl.inst <= S(1921 downto 1890);
  in_r.e.ctrl.cnt <= S(1923 downto 1922);
  in_r.e.ctrl.rd <= S(1931 downto 1924);
  in_r.e.ctrl.tt <= S(1937 downto 1932);
  in_r.e.ctrl.trap <= S(1938);
  in_r.e.ctrl.annul <= S(1939);
  in_r.e.ctrl.wreg <= S(1940);
  in_r.e.ctrl.wicc <= S(1941);
  in_r.e.ctrl.wy <= S(1942);
  in_r.e.ctrl.ld <= S(1943);
  in_r.e.ctrl.pv <= S(1944);
  in_r.e.ctrl.rett <= S(1945);
  in_r.e.op1 <= S(1977 downto 1946);
  in_r.e.op2 <= S(2009 downto 1978);
  in_r.e.aluop <= S(2012 downto 2010);
  in_r.e.alusel <= S(2014 downto 2013);
  in_r.e.aluadd <= S(2015);
  in_r.e.alucin <= S(2016);
  in_r.e.ldbp1 <= S(2017);
  in_r.e.ldbp2 <= S(2018);
  in_r.e.invop2 <= S(2019);
  in_r.e.shcnt <= S(2024 downto 2020);
  in_r.e.sari <= S(2025);
  in_r.e.shleft <= S(2026);
  in_r.e.ymsb <= S(2027);
  in_r.e.rd <= S(2032 downto 2028);
  in_r.e.jmpl <= S(2033);
  in_r.e.su <= S(2034);
  in_r.e.et <= S(2035);
  in_r.e.cwp <= S(2038 downto 2036);
  in_r.e.icc <= S(2042 downto 2039);
  in_r.e.mulstep <= S(2043);
  in_r.e.mul <= S(2044);
  in_r.e.mac <= S(2045);
  in_r.e.bp <= S(2046);
  in_r.m.ctrl.pc <= S(2076 downto 2047);
  in_r.m.ctrl.inst <= S(2108 downto 2077);
  in_r.m.ctrl.cnt <= S(2110 downto 2109);
  in_r.m.ctrl.rd <= S(2118 downto 2111);
  in_r.m.ctrl.tt <= S(2124 downto 2119);
  in_r.m.ctrl.trap <= S(2125);
  in_r.m.ctrl.annul <= S(2126);
  in_r.m.ctrl.wreg <= S(2127);
  in_r.m.ctrl.wicc <= S(2128);
  in_r.m.ctrl.wy <= S(2129);
  in_r.m.ctrl.ld <= S(2130);
  in_r.m.ctrl.pv <= S(2131);
  in_r.m.ctrl.rett <= S(2132);
  in_r.m.result <= S(2164 downto 2133);
  in_r.m.y <= S(2196 downto 2165);
  in_r.m.icc <= S(2200 downto 2197);
  in_r.m.nalign <= S(2201);
  in_r.m.dci.signed <= S(2202);
  in_r.m.dci.enaddr <= S(2203);
  in_r.m.dci.read <= S(2204);
  in_r.m.dci.write <= S(2205);
  in_r.m.dci.lock <= S(2206);
  in_r.m.dci.dsuen <= S(2207);
  in_r.m.dci.size <= S(2209 downto 2208);
  in_r.m.dci.asi <= S(2217 downto 2210);
  in_r.m.werr <= S(2218);
  in_r.m.wcwp <= S(2219);
  in_r.m.irqen <= S(2220);
  in_r.m.irqen2 <= S(2221);
  in_r.m.mac <= S(2222);
  in_r.m.divz <= S(2223);
  in_r.m.su <= S(2224);
  in_r.m.mul <= S(2225);
  in_r.m.casa <= S(2226);
  in_r.m.casaz <= S(2227);
  in_r.x.ctrl.pc <= S(2257 downto 2228);
  in_r.x.ctrl.inst <= S(2289 downto 2258);
  in_r.x.ctrl.cnt <= S(2291 downto 2290);
  in_r.x.ctrl.rd <= S(2299 downto 2292);
  in_r.x.ctrl.tt <= S(2305 downto 2300);
  in_r.x.ctrl.trap <= S(2306);
  in_r.x.ctrl.annul <= S(2307);
  in_r.x.ctrl.wreg <= S(2308);
  in_r.x.ctrl.wicc <= S(2309);
  in_r.x.ctrl.wy <= S(2310);
  in_r.x.ctrl.ld <= S(2311);
  in_r.x.ctrl.pv <= S(2312);
  in_r.x.ctrl.rett <= S(2313);
  in_r.x.result <= S(2345 downto 2314);
  in_r.x.y <= S(2377 downto 2346);
  in_r.x.icc <= S(2381 downto 2378);
  in_r.x.annul_all <= S(2382);
  in_r.x.data(0) <= S(2414 downto 2383);
  in_r.x.data(1) <= S(2446 downto 2415);
  in_r.x.set <= S(2447 downto 2447);
  in_r.x.mexc <= S(2448);
  in_r.x.dci.signed <= S(2449);
  in_r.x.dci.enaddr <= S(2450);
  in_r.x.dci.read <= S(2451);
  in_r.x.dci.write <= S(2452);
  in_r.x.dci.lock <= S(2453);
  in_r.x.dci.dsuen <= S(2454);
  in_r.x.dci.size <= S(2456 downto 2455);
  in_r.x.dci.asi <= S(2464 downto 2457);
  in_r.x.laddr <= S(2466 downto 2465);
  --  in_r.x.rstate <= S(2468 downto 2467);
  in_r.x.npc <= S(2471 downto 2469);
  in_r.x.intack <= S(2472);
  in_r.x.ipend <= S(2473);
  in_r.x.mac <= S(2474);
  in_r.x.debug <= S(2475);
  in_r.x.nerror <= S(2476);
  --fpo.dbg.data <= S(2508 downto 2477);
  in_r.f.pc <= S(2538 downto 2509);
  in_r.w.s.cwp <= S(2541 downto 2539);
  in_r.w.s.icc <= S(2545 downto 2542);
  in_r.w.s.tt <= S(2553 downto 2546);
  in_r.w.s.tba <= S(2573 downto 2554);
  in_r.w.s.wim <= S(2581 downto 2574);
  in_r.w.s.pil <= S(2585 downto 2582);
  in_r.w.s.ec <= S(2586);
  in_r.w.s.ef <= S(2587);
  in_r.w.s.ps <= S(2588);
  in_r.w.s.s <= S(2589);
  in_r.w.s.et <= S(2590);
  in_r.w.s.y <= S(2622 downto 2591);
  in_r.w.s.asr18 <= S(2654 downto 2623);
  in_r.w.s.svt <= S(2655);
  in_r.w.s.dwt <= S(2656);
  in_r.w.s.dbp <= S(2657);
  in_r.w.result <= S(2689 downto 2658);
  in_wpr(0).addr <= S(2719 downto 2690);
  in_wpr(0).mask <= S(2749 downto 2720);
  in_wpr(0).exec <= S(2750);
  in_wpr(0).load <= S(2751);
  in_wpr(0).store <= S(2752);
  in_wpr(1).addr <= S(2782 downto 2753);
  in_wpr(1).mask <= S(2812 downto 2783);
  in_wpr(1).exec <= S(2813);
  in_wpr(1).load <= S(2814);
  in_wpr(1).store <= S(2815);
  in_wpr(2).addr <= S(2845 downto 2816);
  in_wpr(2).mask <= S(2875 downto 2846);
  in_wpr(2).exec <= S(2876);
  in_wpr(2).load <= S(2877);
  in_wpr(2).store <= S(2878);
  in_wpr(3).addr <= S(2908 downto 2879);
  in_wpr(3).mask <= S(2938 downto 2909);
  in_wpr(3).exec <= S(2939);
  in_wpr(3).load <= S(2940);
  in_wpr(3).store <= S(2941);
  in_ir.addr <= S(2971 downto 2942);
  in_ir.pwd <= S(2972);
  in_dsur.asi <= S(2980 downto 2973);
  in_dsur.tbufcnt <= S(2988 downto 2981);
  in_dsur.err <= S(2989);
  in_dsur.tt <= S(2997 downto 2990);
  in_dsur.crdy <= S(2999 downto 2998);
  t_inClk <= S(3000);
  t_cramo_data0 <= S(3032 downto 3001);
  t_mcio_data <= S(3064 downto 3033);
  t_xc_result <= S(3096 downto 3065);
  t_ex_op1 <= S(3128 downto 3097);
  t_v_a_rsel1 <= S(3136 downto 3129);
  t_r_a_rsel1 <= S(3144 downto 3137);
  t_ra_op1 <= S(3176 downto 3145);

  
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
    if(rising_edge(inClk))then
      --iClkCnt := iClkCnt + 1;
    end if;
    if(S(2471 downto 2470)="00")     then in_r.x.rstate <= run;
    elsif (S(2471 downto 2470)="01") then in_r.x.rstate <= trap;
    elsif (S(2471 downto 2470)="10") then in_r.x.rstate <= dsu1;
    else                                  in_r.x.rstate <= dsu2;
    end if;
  end process states;
  
  procCheck : process(inNRst,inClk, in_r, dbgi, in_wpr, mulo, divo, dco, fpo,cpo)
   	variable pd : std_ulogic; 
  begin
    if(rising_edge(inClk) and (iClkCnt>6)) then
      if((ici.rpc(31)/='U')and(ici.rpc(31)/='X')) then
        if(ch_ici.rpc/=ici.rpc) then print("Err: ici.rpc");  iErrCnt:=iErrCnt+1; end if;
      end if;
      if((ici.fpc(31)/='U')and(ici.fpc(31)/='X')) then
        if(ch_ici.fpc/=ici.fpc) then print("Err: ici.fpc");  iErrCnt:=iErrCnt+1; end if;
      end if;
      if((ici.dpc(31)/='U')and(ici.dpc(31)/='X')) then
        if(ch_ici.dpc/=ici.dpc) then print("Err: ici.dpc");  iErrCnt:=iErrCnt+1; end if;
      end if;
      if(ch_ici.rbranch/=ici.rbranch) then print("Err: ici.rbranch");  iErrCnt:=iErrCnt+1; end if;
      if(ch_ici.fbranch/=ici.fbranch) then print("Err: ici.fbranch");  iErrCnt:=iErrCnt+1; end if;
      if(ch_ici.inull/=ici.inull) then print("Err: ici.inull");  iErrCnt:=iErrCnt+1; end if;
      if(ch_ici.su/=ici.su) then print("Err: ici.su");  iErrCnt:=iErrCnt+1; end if;
      if(ch_ici.flush/=ici.flush) then print("Err: ici.flush");  iErrCnt:=iErrCnt+1; end if;
      if(ch_ici.flushl/=ici.flushl) then print("Err: ici.flushl");  iErrCnt:=iErrCnt+1; end if;
      if(ch_ici.fline/=ici.fline) then print("Err: ici.fline");  iErrCnt:=iErrCnt+1; end if;
      if(ch_ici.pnull/=ici.pnull) then print("Err: ici.pnull");  iErrCnt:=iErrCnt+1; end if;
      
      if(dci.asi/="UUUUUUUU") then 
        if(ch_dci.asi/=dci.asi) then print("Err: dci.asi");  iErrCnt:=iErrCnt+1; end if;
      end if;
      if(dci.maddress(0)/='U') then
        if(ch_dci.maddress/=dci.maddress) then print("Err: dci.maddress");  iErrCnt:=iErrCnt+1; end if;
      end if;
      if((dci.eaddress(0)/='U')and(dci.eaddress(0)/='X')) then
        if(ch_dci.eaddress/=dci.eaddress) then print("Err: dci.eaddress");  iErrCnt:=iErrCnt+1; end if;
      end if;
      if(dci.edata(0)/='U') then
        if(ch_dci.edata/=dci.edata) then print("Err: dci.edata");  iErrCnt:=iErrCnt+1; end if;
      end if;
      if(ch_dci.size/=dci.size) then print("Err: dci.size");  iErrCnt:=iErrCnt+1; end if;
      if(ch_dci.enaddr/=dci.enaddr) then print("Err: dci.enaddr");  iErrCnt:=iErrCnt+1; end if;
      if(ch_dci.eenaddr/=dci.eenaddr) then print("Err: dci.eenaddr");  iErrCnt:=iErrCnt+1; end if;
      if(ch_dci.nullify/=dci.nullify) then print("Err: dci.nullify");  iErrCnt:=iErrCnt+1; end if;
      if(ch_dci.lock/=dci.lock) then print("Err: dci.lock");  iErrCnt:=iErrCnt+1; end if;
      if(ch_dci.read/=dci.read) then print("Err: dci.read");  iErrCnt:=iErrCnt+1; end if;
      if(ch_dci.write/=dci.write) then print("Err: dci.write");  iErrCnt:=iErrCnt+1; end if;
      if(ch_dci.flush/=dci.flush) then print("Err: dci.flush");  iErrCnt:=iErrCnt+1; end if;
      if(ch_dci.flushl/=dci.flushl) then print("Err: dci.flushl");  iErrCnt:=iErrCnt+1; end if;
      if(ch_dci.dsuen/=dci.dsuen) then print("Err: dci.dsuen");  iErrCnt:=iErrCnt+1; end if;
      if(ch_dci.msu/=dci.msu) then print("Err: dci.msu");  iErrCnt:=iErrCnt+1; end if;
      if(ch_dci.esu/=dci.esu) then print("Err: dci.esu");  iErrCnt:=iErrCnt+1; end if;
      if(ch_dci.intack/=dci.intack) then print("Err: dci.intack");  iErrCnt:=iErrCnt+1; end if;

      if(rfi.raddr1(4)/='X') then
        if(ch_rfi.raddr1/=rfi.raddr1) then print("Err: rfi.raddr1");  iErrCnt:=iErrCnt+1; end if;
      end if;
      if(rfi.raddr2(4)/='X') then
        if(ch_rfi.raddr2/=rfi.raddr2) then print("Err: rfi.raddr2");  iErrCnt:=iErrCnt+1; end if;
      end if;
      if((rfi.waddr(4)/='X')and(rfi.waddr(4)/='U')) then
        if(ch_rfi.waddr/=rfi.waddr) then print("Err: rfi.waddr");  iErrCnt:=iErrCnt+1; end if;
      end if;
      if((rfi.wdata(0)/='U')and(rfi.wdata(0)/='-')) then
        if(ch_rfi.wdata/=rfi.wdata) then print("Err: rfi.wdata");  iErrCnt:=iErrCnt+1; end if;
      end if;
      if(ch_rfi.ren1/=rfi.ren1) then print("Err: rfi.ren1");  iErrCnt:=iErrCnt+1; end if;
      if(ch_rfi.ren2/=rfi.ren2) then print("Err: rfi.ren2");  iErrCnt:=iErrCnt+1; end if;
      if(ch_rfi.wren/=rfi.wren) then print("Err: rfi.wren");  iErrCnt:=iErrCnt+1; end if;
      if(ch_rfi.diag/=rfi.diag) then print("Err: rfi.diag");  iErrCnt:=iErrCnt+1; end if;

      if(tbi.addr(0)/='U') then 
        if(ch_tbi.addr/=tbi.addr) then print("Err: tbi.addr");  iErrCnt:=iErrCnt+1; end if;
      end if;
      if((tbi.data(63)/='U')and(tbi.data(63)/='X')and(tbi.data(75)/='U')) then 
        if(ch_tbi.data/=tbi.data) then print("Err: tbi.data");  iErrCnt:=iErrCnt+1; end if;
      end if;
      if(ch_tbi.enable/=tbi.enable) then print("Err: tbi.enable");  iErrCnt:=iErrCnt+1; end if;
      if(ch_tbi.write/=tbi.write) then print("Err: tbi.write");  iErrCnt:=iErrCnt+1; end if;
      if(ch_tbi.diag/=tbi.diag) then print("Err: tbi.diag");  iErrCnt:=iErrCnt+1; end if;

      if(dbgo.data(31)/='U') then 
        if(ch_dbgo/=dbgo) then print("Err: dbgo");  iErrCnt:=iErrCnt+1; end if;
      end if;

      if(ch_irqo/=irqo) then print("Err: irqo");  iErrCnt:=iErrCnt+1; end if;

      if(ch_muli.op1/=muli.op1) then print("Err: muli.op1");  iErrCnt:=iErrCnt+1; end if;      
      if(ch_muli.op2/=muli.op2) then print("Err: muli.op2");  iErrCnt:=iErrCnt+1; end if;      
      if(ch_muli.flush/=muli.flush) then print("Err: muli.flush");  iErrCnt:=iErrCnt+1; end if;
      if(ch_muli.signed/=muli.signed) then print("Err: muli.signed");  iErrCnt:=iErrCnt+1; end if;
      if(ch_muli.start/=muli.start) then print("Err: muli.start");  iErrCnt:=iErrCnt+1; end if;
      if(ch_muli.mac/=muli.mac) then print("Err: muli.mac");  iErrCnt:=iErrCnt+1; end if;
      if(muli.acc(0)/='U') then
        if(ch_muli.acc/=muli.acc) then print("Err: muli.acc");  iErrCnt:=iErrCnt+1; end if;
      end if;

      if(divi.y(31)/='U') then
        if(ch_divi.y/=divi.y) then print("Err: divi.y");  iErrCnt:=iErrCnt+1; end if;
      end if;
      if(divi.op1(11)/='U') then
        if(ch_divi.op1/=divi.op1) then print("Err: divi.op1");  iErrCnt:=iErrCnt+1; end if;
      end if;
      if(ch_divi.op2/=divi.op2) then print("Err: divi.op2");  iErrCnt:=iErrCnt+1; end if;
      if(ch_divi.flush/=divi.flush) then print("Err: divi.flush");  iErrCnt:=iErrCnt+1; end if;
      if(ch_divi.signed/=divi.signed) then print("Err: divi.signed");  iErrCnt:=iErrCnt+1; end if;
      if(ch_divi.start/=divi.start) then print("Err: divi.start");  iErrCnt:=iErrCnt+1; end if;
      
--     if(ch_cpi/=cpi) then print("Err: cpi");  iErrCnt:=iErrCnt+1; end if;
    end if;
  end process procCheck;
  

 
end;
