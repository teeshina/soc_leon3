#include "stdtypes.h"
#include "dff.h"
#include "converter.h"


//*******************************************************************
float Bus32ToFloat(int *bus)
{
  int iRet = 0;
  for (int i=0; i<32; i++)
    iRet |= (bus[i]<<i);
  return *((float*)&iRet);
}

//*******************************************************************
double Bus64ToDouble(int *bus)
{
  __int64 iRet = 0;
  for (int i=0; i<64; i++)
    iRet |= (bus[i]<<i);
  return *((double*)&iRet);
}

//*******************************************************************
uint32 BusToInt32(uint32 *bus, int size)
{
  uint32 iRet = 0;
  for (int i=0; i<size; i++)
    iRet |= (bus[i]<<i);
  return iRet;
}

//*******************************************************************
uint32 BusToInt32(TDFF<uint32> *bus, int size)
{
  uint32 iRet = 0;
  for (int i=0; i<size; i++)
    iRet |= (bus[i].Q<<i);
  return iRet;
}


//*******************************************************************
int32 SignBusToInt32(uint32 *bus, int size)
{
  int32 iRet = 0;
  int32 iMask = (-1)<<size;
  for (int i=0; i<size; i++)
    iRet |= (bus[i]<<i);
  if (bus[size-1]) iRet |= iMask;
  return iRet;
}

//*******************************************************************
int32 SignBusToInt32(TDFF<uint32> *bus, int size)
{
  int32 iRet = 0;
  int32 iMask = (-1)<<size;
  for (int i=0; i<size; i++)
    iRet |= (bus[i].Q<<i);
  if (bus[size-1].Q) iRet |= iMask;
  return iRet;
}

//*******************************************************************
void Int32ToBus(int32 in, uint32 *outBus, int outSize)
{
  for (int i=0; i<outSize; i++)
    outBus[i] = (in>>i)&0x1;
}


//*******************************************************************
uint64 BusToInt64(uint32 *bus, int size)
{
  uint64 iRet = 0;
  for (int i=0; i<size; i++)
    iRet |= (((int64)bus[i])<<i);
  return iRet;
}

//*******************************************************************
uint64 BusToInt64(TDFF<uint32> *bus, int size)
{
  uint64 iRet = 0;
  for (int i=0; i<size; i++)
    iRet |= (((uint64)bus[i].Q)<<i);
  return iRet;
}


//*******************************************************************
int64 SignBusToInt64(uint32 *bus, int size)
{
  int64 iRet = 0;
  int64 iMask = (-1)<<size;
  for (int i=0; i<size; i++)
    iRet |= (bus[i]<<i);
  if (bus[size-1]) iRet |= iMask;
  return iRet;
}

//*******************************************************************
int64 SignBusToInt64(TDFF<uint32> *bus, int size)
{
  int64 iRet = 0;
  int64 iMask = (-1)<<size;
  for (int i=0; i<size; i++)
    iRet |= (bus[i].Q<<i);
  if (bus[size-1].Q) iRet |= iMask;
  return iRet;
}


//*******************************************************************
void Int64ToBus(int64 in, uint32 *outBus, int outSize)
{
  for (int i=0; i<outSize; i++)
    outBus[i] = (int)((in>>i)&0x1);
}

