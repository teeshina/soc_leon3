#pragma once

//
#define LSBMSK32(x) (0xFFFFFFFF>>(31-(x)))
#define MSK32(m,l)  (((0xFFFFFFFF>>(31-(m)))>>(l))<<(l))
#define BITS32(val,m,l)  (((val)&(0xFFFFFFFF>>(31-(m))))>>(l))
#define BIT32(val,m)  (((val)>>(m))&0x1)

#define MSK64(m,l)  (((0xFFFFFFFFFFFFFFFF>>(63-(m)))>>(l))<<(l))
#define BITS64(val,m,l)  (((val)&(0xFFFFFFFFFFFFFFFF>>(63-(m))))>>(l))
#define BIT64(val,m)  (((val)>>(m))&0x1)

#define LSBMSK64(x) (0xFFFFFFFFFFFFFFFF>>(63-(x)))
#define ILSBMSK32(x) (-1<<(x))


const uint32 is_fpga[2] = {1,1};
const uint32 need_extra_sync_reset[2] = {0,0};

uint32 orv(uint32 d);
uint32 genmux(uint32 s, uint32 v);
uint32 decode(uint32 v);

