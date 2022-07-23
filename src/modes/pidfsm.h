#ifndef _PIDFSM_H_
#define _PIDFSM_H_

#include "state/mstate.h"

namespace MPid
{
    /* Константы режима */
  struct MConst
  {
    //static constexpr short name = 0;
  };

  class MStart : public MState
  {       
    public:
      MStart(MTools * Tools);
      MState * fsm() override;
  };

  class MMode : public MState
  {       
    public:
      MMode(MTools * Tools);
      MState * fsm() override;
    private:
      short mode;
      static constexpr short fixed = 0;
      static constexpr short above = 3;
      static constexpr short below = 0;
  };

  class MSetPointV : public MState
  {
    public:  
      MSetPointV(MTools * Tools);
      MState * fsm() override;
    private:
      float setpoint;
      static constexpr float fixed =  14.0;
      static constexpr float fixedI = 10.0; //За пределами
      static constexpr float above =  17.0;
      static constexpr float below =   2.0;
  };





  class MStop : public MState
  {
    public:  
      MStop(MTools * Tools);
      MState * fsm() override;
  };

  class MExit : public MState
  {
    public:
      MExit(MTools * Tools);
      MState * fsm() override;
  };
};

#endif  // !_PIDFSM_H_
