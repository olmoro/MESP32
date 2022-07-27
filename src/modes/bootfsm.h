#ifndef _BOOTFSM_H_
#define _BOOTFSM_H_

#include "state/mstate.h"

namespace MBoot
{
  class MStart : public MState
  {       
    public:
      MStart(MTools * Tools);
      MState * fsm() override;
  };

  class MTxPowerStop : public MState
  {
    public:   
      MTxPowerStop(MTools * Tools);
      MState * fsm() override;
  };

  class MTxAdcOffset : public MState
  {
    public:   
      MTxAdcOffset(MTools * Tools);
      MState * fsm() override;
  };
  
  class MTxsetFactorU : public MState
  {
    public:   
      MTxsetFactorU(MTools * Tools);
      MState * fsm() override;
  };

  class MTxSmoothU : public MState
  {
    public:   
      MTxSmoothU(MTools * Tools);
      MState * fsm() override;
  };

  class MTxShiftU : public MState
  {
    public:   
      MTxShiftU(MTools * Tools);
      MState * fsm() override;
  };

  class MTxFactorI : public MState
  {
    public:   
      MTxFactorI(MTools * Tools);
      MState * fsm() override;
  };

  class MTxSmoothI : public MState
  {
    public:   
      MTxSmoothI(MTools * Tools);
      MState * fsm() override;
  };
  
  class MTxShiftI : public MState
  {
    public:   
      MTxShiftI(MTools * Tools);
      MState * fsm() override;
  };

  class MExit : public MState
  {
    public:
      MExit(MTools * Tools);
      MState * fsm() override;
  };

};

#endif//_BOOTFSM_H_
