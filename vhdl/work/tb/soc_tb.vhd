library ieee;
use ieee.std_logic_1164.all;
library std;
use std.textio.all;
library grlib;
use grlib.amba.all;
use grlib.stdlib.all;
use grlib.devices.all;
library techmap;
use techmap.gencomp.all;
library gaisler;
use gaisler.misc.all;
use gaisler.jtag.all;
use gaisler.leon3.all;
use gaisler.uart.all;
library work;
use work.all;
use work.config.all;
use work.util_tb.all;



entity soc_leon3_tb is

  constant CLK_HPERIOD : time := 10 ps;
  constant STRING_SIZE : integer := 1377; -- string size = index of the last element

end soc_leon3_tb;
architecture behavior of soc_leon3_tb is
  
  component soc_leon3 port 
  ( 
    inRst       : in std_logic; -- button "Center"
    inDsuBreak  : in std_logic; -- button "North
    inSysClk_p  : in std_logic;
    inSysClk_n  : in std_logic;
    inCTS   : in std_logic;
    inRX    : in std_logic;
    nTRST : in std_logic; -- in: Test Reset
    TCK   : in std_logic;   -- in: Test Clock
    TMS   : in std_logic;   -- in: Test Mode State
    TDI   : in std_logic;   -- in: Test Data Input
    TDO   : out std_logic;   -- out: Test Data Output
    outRTS  : out std_logic;
    outTX   : out std_logic;
    inDIP   : in std_ulogic_vector(7 downto 0);
    outLED  : out std_ulogic_vector(7 downto 0)
  );
  end component;


  -- input/output signals:
  signal inNRst       : std_logic:= '0';
  signal inClk        : std_logic:= '0';
  --JTAG: 
  signal in_trst  : std_logic;
  signal in_tck  : std_logic;
  signal in_tms  : std_logic;
  signal in_tdi  : std_logic;
  signal ch_tdo  : std_logic;
  signal tdo     : std_logic;
  -- UART
  signal RTS          : std_logic;
  signal TX           : std_logic;
  -- others:
  signal LED          : std_ulogic_vector(7 downto 0);

  signal ch_dbgo : l3_debug_out_vector(0 to CFG_NCPU-1);
  signal t_v_x_nerror  : std_logic;
  signal t_rp_error  : std_logic;
  signal t_a_ctrl_inst  : std_logic_vector(31 downto 0);
  signal t_dummy  : std_logic;
  signal t_pc  : std_logic_vector(31 downto 2);
  signal t_npc  : std_logic_vector(31 downto 2);
  signal t_psr  : std_logic_vector(31 downto 0);
  signal t_tbr  : std_logic_vector(31 downto 0);
  signal dsu_status  : std_logic_vector(11 downto 0);
  signal t_ico0_data  : std_logic_vector(31 downto 0);
  signal t_icramo_data  : std_logic_vector(31 downto 0);
  signal t_hrdata  : std_logic_vector(31 downto 0);
  signal jtag_haddr  : std_logic_vector(31 downto 0);
  signal tResetLogic  : std_logic;
  signal tRunTestIdle  : std_logic;
  
  signal t_r : registers;


  
  signal S: std_logic_vector(STRING_SIZE-1 downto 0);
  signal U: std_ulogic_vector(STRING_SIZE-1 downto 0);

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
    file InputFile:TEXT is "e:/soc_leon3_tb.txt";--open read_mode file_name;
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
  


