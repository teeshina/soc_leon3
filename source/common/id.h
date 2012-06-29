#pragma once

#include "stdtypes.h"

//****************************************************************************
// Virtex6 compatible: see page 179 of ug350.pdf document
const uint32 JTAG_INSTRUCTION_WIDTH = 10;
const uint32 JTAG_INSTRUCTION_USER1 = 0x3C2;  // instruction to accept address (35 bits)
const uint32 JTAG_INSTRUCTION_USER2 = 0x3C3;  // instruction to accept data (33 bits)


// Vendor codes: amba_vendor_type[15:0]
static const uint32 VENDOR_GAISLER    = 0x01;
static const uint32 VENDOR_PENDER     = 0x02;
static const uint32 VENDOR_ESA        = 0x04;
static const uint32 VENDOR_ASTRIUM    = 0x06;
static const uint32 VENDOR_OPENCHIP   = 0x07;
static const uint32 VENDOR_OPENCORES  = 0x08;
static const uint32 VENDOR_CONTRIB    = 0x09;
static const uint32 VENDOR_EONIC      = 0x0B;
static const uint32 VENDOR_RADIONOR   = 0x0F;
static const uint32 VENDOR_GLEICHMANN = 0x10;
static const uint32 VENDOR_MENTA      = 0x11;
static const uint32 VENDOR_SUN        = 0x13;
static const uint32 VENDOR_MOVIDIA    = 0x14;
static const uint32 VENDOR_ORBITA     = 0x17;
static const uint32 VENDOR_SYNOPSYS   = 0x21;
static const uint32 VENDOR_NASA       = 0x22;
static const uint32 VENDOR_S3         = 0x31;
static const uint32 VENDOR_ACTEL      = 0xAC;
static const uint32 VENDOR_APPLECORE  = 0xAE;
static const uint32 VENDOR_CAL        = 0xCA;
static const uint32 VENDOR_CETON      = 0xCB;
static const uint32 VENDOR_EMBEDDIT   = 0xEA;
static const uint32 VENDOR_GNSSSENSOR = 0xF1;


// Gaisler cores 
#define GAISLER_LEON2DSU 0x002
#define GAISLER_LEON3    0x003
#define GAISLER_LEON3DSU 0x004
#define GAISLER_ETHAHB   0x005
#define GAISLER_APBMST   0x006
#define GAISLER_AHBUART  0x007
#define GAISLER_SRCTRL   0x008
#define GAISLER_SDCTRL   0x009
#define GAISLER_SSRCTRL  0x00A
#define GAISLER_APBUART  0x00C
#define GAISLER_IRQMP    0x00D
#define GAISLER_AHBRAM   0x00E
#define GAISLER_GPTIMER  0x011
#define GAISLER_PCITRG   0x012
#define GAISLER_PCISBRG  0x013
#define GAISLER_PCIFBRG  0x014
#define GAISLER_PCITRACE 0x015
#define GAISLER_PCIDMA   0x016
#define GAISLER_AHBTRACE 0x017
#define GAISLER_ETHDSU   0x018
#define GAISLER_CANAHB   0x019
#define GAISLER_GRGPIO   0x01A
#define GAISLER_AHBROM   0x01B
#define GAISLER_AHBJTAG  0x01C
#define GAISLER_ETHMAC   0x01D
#define GAISLER_SPW      0x01F
#define GAISLER_SPACEWIRE 0x01F
#define GAISLER_AHB2AHB  0x020
#define GAISLER_USBCTRL  0x021
#define GAISLER_USBDCL   0x022
#define GAISLER_DDRMP    0x023
#define GAISLER_ATACTRL  0x024
#define GAISLER_DDRSP    0x025
#define GAISLER_EHCI     0x026
#define GAISLER_UHCI     0x027
#define GAISLER_I2CMST   0x028

#define GAISLER_NUHOSP3  0x02b

#define GAISLER_SPICTRL  0x02D

#define GAISLER_GRTM     0x030
#define GAISLER_GRTC     0x031
#define GAISLER_GRPW     0x032
#define GAISLER_GRCTM    0x033
#define GAISLER_GRHCAN   0x034
#define GAISLER_GRFIFO   0x035
#define GAISLER_GRADCDAC 0x036
#define GAISLER_GRPULSE  0x037
#define GAISLER_GRTIMER  0x038
#define GAISLER_AHB2PP   0x039
#define GAISLER_GRVERSION 0x03A

