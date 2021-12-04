#ifndef _MMEASURE_H_
#define _MMEASURE_H_

#include "state/mstate.h"

class MTools;
class MBoard;
class MState;

class MMeasure
{
  public:
    MMeasure(MTools * tools);

    void run();
    //void delegateWork();

  private:
    MTools * Tools = nullptr;
    MBoard * Board = nullptr;
    MState * State = nullptr;
};

namespace MMeasureStates
{
  class MAdcT : public MState
  {
    public:   
      MAdcT(MTools * Tools) : MState(Tools) {}     
      MState * fsm() override;
  };
  
  class MAdcPG : public MState
  {
    public:   
      MAdcPG(MTools * Tools) : MState(Tools) {}     
      MState * fsm() override;
  };
    
  // class MAdcPoff : public MState
  // {
  //   public:   
  //     MAdcPoff(MTools * Tools) : MState(Tools) {}     
  //     MState * fsm() override;
  // };

  // class MAdcDNoff : public MState
  // {
  //   public:   
  //     MAdcDNoff(MTools * Tools) : MState(Tools) {}     
  //     MState * fsm() override;
  // };

  // class MAdcBoff : public MState
  // {
  //   public:   
  //     MAdcBoff(MTools * Tools) : MState(Tools) {}     
  //     MState * fsm() override;
  // };

  // class MAdcUPoff : public MState
  // {
  //   public:   
  //     MAdcUPoff(MTools * Tools) : MState(Tools) {}     
  //     MState * fsm() override;
  // };

  // class MAdcCoff : public MState
  // {
  //   public:   
  //     MAdcCoff(MTools * Tools) : MState(Tools) {}     
  //     MState * fsm() override;
  // };

  // class MAdcPon : public MState
  // {
  //   public:   
  //     MAdcPon(MTools * Tools) : MState(Tools) {}     
  //     MState * fsm() override;
  // };

  // class MAdcDNon : public MState
  // {
  //   public:   
  //     MAdcDNon(MTools * Tools) : MState(Tools) {}     
  //     MState * fsm() override;
  // };

  // class MAdcBon : public MState
  // {
  //   public:   
  //     MAdcBon(MTools * Tools) : MState(Tools) {}     
  //     MState * fsm() override;
  // };

  // class MAdcUPon : public MState
  // {
  //   public:   
  //     MAdcUPon(MTools * Tools) : MState(Tools) {}     
  //     MState * fsm() override;
  // };

  // class MAdcCon : public MState
  // {
  //   public:   
  //     MAdcCon(MTools * Tools) : MState(Tools) {}     
  //     MState * fsm() override;
  // };

};

#endif  // !_MMEASURE_H_
