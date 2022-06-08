#ifndef _MQUEUEFSM_H_
#define _MQUEUEFSM_H_

#include "mcmd.h"
#include "state/mstate.h"
#include "stdint.h"

namespace QueueFsm
{
  // struct MQueConsts  
  // {
  //   // Команды фоновые
  //   MCmd::cmd_get_u           // 0x11 Чтение напряжения (мВ);
  //   MCmd::cmd_get_i           // 0x12 Чтение тока (мА)
  //   MCmd::cmd_get_state       // 0x14 Чтение состояния;
  //   MCmd::cmd_get_celsius     // 0x15 Чтение температуры радиатора
  // };

  class MStart : public MState
  {       
    public:
      MStart(MTools * Tools);
      MState * fsm() override;
  };

  class MGetU : public MState
  {
    public:   
      MGetU(MTools * Tools);
      MState * fsm() override;
  };

  class MGetSU : public MState
  {
    public:   
      MGetSU(MTools * Tools);
      MState * fsm() override;
  };

  class MGetI : public MState
  {
    public:   
      MGetI(MTools * Tools);
      MState * fsm() override;
  };

  class MGetSI : public MState
  {
    public:   
      MGetSI(MTools * Tools);
      MState * fsm() override;
  };

  class MGetT : public MState
  {
    public:   
      MGetT(MTools * Tools);
      MState * fsm() override;
  };

    class MGetST : public MState
  {
    public:   
      MGetST(MTools * Tools);
      MState * fsm() override;
  };

  class MExt : public MState
  {
    public:   
      MExt(MTools * Tools);
      MState * fsm() override;
  };

    class MGetSE : public MState
  {
    public:   
      MGetSE(MTools * Tools);
      MState * fsm() override;
  };

};

#endif  //!_MQUEUEFSM_H_    