#pragma once

#include "stdtypes.h"

const uint32 is_fpga[2] = {1,1};
const uint32 need_extra_sync_reset[2] = {0,0};

uint32 orv(uint32 d);
uint32 genmux(uint32 s, uint32 v);
uint32 decode(uint32 v);

