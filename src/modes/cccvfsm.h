#ifndef _CCCVFSM_H_
#define _CCCVFSM_H_

#include "state/mstate.h"

namespace Cccv
{
  struct MConst
  {
    // Пределы регулирования min/max токов и напряжений
    static constexpr float i_l =  0.2f;
    static constexpr float i_h =  6.0f;
    static constexpr float v_l = 10.0f;
    static constexpr float v_h = 16.2f;


    // Параметры условий заряда (здесь – для батарей типа AGM)
    static constexpr float voltageMaxFactor     = 1.234f;    // 12v  * 1.234 = 14.8v
    static constexpr float voltageMinFactor     = 0.890f;    // 12v  * 0.89  = 10.7v
    static constexpr float currentMaxFactor     = 0.100f;    // 55ah * 0.1   = 5,5A 
    static constexpr float currentMinFactor     = 0.050f;    // 55ah * 0.05  = 2.75A
  };

  // //pid settings and gains
  // struct MPidConstants
  // {
  //   // Параметры регулирования
  //   static constexpr float outputMin            = 0.0f;
  //   static constexpr float outputMaxFactor      = 1.05f;     // factor for current limit
  //   static constexpr float bangMin              = 20.0f;     // За пределами, - отключить
  //   static constexpr float bangMax              = 20.0f;
  //   static constexpr unsigned long timeStep     = 100;
  //   // Подобранные значения для ПИД, фаза подъёма тока и далее, если в последующих фазах эти настройки будут устраивать
  //   static constexpr float k_p                  = 0.13f;
  //   static constexpr float k_i                  = 0.10f;
  //   static constexpr float k_d                  = 0.04f;
  // };

  class MStart : public MState
  {       
    public:
      MStart(MTools * Tools);
      MState * fsm() override;
  };

  class MSetPidCoeffU : public MState
  {
    public:   
      MSetPidCoeffU(MTools * Tools);
      MState * fsm() override;
    private:
      float kpU, kiU, kdU;
  };

  class MSetPidCoeffI : public MState
  {
    public:   
      MSetPidCoeffI(MTools * Tools);
      MState * fsm() override;
    private:
     float kpI, kiI, kdI;
  };

  class MSetPidCoeffD : public MState
  {
    public:   
      MSetPidCoeffD(MTools * Tools);
      MState * fsm() override;
    private:
      float kpD, kiD, kdD;
  };

  class MSetCurrentMax : public MState
  {
    public:   
      MSetCurrentMax(MTools * Tools);
      MState * fsm() override;
  };
  
  class MSetVoltageMax : public MState
  {
    public:   
      MSetVoltageMax(MTools * Tools);
      MState * fsm() override;
  };

  class MSetCurrentMin : public MState
  {
    public:     
      MSetCurrentMin(MTools * Tools);
      MState * fsm() override;
  };

  class MSetVoltageMin : public MState
  {
    public:     
      MSetVoltageMin(MTools * Tools);
      MState * fsm() override;
  };

  class MPostpone : public MState
  {
    public:   
      MPostpone(MTools * Tools);
      MState * fsm() override;
  };

  class MUpCurrent : public MState
  {
    public:   
      MUpCurrent(MTools * Tools);
      MState * fsm() override;
  };

  class MKeepVmax : public MState
  {
    public: 
      MKeepVmax(MTools * Tools);
      MState * fsm() override;
  };

  class MKeepVmin : public MState
  {
    public:   
      MKeepVmin(MTools * Tools);
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

#endif  // !_CCCVFSM_H_
