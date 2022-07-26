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
    private:
      short m = fixed;
      static constexpr short fixed = 0;
      static constexpr short above = 1;
      static constexpr short below = 0;
  };

  // class MMode : public MState
  // {       
  //   public:
  //     MMode(MTools * Tools);
  //     MState * fsm() override;
  //   private:
  //     short m;
  //     static constexpr short fixed = 0;
  //     static constexpr short above = 1;
  //     static constexpr short below = 0;
  // };

  class MSetPointV : public MState
  {
    public:  
      MSetPointV(MTools * Tools);
      MState * fsm() override;
    private:
      static constexpr float fixedV = 14.0;
      static constexpr float fixedI =  2.0;
      static constexpr float above  = 17.0;
      static constexpr float below  =  2.0;
  };

  class MSetPointI : public MState
  {
    public:  
      MSetPointI(MTools * Tools);
      MState * fsm() override;
    private:
      static constexpr float fixedV = 14.0;
      static constexpr float fixedI =  2.0;
      static constexpr float above  =  6.0;
      static constexpr float below  =  0.1;
  };

  class MSelectKpV : public MState
  {
    public:  
      MSelectKpV(MTools * Tools);
      MState * fsm() override;
    private:
      static constexpr float fixed =   0.06F;
      static constexpr float above = ((0x1ULL << 16)-1) >> 12;  // 0x000F
      static constexpr float below =   0.00F;
  };

  class MSelectKiV : public MState
  {
    public:  
      MSelectKiV(MTools * Tools);
      MState * fsm() override;
    private:
      static constexpr float fixed =   0.20F;
      static constexpr float above = ((0x1ULL << 16)-1) >> 12;  // 0x000F
      static constexpr float below =   0.00F;
  };

  class MSelectKdV : public MState
  {
    public:  
      MSelectKdV(MTools * Tools);
      MState * fsm() override;
    private:
      static constexpr float fixed =   0.00F;
      static constexpr float above = ((0x1ULL << 16)-1) >> 12;  // 0x000F
      static constexpr float below =   0.00F;
  };

  class MSelectKpI : public MState
  {
    public:  
      MSelectKpI(MTools * Tools);
      MState * fsm() override;
    private:
      static constexpr float fixed =   0.02F;
      static constexpr float above = ((0x1ULL << 16)-1) >> 12;  // 0x000F
      static constexpr float below =   0.00F;
  };

  class MSelectKiI : public MState
  {
    public:  
      MSelectKiI(MTools * Tools);
      MState * fsm() override;
    private:
      static constexpr float fixed =   0.20F;
      static constexpr float above = ((0x1ULL << 16)-1) >> 12;  // 0x000F
      static constexpr float below =   0.00F;
  };

  class MSelectKdI : public MState
  {
    public:  
      MSelectKdI(MTools * Tools);
      MState * fsm() override;
    private:
      static constexpr float fixed =   0.00F;
      static constexpr float above = ((0x1ULL << 16)-1) >> 12;  // 0x000F
      static constexpr float below =   0.00F;
  };


  class MSetPointD : public MState
  {
    public:  
      MSetPointD(MTools * Tools);
      MState * fsm() override;
    private:
      static constexpr float fixed =   0.02F;
      static constexpr float above = ((0x1ULL << 16)-1) >> 12;  // 0x000F
      static constexpr float below =   0.00F;
  };

  class MSelectKpD : public MState
  {
    public:  
      MSelectKpD(MTools * Tools);
      MState * fsm() override;
    private:
      static constexpr float fixed =   0.02F;
      static constexpr float above = ((0x1ULL << 16)-1) >> 12;  // 0x000F
      static constexpr float below =   0.00F;
  };





  // class MSelectKi : public MState
  // {
  //   public:  
  //     MSelectKi(MTools * Tools);
  //     MState * fsm() override;
  //   private:
  //     float par;
  //     static constexpr float fixedV =   0.02F;
  //     static constexpr float fixedI =   0.10F;
  //     static constexpr float fixedD =   0.02F;
  //     static constexpr float above  = ((0x1ULL << 16)-1) >> 12;  // 0x000F
  //     static constexpr float below  =   0.00F;
  // };

  // class MSelectKd : public MState
  // {
  //   public:  
  //     MSelectKd(MTools * Tools);
  //     MState * fsm() override;
  //   private:
  //     float par;
  //     static constexpr float fixedV =   0.00F;
  //     static constexpr float fixedI =   0.00F;
  //     static constexpr float fixedD =   0.00F;
  //     static constexpr float above  = ((0x1ULL << 16)-1) >> 12;  // 0x000F
  //     static constexpr float below  =   0.00F;
  // };

  class MGo : public MState
  {
    public:  
      MGo(MTools * Tools);
      MState * fsm() override;
    private:
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
