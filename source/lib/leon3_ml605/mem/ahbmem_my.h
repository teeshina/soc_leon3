#pragma once

class AhbSlaveMem
{
  friend class dbg;
  private:
    static const uint32 RAM_SIZE      = 1024;  //[words] = 4096 bytes
    static const uint32 RAM_ADDR_MASK = 0xFFF;
    uint32 hindex;
    uint32 uiVendor;
    uint32 uiDevId;
    uint32 uiMemaddr;
    uint32 uiAddrmask;

    uint32 arrMem[RAM_SIZE];
    TDFF<uint32>rbData;
  public:
    AhbSlaveMem(uint32 hindex_,
                uint32 uiVendor_,
                uint32 uiDevId_,
                uint32 uiMemaddr_,
                uint32 uiAddrmask_);

    void Update(uint32 inNRst,
                SClock inClk,
                ahb_slv_in_type   inCtrl2Slv,
                ahb_slv_out_type  &outSlv2Ctrl );

    void ClkUpdate()
    {
      rbData.ClkUpdate();
    }
};
