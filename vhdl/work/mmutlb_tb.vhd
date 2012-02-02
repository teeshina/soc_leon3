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

entity mmutlb_tb is
  constant CLK_HPERIOD : time := 10 ps;
  constant STRING_SIZE : integer := 2251; -- string size = index of the last element
end mmutlb_tb;
architecture behavior of mmutlb_tb is
  -- tlb cams
  component mmutlb
    generic ( 
      tech     : integer range 0 to NTECH := 0;
      entries  : integer range 2 to 32 := 8;
      tlb_type  : integer range 0 to 3 := 1;
      tlb_rep   : integer range 0 to 1 := 0;
      mmupgsz   : integer range 0 to 5  := 0
      );
    port (
      rst   : in std_logic;
      clk   : in std_logic;
      tlbi  : in mmutlb_in_type;
      tlbo  : out mmutlb_out_type;
      two  : in mmutw_out_type;
      twi  : out mmutw_in_type
      );
  end component;

  -- input/output signals:
  signal inNRst       : std_logic:= '0';
  signal inClk        : std_logic:= '0';
  signal in_tlbi  : mmutlb_in_type;
  signal ch_tlbo  : mmutlb_out_type;
  signal tlbo  : mmutlb_out_type;
  signal in_two   : mmutw_out_type;
  signal ch_twi   : mmutw_in_type;
  signal twi   : mmutw_in_type;
  signal in_tlbi_transdata_isid : std_logic;
  signal ch_tlbo_fault_fault_isid : std_logic;

  signal tst_dr1_addr  : std_logic_vector(2 downto 0);
  signal tst_dr1_dataout  : std_logic_vector(29 downto 0);
  signal tst_cam_hitaddr  : std_logic_vector(5 downto 0);

  
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
    file InputFile:TEXT is "e:/mmutlb_tb.txt";
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
      if(iClkCnt=258) then
        print("break");
      end if;
    end loop;
  end process procReadingFile;


  -- Input signals:
  in_tlbi.tlbcami <= (others => mmutlbcam_in_type_none);
  ch_tlbo.tlbcamo <= (others => mmutlbcam_out_none);
  
  inNRst                  <= S(0);
  in_tlbi.flush_op <= S(1);
  in_tlbi.wb_op <= S(2);
  in_tlbi.trans_op <= S(3);
  in_tlbi.transdata.data <= S(35 downto 4);
  in_tlbi.transdata.su <= S(36);
  in_tlbi.transdata.read <= S(37);
  in_tlbi_transdata_isid <= S(38);
  in_tlbi.transdata.wb_data <= S(70 downto 39);
  in_tlbi.s2valid <= S(71);
  in_tlbi.mmctrl1.e <= S(72);
  in_tlbi.mmctrl1.nf <= S(73);
  in_tlbi.mmctrl1.pso <= S(74);
  in_tlbi.mmctrl1.pagesize <= S(76 downto 75);
  in_tlbi.mmctrl1.ctx <= S(84 downto 77);
  in_tlbi.mmctrl1.ctxp <= S(114 downto 85);
  in_tlbi.mmctrl1.tlbdis <= S(115);
  in_tlbi.mmctrl1.bar <= S(117 downto 116);
  in_tlbi.tlbcami(0).mmctrl.e <= S(118);
  in_tlbi.tlbcami(0).mmctrl.nf <= S(119);
  in_tlbi.tlbcami(0).mmctrl.pso <= S(120);
  in_tlbi.tlbcami(0).mmctrl.pagesize <= S(122 downto 121);
  in_tlbi.tlbcami(0).mmctrl.ctx <= S(130 downto 123);
  in_tlbi.tlbcami(0).mmctrl.ctxp <= S(160 downto 131);
  in_tlbi.tlbcami(0).mmctrl.tlbdis <= S(161);
  in_tlbi.tlbcami(0).mmctrl.bar <= S(163 downto 162);
  in_tlbi.tlbcami(0).tagin.TYP <= S(166 downto 164);
  in_tlbi.tlbcami(0).tagin.I1 <= S(174 downto 167);
  in_tlbi.tlbcami(0).tagin.I2 <= S(180 downto 175);
  in_tlbi.tlbcami(0).tagin.I3 <= S(186 downto 181);
  in_tlbi.tlbcami(0).tagin.CTX <= S(194 downto 187);
  in_tlbi.tlbcami(0).tagin.M <= S(195);
  in_tlbi.tlbcami(0).tagwrite.ET <= S(197 downto 196);
  in_tlbi.tlbcami(0).tagwrite.ACC <= S(200 downto 198);
  in_tlbi.tlbcami(0).tagwrite.M <= S(201);
  in_tlbi.tlbcami(0).tagwrite.R <= S(202);
  in_tlbi.tlbcami(0).tagwrite.SU <= S(203);
  in_tlbi.tlbcami(0).tagwrite.VALID <= S(204);
  in_tlbi.tlbcami(0).tagwrite.LVL <= S(206 downto 205);
  in_tlbi.tlbcami(0).tagwrite.I1 <= S(214 downto 207);
  in_tlbi.tlbcami(0).tagwrite.I2 <= S(220 downto 215);
  in_tlbi.tlbcami(0).tagwrite.I3 <= S(226 downto 221);
  in_tlbi.tlbcami(0).tagwrite.CTX <= S(234 downto 227);
  in_tlbi.tlbcami(0).tagwrite.PPN <= S(258 downto 235);
  in_tlbi.tlbcami(0).tagwrite.C <= S(259);
  in_tlbi.tlbcami(0).trans_op <= S(260);
  in_tlbi.tlbcami(0).flush_op <= S(261);
  in_tlbi.tlbcami(0).write_op <= S(262);
  in_tlbi.tlbcami(0).wb_op <= S(263);
  in_tlbi.tlbcami(0).mmuen <= S(264);
  in_tlbi.tlbcami(0).mset <= S(265);
  in_tlbi.tlbcami(1).mmctrl.e <= S(266);
  in_tlbi.tlbcami(1).mmctrl.nf <= S(267);
  in_tlbi.tlbcami(1).mmctrl.pso <= S(268);
  in_tlbi.tlbcami(1).mmctrl.pagesize <= S(270 downto 269);
  in_tlbi.tlbcami(1).mmctrl.ctx <= S(278 downto 271);
  in_tlbi.tlbcami(1).mmctrl.ctxp <= S(308 downto 279);
  in_tlbi.tlbcami(1).mmctrl.tlbdis <= S(309);
  in_tlbi.tlbcami(1).mmctrl.bar <= S(311 downto 310);
  in_tlbi.tlbcami(1).tagin.TYP <= S(314 downto 312);
  in_tlbi.tlbcami(1).tagin.I1 <= S(322 downto 315);
  in_tlbi.tlbcami(1).tagin.I2 <= S(328 downto 323);
  in_tlbi.tlbcami(1).tagin.I3 <= S(334 downto 329);
  in_tlbi.tlbcami(1).tagin.CTX <= S(342 downto 335);
  in_tlbi.tlbcami(1).tagin.M <= S(343);
  in_tlbi.tlbcami(1).tagwrite.ET <= S(345 downto 344);
  in_tlbi.tlbcami(1).tagwrite.ACC <= S(348 downto 346);
  in_tlbi.tlbcami(1).tagwrite.M <= S(349);
  in_tlbi.tlbcami(1).tagwrite.R <= S(350);
  in_tlbi.tlbcami(1).tagwrite.SU <= S(351);
  in_tlbi.tlbcami(1).tagwrite.VALID <= S(352);
  in_tlbi.tlbcami(1).tagwrite.LVL <= S(354 downto 353);
  in_tlbi.tlbcami(1).tagwrite.I1 <= S(362 downto 355);
  in_tlbi.tlbcami(1).tagwrite.I2 <= S(368 downto 363);
  in_tlbi.tlbcami(1).tagwrite.I3 <= S(374 downto 369);
  in_tlbi.tlbcami(1).tagwrite.CTX <= S(382 downto 375);
  in_tlbi.tlbcami(1).tagwrite.PPN <= S(406 downto 383);
  in_tlbi.tlbcami(1).tagwrite.C <= S(407);
  in_tlbi.tlbcami(1).trans_op <= S(408);
  in_tlbi.tlbcami(1).flush_op <= S(409);
  in_tlbi.tlbcami(1).write_op <= S(410);
  in_tlbi.tlbcami(1).wb_op <= S(411);
  in_tlbi.tlbcami(1).mmuen <= S(412);
  in_tlbi.tlbcami(1).mset <= S(413);
  in_tlbi.tlbcami(2).mmctrl.e <= S(414);
  in_tlbi.tlbcami(2).mmctrl.nf <= S(415);
  in_tlbi.tlbcami(2).mmctrl.pso <= S(416);
  in_tlbi.tlbcami(2).mmctrl.pagesize <= S(418 downto 417);
  in_tlbi.tlbcami(2).mmctrl.ctx <= S(426 downto 419);
  in_tlbi.tlbcami(2).mmctrl.ctxp <= S(456 downto 427);
  in_tlbi.tlbcami(2).mmctrl.tlbdis <= S(457);
  in_tlbi.tlbcami(2).mmctrl.bar <= S(459 downto 458);
  in_tlbi.tlbcami(2).tagin.TYP <= S(462 downto 460);
  in_tlbi.tlbcami(2).tagin.I1 <= S(470 downto 463);
  in_tlbi.tlbcami(2).tagin.I2 <= S(476 downto 471);
  in_tlbi.tlbcami(2).tagin.I3 <= S(482 downto 477);
  in_tlbi.tlbcami(2).tagin.CTX <= S(490 downto 483);
  in_tlbi.tlbcami(2).tagin.M <= S(491);
  in_tlbi.tlbcami(2).tagwrite.ET <= S(493 downto 492);
  in_tlbi.tlbcami(2).tagwrite.ACC <= S(496 downto 494);
  in_tlbi.tlbcami(2).tagwrite.M <= S(497);
  in_tlbi.tlbcami(2).tagwrite.R <= S(498);
  in_tlbi.tlbcami(2).tagwrite.SU <= S(499);
  in_tlbi.tlbcami(2).tagwrite.VALID <= S(500);
  in_tlbi.tlbcami(2).tagwrite.LVL <= S(502 downto 501);
  in_tlbi.tlbcami(2).tagwrite.I1 <= S(510 downto 503);
  in_tlbi.tlbcami(2).tagwrite.I2 <= S(516 downto 511);
  in_tlbi.tlbcami(2).tagwrite.I3 <= S(522 downto 517);
  in_tlbi.tlbcami(2).tagwrite.CTX <= S(530 downto 523);
  in_tlbi.tlbcami(2).tagwrite.PPN <= S(554 downto 531);
  in_tlbi.tlbcami(2).tagwrite.C <= S(555);
  in_tlbi.tlbcami(2).trans_op <= S(556);
  in_tlbi.tlbcami(2).flush_op <= S(557);
  in_tlbi.tlbcami(2).write_op <= S(558);
  in_tlbi.tlbcami(2).wb_op <= S(559);
  in_tlbi.tlbcami(2).mmuen <= S(560);
  in_tlbi.tlbcami(2).mset <= S(561);
  in_tlbi.tlbcami(3).mmctrl.e <= S(562);
  in_tlbi.tlbcami(3).mmctrl.nf <= S(563);
  in_tlbi.tlbcami(3).mmctrl.pso <= S(564);
  in_tlbi.tlbcami(3).mmctrl.pagesize <= S(566 downto 565);
  in_tlbi.tlbcami(3).mmctrl.ctx <= S(574 downto 567);
  in_tlbi.tlbcami(3).mmctrl.ctxp <= S(604 downto 575);
  in_tlbi.tlbcami(3).mmctrl.tlbdis <= S(605);
  in_tlbi.tlbcami(3).mmctrl.bar <= S(607 downto 606);
  in_tlbi.tlbcami(3).tagin.TYP <= S(610 downto 608);
  in_tlbi.tlbcami(3).tagin.I1 <= S(618 downto 611);
  in_tlbi.tlbcami(3).tagin.I2 <= S(624 downto 619);
  in_tlbi.tlbcami(3).tagin.I3 <= S(630 downto 625);
  in_tlbi.tlbcami(3).tagin.CTX <= S(638 downto 631);
  in_tlbi.tlbcami(3).tagin.M <= S(639);
  in_tlbi.tlbcami(3).tagwrite.ET <= S(641 downto 640);
  in_tlbi.tlbcami(3).tagwrite.ACC <= S(644 downto 642);
  in_tlbi.tlbcami(3).tagwrite.M <= S(645);
  in_tlbi.tlbcami(3).tagwrite.R <= S(646);
  in_tlbi.tlbcami(3).tagwrite.SU <= S(647);
  in_tlbi.tlbcami(3).tagwrite.VALID <= S(648);
  in_tlbi.tlbcami(3).tagwrite.LVL <= S(650 downto 649);
  in_tlbi.tlbcami(3).tagwrite.I1 <= S(658 downto 651);
  in_tlbi.tlbcami(3).tagwrite.I2 <= S(664 downto 659);
  in_tlbi.tlbcami(3).tagwrite.I3 <= S(670 downto 665);
  in_tlbi.tlbcami(3).tagwrite.CTX <= S(678 downto 671);
  in_tlbi.tlbcami(3).tagwrite.PPN <= S(702 downto 679);
  in_tlbi.tlbcami(3).tagwrite.C <= S(703);
  in_tlbi.tlbcami(3).trans_op <= S(704);
  in_tlbi.tlbcami(3).flush_op <= S(705);
  in_tlbi.tlbcami(3).write_op <= S(706);
  in_tlbi.tlbcami(3).wb_op <= S(707);
  in_tlbi.tlbcami(3).mmuen <= S(708);
  in_tlbi.tlbcami(3).mset <= S(709);
  in_tlbi.tlbcami(4).mmctrl.e <= S(710);
  in_tlbi.tlbcami(4).mmctrl.nf <= S(711);
  in_tlbi.tlbcami(4).mmctrl.pso <= S(712);
  in_tlbi.tlbcami(4).mmctrl.pagesize <= S(714 downto 713);
  in_tlbi.tlbcami(4).mmctrl.ctx <= S(722 downto 715);
  in_tlbi.tlbcami(4).mmctrl.ctxp <= S(752 downto 723);
  in_tlbi.tlbcami(4).mmctrl.tlbdis <= S(753);
  in_tlbi.tlbcami(4).mmctrl.bar <= S(755 downto 754);
  in_tlbi.tlbcami(4).tagin.TYP <= S(758 downto 756);
  in_tlbi.tlbcami(4).tagin.I1 <= S(766 downto 759);
  in_tlbi.tlbcami(4).tagin.I2 <= S(772 downto 767);
  in_tlbi.tlbcami(4).tagin.I3 <= S(778 downto 773);
  in_tlbi.tlbcami(4).tagin.CTX <= S(786 downto 779);
  in_tlbi.tlbcami(4).tagin.M <= S(787);
  in_tlbi.tlbcami(4).tagwrite.ET <= S(789 downto 788);
  in_tlbi.tlbcami(4).tagwrite.ACC <= S(792 downto 790);
  in_tlbi.tlbcami(4).tagwrite.M <= S(793);
  in_tlbi.tlbcami(4).tagwrite.R <= S(794);
  in_tlbi.tlbcami(4).tagwrite.SU <= S(795);
  in_tlbi.tlbcami(4).tagwrite.VALID <= S(796);
  in_tlbi.tlbcami(4).tagwrite.LVL <= S(798 downto 797);
  in_tlbi.tlbcami(4).tagwrite.I1 <= S(806 downto 799);
  in_tlbi.tlbcami(4).tagwrite.I2 <= S(812 downto 807);
  in_tlbi.tlbcami(4).tagwrite.I3 <= S(818 downto 813);
  in_tlbi.tlbcami(4).tagwrite.CTX <= S(826 downto 819);
  in_tlbi.tlbcami(4).tagwrite.PPN <= S(850 downto 827);
  in_tlbi.tlbcami(4).tagwrite.C <= S(851);
  in_tlbi.tlbcami(4).trans_op <= S(852);
  in_tlbi.tlbcami(4).flush_op <= S(853);
  in_tlbi.tlbcami(4).write_op <= S(854);
  in_tlbi.tlbcami(4).wb_op <= S(855);
  in_tlbi.tlbcami(4).mmuen <= S(856);
  in_tlbi.tlbcami(4).mset <= S(857);
  in_tlbi.tlbcami(5).mmctrl.e <= S(858);
  in_tlbi.tlbcami(5).mmctrl.nf <= S(859);
  in_tlbi.tlbcami(5).mmctrl.pso <= S(860);
  in_tlbi.tlbcami(5).mmctrl.pagesize <= S(862 downto 861);
  in_tlbi.tlbcami(5).mmctrl.ctx <= S(870 downto 863);
  in_tlbi.tlbcami(5).mmctrl.ctxp <= S(900 downto 871);
  in_tlbi.tlbcami(5).mmctrl.tlbdis <= S(901);
  in_tlbi.tlbcami(5).mmctrl.bar <= S(903 downto 902);
  in_tlbi.tlbcami(5).tagin.TYP <= S(906 downto 904);
  in_tlbi.tlbcami(5).tagin.I1 <= S(914 downto 907);
  in_tlbi.tlbcami(5).tagin.I2 <= S(920 downto 915);
  in_tlbi.tlbcami(5).tagin.I3 <= S(926 downto 921);
  in_tlbi.tlbcami(5).tagin.CTX <= S(934 downto 927);
  in_tlbi.tlbcami(5).tagin.M <= S(935);
  in_tlbi.tlbcami(5).tagwrite.ET <= S(937 downto 936);
  in_tlbi.tlbcami(5).tagwrite.ACC <= S(940 downto 938);
  in_tlbi.tlbcami(5).tagwrite.M <= S(941);
  in_tlbi.tlbcami(5).tagwrite.R <= S(942);
  in_tlbi.tlbcami(5).tagwrite.SU <= S(943);
  in_tlbi.tlbcami(5).tagwrite.VALID <= S(944);
  in_tlbi.tlbcami(5).tagwrite.LVL <= S(946 downto 945);
  in_tlbi.tlbcami(5).tagwrite.I1 <= S(954 downto 947);
  in_tlbi.tlbcami(5).tagwrite.I2 <= S(960 downto 955);
  in_tlbi.tlbcami(5).tagwrite.I3 <= S(966 downto 961);
  in_tlbi.tlbcami(5).tagwrite.CTX <= S(974 downto 967);
  in_tlbi.tlbcami(5).tagwrite.PPN <= S(998 downto 975);
  in_tlbi.tlbcami(5).tagwrite.C <= S(999);
  in_tlbi.tlbcami(5).trans_op <= S(1000);
  in_tlbi.tlbcami(5).flush_op <= S(1001);
  in_tlbi.tlbcami(5).write_op <= S(1002);
  in_tlbi.tlbcami(5).wb_op <= S(1003);
  in_tlbi.tlbcami(5).mmuen <= S(1004);
  in_tlbi.tlbcami(5).mset <= S(1005);
  in_tlbi.tlbcami(6).mmctrl.e <= S(1006);
  in_tlbi.tlbcami(6).mmctrl.nf <= S(1007);
  in_tlbi.tlbcami(6).mmctrl.pso <= S(1008);
  in_tlbi.tlbcami(6).mmctrl.pagesize <= S(1010 downto 1009);
  in_tlbi.tlbcami(6).mmctrl.ctx <= S(1018 downto 1011);
  in_tlbi.tlbcami(6).mmctrl.ctxp <= S(1048 downto 1019);
  in_tlbi.tlbcami(6).mmctrl.tlbdis <= S(1049);
  in_tlbi.tlbcami(6).mmctrl.bar <= S(1051 downto 1050);
  in_tlbi.tlbcami(6).tagin.TYP <= S(1054 downto 1052);
  in_tlbi.tlbcami(6).tagin.I1 <= S(1062 downto 1055);
  in_tlbi.tlbcami(6).tagin.I2 <= S(1068 downto 1063);
  in_tlbi.tlbcami(6).tagin.I3 <= S(1074 downto 1069);
  in_tlbi.tlbcami(6).tagin.CTX <= S(1082 downto 1075);
  in_tlbi.tlbcami(6).tagin.M <= S(1083);
  in_tlbi.tlbcami(6).tagwrite.ET <= S(1085 downto 1084);
  in_tlbi.tlbcami(6).tagwrite.ACC <= S(1088 downto 1086);
  in_tlbi.tlbcami(6).tagwrite.M <= S(1089);
  in_tlbi.tlbcami(6).tagwrite.R <= S(1090);
  in_tlbi.tlbcami(6).tagwrite.SU <= S(1091);
  in_tlbi.tlbcami(6).tagwrite.VALID <= S(1092);
  in_tlbi.tlbcami(6).tagwrite.LVL <= S(1094 downto 1093);
  in_tlbi.tlbcami(6).tagwrite.I1 <= S(1102 downto 1095);
  in_tlbi.tlbcami(6).tagwrite.I2 <= S(1108 downto 1103);
  in_tlbi.tlbcami(6).tagwrite.I3 <= S(1114 downto 1109);
  in_tlbi.tlbcami(6).tagwrite.CTX <= S(1122 downto 1115);
  in_tlbi.tlbcami(6).tagwrite.PPN <= S(1146 downto 1123);
  in_tlbi.tlbcami(6).tagwrite.C <= S(1147);
  in_tlbi.tlbcami(6).trans_op <= S(1148);
  in_tlbi.tlbcami(6).flush_op <= S(1149);
  in_tlbi.tlbcami(6).write_op <= S(1150);
  in_tlbi.tlbcami(6).wb_op <= S(1151);
  in_tlbi.tlbcami(6).mmuen <= S(1152);
  in_tlbi.tlbcami(6).mset <= S(1153);
  in_tlbi.tlbcami(7).mmctrl.e <= S(1154);
  in_tlbi.tlbcami(7).mmctrl.nf <= S(1155);
  in_tlbi.tlbcami(7).mmctrl.pso <= S(1156);
  in_tlbi.tlbcami(7).mmctrl.pagesize <= S(1158 downto 1157);
  in_tlbi.tlbcami(7).mmctrl.ctx <= S(1166 downto 1159);
  in_tlbi.tlbcami(7).mmctrl.ctxp <= S(1196 downto 1167);
  in_tlbi.tlbcami(7).mmctrl.tlbdis <= S(1197);
  in_tlbi.tlbcami(7).mmctrl.bar <= S(1199 downto 1198);
  in_tlbi.tlbcami(7).tagin.TYP <= S(1202 downto 1200);
  in_tlbi.tlbcami(7).tagin.I1 <= S(1210 downto 1203);
  in_tlbi.tlbcami(7).tagin.I2 <= S(1216 downto 1211);
  in_tlbi.tlbcami(7).tagin.I3 <= S(1222 downto 1217);
  in_tlbi.tlbcami(7).tagin.CTX <= S(1230 downto 1223);
  in_tlbi.tlbcami(7).tagin.M <= S(1231);
  in_tlbi.tlbcami(7).tagwrite.ET <= S(1233 downto 1232);
  in_tlbi.tlbcami(7).tagwrite.ACC <= S(1236 downto 1234);
  in_tlbi.tlbcami(7).tagwrite.M <= S(1237);
  in_tlbi.tlbcami(7).tagwrite.R <= S(1238);
  in_tlbi.tlbcami(7).tagwrite.SU <= S(1239);
  in_tlbi.tlbcami(7).tagwrite.VALID <= S(1240);
  in_tlbi.tlbcami(7).tagwrite.LVL <= S(1242 downto 1241);
  in_tlbi.tlbcami(7).tagwrite.I1 <= S(1250 downto 1243);
  in_tlbi.tlbcami(7).tagwrite.I2 <= S(1256 downto 1251);
  in_tlbi.tlbcami(7).tagwrite.I3 <= S(1262 downto 1257);
  in_tlbi.tlbcami(7).tagwrite.CTX <= S(1270 downto 1263);
  in_tlbi.tlbcami(7).tagwrite.PPN <= S(1294 downto 1271);
  in_tlbi.tlbcami(7).tagwrite.C <= S(1295);
  in_tlbi.tlbcami(7).trans_op <= S(1296);
  in_tlbi.tlbcami(7).flush_op <= S(1297);
  in_tlbi.tlbcami(7).write_op <= S(1298);
  in_tlbi.tlbcami(7).wb_op <= S(1299);
  in_tlbi.tlbcami(7).mmuen <= S(1300);
  in_tlbi.tlbcami(7).mset <= S(1301);
  in_two.finish <= S(1302);
  in_two.data <= S(1334 downto 1303);
  in_two.addr <= S(1366 downto 1335);
  in_two.lvl <= S(1368 downto 1367);
  in_two.fault_mexc <= S(1369);
  in_two.fault_trans <= S(1370);
  in_two.fault_inv <= S(1371);
  in_two.fault_lvl <= S(1373 downto 1372);
  ch_twi.walk_op_ur <= S(1374);
  ch_twi.areq_ur <= S(1375);
  ch_twi.tlbmiss <= S(1376);
  ch_twi.data <= S(1408 downto 1377);
  ch_twi.adata <= S(1440 downto 1409);
  ch_twi.aaddr <= S(1472 downto 1441);
  ch_tlbo.transdata.finish <= S(1473);
  ch_tlbo.transdata.data <= S(1505 downto 1474);
  ch_tlbo.transdata.cache <= S(1506);
  ch_tlbo.transdata.accexc <= S(1507);
  ch_tlbo.fault.fault_pro <= S(1508);
  ch_tlbo.fault.fault_pri <= S(1509);
  ch_tlbo.fault.fault_access <= S(1510);
  ch_tlbo.fault.fault_mexc <= S(1511);
  ch_tlbo.fault.fault_trans <= S(1512);
  ch_tlbo.fault.fault_inv <= S(1513);
  ch_tlbo.fault.fault_lvl <= S(1515 downto 1514);
  ch_tlbo.fault.fault_su <= S(1516);
  ch_tlbo.fault.fault_read <= S(1517);
  ch_tlbo_fault_fault_isid <= S(1518);
  ch_tlbo.fault.fault_addr <= S(1550 downto 1519);
  ch_tlbo.nexttrans <= S(1551);
  ch_tlbo.s1finished <= S(1552);
  ch_tlbo.tlbcamo(0).pteout <= S(1584 downto 1553);
  ch_tlbo.tlbcamo(0).LVL <= S(1586 downto 1585);
  ch_tlbo.tlbcamo(0).hit <= S(1587);
  ch_tlbo.tlbcamo(0).ctx <= S(1595 downto 1588);
  ch_tlbo.tlbcamo(0).valid <= S(1596);
  ch_tlbo.tlbcamo(0).vaddr <= S(1628 downto 1597);
  ch_tlbo.tlbcamo(0).NEEDSYNC <= S(1629);
  ch_tlbo.tlbcamo(0).WBNEEDSYNC <= S(1630);
  ch_tlbo.tlbcamo(1).pteout <= S(1662 downto 1631);
  ch_tlbo.tlbcamo(1).LVL <= S(1664 downto 1663);
  ch_tlbo.tlbcamo(1).hit <= S(1665);
  ch_tlbo.tlbcamo(1).ctx <= S(1673 downto 1666);
  ch_tlbo.tlbcamo(1).valid <= S(1674);
  ch_tlbo.tlbcamo(1).vaddr <= S(1706 downto 1675);
  ch_tlbo.tlbcamo(1).NEEDSYNC <= S(1707);
  ch_tlbo.tlbcamo(1).WBNEEDSYNC <= S(1708);
  ch_tlbo.tlbcamo(2).pteout <= S(1740 downto 1709);
  ch_tlbo.tlbcamo(2).LVL <= S(1742 downto 1741);
  ch_tlbo.tlbcamo(2).hit <= S(1743);
  ch_tlbo.tlbcamo(2).ctx <= S(1751 downto 1744);
  ch_tlbo.tlbcamo(2).valid <= S(1752);
  ch_tlbo.tlbcamo(2).vaddr <= S(1784 downto 1753);
  ch_tlbo.tlbcamo(2).NEEDSYNC <= S(1785);
  ch_tlbo.tlbcamo(2).WBNEEDSYNC <= S(1786);
  ch_tlbo.tlbcamo(3).pteout <= S(1818 downto 1787);
  ch_tlbo.tlbcamo(3).LVL <= S(1820 downto 1819);
  ch_tlbo.tlbcamo(3).hit <= S(1821);
  ch_tlbo.tlbcamo(3).ctx <= S(1829 downto 1822);
  ch_tlbo.tlbcamo(3).valid <= S(1830);
  ch_tlbo.tlbcamo(3).vaddr <= S(1862 downto 1831);
  ch_tlbo.tlbcamo(3).NEEDSYNC <= S(1863);
  ch_tlbo.tlbcamo(3).WBNEEDSYNC <= S(1864);
  ch_tlbo.tlbcamo(4).pteout <= S(1896 downto 1865);
  ch_tlbo.tlbcamo(4).LVL <= S(1898 downto 1897);
  ch_tlbo.tlbcamo(4).hit <= S(1899);
  ch_tlbo.tlbcamo(4).ctx <= S(1907 downto 1900);
  ch_tlbo.tlbcamo(4).valid <= S(1908);
  ch_tlbo.tlbcamo(4).vaddr <= S(1940 downto 1909);
  ch_tlbo.tlbcamo(4).NEEDSYNC <= S(1941);
  ch_tlbo.tlbcamo(4).WBNEEDSYNC <= S(1942);
  ch_tlbo.tlbcamo(5).pteout <= S(1974 downto 1943);
  ch_tlbo.tlbcamo(5).LVL <= S(1976 downto 1975);
  ch_tlbo.tlbcamo(5).hit <= S(1977);
  ch_tlbo.tlbcamo(5).ctx <= S(1985 downto 1978);
  ch_tlbo.tlbcamo(5).valid <= S(1986);
  ch_tlbo.tlbcamo(5).vaddr <= S(2018 downto 1987);
  ch_tlbo.tlbcamo(5).NEEDSYNC <= S(2019);
  ch_tlbo.tlbcamo(5).WBNEEDSYNC <= S(2020);
  ch_tlbo.tlbcamo(6).pteout <= S(2052 downto 2021);
  ch_tlbo.tlbcamo(6).LVL <= S(2054 downto 2053);
  ch_tlbo.tlbcamo(6).hit <= S(2055);
  ch_tlbo.tlbcamo(6).ctx <= S(2063 downto 2056);
  ch_tlbo.tlbcamo(6).valid <= S(2064);
  ch_tlbo.tlbcamo(6).vaddr <= S(2096 downto 2065);
  ch_tlbo.tlbcamo(6).NEEDSYNC <= S(2097);
  ch_tlbo.tlbcamo(6).WBNEEDSYNC <= S(2098);
  ch_tlbo.tlbcamo(7).pteout <= S(2130 downto 2099);
  ch_tlbo.tlbcamo(7).LVL <= S(2132 downto 2131);
  ch_tlbo.tlbcamo(7).hit <= S(2133);
  ch_tlbo.tlbcamo(7).ctx <= S(2141 downto 2134);
  ch_tlbo.tlbcamo(7).valid <= S(2142);
  ch_tlbo.tlbcamo(7).vaddr <= S(2174 downto 2143);
  ch_tlbo.tlbcamo(7).NEEDSYNC <= S(2175);
  ch_tlbo.tlbcamo(7).WBNEEDSYNC <= S(2176);
  ch_tlbo.wbtransdata.finish <= S(2177);
  ch_tlbo.wbtransdata.data <= S(2209 downto 2178);
  ch_tlbo.wbtransdata.cache <= S(2210);
  ch_tlbo.wbtransdata.accexc <= S(2211);
  tst_dr1_addr <= S(2214 downto 2212);
  tst_dr1_dataout <= S(2244 downto 2215);
  tst_cam_hitaddr <= S(2250 downto 2245);


  enum : process (inClk,in_tlbi_transdata_isid, ch_tlbo_fault_fault_isid)
  begin
    if(in_tlbi_transdata_isid='0')then in_tlbi.transdata.isid <= id_icache;
    else                               in_tlbi.transdata.isid <= id_dcache; end if;

    if(ch_tlbo_fault_fault_isid='0')then ch_tlbo.fault.fault_isid <= id_icache;
    else                                 ch_tlbo.fault.fault_isid <= id_dcache; end if;
  end process enum;


  tt : mmutlb generic map ( 
      inferred,--tech     : integer range 0 to NTECH := 0;
      8,--entries  : integer range 2 to 32 := 8;
      1,--tlb_type  : integer range 0 to 3 := 1;
      CFG_TLB_REP,--tlb_rep   : integer range 0 to 1 := 0;
      0--mmupgsz   : integer range 0 to 5  := 0
  )port map
  (
      inNRst,
      inClk,
      in_tlbi,
      tlbo,
      in_two,
      twi
  );  

