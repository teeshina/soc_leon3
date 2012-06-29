//****************************************************************************
// Property:    GNSS Sensor Limited
// Author:      Khabarov Sergey
// License:     GPL
// Contact:     sergey.khabarov@gnss-sensor.com
// Repository:  git@github.com:teeshina/soc_leon3.git
//****************************************************************************

#pragma once

#include "stdtypes.h"
#include "leon3_ml605\config.h"

struct Ctrl2Module
{
  uint32 rd_ena         : 1;
  uint32 rd_module_sel;  // : CFG_GNSS_ADDR_WIDTH-1
  uint32 rd_field_sel   : 3;
  uint32 wr_ena         : 1;
  uint32 wr_module_sel; // : CFG_GNSS_ADDR_WIDTH-1
  uint32 wr_field_sel   : 4;
  uint32 wr_data;
};

struct Module2Ctrl
{
  uint64 rdata;
  uint32 rdata_rdy : 1;
};

struct m2c_tot
{
  Module2Ctrl arr[CFG_GNSS_MODULES_TOTAL];
};

struct m2c_chn
{
  Module2Ctrl arr[CFG_GNSS_CHANNELS_TOTAL];
};


