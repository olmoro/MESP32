#ifndef _CCCVFSM_H_
#define _CCCVFSM_H_

#include "state/mstate.h"

namespace MCccv
{
  struct MConst
  {

    // // Параметры условий заряда (здесь – для батарей типа AGM)
    // static constexpr float voltageMaxFactor     = 1.234f;    // 12v  * 1.234 = 14.8v
    // static constexpr float voltageMinFactor     = 0.890f;    // 12v  * 0.89  = 10.7v
    // static constexpr float currentMaxFactor     = 0.100f;    // 55ah * 0.1   = 5,5A 
    // static constexpr float currentMinFactor     = 0.050f;    // 55ah * 0.05  = 2.75A
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
    private:
      short cnt;
      float voltageNom = 12.0;
      float capacity   = 55.0;
      static constexpr float voltageMaxFactor = 1.234f;    // 12v  * 1.234 = 14.8v
      static constexpr float voltageMinFactor = 0.890f;    // 12v  * 0.89  = 10.7v
      static constexpr float currentMaxFactor = 0.100f;    // 55ah * 0.1   = 5,5A 
      static constexpr float currentMinFactor = 0.050f;    // 55ah * 0.05  = 2.75A
  };

  class MClearCccvKeys : public MState
  {
    public:  
      MClearCccvKeys(MTools * Tools);
      MState * fsm() override;
    private:
      short cnt;
      bool done;
  };

  class MSetPidCoeffU : public MState
  {
    public:   
      MSetPidCoeffU(MTools * Tools);
      MState * fsm() override;
    private:
      float kp, ki, kd;

  };

  class MSetPidCoeffI : public MState
  {
    public:   
      MSetPidCoeffI(MTools * Tools);
      MState * fsm() override;
    private:
     float kp, ki, kd;
  };

  class MSetPidCoeffD : public MState
  {
    public:   
      MSetPidCoeffD(MTools * Tools);
      MState * fsm() override;
    private:
      float kp, ki, kd;
  };

  class MSetCurrentMax : public MState
  {
    public:   
      MSetCurrentMax(MTools * Tools);
      MState * fsm() override;
    private:
        // Пределы регулирования max тока
      static constexpr float above = 6.0f;
      static constexpr float below = 0.2f;
  };
  
  class MSetVoltageMax : public MState
  {
    public:   
      MSetVoltageMax(MTools * Tools);
      MState * fsm() override;
    private:
        // Пределы регулирования max напряжения
      static constexpr float above = 16.2f;
      static constexpr float below = 10.0f;
  };

  class MSetCurrentMin : public MState
  {
    public:     
      MSetCurrentMin(MTools * Tools);
      MState * fsm() override;
    private:
        // Пределы регулирования min тока
      static constexpr float above = 6.0f;
      static constexpr float below = 0.2f;
  };

  class MSetVoltageMin : public MState
  {
    public:     
      MSetVoltageMin(MTools * Tools);
      MState * fsm() override;
    private:
        // Пределы регулирования min напряжения
      static constexpr float above = 16.2f;
      static constexpr float below = 10.0f;
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
