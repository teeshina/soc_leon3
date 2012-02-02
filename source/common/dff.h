#pragma once

struct SClock
{
  enum EClock
  {
    CLK_NEGATIVE,
    CLK_POSEDGE,
    CLK_POSITIVE,
    CLK_NEGEDGE
  };
  EClock eClock;
  EClock eClock_z;
};

template<class T> class TDFF
{
  friend class dbg;
  public:
    T Q;
    T D;
    SClock CLK;
  public:
    TDFF()
    {
      memset((void*)&Q, 0, sizeof(T));
      memset((void*)&D, 0, sizeof(T));
    }
    void ClkUpdate() 
    {
      if(CLK.eClock==SClock::CLK_POSEDGE) Q = D;
      else                                D = Q;
    }
};

template<class T> class TDFFx
{
  friend class dbg;
  public:
    T Q;
    T D;
    SClock CLK;
  public:
    void ClkUpdate() 
    {
      if(CLK.eClock==SClock::CLK_POSEDGE) Q = D;
      else                                D = Q;
    }
};


