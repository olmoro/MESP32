#ifndef _BOOTFSM_H_
#define _BOOTFSM_H_

#include "state/mstate.h"

namespace Bootfsm
{
  class MStart : public MState
  {       
    public:
      MStart(MTools * Tools);
      MState * fsm() override;
    private:
      // Не беспокоим драйвер 3 секунды после рестарта 
      //int cnt;
      //const int duration = 10 * 3;   // По 10 вызовов за секунду = 3 секунды.
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

  // class MTxPidConfigure : public MState
  // {
  //   public:   
  //     MTxPidConfigure(MTools * Tools);
  //     MState * fsm() override;
  // };
  
  // class MTxPidCoefficients : public MState
  // {
  //   public:   
  //     MTxPidCoefficients(MTools * Tools);
  //     MState * fsm() override;
  // };

  // class MTxPidOutputRange : public MState
  // {
  //   public:   
  //     MTxPidOutputRange(MTools * Tools);
  //     MState * fsm() override;
  // };

  // class MTxPidReconfigure : public MState
  // {
  //   public:   
  //     MTxPidReconfigure(MTools * Tools);
  //     MState * fsm() override;
  // };

  // class MTxPidClear : public MState
  // {
  //   public:   
  //     MTxPidClear(MTools * Tools);
  //     MState * fsm() override;
  // };

  // class MTxPidTest : public MState
  // {
  //   public:   
  //     MTxPidTest(MTools * Tools);
  //     MState * fsm() override;
  // };

  // class MGetPidConfigure : public MState
  // {
  //   public:   
  //     MGetPidConfigure(MTools * Tools);
  //     MState * fsm() override;
  // };



  class MExit : public MState
  {
    public:
      MExit(MTools * Tools);
      MState * fsm() override;
  };

};

#endif//_BOOTFSM_H_