-- signal parsment and assignment
  inNRst <= S(0);
  in_trst <= S(1);
  in_tck <= S(2);
  in_tms <= S(3);
  in_tdi <= S(4);
  ch_tdo <= S(5);

  ch_dbgo(0).error <= S(6);
  t_v_x_nerror <= S(7);
  t_rp_error <= S(8);
  t_a_ctrl_inst <= S(40 downto 9);
  t_dummy <= S(41);
  t_pc <= S(71 downto 42);
  t_npc <= S(101 downto 72);
  t_psr <= S(133 downto 102);
  t_tbr <= S(165 downto 134);
  dsu_status <= S(177 downto 166);
  t_ico0_data <= S(209 downto 178);
  t_icramo_data <= S(241 downto 210);
  t_hrdata <= S(273 downto 242);
  jtag_haddr <= S(305 downto 274);
  tResetLogic <= S(306);
  tRunTestIdle <= S(307);

  t_r.d.pc <= S(337 downto 308);
  t_r.d.inst(0) <= S(369 downto 338);
  t_r.d.inst(1) <= S(401 downto 370);
  t_r.d.cwp <= S(404 downto 402);
  t_r.d.set <= S(405 downto 405);
  t_r.d.mexc <= S(406);
  t_r.d.cnt <= S(408 downto 407);
  t_r.d.pv <= S(409);
  t_r.d.annul <= S(410);
  t_r.d.inull <= S(411);
  t_r.d.step <= S(412);
  t_r.d.divrdy <= S(413);
  t_r.a.ctrl.pc <= S(443 downto 414);
  t_r.a.ctrl.inst <= S(475 downto 444);
  t_r.a.ctrl.cnt <= S(477 downto 476);
  t_r.a.ctrl.rd <= S(485 downto 478);
  t_r.a.ctrl.tt <= S(491 downto 486);
  t_r.a.ctrl.trap <= S(492);
  t_r.a.ctrl.annul <= S(493);
  t_r.a.ctrl.wreg <= S(494);
  t_r.a.ctrl.wicc <= S(495);
  t_r.a.ctrl.wy <= S(496);
  t_r.a.ctrl.ld <= S(497);
  t_r.a.ctrl.pv <= S(498);
  t_r.a.ctrl.rett <= S(499);
  t_r.a.rs1 <= S(504 downto 500);
  t_r.a.rfa1 <= S(512 downto 505);
  t_r.a.rfa2 <= S(520 downto 513);
  t_r.a.rsel1 <= S(523 downto 521);
  t_r.a.rsel2 <= S(526 downto 524);
  t_r.a.rfe1 <= S(527);
  t_r.a.rfe2 <= S(528);
  t_r.a.cwp <= S(531 downto 529);
  t_r.a.imm <= S(563 downto 532);
  t_r.a.ldcheck1 <= S(564);
  t_r.a.ldcheck2 <= S(565);
  t_r.a.ldchkra <= S(566);
  t_r.a.ldchkex <= S(567);
  t_r.a.su <= S(568);
  t_r.a.et <= S(569);
  t_r.a.wovf <= S(570);
  t_r.a.wunf <= S(571);
  t_r.a.ticc <= S(572);
  t_r.a.jmpl <= S(573);
  t_r.a.step <= S(574);
  t_r.a.mulstart <= S(575);
  t_r.a.divstart <= S(576);
  t_r.a.bp <= S(577);
  t_r.a.nobp <= S(578);
  t_r.e.ctrl.pc <= S(608 downto 579);
  t_r.e.ctrl.inst <= S(640 downto 609);
  t_r.e.ctrl.cnt <= S(642 downto 641);
  t_r.e.ctrl.rd <= S(650 downto 643);
  t_r.e.ctrl.tt <= S(656 downto 651);
  t_r.e.ctrl.trap <= S(657);
  t_r.e.ctrl.annul <= S(658);
  t_r.e.ctrl.wreg <= S(659);
  t_r.e.ctrl.wicc <= S(660);
  t_r.e.ctrl.wy <= S(661);
  t_r.e.ctrl.ld <= S(662);
  t_r.e.ctrl.pv <= S(663);
  t_r.e.ctrl.rett <= S(664);
  t_r.e.op1 <= S(696 downto 665);
  t_r.e.op2 <= S(728 downto 697);
  t_r.e.aluop <= S(731 downto 729);
  t_r.e.alusel <= S(733 downto 732);
  t_r.e.aluadd <= S(734);
  t_r.e.alucin <= S(735);
  t_r.e.ldbp1 <= S(736);
  t_r.e.ldbp2 <= S(737);
  t_r.e.invop2 <= S(738);
  t_r.e.shcnt <= S(743 downto 739);
  t_r.e.sari <= S(744);
  t_r.e.shleft <= S(745);
  t_r.e.ymsb <= S(746);
  t_r.e.rd <= S(751 downto 747);
  t_r.e.jmpl <= S(752);
  t_r.e.su <= S(753);
  t_r.e.et <= S(754);
  t_r.e.cwp <= S(757 downto 755);
  t_r.e.icc <= S(761 downto 758);
  t_r.e.mulstep <= S(762);
  t_r.e.mul <= S(763);
  t_r.e.mac <= S(764);
  t_r.e.bp <= S(765);
  t_r.m.ctrl.pc <= S(795 downto 766);
  t_r.m.ctrl.inst <= S(827 downto 796);
  t_r.m.ctrl.cnt <= S(829 downto 828);
  t_r.m.ctrl.rd <= S(837 downto 830);
  t_r.m.ctrl.tt <= S(843 downto 838);
  t_r.m.ctrl.trap <= S(844);
  t_r.m.ctrl.annul <= S(845);
  t_r.m.ctrl.wreg <= S(846);
  t_r.m.ctrl.wicc <= S(847);
  t_r.m.ctrl.wy <= S(848);
  t_r.m.ctrl.ld <= S(849);
  t_r.m.ctrl.pv <= S(850);
  t_r.m.ctrl.rett <= S(851);
  t_r.m.result <= S(883 downto 852);
  t_r.m.y <= S(915 downto 884);
  t_r.m.icc <= S(919 downto 916);
  t_r.m.nalign <= S(920);
  t_r.m.dci.signed <= S(921);
  t_r.m.dci.enaddr <= S(922);
  t_r.m.dci.read <= S(923);
  t_r.m.dci.write <= S(924);
  t_r.m.dci.lock <= S(925);
  t_r.m.dci.dsuen <= S(926);
  t_r.m.dci.size <= S(928 downto 927);
  t_r.m.dci.asi <= S(936 downto 929);
  t_r.m.werr <= S(937);
  t_r.m.wcwp <= S(938);
  t_r.m.irqen <= S(939);
  t_r.m.irqen2 <= S(940);
  t_r.m.mac <= S(941);
  t_r.m.divz <= S(942);
  t_r.m.su <= S(943);
  t_r.m.mul <= S(944);
  t_r.m.casa <= S(945);
  t_r.m.casaz <= S(946);
  t_r.x.ctrl.pc <= S(976 downto 947);
  t_r.x.ctrl.inst <= S(1008 downto 977);
  t_r.x.ctrl.cnt <= S(1010 downto 1009);
  t_r.x.ctrl.rd <= S(1018 downto 1011);
  t_r.x.ctrl.tt <= S(1024 downto 1019);
  t_r.x.ctrl.trap <= S(1025);
  t_r.x.ctrl.annul <= S(1026);
  t_r.x.ctrl.wreg <= S(1027);
  t_r.x.ctrl.wicc <= S(1028);
  t_r.x.ctrl.wy <= S(1029);
  t_r.x.ctrl.ld <= S(1030);
  t_r.x.ctrl.pv <= S(1031);
  t_r.x.ctrl.rett <= S(1032);
  t_r.x.result <= S(1064 downto 1033);
  t_r.x.y <= S(1096 downto 1065);
  t_r.x.icc <= S(1100 downto 1097);
  t_r.x.annul_all <= S(1101);
  t_r.x.data(0) <= S(1133 downto 1102);
  t_r.x.data(1) <= S(1165 downto 1134);
  t_r.x.set <= S(1166 downto 1166);
  t_r.x.mexc <= S(1167);
  t_r.x.dci.signed <= S(1168);
  t_r.x.dci.enaddr <= S(1169);
  t_r.x.dci.read <= S(1170);
  t_r.x.dci.write <= S(1171);
  t_r.x.dci.lock <= S(1172);
  t_r.x.dci.dsuen <= S(1173);
  t_r.x.dci.size <= S(1175 downto 1174);
  t_r.x.dci.asi <= S(1183 downto 1176);
  t_r.x.laddr <= S(1185 downto 1184);
  --  in_r.x.rstate <= S(1187 downto 1186);
  t_r.x.npc <= S(1190 downto 1188);
  t_r.x.intack <= S(1191);
  t_r.x.ipend <= S(1192);
  t_r.x.mac <= S(1193);
  t_r.x.debug <= S(1194);
  t_r.x.nerror <= S(1195);
  t_r.f.pc <= S(1225 downto 1196);
  t_r.w.s.cwp <= S(1228 downto 1226);
  t_r.w.s.icc <= S(1232 downto 1229);
  t_r.w.s.tt <= S(1240 downto 1233);
  t_r.w.s.tba <= S(1260 downto 1241);
  t_r.w.s.wim <= S(1268 downto 1261);
  t_r.w.s.pil <= S(1272 downto 1269);
  t_r.w.s.ec <= S(1273);
  t_r.w.s.ef <= S(1274);
  t_r.w.s.ps <= S(1275);
  t_r.w.s.s <= S(1276);
  t_r.w.s.et <= S(1277);
  t_r.w.s.y <= S(1309 downto 1278);
  t_r.w.s.asr18 <= S(1341 downto 1310);
  t_r.w.s.svt <= S(1342);
  t_r.w.s.dwt <= S(1343);
  t_r.w.s.dbp <= S(1344);
  t_r.w.result <= S(1376 downto 1345);


  
  tt : soc_leon3  port map
  ( 
    inRst => inNRst,
    inDsuBreak => '0',
    inSysClk_p  => inClk,
    inSysClk_n  => '0',
    inCTS  => '0',
    inRX  => '0',
    outRTS  => RTS,
    outTX   => TX,
    nTRST => in_trst,
    TCK => in_tck,
    TMS => in_tms,
    TDI => in_tdi,
    TDO => tdo,
    inDIP    => "00000000",
    outLED  => LED
  );

  

procCheck : process (inNRst,inClk)
begin


  if(rising_edge(inClk) and (iClkCnt>2)) then
    --if(ch_dmai.address/=dmai.address) then print("Err: dmai.address");  iErrCnt:=iErrCnt+1; end if;
  end if;
end process procCheck;


end;
 
 