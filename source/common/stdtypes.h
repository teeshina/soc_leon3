#pragma once

typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef __int64 int64;
typedef unsigned __int64 uint64;

typedef float float32;
typedef double float64;

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


