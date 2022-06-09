#ifndef _MORDER_H_
#define _MORDER_H_

#include "mcmd.h"
#include "state/mstate.h"
//#include "stdint.h"

class MTools;
class MBoard;
class MState;

class MOrder
{
  public:
    MOrder(MTools * tools);

    void run();
    //void delegateWork();

  private:
    MTools * Tools = nullptr;
    MBoard * Board = nullptr;
    MState * State = nullptr;
};

namespace MOrderStates
{
  class MGetU : public MState
  {
    public:   
      MGetU(MTools * Tools) : MState(Tools) {}
      MState * fsm() override;
  };

  class MGetSU : public MState
  {
    public:   
      MGetSU(MTools * Tools) : MState(Tools) {}
      MState * fsm() override;
  };

  class MGetI : public MState
  {
    public:   
      MGetI(MTools * Tools) : MState(Tools) {}
      MState * fsm() override;
  };

  class MGetSI : public MState
  {
    public:   
      MGetSI(MTools * Tools) : MState(Tools) {}
      MState * fsm() override;
  };

  class MGetT : public MState
  {
    public:   
      MGetT(MTools * Tools) : MState(Tools) {}
      MState * fsm() override;
  };

    class MGetST : public MState
  {
    public:   
      MGetST(MTools * Tools) : MState(Tools) {}
      MState * fsm() override;
  };

  class MExt : public MState
  {
    public:   
      MExt(MTools * Tools) : MState(Tools) {}
      MState * fsm() override;
  };

    class MGetSE : public MState
  {
    public:   
      MGetSE(MTools * Tools) : MState(Tools) {}
      MState * fsm() override;
  };

};

#endif  //!_MORDER_H_    