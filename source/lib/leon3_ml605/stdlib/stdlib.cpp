#include "lheaders.h"

uint32 orv(uint32 d) { return (d?1:0);}

uint32 genmux(uint32 s, uint32 v){ return BIT32(v,s); }

uint32 decode(uint32 v) { return(0x1<<v); }


