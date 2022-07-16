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

  class MSetAdcOffset : public MState
  {
    public:   
      MSetAdcOffset(MTools * Tools);
      MState * fsm() override;
  };

  class MSetsetFactorU : public MState
  {
    public:   
      MSetsetFactorU(MTools * Tools);
      MState * fsm() override;
  };

  class MSetSmoothU : public MState
  {
    public:   
      MSetSmoothU(MTools * Tools);
      MState * fsm() override;
  };

  class MSetShiftU : public MState
  {
    public:   
      MSetShiftU(MTools * Tools);
      MState * fsm() override;
  };

  class MSetFactorI : public MState
  {
    public:   
      MSetFactorI(MTools * Tools);
      MState * fsm() override;
  };

  class MSetSmoothI : public MState
  {
    public:   
      MSetSmoothI(MTools * Tools);
      MState * fsm() override;
  };
  
  class MSetShiftI : public MState
  {
    public:   
      MSetShiftI(MTools * Tools);
      MState * fsm() override;
  };

  // class MSetPidConfigure : public MState
  // {
  //   public:   
  //     MSetPidConfigure(MTools * Tools);
  //     MState * fsm() override;
  // };
  
  // class MSetPidCoefficients : public MState
  // {
  //   public:   
  //     MSetPidCoefficients(MTools * Tools);
  //     MState * fsm() override;
  // };

  // class MSetPidOutputRange : public MState
  // {
  //   public:   
  //     MSetPidOutputRange(MTools * Tools);
  //     MState * fsm() override;
  // };

  // class MSetPidReconfigure : public MState
  // {
  //   public:   
  //     MSetPidReconfigure(MTools * Tools);
  //     MState * fsm() override;
  // };

  // class MSetPidClear : public MState
  // {
  //   public:   
  //     MSetPidClear(MTools * Tools);
  //     MState * fsm() override;
  // };

  // class MSetPidTest : public MState
  // {
  //   public:   
  //     MSetPidTest(MTools * Tools);
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