#define GAISLER_FTAHBRAM 0x050
#define GAISLER_FTSRCTRL 0x051
#define GAISLER_AHBSTAT  0x052
#define GAISLER_LEON3FT  0x053
#define GAISLER_FTMCTRL  0x054
#define GAISLER_FTSDCTRL 0x055
#define GAISLER_FTSRCTRL8 0x056

#define GAISLER_KBD      0x060
#define GAISLER_VGA      0x061
#define GAISLER_LOGAN    0x062
#define GAISLER_SVGA     0x063

#define GAISLER_B1553BC  0x070
#define GAISLER_B1553RT  0x071
#define GAISLER_B1553BRM 0x072

#define GAISLER_PCIF   0x75

#define GAISLER_SATCAN   0x080
#define GAISLER_CANMUX   0x081

#define GAISLER_PIPEWRAPPER 0xffa
#define GAISLER_LEON2    0xffb
#define GAISLER_L2IRQ    0xffc /* internal device: leon2 interrupt controller */
#define GAISLER_L2TIME   0xffd /* internal device: leon2 timer */
#define GAISLER_L2C      0xffe /* internal device: leon2compat */
#define GAISLER_PLUGPLAY 0xfff /* internal device: plug & play configarea */

/* ESA cores */
#define  ESA_LEON2       0x002
#define  ESA_LEON2APB    0x003
#define  ESA_L2IRQ       0x005
#define  ESA_L2TIMER     0x006
#define  ESA_L2UART      0x007
#define  ESA_L2CFG       0x008
#define  ESA_L2IO        0x009
#define  ESA_MCTRL       0x00F
#define  ESA_PCIARB      0x010
#define  ESA_HURRICANE   0x011
#define  ESA_SPW_RMAP    0x012
#define  ESA_AHBUART     0x013
#define  ESA_SPWA        0x014
#define  ESA_BOSCHCAN    0x015
#define  ESA_L2IRQ2      0x016
#define  ESA_L2STAT      0x017
#define  ESA_L2WPROT     0x018
#define  ESA_L2WPROT2    0x019

/* GLEICHMANN cores */
#define GLEICHMANN_CUSTOM 0x001
#define GLEICHMANN_GEOLCD01 0x002
#define GLEICHMANN_DAC 0x003


#define LEON3_NEXTREME1      0x0101
#define LEON4_NEXTREME1      0x0102
#define LEON3_ACT_FUSION     0x0105
#define LEON3_RTAX_CID2      0x0202
#define LEON3_RTAX_CID5      0x0205
#define LEON3_RTAX_CID6      0x0206
#define LEON3_RTAX_CID7      0x0207
#define LEON3_RTAX_CID8      0x0208
#define LEON3_IHP25RH1       0x0251
#define NGMP_PROTOTYPE       0x0281
#define ALTERA_DE2           0x0302
#define XILINX_ML401         0x0401
#define LEON3FT_GRXC4V       0x0453
#define XILINX_ML501         0x0501
#define XILINX_ML505         0x0505
#define XILINX_ML506         0x0506
#define XILINX_ML507         0x0507
#define XILINX_ML509         0x0509
#define XILINX_ML510         0x0510
#define XILINX_SP601         0x0601
#define XILINX_ML605         0x0605
#define ORBITA_1             0x0631
#define AEROFLEX_UT699       0x0699
#define AEROFLEX_UT700       0x0700
#define GAISLER_GR702        0x0702
#define GAISLER_DARE1        0x0704
#define GAISLER_GR712RC      0x0712

#define CFGAREA_TYPE_APB_IO  0x1
#define CFGAREA_TYPE_AHB_MEM 0x2
#define CFGAREA_TYPE_AHB_IO  0x3

#define GNSSSENSOR_RF_CONTROL    0x077
#define GNSSSENSOR_ENGINE        0x078
#define GNSSSENSOR_GYROSCOPE     0x079
#define GNSSSENSOR_ACCELEROMETER 0x07A
#define GNSSSENSOR_ROM_IMAGE     0x07B
