#ifndef _DEVICEFSM_H_
#define _DEVICEFSM_H_

#include "state/mstate.h"

namespace MDevice
{

  struct MConst
  {
  //   // Пределы регулирования min/max токов и напряжений
  //   static constexpr float i_l =  0.2f;
  //   static constexpr float i_h =  6.0f;
      // Пределы регулирования смещения АЦП
    static constexpr short adc_l = -20;
    static constexpr short adc_h =  20;
    static constexpr short shift_u_l = -200;
    static constexpr short shift_u_h = 200;
    static constexpr short factor_u_l = 0x2D4F - 20;
    static constexpr short factor_u_h = 0x2D4F + 20;



      // по умолчанию
    static constexpr short adc_offset = 0;
    static constexpr short shift_u    = 0;
    static constexpr short factor_u   = 0x2D4F;
    static constexpr short smooth_u   = 3;


  //   // Параметры условий заряда (здесь – для батарей типа AGM)
  //   static constexpr float voltageMaxFactor     = 1.234f;    // 12v  * 1.234 = 14.8v
  //   static constexpr float voltageMinFactor     = 0.890f;    // 12v  * 0.89  = 10.7v
  //   static constexpr float currentMaxFactor     = 0.100f;    // 55ah * 0.1   = 5,5A 
  //   static constexpr float currentMinFactor     = 0.050f;    // 55ah * 0.05  = 2.75A
  };

  class MStart : public MState
  {       
    public:
      MStart(MTools * Tools);
      MState * fsm() override;
  };

  class MAuto : public MState
  {       
    public:
      MAuto(MTools * Tools);
      MState * fsm() override;
  };

  class MManual : public MState
  {       
    public:
      MManual(MTools * Tools);
      MState * fsm() override;
  };

  class MShiftFactorU : public MState
  {       
    public:
      MShiftFactorU(MTools * Tools);
      MState * fsm() override;
  };



  class MShiftU : public MState
  {       
    public:
      MShiftU(MTools * Tools);
      MState * fsm() override;
  };

  class MFactorU : public MState
  {       
    public:
      MFactorU(MTools * Tools);
      MState * fsm() override;
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

#endif  // !_DEVICEFSM_H_