procCheck : process (inClk, ch_tlbo, ch_twi)
  variable iErrCnt : integer := 0;
begin
  if(rising_edge(inClk) and (iClkCnt>2)) then
    if(ch_tlbo.transdata.finish /= tlbo.transdata.finish) then print("Err: tlbo.transdata.finish");  iErrCnt:=iErrCnt+1; end if;
    if(tlbo.transdata.cache/='U') then
      if(ch_tlbo.transdata.cache /= tlbo.transdata.cache) then print("Err: tlbo.transdata.cache");  iErrCnt:=iErrCnt+1; end if;
    end if;
    if (tlbo.transdata.data/="XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")then
      if(ch_tlbo.transdata.data /= tlbo.transdata.data) then print("Err: tlbo.transdata.data");  iErrCnt:=iErrCnt+1; end if;
    end if;
    if(ch_tlbo.transdata.accexc /= tlbo.transdata.accexc) then print("Err: tlbo.transdata.accexc");  iErrCnt:=iErrCnt+1; end if;
    if(ch_tlbo.fault /= tlbo.fault) then print("Err: tlbo.fault");  iErrCnt:=iErrCnt+1; end if;
    if(ch_tlbo.nexttrans /= tlbo.nexttrans) then print("Err: tlbo.nexttrans");  iErrCnt:=iErrCnt+1; end if;
    if(ch_tlbo.s1finished /= tlbo.s1finished) then print("Err: tlbo.s1finished");  iErrCnt:=iErrCnt+1; end if;
    if(ch_tlbo.tlbcamo /= tlbo.tlbcamo) then print("Err: tlbo.tlbcamo");  iErrCnt:=iErrCnt+1; end if;
    if(ch_tlbo.wbtransdata /= tlbo.wbtransdata) then print("Err: tlbo.wbtransdata");  iErrCnt:=iErrCnt+1; end if;

    if(ch_twi.walk_op_ur /= twi.walk_op_ur) then print("Err: twi.walk_op_ur");  iErrCnt:=iErrCnt+1; end if;
    if(ch_twi.areq_ur /= twi.areq_ur) then print("Err: twi.areq_ur");  iErrCnt:=iErrCnt+1; end if;
    if(ch_twi.tlbmiss /= twi.tlbmiss) then print("Err: twi.tlbmiss");  iErrCnt:=iErrCnt+1; end if;
    if(ch_twi.data /= twi.data) then print("Err: twi.data");  iErrCnt:=iErrCnt+1; end if;
    if(twi.adata/="UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU") then
      if(ch_twi.adata /= twi.adata) then print("Err: twi.adata");  iErrCnt:=iErrCnt+1; end if;
    end if;
    if(twi.aaddr/="UUUUUUUUUUUUUUUUUUUUUUUUUUUUUU00") then
      if(ch_twi.aaddr /= twi.aaddr) then print("Err: twi.aaddr");  iErrCnt:=iErrCnt+1; end if;
    end if;
  end if;
end process procCheck;

  
end;
