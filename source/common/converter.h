#pragma once


extern float Bus32ToFloat(int *bus);
extern double Bus64ToDouble(int *bus);

extern uint32 BusToInt32(uint32 *bus, int size);
extern uint32 BusToInt32(TDFF<uint32> *bus, int size);
extern uint64 BusToInt64(uint32 *bus, int size);
extern uint64 BusToInt64(TDFF<uint32> *bus, int size);

extern int32 SignBusToInt32(uint32 *bus, int size);
extern int64 SignBusToInt64(uint32 *bus, int size);

extern int32 SignBusToInt32(TDFF<uint32> *bus, int size);
extern int64 SignBusToInt64(TDFF<uint32> *bus, int size);

extern void Int32ToBus(int32 in, uint32 *outBus, int outSize);
extern void Int64ToBus(int64 in, uint32 *outBus, int outSize);



