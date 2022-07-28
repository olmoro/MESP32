#ifndef _OPTIONSFSM_H_
#define _OPTIONSFSM_H_

/*
 * Доступные пользователю регулировки измерителей,
 *
 *
 */

#include "state/mstate.h"

namespace MOption
{
  class MStart : public MState
  {       
    public:
      MStart(MTools * Tools);
      MState * fsm() override;
  };

  class MSetPostpone : public MState
  {       
    public:
      MSetPostpone(MTools * Tools);
      MState * fsm() override;
    private:
        // Пределы регулирования задержки пуска, час
      static constexpr short fixed =  0;
      static constexpr short above = 24;
      static constexpr short below =  0;

      short pp = fixed;
  };

 

//***************

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

#endif // !_OPTIONSFSM_H_